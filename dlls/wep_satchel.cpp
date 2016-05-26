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

// This file is bad called "satchel". it's a semtex

#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

enum satchel_e {
	SATCHEL_IDLE1 = 0,
	SATCHEL_FIDGET1,
	SATCHEL_DRAW,
	SATCHEL_DROP
};

enum satchel_radio_e {
	SATCHEL_RADIO_IDLE1 = 0,
	SATCHEL_RADIO_FIDGET1,
	SATCHEL_RADIO_DRAW,
	SATCHEL_RADIO_FIRE,
	SATCHEL_RADIO_HOLSTER
};



class CSatchelCharge : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	void BounceSound( void );

	void EXPORT SatchelSlide( CBaseEntity *pOther );
	void EXPORT SatchelThink( void );

public:
	void Deactivate( void );
};
LINK_ENTITY_TO_CLASS( monster_semtex, CSatchelCharge );

//=========================================================
// Deactivate - do whatever it is we do to an orphaned 
// satchel when we don't want it in the world anymore.
//=========================================================
void CSatchelCharge::Deactivate( void )
{
	pev->solid = SOLID_NOT;
	UTIL_Remove( this );
}


void CSatchelCharge :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	pev->classname = MAKE_STRING("monster_semtex"); // hack to allow for old names

	SET_MODEL(ENT(pev), "models/weapons/semtex/w_semtex.mdl");
	//UTIL_SetSize(pev, Vector( -16, -16, -4), Vector(16, 16, 32));	// Old box -- size of headcrab monsters/players get blocked by this
	UTIL_SetSize(pev, Vector( -4, -4, -4), Vector(4, 4, 4));	// Uses point-sized, and can be stepped over
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( SatchelSlide );
	SetUse( DetonateUse );
	SetThink( SatchelThink );
	pev->nextthink = gpGlobals->time + 0.1;

	pev->gravity = 0.8;
	pev->friction = 0.9;
//	pev->gravity = 0.5;
//	pev->friction = 0.8;

	pev->dmg = gSkillData.plrDmgSatchel;
	// ResetSequenceInfo( );
	pev->sequence = 1;
}

void CSatchelCharge::SatchelSlide( CBaseEntity *pOther )
{
	entvars_t	*pevOther = pOther->pev;

	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// pev->avelocity = Vector (300, 300, 300);
	pev->gravity = 0.0;// 1

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin - Vector(0,0,10), ignore_monsters, edict(), &tr );

	if ( tr.flFraction < 1.0 )
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
		// play sliding sound, volume based on velocity
	}
	if ( !(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10 )
	{
		BounceSound();
	}
	StudioFrameAdvance( );
	
	//SYS: add a satchel
	if ( (pev->flags & FL_ONGROUND)/* && pev->velocity.Length2D() < 0*/ )//if it's still
	{
		ALERT( at_console, ">NO ROOM TO FALL BACKWARD!\n");

		CBasePlayer *m_pPlayer = GetClassPtr((CBasePlayer *)pev);

		edict_t *pPlayer = m_pPlayer->edict( );

		CBaseEntity *pSatchel = NULL;

		while ((pSatchel = UTIL_FindEntityInSphere( pSatchel, m_pPlayer->pev->origin, 4096 )) != NULL)
		{
			if (FClassnameIs( pSatchel->pev, "monster_semtex"))
			{
				if (pSatchel->pev->owner == pPlayer)
				{
					pSatchel->Use( m_pPlayer, m_pPlayer, USE_ON, 0 );
					m_chargeReady = 2;
				}
			}
		}

		m_chargeReady = 2;
	}
}


void CSatchelCharge :: SatchelThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	if (pev->waterlevel == 3)
	{
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = pev->velocity * 0.8;
		pev->avelocity = pev->avelocity * 0.9;
		pev->velocity.z += 8;
	}
	else if (pev->waterlevel == 0)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}
	else
	{
		pev->velocity.z -= 8;
	}	
}

void CSatchelCharge :: Precache( void )
{
//	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
}

void CSatchelCharge :: BounceSound( void )
{
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce1.wav", 1, ATTN_NORM);	break;
	case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce2.wav", 1, ATTN_NORM);	break;
	case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce3.wav", 1, ATTN_NORM);	break;
	}
}

LINK_ENTITY_TO_CLASS( weapon_semtex, CSatchel );

//=========================================================
// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
//=========================================================
int CSatchel::AddDuplicate( CBasePlayerItem *pOriginal )
{
	CSatchel *pSatchel;

#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		pSatchel = (CSatchel *)pOriginal;

		if ( pSatchel->m_chargeReady != 0 )
		{
			// player has some satchels deployed. Refuse to add more.
			return FALSE;
		}
	}

	return CBasePlayerWeapon::AddDuplicate ( pOriginal );
}

//=========================================================
//=========================================================
int CSatchel::AddToPlayer( CBasePlayer *pPlayer )
{
	int bResult = CBasePlayerItem::AddToPlayer( pPlayer );

	pPlayer->pev->weapons |= (1<<m_iId);
	m_chargeReady = 0;// this satchel charge weapon now forgets that any satchels are deployed by it.

	if ( bResult )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Semtex"); //digamos al cliente

		return AddWeapon( );
	}
	return FALSE;
}

void CSatchel::Spawn( )
{
	Precache( );
	m_iId = WEAPON_C4;

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/semtex/w_semtex.mdl");

	m_iDefaultAmmo = SATCHEL_DEFAULT_GIVE;
		
	FallInit();// get ready to fall down.
}

void CSatchel::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/semtex/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/semtex/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/semtex/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CSatchel::Precache( void )
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
		PRECACHE_MODEL("models/weapons/semtex/v_semtex.mdl");
		PRECACHE_MODEL("models/weapons/semtex/v_semtex_radio.mdl");
		PRECACHE_MODEL("models/weapons/semtex/w_semtex.mdl");
	}
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		{
	PRECACHE_MODEL("models/p_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel_radio.mdl");
		}
	UTIL_PrecacheOther( "monster_semtex" );
}


int CSatchel::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Satchel Charge";
	p->iMaxAmmo1 = SATCHEL_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 6;//4
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = m_iId = WEAPON_C4;
	p->iWeight = SATCHEL_WEIGHT;
	p->weaponName = "Explosivo C4";

	return 1;
}

//=========================================================
//=========================================================
BOOL CSatchel::IsUseable( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return TRUE;
	}

	if ( m_chargeReady != 0 )
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}

	return FALSE;
}

BOOL CSatchel::CanDeploy( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return TRUE;
	}

	if ( m_chargeReady != 0 )
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}

	return FALSE;
}

BOOL CSatchel::Deploy( )
{
	m_pPlayer->m_fCrosshairOff = TRUE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );

	if ( m_chargeReady )
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( SATCHEL_RADIO_DRAW, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy( "models/weapons/semtex/v_semtex_radio.mdl", "models/null.mdl", SATCHEL_RADIO_DRAW, "hive" );
	}
	else
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( SATCHEL_DRAW, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy( "models/weapons/semtex/v_semtex.mdl", "models/null.mdl", SATCHEL_DRAW, "trip" );
	}
	
	return TRUE;
}


void CSatchel::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_fCrosshairOff = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	if ( m_chargeReady )
	{
		SendWeaponAnim( SATCHEL_RADIO_HOLSTER );
	}
	else
	{
		SendWeaponAnim( SATCHEL_DROP );
	}
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);

	if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_chargeReady )
	{
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_C4);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}



void CSatchel::PrimaryAttack()
{

		switch (m_chargeReady)
		{
		case 0:
			{
				if (m_pPlayer->b_InFuncSemtexZone == TRUE)
				{
					Throw( );
				}
				else
				{
					ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#SemtexCanOnlyUsedInSemtexZone");
				}
			}
			break;
		case 1:
			{
			SendWeaponAnim( SATCHEL_RADIO_FIRE );

			edict_t *pPlayer = m_pPlayer->edict( );

			CBaseEntity *pSatchel = NULL;

			while ((pSatchel = UTIL_FindEntityInSphere( pSatchel, m_pPlayer->pev->origin, 4096 )) != NULL)
			{
				if (FClassnameIs( pSatchel->pev, "monster_semtex"))
				{
					if (pSatchel->pev->owner == pPlayer)
					{
						pSatchel->Use( m_pPlayer, m_pPlayer, USE_ON, 0 );
						m_chargeReady = 2;
					}
				}
			}

			m_chargeReady = 2;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
			break;
			}

		case 2:
			// we're reloading, don't allow fire
			{
			}
			break;
		}

}


void CSatchel::SecondaryAttack( void )
{		
	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Client is attempting to create satchel!");

	/*
	if ( m_chargeReady != 2 )
	{
		Throw( );
	}
	*/
}

void CSatchel::Throw( void )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		/*
		Vector angThrow = pev->v_angle + pev->punchangle;

		if ( angThrow.x < 0 )
			angThrow.x = -10 + angThrow.x * ( ( 90 - 10 ) / 90.0 );
		else
			angThrow.x = -10 + angThrow.x * ( ( 90 + 10 ) / 90.0 );

		float flVel = ( 90 - angThrow.x ) * 4;
		if ( flVel > 500 )
			flVel = 500;

		UTIL_MakeVectors( angThrow );

		Vector vecSrc = pev->origin + pev->view_ofs + gpGlobals->v_forward * 16;//16

		Vector vecThrow = gpGlobals->v_forward * flVel + pev->velocity;

		Vector SatchelvecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

#ifndef CLIENT_DLL
		CBaseEntity *pSatchel = Create( "monster_semtex", vecSrc, vecThrow, m_pPlayer->edict() );
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#SemtexIsCreated");
	
		pSatchel->pev->velocity = SatchelvecThrow;
		pSatchel->pev->avelocity.y = 400;

		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/semtex/v_semtex_radio.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");
#else
		LoadVModel ( "models/weapons/semtex/v_semtex_radio.mdl", m_pPlayer );
#endif
*/
		
		//Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

	//	Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;
		Vector vecThrow = gpGlobals->v_forward + m_pPlayer->pev->velocity;

#ifndef CLIENT_DLL
		CBaseEntity *pSatchel = Create( "monster_semtex", vecSrc, Vector( 0, 0, 0), m_pPlayer->edict() );
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#SemtexIsCreated");
	
		pSatchel->pev->velocity = vecThrow;
		pSatchel->pev->avelocity.y = 10;//400

		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		}
		else
		{
			m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/semtex/v_semtex_radio.mdl");
			m_pPlayer->pev->weaponmodel = MAKE_STRING("models/null.mdl");
		}
#else
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		LoadVModel ( "models/weapons/semtex/v_semtex_radio.mdl", m_pPlayer );
#endif

		SendWeaponAnim( SATCHEL_RADIO_DRAW );

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_chargeReady = 1;
		
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}

void CSatchel::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	switch( m_chargeReady )
	{
	case 0:
		SendWeaponAnim( SATCHEL_FIDGET1 );
		// use tripmine animations
		strcpy( m_pPlayer->m_szAnimExtention, "trip" );
		break;
	case 1:
		SendWeaponAnim( SATCHEL_RADIO_FIDGET1 );
		// use hivehand animations
		strcpy( m_pPlayer->m_szAnimExtention, "hive" );
		break;
	case 2:
		if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			m_chargeReady = 0;
			RetireWeapon();
			return;
		}

#ifndef CLIENT_DLL
		
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		}
		else
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/semtex/v_semtex.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/null.mdl");
#else
		if (!FStringNull (v_model) )
		m_pPlayer->pev->viewmodel = v_model;
		else
		LoadVModel ( "models/weapons/semtex/v_semtex.mdl", m_pPlayer );
#endif

		SendWeaponAnim( SATCHEL_DRAW );

		// use tripmine animations
		strcpy( m_pPlayer->m_szAnimExtention, "trip" );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = 0;
		break;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
}

//=========================================================
// DeactivateSatchels - removes all satchels owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateSatchels( CBasePlayer *pOwner )
{
	edict_t *pFind; 

	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_semtex" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
		CSatchelCharge *pSatchel = (CSatchelCharge *)pEnt;

		if ( pSatchel )
		{
			if ( pSatchel->pev->owner == pOwner->edict() )
			{
				pSatchel->Deactivate();
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_semtex" );
	}
}

#endif