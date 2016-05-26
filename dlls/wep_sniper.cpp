/***
*
*		   ��
*			 �    �������
*			� �   �  �  �
*		   �   �  �  �  �
*		  �     � �  �  �
*	   HALF-LIFE: ARRANGEMENT
*
*	AR Software (c) 2004-2007. ArrangeMent, S2P Physics, Spirit Of Half-Life and their
*	logos are the property of their respective owners.
*
*	You should have received a copy of the Developers Readme File
*   along with Arrange Mode's Source Code for Half-Life. If you
*	are going to copy, modify, translate or distribute this file
*	you should agree whit the terms of Developers Readme File.
*
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*
*	This product includes information related to Half-Life 2 licensed from Valve 
*	(c) 2004. 
*
*	All Rights Reserved.
*
*	Modifications by SysOp (sysop_axis@hotmail).
*
***/

//===================
// NEW WEAPON FILE
//===================

//OLD TIME 	1.37 

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "shake.h" //This is required for the screen fade

enum sniper_e
{
	SNIPER_IDLE1 = 0,
	SNIPER_IDLE2,
	SNIPER_FIRE1,
	SNIPER_FIRE2,
	SNIPER_FIRE_LAST,
	SNIPER_RELOAD_MORE,
	SNIPER_RELOAD,
	SNIPER_RELOAD_LAST,
	SNIPER_DEPLOY,
	SNIPER_HOLSTER,
	SNIPER_FIRE1_PRONE,
	SNIPER_FIRE2_PRONE,

};
LINK_ENTITY_TO_CLASS( weapon_sniper, CSniper );
//LINK_ENTITY_TO_CLASS( weapon_scout, CSniper );
LINK_ENTITY_TO_CLASS( ammo_sniper, CSniperAmmoClip );

void CSniper::Spawn( )
{
    pev->classname = MAKE_STRING("weapon_sniper"); // hack to allow for old names
    Precache( );

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
    SET_MODEL(ENT(pev), SNIPER_MODEL_WORLD);

    m_iId          = WEAPON_SNIPER;  
	
	if (pev->armorvalue)
	{ 
		if ((pev->armorvalue > 5) && (pev->armorvalue < 0))
		pev->armorvalue = 5;

		m_iDefaultAmmo = pev->armorvalue;
	}
	else
    m_iDefaultAmmo = 5; //SNIPER_DEFAULT_AMMO;					//SNIPER_DEFAULT_AMMO; 
   
	FallInit();  // get ready to fall down. //se supone que fueron disparadas
}											//4 rafagas
//This is our spawn function

void CSniper::Precache( void )
{
	if (!FStringNull (v_model) )
	{
		PRECACHE_MODEL( (char *)STRING(v_model) );
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL( (char *)STRING(p_model) );
		PRECACHE_MODEL( (char *)STRING(w_model) );
	}
	else
	{
		PRECACHE_MODEL(SNIPER_MODEL_1STPERSON);
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL(SNIPER_MODEL_3RDPERSON);
		PRECACHE_MODEL(SNIPER_MODEL_WORLD);
	}
    
	PRECACHE_MODEL("models/weapons/sniper/v_scope.mdl");	// 1x
	PRECACHE_MODEL("models/weapons/sniper/v_scope_2x.mdl");	// 2x

//    m_iShell = PRECACHE_MODEL ("models/weapons/shell_sniper.mdl");// brass shell
           
    PRECACHE_SOUND (SNIPER_SOUND_FIRE1);
    PRECACHE_SOUND (SNIPER_SOUND_FIRE2);   
    
	PRECACHE_SOUND ("player/damage/breath_hearth.wav");

    m_usFireSniper = PRECACHE_EVENT( 1, "scripts/events/sniper.sc" );
	m_usDoubleFire = PRECACHE_EVENT( 1, "scripts/events/sniper_shell.sc" );
}
//This is where we load any of the models or sounds that our weapon uses

void CSniper::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/sniper/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/sniper/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/sniper/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

int CSniper::GetItemInfo(ItemInfo *p)
{
    p->pszName   = STRING(pev->classname);
    p->pszAmmo1  = "ammo_sniper";              // The type of ammo it uses
    p->iMaxAmmo1 = SNIPER_MAX_AMMO;            // Max ammo the player can carry
    p->pszAmmo2  = NULL;                    // No secondary ammo
    p->iMaxAmmo2 = -1;
    p->iMaxClip  = 5; //SNIPER_DEFAULT_AMMO;        // The clip size
    p->iSlot     = 5;	//SNIPER_SLOT - 1;     // The number in the HUD
    p->iPosition = 0;    //SNIPER_POSITION;            // The position in a HUD slot
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
    p->iId       = m_iId = WEAPON_SNIPER;      // The weapon id
    p->iWeight   = SNIPER_WEIGHT;              // for autoswitching
    p->weaponName = "Sig SGG 3000";
    return 1;
}
//Here we set the information for the weapon

int CSniper::AddToPlayer( CBasePlayer *pPlayer )
{
    if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
    {
	//	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Recogida Sig SGG 3000"); //digamos al cliente
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Sig"); //digamos al cliente

          MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
          WRITE_BYTE( m_iId );
          MESSAGE_END();
          return TRUE;
    }
    return FALSE;
}

BOOL CSniper::Deploy( )
{
//	CVAR_SET_FLOAT( "in_reload", 1 );

	m_pPlayer->m_fCrosshairOff = TRUE;

	if (!FStringNull (v_model) )
	{
		m_pPlayer->pev->viewmodel = v_model;
//		m_pPlayer->pev->weaponmodel = p_model;
	
		SendWeaponAnim( SNIPER_DEPLOY, 1, 0 );	
		return TRUE;
	}
	else
    return DefaultDeploy( SNIPER_MODEL_1STPERSON, SNIPER_MODEL_3RDPERSON, 
                          SNIPER_DEPLOY, "awp" );
}
//animation and "mp5" is the series of animations in the player model

void CSniper::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_fCrosshairOff = FALSE;
	m_fInReload = FALSE;// cancel any reload in progress.

	m_pPlayer->b_EstaEnZoom = FALSE;
if (m_fInZoom == 0) 
{
	//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
m_fInZoom = 0;
}
else if (m_fInZoom == 1) 
{
	//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
m_fInZoom = 0;
}
else if (m_fInZoom == 2) 
{
	//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
m_fInZoom = 0;
}
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	if (m_iClip)
		SendWeaponAnim( SNIPER_HOLSTER );
	else
		SendWeaponAnim( SNIPER_HOLSTER );

//	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

}

void CSniper::PrimaryAttack()
{
/*
	if ( m_pPlayer->pev->fov != 0 )
	{
		SecondaryAttack();
	}
*/
    // don't fire underwater
    if (m_pPlayer->pev->waterlevel == 3)
    {
          PlayEmptySound( );
          m_flNextPrimaryAttack = 0.15;
          return;
    }
    

    // don't fire if empty
	if (m_iClip <= 0) 
	{ 
		if (m_fFireOnEmpty) 
		{ 
			//PlayEmptySound();//for some reason, doesn't work
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2; 
		} 
		return; 
	} 

    // Weapon sound
    m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; //QUIET_GUN_VOLUME;
    m_pPlayer->m_iWeaponFlash  = NORMAL_GUN_FLASH;//DIM_GUN_FLASH;

    // one less round in the clip
    m_iClip--;
    
    // add a muzzle flash
//    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
    
    // player "shoot" animation
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
    
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

//#ifdef CLIENT_DLL
//	if ( !bIsMultiplayer() )
//#else
//	if ( !g_pGameRules->IsMultiplayer() )
//#endif
//	{
	// ### COD RECOIL START ###
	if ( m_pPlayer->pev->velocity.Length2D() <= 200 ) //menor e igual
	{// solo pegar si esta caminano o menos, si es m�s no :|
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
		{	
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{	//  si esta en el suelo y agachado
				vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_PERFECT, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
				m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 8 * 5; //*8 * 5
			}
			else // si no es porq esta parado
			{			
				if ( (m_fInZoom == 1) || (m_fInZoom == 2) )//si tiene zoom, precision, sino no..					
				{
					vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_PERFECT, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
				//	m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 8 * 15; //*8 * 5
					ALERT ( at_console, "m_fInZoom 1 o 2\n" );
				}
				else
				{
					vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
				//	m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 8 * 15; //*8 * 5
					ALERT ( at_console, "no m_fInZoom mode!\n" );
				}
			}
		}
		else // si no esta en el suelo es porque esta en el aire
		{
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{// y si esta agachado en el aire gana un punto
				vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			//	ALERT ( at_console, "Firing whit 20 recoil # Agachado aire #  :( \n" );
			}						// 15 puntos igual
			else
			{ // y si no esta agachado 15 puntos (still on air)
				vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			//	ALERT ( at_console, "Firing whit 20 recoil # Saltando #  :( \n" ); // 15 puntos
			}
		}
	}
	else // si tiene mas velocidad q 220, corre
	{
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) // si esta en el piso, weno 7 degrees
		{																//perfe 3d
			vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		//	ALERT ( at_console, "Firing whit 7 recoil # Corriendo en Piso # :| \n" ); 
		}
		else // si no esta en el suelo es porque esta en el aire, matate
		{
			vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 8192, BULLET_PLAYER_SNIPER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			//ALERT ( at_console, "Firing whit 20 recoil # Corriendo en Aire #  X( \n" ); // 15 puntos
		}
	}
  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireSniper, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
    
    // Add a delay before the player can fire the next shot
    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.9;    //SNIPER_FIRE_DELAY 1.95
    m_flTimeWeaponIdle    = UTIL_WeaponTimeBase() + 
                            UTIL_SharedRandomFloat(m_pPlayer->random_seed, 
							0.9 + 1, 0.9 + 2);
	if (m_iClip != 0)
	m_flPumpTime = gpGlobals->time + 0.01;//call the pump!!!
//	m_flPumpTime = gpGlobals->time + 0.7;//call the pump!!!

if ( (m_fInZoom == 1) || (m_fInZoom == 2) )//si tiene zoom, precision, sino no..					
{
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0; //0.25
	SecondaryAttack();
	ALERT ( at_console, "secondary attack!\n" );
}
	m_pPlayer->b_EstaEnZoom = FALSE;
if (m_fInZoom == 0) 
{
	#ifndef CLIENT_DLL
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
m_fInZoom = 0;
}
else if (m_fInZoom == 1) 
{
	#ifndef CLIENT_DLL
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos
m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
m_fInZoom = 0;
}
else if (m_fInZoom == 2) 
{
	#ifndef CLIENT_DLL
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
m_fInZoom = 0;
}
}
//This is the meat of the weapon.


void CSniper::SecondaryAttack()
{
	if (m_iClip == 0)
	return;

	m_pPlayer->b_EstaEnZoom = TRUE;

if (m_fInZoom == 0) 
{
m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 20; // 0 means reset to default fov //45
m_fInZoom = 1;	//valor de zoom 1	//25
//ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "1x"); //digamos al cliente que tiene 1x de zoom
#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_scope.mdl"); // pongamos el circulo negro
#else
		LoadVModel ( "models/weapons/sniper/v_scope.mdl", m_pPlayer ); //carguemos el model al jugador
#endif									//0.1	0.01

		UTIL_ScreenFade( m_pPlayer, Vector(0,0,0), 1, 0.1, 255, FFADE_IN); //fade to red 
if(!m_pPlayer->ConditionsPresent)
EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "player/damage/breath_hearth.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
}
else if (m_fInZoom == 1) 
{
m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 10;//20
m_fInZoom = 2;
//ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "2x");

#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_scope_2x.mdl"); // pongamos el circulo negro
#else
		LoadVModel ( "models/weapons/sniper/v_scope_2x.mdl", m_pPlayer ); //carguemos el model al jugador
#endif									//0.1	0.01
}

else if (m_fInZoom == 2)
{
m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
#ifndef CLIENT_DLL
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");//model default
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
m_fInZoom = 0; //zoom ninguno (=ZERO)
	m_pPlayer->b_EstaEnZoom = FALSE;
//ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "0x");// el jugador ya no tiene zoom
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos
// limpiemos canal de sonido
EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
}
/*
	if ( m_pPlayer->pev->fov != 0 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
		m_fInZoom = 0;
	}

//  if ( m_pPlayer->pev->fov !=  40)
//	{
//	m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 40;
//		m_fInZoom = 1;
//	}

	else if ( m_pPlayer->pev->fov != 10 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 10;
		m_fInZoom = 1; 
	}
*/
	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5; 
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
}

void CSniper::Reload( void )
{
EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

//m_pPlayer->m_iHideHUD &= ~HIDEHUD_WEAPONS;
	//ya no es necesario, al cargar el v_scope el otro model desaparece
m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->b_EstaEnZoom = FALSE;

if (m_fInZoom == 0) 
{
	#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
m_fInZoom = 0;
}
else if (m_fInZoom == 1) 
{
	#ifndef CLIENT_DLL
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos
m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
m_fInZoom = 0;
}
else if (m_fInZoom == 2) 
{
	#ifndef CLIENT_DLL
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/sniper/v_sniper.mdl");
#else
	//	LoadVModel ( "models/v_scope.mdl", m_pPlayer );
#endif
//UTIL_ScreenFade( m_pPlayer, Vector(128,0,0), 0.001, 0.001, 16, FFADE_OUT); //fade desde rojo y sacamos

m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
m_fInZoom = 0;
}
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 5, SNIPER_RELOAD_LAST, 3.43 );
	else
	{
		switch (RANDOM_LONG(0,1))
		{
		case 0: iResult = DefaultReload( 5, SNIPER_RELOAD_MORE, 3.43 );
			break;
		case 1: iResult = DefaultReload( 5, SNIPER_RELOAD, 3.43 );
			break;
		}
	}
	/*
	if (m_iClip == 0)
		iResult = DefaultReload( 5, SNIPER_RELOAD_LAST, 3.05 );
	else
	{
		switch (RANDOM_LONG(0,1))
		{
		case 0: iResult = DefaultReload( 5, SNIPER_RELOAD_MORE, 2.05 );
			break;
		case 1: iResult = DefaultReload( 5, SNIPER_RELOAD, 1.70 );
			break;
		}
	}
	*/
	//	iResult = DefaultReload( 10, AK_RELOAD, 3.2 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 
				
		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		CBaseEntity *pEntity = Create( "item_clip_rifle", vecSrc, pev->angles );
	}
}

void CSniper::WeaponIdle( void )
{
	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{	
		int flags;
		#if defined( CLIENT_WEAPONS )
			flags = FEV_NOTHOST;
		#else
			flags = 0;
		#endif

		PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usDoubleFire );

		m_flPumpTime = 0;
	}

	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = SNIPER_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = SNIPER_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

void CSniperAmmoClip::Spawn( void )
{ 
    Precache( );
    SET_MODEL(ENT(pev), "models/weapons/sniper/w_sniperclip.mdl");
    CBasePlayerAmmo::Spawn( );
}

void CSniperAmmoClip::Precache( void )
{
    PRECACHE_MODEL ("models/weapons/sniper/w_sniperclip.mdl");
    PRECACHE_SOUND("items/9mmclip1.wav");
}

BOOL CSniperAmmoClip::AddAmmo( CBaseEntity *pOther ) 
{								//SNIPER_DEFAULT_AMMO
    int bResult = (pOther->GiveAmmo(5, "ammo_sniper", 
                                    SNIPER_MAX_AMMO) != -1);
    if (bResult)
    {
        EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
    }
    return bResult;
}
