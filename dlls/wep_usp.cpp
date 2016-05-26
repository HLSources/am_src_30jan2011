/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "recoil_manager.h"

extern int gmsgClcommand;

enum elites_e {
	PISTOL_IDLE1 = 0,
	PISTOL_RELOAD,
	PISTOL_RELOAD_LAST,
	PISTOL_FIRE1,
	PISTOL_FIRE2,
	PISTOL_FIRE_LAST,
	PISTOL_DEPLOY,
	PISTOL_DEPLOY_VACIO,
	PISTOL_HOLSTER,
	PISTOL_HOLSTER_VACIO
};
LINK_ENTITY_TO_CLASS( weapon_elites, CElite );
LINK_ENTITY_TO_CLASS( weapon_pistol, CElite );

void CElite::Spawn( )
{
    pev->classname = MAKE_STRING("weapon_pistol"); // hack to allow for old names
    Precache( );
    m_iId = WEAPON_ELITE;
    SET_MODEL(ENT(pev), "models/weapons/pistol/w_pistol.mdl");
 //   m_fDefaultAnim = USP_DEFAULT_ANIM; //to define on startup if the silencer added or not
   
	m_iDefaultAmmo = 20;//USP_DEFAULT_GIVE;

    FallInit();// get ready to fall down.
}


void CElite::Precache( void )
{
    PRECACHE_MODEL("models/weapons/pistol/v_pistol.mdl");
    PRECACHE_MODEL("models/weapons/pistol/w_pistol.mdl");
    PRECACHE_MODEL("models/weapons/pistol/p_pistol.mdl");

    m_iShell = PRECACHE_MODEL ("models/weapons/shell_9mm.mdl");// brass shell

    PRECACHE_SOUND ("weapons/pistol/pistol_fire-1.wav");
    PRECACHE_SOUND ("weapons/pistol/pistol_fire-2.wav");
  
	//PRECACHE_SOUND ("weapons/elites/elites_fireboth-1.wav");

    m_usFirePistol = PRECACHE_EVENT( 1, "scripts/events/pistol.sc" );
}

int CElite::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "9mm";
    p->iMaxAmmo1 = _9MM_MAX_CARRY;
 //   p->pszAmmo2 = "9mm";
 //   p->iMaxAmmo2 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
    p->iMaxClip = 20; // USP_MAX_CLIP;
    p->iSlot = 1;
    p->iPosition = 5;//4
    p->iFlags = ITEM_FLAG_SELECTONEMPTY;
    p->iId = m_iId = WEAPON_ELITE;
    p->iWeight = ELITE_WEIGHT;
	p->weaponName = "HL2 Pistol";

    return 1;
}

int CElite::AddToPlayer( CBasePlayer *pPlayer )
{
    if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
    {
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Pistol"); //digamos al cliente

        MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
            WRITE_BYTE( m_iId );
        MESSAGE_END();
        return TRUE;
    }
    return FALSE;
}

void CElite::Holster( int skiplocal /* = 0 */ )
{
	/*
	m_pPlayer->m_fCanUseFlashlight = FALSE;

	if ( m_pPlayer->FlashlightIsOn() )//FIX: para no apagar lo q no est� prendido.
	{
		m_pPlayer->FlashlightTurnOff();
	}
*/
    m_fInReload = FALSE;// cancel any reload in progress.

    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
    m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

BOOL CElite::Deploy( )
{
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

    return DefaultDeploy( "models/weapons/pistol/v_pistol.mdl", "models/weapons/pistol/p_pistol.mdl", PISTOL_DEPLOY, "9mm", 2.50f);
}

void CElite::PrimaryAttack( void )
{
	// Aiming Mechanics 
	float targetx=0.312; // these are the numbers we will use for the aiming vector (X Y Z) 
	float targety=0.312; // these are the numbers the will be loward accordingly to adjust the aim 
	float targetz=0.312; 
	// Aiming Mechanics 
/*
	if (m_iClip == 0) 
	{
		SendWeaponAnim( PISTOL_IDLE1 );
	}
*/
    // don't fire if empty
    if (m_iClip <= 0)
    {
          PlayEmptySound();
          m_flNextPrimaryAttack = 0.15;
          return;
    }

	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) 
	return; // SP: Fix to allow multichange pressed


    // Weapon sound
    m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
    m_pPlayer->m_iWeaponFlash  = NORMAL_GUN_FLASH;

    // one less round in the clip
    m_iClip--;

    // add a muzzle flash
    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
    
    // player "shoot" animation
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

// Aiming Mechanics 
if(!(m_pPlayer->pev->button & (IN_FORWARD|IN_BACK))) //test to see if you are moving forward or back 
{ 
targetx-=0.090; //if you are not moving forward or back then we lower these numbers 
targety-=0.132; 
targetz-=0.090; 
} 
else 
{ 
targetx-=0.058; //if you are moving forward or back then we lower these numbers 
targety-=0.018; //notice the diffrence in the values from the code above 
targetz-=0.058; 
} 

if(!(m_pPlayer->pev->button & (IN_MOVELEFT|IN_MOVERIGHT))) //test to see if you are moving left or right 
{ 
targetx-=0.132; //do not mistake the above test for looking left or right this test is for straifing not turning 
targety-=0.090; // these values are almost the same as the above only we alter the x more then y and z 
targetz-=0.090; 
} 
else 
{ 
targetx-=0.018; 
targety-=0.058; 
targetz-=0.058; 
} 
if((m_pPlayer->pev->button & (IN_DUCK))) //this test checks if you are crouched 
{ 
targetx-=0.090; //the values here are only slightly diffrent from the above here we alter the z more then anything 
targety-=0.090; 
targetz-=0.132; 
} 
else 
{ 
targetx-=0.020; 
targety-=0.020; 
targetz-=0.020; 
} 
// Aiming Mechanics 
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( targetx, targety, targetz ), 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed ); 
//	Recoil( m_pPlayer, CVAR_GET_FLOAT( "rec_usp_moving" ), CVAR_GET_FLOAT( "rec_usp_stand" ), CVAR_GET_FLOAT( "rec_usp_crouch" ));
//	Recoil( m_pPlayer, 2, 1, 0.5);

  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFirePistol, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

    // Add a delay before the player can fire the next shot
//    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + FAMAS_FIRE_DELAY;    
    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.10;

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.10; // tata
	
	m_flTimeWeaponIdle    = UTIL_WeaponTimeBase() + 
                            UTIL_SharedRandomFloat(m_pPlayer->random_seed, 
                                        FAMAS_FIRE_DELAY + 1, FAMAS_FIRE_DELAY + 2);
}

void CElite::SecondaryAttack( void )
{

}

void CElite::Reload( void )
{
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 20, PISTOL_RELOAD, 2.36 );//4.5
	else
		iResult = DefaultReload( 20, PISTOL_RELOAD, 2.36 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 
	}
}
void CElite::WeaponIdle( void )
{
	ResetEmptySound( );
	SendShine();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = PISTOL_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = PISTOL_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

void CElite::SendShine( void )
{
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc = m_pPlayer->GetGunPosition( ); // + gpGlobals->v_up * -8 + gpGlobals->v_right * 8;
	Vector vecDir = gpGlobals->v_forward;
	Vector vecDest = vecSrc + gpGlobals->v_up * 200;//-100

	edict_t *pentIgnore;
	TraceResult tr;
	pentIgnore = m_pPlayer->edict();

	UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);
/*
	
		//UTIL_TraceLine( pev->origin, pev->origin - Vector(0,0,64), ignore_monsters, edict(), &tr );
		Vector		vecSpot;
		TraceResult	tr;

		vecSpot = pev->origin + Vector ( 0 , 0 , 8 );//move up a bit, and trace down.
		UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, 4096 ),  ignore_monsters, pentIgnore, & tr);
*/
		
	if (tr.flFraction != 1.0) // != 1.0)// Hemos tocado algo
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		float flDist = (pEntity->Center() - pev->origin).Length();

		if ( pEntity->IsBSPModel() )
		{
//			ALERT ( at_console, "I touch something!  %f\n", tr.flFraction );
			pev->skin = 0;
		}
		else
		{
//			ALERT ( at_console, "Is not a BSP model  %f\n", tr.flFraction );
		}
	}
	else
	{
//		ALERT ( at_console, "Im seeing the sky!  %f\n", tr.flFraction );
		pev->skin = 1;
	}
/*
	extern short g_sModelIndexLaser;
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMPOINTS );
			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );

			WRITE_COORD( vecDest.x );
			WRITE_COORD( vecDest.y );
			WRITE_COORD( vecDest.z );
			WRITE_SHORT( g_sModelIndexLaser );
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 0 ); // framerate

			WRITE_BYTE( 1 ); // life
			WRITE_BYTE( 1 );  // width

			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 255 );	// brightness
			WRITE_BYTE( 111 );		// speed
		MESSAGE_END();
*/
	/*
	MESSAGE_BEGIN( MSG_ONE, gmsgSetSkin, NULL, m_pPlayer->pev );
		WRITE_BYTE( pev->skin ); //weaponmodel skin.
	MESSAGE_END();
	*/
}

class CEliteAmmo : public CBasePlayerAmmo
{
    void Spawn( void )
    { 
        Precache( );
        SET_MODEL(ENT(pev), "models/w_9mmclip.mdl");
        CBasePlayerAmmo::Spawn( );
    }
    void Precache( void )
    {
        PRECACHE_MODEL ("models/w_9mmclip.mdl");
        PRECACHE_SOUND("items/9mmclip1.wav");
    }
    BOOL AddAmmo( CBaseEntity *pOther ) 
    { 
        if (pOther->GiveAmmo( AMMO_USP_GIVE, "9mm", USP_MAX_CARRY ) != -1)
        {
            EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
            return TRUE;
        }
        return FALSE;
    }
};
LINK_ENTITY_TO_CLASS( ammo_pistolclip, CEliteAmmo );
