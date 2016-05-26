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

/**

  CHANGES ON THIS FILE:
  
A lot... I need to delete this... All this is really obsolete ('cause Am is not MP)

***/
//
// teamplay_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
 
#include	"skill.h"
#include	"game.h"
#include	"items.h"
#include	"voice_gamemgr.h"
#include	"hltv.h"

int			 m_bitsDamageType;	// what types of damage has monster (player) taken

#define HITGROUP_HEAD	1

#define HEAL_MAX_CARRY	5 //despues definier esto en weapons-h

extern DLL_GLOBAL CGameRules	*g_pGameRules;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgScoreInfo;
extern int gmsgMOTD;
extern int gmsgServerName;

#if NO_COMPILE_HOE
extern int g_teamplay;
#endif

extern int gmsgClcommand;

extern cvar_t mp_am_default_weapons;

#define ITEM_RESPAWN_TIME	30
#define WEAPON_RESPAWN_TIME	20
#define AMMO_RESPAWN_TIME	20

float g_flIntermissionStartTime = 0;

CVoiceGameMgr	g_VoiceGameMgr;

class CMultiplayGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool		CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker)
	{
#if NO_COMPILE_HOE
		if ( g_teamplay )
		{
			if ( g_pGameRules->PlayerRelationship( pListener, pTalker ) != GR_TEAMMATE )
			{
				return false;
			}
		}
#endif
		return true;
	}
};
static CMultiplayGameMgrHelper g_GameMgrHelper;

//*********************************************************
// Rules for the half-life multiplayer game.
//*********************************************************

CHalfLifeMultiplay :: CHalfLifeMultiplay()
{
	g_VoiceGameMgr.Init(&g_GameMgrHelper, gpGlobals->maxClients);

	RefreshSkillData();
	m_flIntermissionEndTime = 0;
	g_flIntermissionStartTime = 0;
	
	// 11/8/98
	// Modified by YWB:  Server .cfg file is now a cvar, so that 
	//  server ops can run multiple game servers, with different server .cfg files,
	//  from a single installed directory.
	// Mapcyclefile is already a cvar.

	// 3/31/99
	// Added lservercfg file cvar, since listen and dedicated servers should not
	// share a single config file. (sjb)
	if ( IS_DEDICATED_SERVER() )
	{
		// dedicated server
		char *servercfgfile = (char *)CVAR_GET_STRING( "servercfgfile" );

		if ( servercfgfile && servercfgfile[0] )
		{
			char szCommand[256];
			
			ALERT( at_console, "Executing dedicated server config file\n" );
			sprintf( szCommand, "exec %s\n", servercfgfile );
			SERVER_COMMAND( szCommand );
		}
	}
	else
	{
		// listen server
		char *lservercfgfile = (char *)CVAR_GET_STRING( "lservercfgfile" );

		if ( lservercfgfile && lservercfgfile[0] )
		{
			char szCommand[256];
			
			ALERT( at_console, "Executing listen server config file\n" );
			sprintf( szCommand, "exec %s\n", lservercfgfile );
			SERVER_COMMAND( szCommand );
		}
	}
}

/*
	BEGIN_READ( pbuf, iSize );

	const char *tmpcmd2 = READ_STRING();
	int sayteamno		= READ_BYTE();

	char tmpcmd3[512];

	if (sayteamno == 0)
		sprintf(tmpcmd3, "say_team %s", CHudTextMessage::BufferedLocaliseTextString( tmpcmd2 ));
	else
		sprintf(tmpcmd3, "%s", tmpcmd2);

	ClientCmd(tmpcmd3);

	return 1;
				ALERT ( at_console, "MESSAGE_BEGIN 1\n");
			MESSAGE_BEGIN(MSG_ONE, gmsgClcommand, NULL, pPlayer->pev);
				WRITE_STRING("thirdperson");
				WRITE_BYTE(1);//say_team creo
			MESSAGE_END();
*/

BOOL CHalfLifeMultiplay::ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if( g_VoiceGameMgr.ClientCommand(pPlayer, pcmd) )
		return TRUE;
#if NO_COMPILE_HOE
	if ( FStrEq( pcmd, "menuselect" ) )
    {
	//	extern int gmsgClcommand;

        if ( CMD_ARGC() < 2 )
            return TRUE;

        int slot = atoi( CMD_ARGV(1) );

		//if (pPlayer->pev->playerclass == 1) 
		if (pPlayer->pev->playerclass == 0) 
		{
			if (pPlayer->m_nmenudisp == 1) 
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgClcommand, NULL, pPlayer->pev);

				if (slot == 1) 
				{
					ALERT ( at_console, "slot 1\n");

					WRITE_STRING("#msg_go_go_go");
					//EMIT_SOUND_ALL_TEAM(CHAN_VOICE, "radio/go.wav", 1, ATTN_NORM, pPlayer->pev->team);
				}

				if (slot == 2) 
				{
						ALERT ( at_console, "slot 2\n");
					WRITE_STRING("#msg_fallback");
				//	EMIT_SOUND_ALL_TEAM(CHAN_VOICE, "radio/fallback.wav", 1, ATTN_NORM, pPlayer->pev->team); 
				}

				if (slot == 3) 
				{
						ALERT ( at_console, "slot 3\n");
					WRITE_STRING("#msg_assault");
				//	EMIT_SOUND_ALL_TEAM(CHAN_VOICE, "radio/assault.wav", 1, ATTN_NORM, pPlayer->pev->team); 
				}

				if (slot == 4) 
				{
						ALERT ( at_console, "slot 4\n");
					WRITE_STRING("#msg_formation");
				//	EMIT_SOUND_ALL_TEAM(CHAN_VOICE, "radio/formation.wav", 1, ATTN_NORM, pPlayer->pev->team); 
				}

				if (slot == 5) 
				{
						ALERT ( at_console, "slot 5\n");
					WRITE_STRING("#msg_air_attack");
				//	EMIT_SOUND_ALL_TEAM(CHAN_VOICE, "radio/airattack.wav", 1, ATTN_NORM, pPlayer->pev->team); 
				}

				if (slot == 6) 
				{
						ALERT ( at_console, "slot 6\n");
					WRITE_STRING("#msg_ground_attack");
				//	EMIT_SOUND_ALL_TEAM(CHAN_VOICE, "radio/groundattack.wav", 1, ATTN_NORM, pPlayer->pev->team); 
				}

				if (slot == 0) 
				{
						ALERT ( at_console, "slot 0\n");
					WRITE_STRING("menuselect");
					WRITE_BYTE(1);
				} 
				else
				{
						ALERT ( at_console, "0\n");
				}
					WRITE_BYTE(0);// say, solo creo

				MESSAGE_END();
			}//sys w
			else if (pPlayer->m_nmenudisp == 2) 
			{
			//	MESSAGE_BEGIN(MSG_ONE, gmsgClcommand, NULL, pPlayer->pev);

				if (slot == 1) 
				{
					pPlayer->GiveNamedItem( "weapon_knife" ); 
					pPlayer->GiveNamedItem( "weapon_glock18" );
					pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
					pPlayer->GiveNamedItem( "weapon_AK47" );
					pPlayer->GiveAmmo( 150, "ammo_AK47", AK74_MAX_AMMO );

					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Terrorist" );
				}

				if (slot == 2) 
				{
					pPlayer->GiveNamedItem( "weapon_heal" );
					pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!
					pPlayer->GiveNamedItem( "weapon_knife" );
					pPlayer->GiveNamedItem( "weapon_ber92f" );
					pPlayer->GiveNamedItem( "item_ber92f_silencer" );//silencer
					pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
					pPlayer->GiveNamedItem( "weapon_sniper" );
					pPlayer->GiveAmmo( 10, "ammo_sniper", SNIPER_MAX_AMMO );// 4 full reloads

					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Sniper" );
				}

				if (slot == 3) 
				{
					pPlayer->GiveNamedItem( "weapon_heal" );
					pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!
					pPlayer->GiveNamedItem( "weapon_knife" );
					pPlayer->GiveNamedItem( "weapon_hegrenade" );
					pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
					pPlayer->GiveAmmo( 130, "9mm", _9MM_MAX_CARRY );
					pPlayer->GiveNamedItem( "weapon_9mmAR" );
					pPlayer->GiveNamedItem( "item_mp5_silencer" );//silencer
					pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );// 4 full reloads
						
					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Soldier" );				
				}

				if (slot == 4) 
				{
					pPlayer->GiveNamedItem( "weapon_heal" );
					pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!
					pPlayer->GiveNamedItem( "weapon_knife" ); 
					pPlayer->GiveNamedItem( "weapon_ber92f" );
					pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
					pPlayer->GiveNamedItem( "weapon_uzi" );
					pPlayer->GiveNamedItem( "item_uzi_silencer" );//silencer

					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Spy" );
				}

				if (slot == 5) 
				{
					pPlayer->GiveNamedItem( "weapon_knife" ); 
					pPlayer->GiveNamedItem( "item_battery" );
					pPlayer->GiveNamedItem( "weapon_deagle" );
					pPlayer->GiveAmmo( 28, "ammo_deagle", DEAGLE_MAX_CARRY );
					pPlayer->GiveNamedItem( "weapon_m4a1" );
					pPlayer->GiveAmmo( 150, "556", _556_MAX_AMMO );
					pPlayer->GiveNamedItem( "item_m4a1_silencer" );//silencer

					ClientPrint(pPlayer->pev, HUD_PRINTNOTIFY, "#SpawnAs_Seal" );
				}

				if (slot == 6) 
				{
					pPlayer->GiveNamedItem( "weapon_knife" ); //que mas queres?
					pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
					pPlayer->GiveAmmo( 130, "9mm", _9MM_MAX_CARRY );
					pPlayer->GiveNamedItem( "weapon_M249" );
					pPlayer->GiveAmmo( 200, "ammo_M249", M249_MAX_AMMO );// 4 full reloads

					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_FireSupport" );
				}
				if (slot == 0) 
				{
					ALERT ( at_console, "slot 0\n");
			//		WRITE_STRING("menuselect");
			//		WRITE_BYTE(1);
				} 
				else
				{
						ALERT ( at_console, "0\n");
				}
				//	WRITE_BYTE(0);// say, solo creo

			//	MESSAGE_END();
			}
		}
		else
		{
			//	ALERT ( at_console, "MESSAGE_BEGIN 1\n");
		//	MESSAGE_BEGIN(MSG_ONE, gmsgClcommand, NULL, pPlayer->pev);
		//		WRITE_STRING("menuselect");
		//		WRITE_BYTE(1);//say_team creo
		//	MESSAGE_END();
		}

        return TRUE;
    }
#endif
	return FALSE;
}
//=========================================================
//=========================================================
void CHalfLifeMultiplay::RefreshSkillData( void )
{
// load all default values
	CGameRules::RefreshSkillData();

// override some values for multiplay.

	// suitcharger
	gSkillData.suitchargerCapacity = 30;
}

// longest the intermission can last, in seconds
#define MAX_INTERMISSION_TIME		120

extern cvar_t timeleft, fragsleft;

extern cvar_t mp_chattime;

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: Think ( void )
{
	g_VoiceGameMgr.Update(gpGlobals->frametime);

	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	if ( g_fGameOver )   // someone else quit the game already
	{
		// bounds check
		int time = (int)CVAR_GET_FLOAT( "mp_chattime" );
		if ( time < 1 )
			CVAR_SET_STRING( "mp_chattime", "1" );
		else if ( time > MAX_INTERMISSION_TIME )
			CVAR_SET_STRING( "mp_chattime", UTIL_dtos1( MAX_INTERMISSION_TIME ) );

		m_flIntermissionEndTime = g_flIntermissionStartTime + mp_chattime.value;

		// check to see if we should change levels now
		if ( m_flIntermissionEndTime < gpGlobals->time )
		{
			if ( m_iEndIntermissionButtonHit  // check that someone has pressed a key, or the max intermission time is over
				|| ( ( g_flIntermissionStartTime + MAX_INTERMISSION_TIME ) < gpGlobals->time) ) 
				ChangeLevel(); // intermission is over
		}

		return;
	}

	float flTimeLimit = timelimit.value * 60;
	float flFragLimit = fraglimit.value;

	time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);
	
	if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
	{
		GoToIntermission();
		return;
	}

	if ( flFragLimit )
	{
		int bestfrags = 9999;
		int remain;

		// check if any player is over the frag limit
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

			if ( pPlayer && pPlayer->pev->frags >= flFragLimit )
			{
				GoToIntermission();
				return;
			}


			if ( pPlayer )
			{
				remain = flFragLimit - pPlayer->pev->frags;
				if ( remain < bestfrags )
				{
					bestfrags = remain;
				}
			}

		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if ( frags_remaining != last_frags )
	{
		g_engfuncs.pfnCvar_DirectSet( &fragsleft, UTIL_VarArgs( "%i", frags_remaining ) );
	}

	// Updates once per second
	if ( timeleft.value != last_time )
	{
		g_engfuncs.pfnCvar_DirectSet( &timeleft, UTIL_VarArgs( "%i", time_remaining ) );
	}

	last_frags = frags_remaining;
	last_time  = time_remaining;
}


//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsMultiplayer( void )
{
	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsDeathmatch( void )
{
	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsCoOp( void )
{
	return gpGlobals->coop;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
//	if ( pPlayer->autoswitch == 0 )
//	return FALSE;

	if ( !pWeapon->CanDeploy() )
	{
		// that weapon can't deploy anyway.
		return FALSE;
	}

	if ( !pPlayer->m_pActiveItem )
	{
		// player doesn't have an active item!
		return TRUE;
	}

	if ( !pPlayer->m_pActiveItem->CanHolster() )
	{
		// can't put away the active item.
		return FALSE;
	}

	if ( pWeapon->iWeight() > pPlayer->m_pActiveItem->iWeight() )
	{
		return TRUE;
	}

	return FALSE;
}
void CHalfLifeMultiplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	// On r�cup�re ici les cvars sp�cifiques � chaque client
//	pPlayer->autoswitch			= atoi( g_engfuncs.pfnInfoKeyValue( infobuffer, "cl_autoswitch" ) );
}

BOOL CHalfLifeMultiplay :: GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon )
{
	/*
	CBasePlayerItem *pCheck;
	CBasePlayerItem *pBest;
	int iBestWeight;
	int i;

	iBestWeight = -1;
	pBest = NULL;

//	if ( pPlayer->autoswitch == 0 && pPlayer->m_pActiveItem )
//			return FALSE;

	if (pCurrentWeapon)
	{
		if ( !pCurrentWeapon->CanHolster() )
			return FALSE;

		for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
		{
			pCheck = pPlayer->m_rgpPlayerItems[ i ];

			while ( pCheck )
			{
				if ( pCheck->iWeight() > -1 && pCheck->iWeight() == pCurrentWeapon->iWeight() && pCheck != pCurrentWeapon )
				{
					if ( pCheck->CanDeploy() )
					{
						if ( pPlayer->SwitchWeapon( pCheck ) )
							return TRUE;
					}
				}
				else if ( pCheck->iWeight() > iBestWeight && pCheck != pCurrentWeapon )// don't reselect the weapon we're trying to get rid of
				{
					if ( pCheck->CanDeploy() )
					{
						iBestWeight = pCheck->iWeight();
						pBest = pCheck;
					}
				}

				pCheck = pCheck->m_pNext;
			}
		}
	}

	if ( !pBest )
		return FALSE;

	pPlayer->SwitchWeapon( pBest );

	return TRUE;
*/
	CBasePlayerItem *pCheck;
	CBasePlayerItem *pBest;// this will be used in the event that we don't find a weapon in the same category.
	int iBestWeight;
	int i;

	iBestWeight = -1;// no weapon lower than -1 can be autoswitched to
	pBest = NULL;

	if ( !pCurrentWeapon->CanHolster() )
	{
		// can't put this gun away right now, so can't switch.
		return FALSE;
	}

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pCheck = pPlayer->m_rgpPlayerItems[ i ];

		while ( pCheck )
		{
			if ( pCheck->iWeight() > -1 && pCheck->iWeight() == pCurrentWeapon->iWeight() && pCheck != pCurrentWeapon )
			{
				// this weapon is from the same category. 
				if ( pCheck->CanDeploy() )
				{
					if ( pPlayer->SwitchWeapon( pCheck ) )
					{
						return TRUE;
					}
				}
			}
			else if ( pCheck->iWeight() > iBestWeight && pCheck != pCurrentWeapon )// don't reselect the weapon we're trying to get rid of
			{
				//ALERT ( at_console, "Considering %s\n", STRING( pCheck->pev->classname ) );
				// we keep updating the 'best' weapon just in case we can't find a weapon of the same weight
				// that the player was using. This will end up leaving the player with his heaviest-weighted 
				// weapon. 
				if ( pCheck->CanDeploy() )
				{
					// if this weapon is useable, flag it as the best
					iBestWeight = pCheck->iWeight();
					pBest = pCheck;
				}
			}

			pCheck = pCheck->m_pNext;
		}
	}

	// if we make it here, we've checked all the weapons and found no useable 
	// weapon in the same catagory as the current weapon. 
	
	// if pBest is null, we didn't find ANYTHING. Shouldn't be possible- should always 
	// at least get the crowbar, but ya never know.
	if ( !pBest )
	{
		return FALSE;
	}

	pPlayer->SwitchWeapon( pBest );

	return TRUE;
	
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay :: ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	g_VoiceGameMgr.ClientConnected(pEntity);
	return TRUE;
}

extern int gmsgSayText;
extern int gmsgGameMode;

void CHalfLifeMultiplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 0 );  // game mode none
	MESSAGE_END();
}

void CHalfLifeMultiplay :: InitHUD( CBasePlayer *pl )
{
	// notify other clients of player joining the game
	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has joined the game\n", 
		( pl->pev->netname && STRING(pl->pev->netname)[0] != 0 ) ? STRING(pl->pev->netname) : "unconnected" ) );

	// team match?
#if NO_COMPILE_HOE
	if ( g_teamplay )
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" entered the game\n",  
			STRING( pl->pev->netname ), 
			GETPLAYERUSERID( pl->edict() ),
			GETPLAYERAUTHID( pl->edict() ),
			g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pl->edict() ), "model" ) );
	}
	else
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%i>\" entered the game\n",  
			STRING( pl->pev->netname ), 
			GETPLAYERUSERID( pl->edict() ),
			GETPLAYERAUTHID( pl->edict() ),
			GETPLAYERUSERID( pl->edict() ) );
	}
#endif
	UpdateGameMode( pl );

	// sending just one score makes the hud scoreboard active;  otherwise
	// it is just disabled for single play
	MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pl->edict() );
		WRITE_BYTE( ENTINDEX(pl->edict()) );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
	MESSAGE_END();

	if ( mp_am_default_weapons.value != 0  ) //no es cero esta _Activado_
	{
		SendMOTDToClient( pl->edict() );
	}
	else
	{
		ALERT ( at_console, "MESSAGE_BEGIN FIRST wepselect\n");
			MESSAGE_BEGIN(MSG_ONE, gmsgClcommand, NULL, pl->pev);
			WRITE_STRING("wepselect");
			WRITE_BYTE(1);//say_team creo
		MESSAGE_END();
	}



	// loop through all active players and send their score info to the new client
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		// FIXME:  Probably don't need to cast this just to read m_iDeaths
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );

		if ( plr )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pl->edict() );
				WRITE_BYTE( i );	// client number
				WRITE_SHORT( plr->pev->frags );
				WRITE_SHORT( plr->m_iDeaths );
				WRITE_SHORT( 0 );
				WRITE_SHORT( GetTeamIndex( plr->m_szTeamName ) + 1 );
			MESSAGE_END();
		}
	}

	if ( g_fGameOver )
	{
		MESSAGE_BEGIN( MSG_ONE, SVC_INTERMISSION, NULL, pl->edict() );
		MESSAGE_END();
	}
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: ClientDisconnected( edict_t *pClient )
{
	if ( pClient )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );

		if ( pPlayer )
		{
			FireTargets( "game_playerleave", pPlayer, pPlayer, USE_TOGGLE, 0 );

			// team match?
#if NO_COMPILE_HOE
			if ( g_teamplay )
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%s>\" disconnected\n",  
					STRING( pPlayer->pev->netname ), 
					GETPLAYERUSERID( pPlayer->edict() ),
					GETPLAYERAUTHID( pPlayer->edict() ),
					g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" ) );
			}
			else
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%i>\" disconnected\n",  
					STRING( pPlayer->pev->netname ), 
					GETPLAYERUSERID( pPlayer->edict() ),
					GETPLAYERAUTHID( pPlayer->edict() ),
					GETPLAYERUSERID( pPlayer->edict() ) );
			}
#endif
			pPlayer->RemoveAllItems( TRUE );// destroy all of the players weapons and items
		}
	}
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay :: FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	int iFallDamage = (int)falldamage.value;

	switch ( iFallDamage )
	{
	case 1://progressive
		pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
		return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
		break;
	default:
	case 0:// fixed
		return 10;
		break;
	}
} 

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	return TRUE;
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: PlayerThink( CBasePlayer *pPlayer )
{
	if ( g_fGameOver )
	{
		// check for button presses
		if ( pPlayer->m_afButtonPressed & ( IN_DUCK | IN_ATTACK | IN_ATTACK2 | IN_USE | IN_JUMP ) )
			m_iEndIntermissionButtonHit = TRUE;

		// clear attack/use commands from player
		pPlayer->m_afButtonPressed = 0;
		pPlayer->pev->button = 0;
		pPlayer->m_afButtonReleased = 0;
	}
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	
	addDefault = TRUE;

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}
#if NO_COMPILE_HOE
	if ( addDefault )
	{//esto se llama despues del primer spawn
		if ( mp_am_default_weapons.value != 0  ) //no es cero esta _Activado_
		{
			switch (RANDOM_LONG(1,12)) // 11 clases al azar
			{
			case 1: 
			pPlayer->GiveNamedItem( "weapon_knife" );
			pPlayer->GiveNamedItem( "weapon_hegrenade" );
			pPlayer->GiveNamedItem( "item_battery" );
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveAmmo( 130, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_famas" );
			pPlayer->GiveAmmo( 150, "556", _556_MAX_AMMO );
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Ging" );
			break;

			case 2: 
			pPlayer->GiveNamedItem( "weapon_heal" );
			pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!

			pPlayer->GiveNamedItem( "weapon_knife" );
			pPlayer->GiveNamedItem( "weapon_ber92f" );
			
			pPlayer->GiveNamedItem( "item_ber92f_silencer" );//silencer
			pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_sniper" );
			pPlayer->GiveAmmo( 10, "ammo_sniper", SNIPER_MAX_AMMO );// 4 full reloads
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Sniper" );
			break;

			case 3: 
			pPlayer->GiveNamedItem( "weapon_heal" );
			pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!

			pPlayer->GiveNamedItem( "weapon_knife" );
			pPlayer->GiveNamedItem( "weapon_hegrenade" );
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveAmmo( 130, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_9mmAR" );
					pPlayer->GiveNamedItem( "item_mp5_silencer" );//silencer
			pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );// 4 full reloads
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Soldier" );
			break;

			case 4: 
			pPlayer->GiveNamedItem( "weapon_knife" );
			pPlayer->GiveNamedItem( "weapon_hegrenade" );
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveAmmo( 130, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_shotgun" );
			pPlayer->GiveAmmo( 50, "buckshot", BUCKSHOT_MAX_CARRY );// 4 full reloads
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Shotgun" );
			break;

			case 5: 
			pPlayer->GiveNamedItem( "weapon_knife" ); //que mas queres?
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveAmmo( 130, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_M249" );
			pPlayer->GiveAmmo( 200, "ammo_M249", M249_MAX_AMMO );// 4 full reloads
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_FireSupport" );
			break;

			case 6: 
			pPlayer->GiveNamedItem( "weapon_heal" );
			pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!

			pPlayer->GiveNamedItem( "weapon_knife" ); 
			pPlayer->GiveNamedItem( "weapon_ber92f" );
			pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_uzi" );
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Spy" );
					pPlayer->GiveNamedItem( "item_uzi_silencer" );//silencer
			break;

			case 7: 
			pPlayer->GiveNamedItem( "weapon_knife" ); 
			pPlayer->GiveNamedItem( "weapon_glock18" );
			pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_AK47" );
			pPlayer->GiveAmmo( 150, "ammo_AK47", AK74_MAX_AMMO );
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Terrorist" );
			break;
	//sys w
			case 8: 
			pPlayer->GiveNamedItem( "weapon_knife" ); 
			pPlayer->GiveNamedItem( "item_battery" );
			pPlayer->GiveNamedItem( "weapon_deagle" );
			pPlayer->GiveAmmo( 28, "ammo_deagle", DEAGLE_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_m4a1" );
			pPlayer->GiveAmmo( 150, "556", _556_MAX_AMMO );
			ClientPrint(pPlayer->pev, HUD_PRINTNOTIFY, "#SpawnAs_Seal" );
					pPlayer->GiveNamedItem( "item_m4a1_silencer" );//silencer
			break;

			case 9: 
			pPlayer->GiveNamedItem( "weapon_knife" ); 
			pPlayer->GiveNamedItem( "weapon_glock18" );
			pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_usas" );
			pPlayer->GiveAmmo( 100, "buckshot", BUCKSHOT_MAX_CARRY );
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Usas" );
			break;

			case 10: 
			pPlayer->GiveNamedItem( "weapon_knife" ); 
			pPlayer->GiveNamedItem( "weapon_hegrenade" );
			pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
			pPlayer->GiveAmmo( 200, "9mm", _9MM_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_mp7" );
			pPlayer->GiveAmmo( 200, "P90", 30 );
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Assault" );
			break;

			case 11: 
			pPlayer->GiveNamedItem( "weapon_heal" );
			pPlayer->GiveAmmo( 5, "heal", HEAL_MAX_CARRY ); //Medicine!

			pPlayer->GiveNamedItem( "weapon_knife" );
			pPlayer->GiveNamedItem( "weapon_pistol" );
			pPlayer->GiveAmmo( 200, "45", USP_MAX_CARRY );
			pPlayer->GiveNamedItem( "weapon_m82" );
			pPlayer->GiveAmmo(10, "ammo_m82", 5 );//SVD_MAX_AMMO 
			pPlayer->GiveNamedItem( "weapon_clustergrenade" );
				pPlayer->GiveNamedItem( "weapon_clustergrenade" );
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Specialist" );
			break;

			case 12: 
			pPlayer->GiveNamedItem( "weapon_knife" );
			pPlayer->GiveNamedItem( "weapon_lx38" );
			pPlayer->GiveAmmo( 100, "uranium", URANIUM_MAX_CARRY );		
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#SpawnAs_Gunman" );
			break;
			} //cierre el switch
		}
		else
		{
			ALERT ( at_console, "MESSAGE_BEGIN  wepselect\n");
				MESSAGE_BEGIN(MSG_ONE, gmsgClcommand, NULL, pPlayer->pev);
				WRITE_STRING("wepselect");
				WRITE_BYTE(1);//say_team creo
			MESSAGE_END();
		}
	}//cierre del if
#endif
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay :: FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	return TRUE;
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}

BOOL CHalfLifeMultiplay :: AllowAutoTargetCrosshair( void )
{
	return ( aimcrosshair.value != 0 );
}

//=========================================================
// IPointsForKill - how many points awarded to anyone
// that kills this player?
//=========================================================
int CHalfLifeMultiplay :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	return 1;
}


//=========================================================
// PlayerKilled - someone/something killed this player
//=========================================================
void CHalfLifeMultiplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	DeathNotice( pVictim, pKiller, pInflictor );

	pVictim->m_iDeaths += 1;


	FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );
	CBasePlayer *peKiller = NULL;
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );
	if ( ktmp && (ktmp->Classify() == CLASS_PLAYER) )
		peKiller = (CBasePlayer*)ktmp;

	if ( pVictim->pev == pKiller )  
	{  // killed self
		pKiller->frags -= 1;
	}
	else if ( ktmp && ktmp->IsPlayer() )
	{
		// if a player dies in a deathmatch game and the killer is a client, award the killer some points
		pKiller->frags += IPointsForKill( peKiller, pVictim );

		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	}
	else
	{  // killed by the world
	//	pKiller->frags -= 1; anterior
//MH	pKiller->frags -= 1;	this should be victim (we don't want to give the world frags)
		pVictim->pev->frags -= 1;
//END
	}
	// update the scores
	// killed scores
	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );
		WRITE_SHORT( pVictim->pev->frags );
		WRITE_SHORT( pVictim->m_iDeaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( GetTeamIndex( pVictim->m_szTeamName ) + 1 );
	MESSAGE_END();

	// killers score, if it's a player
	CBaseEntity *ep = CBaseEntity::Instance( pKiller );
	if ( ep && ep->Classify() == CLASS_PLAYER )
	{
		CBasePlayer *PK = (CBasePlayer*)ep;

		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(PK->edict()) );
			WRITE_SHORT( PK->pev->frags );
			WRITE_SHORT( PK->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( GetTeamIndex( PK->m_szTeamName) + 1 );
		MESSAGE_END();

		// let the killer paint another decal as soon as he'd like.
		PK->m_flNextDecalTime = gpGlobals->time;
	}
	/*
#ifndef HLDEMO_BUILD
	if ( pVictim->HasNamedPlayerItem("weapon_c4") )
	{
		DeactivateSatchels( pVictim );
	}
#endif
	*/
}

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeMultiplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	// Work out what killed the player, and send a message to all clients about it
	CBaseEntity *Killer = CBaseEntity::Instance( pKiller );

	const char *killer_weapon_name = "world";		// by default, the player is killed by the world
	int killer_index = 0;
	
	// Hack to fix name change
	char *tau = "tau_cannon";
	char *gluon = "gluon gun";
//SP
	char *headshot = "headshot";

	if ( pKiller->flags & FL_CLIENT )
	{
		killer_index = ENTINDEX(ENT(pKiller));
		
		if ( pevInflictor )
		{
			if ( pevInflictor == pKiller )
			{
				// If the inflictor is the killer,  then it must be their current weapon doing the damage
				CBasePlayer *pPlayer = (CBasePlayer*)CBaseEntity::Instance( pKiller );
				
				if ( pPlayer->m_pActiveItem )
				{
					killer_weapon_name = pPlayer->m_pActiveItem->pszName();
				}
			}
			else
			{
				killer_weapon_name = STRING( pevInflictor->classname );  // it's just that easy
			}
		}
	}
	else
	{
		killer_weapon_name = STRING( pevInflictor->classname );
	}

	// strip the monster_* or weapon_* from the inflictor's classname
	if (pVictim->m_LastHitGroup == HITGROUP_HEAD)
	{
		//|SP| Headshot, o sea DISPARO a la cabeza, si es otro da�o que no cuente
//		if ( m_bitsDamageType == DMG_BULLET ) 											
//		{
			strcmp( killer_weapon_name, "headshot" );
			killer_weapon_name = headshot;
//		}

	}
	else if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
		killer_weapon_name += 7;
	else if ( strncmp( killer_weapon_name, "monster_", 8 ) == 0 )
		killer_weapon_name += 8;
	else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
		killer_weapon_name += 5;


	MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
		WRITE_BYTE( killer_index );						// the killer
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );		// the victim
		WRITE_STRING( killer_weapon_name );		// what they were killed by (should this be a string?)
	MESSAGE_END();

	// replace the code names with the 'real' names
	if ( !strcmp( killer_weapon_name, "egon" ) )
		killer_weapon_name = gluon;
	else if ( !strcmp( killer_weapon_name, "gauss" ) )
		killer_weapon_name = tau;

	if ( pVictim->pev == pKiller )  
	{
		// killed self

		// team match?
#if NO_COMPILE_HOE
		if ( g_teamplay )
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" committed suicide with \"%s\"\n",  
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );		
		}
		else
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" committed suicide with \"%s\"\n",  
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				GETPLAYERUSERID( pVictim->edict() ),
				killer_weapon_name );		
		}
#endif
	}
	else if ( pKiller->flags & FL_CLIENT || FL_FAKECLIENT)
	{
		// team match?
		// Decapitated?
		if ( pVictim->m_LastHitGroup == HITGROUP_HEAD )
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" killed \"%s<%i><%s><%i>\" with a headshot from \"%s\"\n",  
			STRING( pKiller->netname ),
			GETPLAYERUSERID( ENT(pKiller) ),
			GETPLAYERAUTHID( ENT(pKiller) ),
			GETPLAYERUSERID( ENT(pKiller) ),
			STRING( pVictim->pev->netname ),
			GETPLAYERUSERID( pVictim->edict() ),
			GETPLAYERAUTHID( pVictim->edict() ),
			GETPLAYERUSERID( pVictim->edict() ),
			killer_weapon_name );

			EMIT_SOUND_DYN(ENT(0), CHAN_STATIC, "player/damage/headshot_mp.wav", 1.0, ATTN_NONE, 0, PITCH_NORM);
		}
#if NO_COMPILE_HOE
		if ( g_teamplay )
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" killed \"%s<%i><%s><%s>\" with \"%s\"\n",  
				STRING( pKiller->netname ),
				GETPLAYERUSERID( ENT(pKiller) ),
				GETPLAYERAUTHID( ENT(pKiller) ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( ENT(pKiller) ), "model" ),
				STRING( pVictim->pev->netname ),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );
		}
		else
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" killed \"%s<%i><%s><%i>\" with \"%s\"\n",  
				STRING( pKiller->netname ),
				GETPLAYERUSERID( ENT(pKiller) ),
				GETPLAYERAUTHID( ENT(pKiller) ),
				GETPLAYERUSERID( ENT(pKiller) ),
				STRING( pVictim->pev->netname ),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				GETPLAYERUSERID( pVictim->edict() ),
				killer_weapon_name );
		}
#endif
	}
	else
	{ 
		// killed by the world

		// team match?
#if NO_COMPILE_HOE
		if ( g_teamplay )
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" committed suicide with \"%s\" (world)\n",
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ), 
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );		
		}
		else
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" committed suicide with \"%s\" (world)\n",
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ), 
				GETPLAYERAUTHID( pVictim->edict() ),
				GETPLAYERUSERID( pVictim->edict() ),
				killer_weapon_name );		
		}
#endif
	}

	MESSAGE_BEGIN( MSG_SPEC, SVC_DIRECTOR );
		WRITE_BYTE ( 9 );	// command length in bytes
		WRITE_BYTE ( DRC_CMD_EVENT );	// player killed
		WRITE_SHORT( ENTINDEX(pVictim->edict()) );	// index number of primary entity
		if (pevInflictor)
			WRITE_SHORT( ENTINDEX(ENT(pevInflictor)) );	// index number of secondary entity
		else
			WRITE_SHORT( ENTINDEX(ENT(pKiller)) );	// index number of secondary entity
		WRITE_LONG( 7 | DRC_FLAG_DRAMATIC);   // eventflags (priority and flags)
	MESSAGE_END();

//  Print a standard message
	// TODO: make this go direct to console
	//SP MP
//	return; // just remove for now

	char	szText[ 128 ];

	if ( pKiller->flags & FL_MONSTER )
	{
		// killed by a monster
		strcpy ( szText, STRING( pVictim->pev->netname ) );
		strcat ( szText, " fue alcanzado por una entidad.\n" );
		return;
	}

	if ( pKiller == pVictim->pev )
	{
		strcpy ( szText, STRING( pVictim->pev->netname ) );
		strcat ( szText, " se mato solo.\n" );
	}
	
	else if ( pKiller->flags & FL_CLIENT )
	{
		
		strcpy ( szText, STRING( pKiller->netname ) );

		strcat( szText, " : " );
		strcat( szText, killer_weapon_name );
		strcat( szText, " : " );

		strcat ( szText, STRING( pVictim->pev->netname ) );
		strcat ( szText, "\n" );
		
	}
	
	else if ( FClassnameIs ( pKiller, "worldspawn" ) )
	{
		strcpy ( szText, STRING( pVictim->pev->netname ) );
		strcat ( szText, " se cayo, se ahogo o algo.\n" );
	}
	else if ( pKiller->solid == SOLID_BSP )
	{
		strcpy ( szText, STRING( pVictim->pev->netname ) );
		strcat ( szText, " fue aplastado.\n" );
	}
	else
	{
		strcpy ( szText, STRING( pVictim->pev->netname ) );
		strcat ( szText, " murio misteriosamente.\n" );
	}

//	UTIL_ClientPrintAll( szText );
	if ( pKiller->flags & FL_CLIENT )
	UTIL_ClientPrintAll( HUD_PRINTCONSOLE, UTIL_VarArgs( szText) );
	else
	UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs( szText) );

//	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has joined the game\n"));

	return;
}

//=========================================================
// PlayerGotWeapon - player has grabbed a weapon that was
// sitting in the world
//=========================================================
void CHalfLifeMultiplay :: PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
}

//=========================================================
// FlWeaponRespawnTime - what is the time in the future
// at which this weapon may spawn?
//=========================================================
float CHalfLifeMultiplay :: FlWeaponRespawnTime( CBasePlayerItem *pWeapon )
{
	if ( weaponstay.value > 0 )
	{
		// make sure it's only certain weapons
		if ( !(pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD) )
		{
			return gpGlobals->time + 0;		// weapon respawns almost instantly
		}
	}

	return gpGlobals->time + WEAPON_RESPAWN_TIME;
}

// when we are within this close to running out of entities,  items 
// marked with the ITEM_FLAG_LIMITINWORLD will delay their respawn
#define ENTITY_INTOLERANCE	100

//=========================================================
// FlWeaponRespawnTime - Returns 0 if the weapon can respawn 
// now,  otherwise it returns the time at which it can try
// to spawn again.
//=========================================================
float CHalfLifeMultiplay :: FlWeaponTryRespawn( CBasePlayerItem *pWeapon )
{
	if ( pWeapon && pWeapon->m_iId && (pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD) )
	{
		if ( NUMBER_OF_ENTITIES() < (gpGlobals->maxEntities - ENTITY_INTOLERANCE) )
			return 0;

		// we're past the entity tolerance level,  so delay the respawn
		return FlWeaponRespawnTime( pWeapon );
	}

	return 0;
}

//=========================================================
// VecWeaponRespawnSpot - where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeMultiplay :: VecWeaponRespawnSpot( CBasePlayerItem *pWeapon )
{
	return pWeapon->pev->origin;
}

//=========================================================
// WeaponShouldRespawn - any conditions inhibiting the
// respawning of this weapon?
//=========================================================
int CHalfLifeMultiplay :: WeaponShouldRespawn( CBasePlayerItem *pWeapon )
{
	if ( pWeapon->pev->spawnflags & SF_NORESPAWN )
	{
		return GR_WEAPON_RESPAWN_NO;
	}

	return GR_WEAPON_RESPAWN_YES;
}

//=========================================================
// CanHaveWeapon - returns FALSE if the player is not allowed
// to pick up this weapon
//=========================================================
BOOL CHalfLifeMultiplay::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pItem )
{
	if ( weaponstay.value > 0 )
	{
		if ( pItem->iFlags() & ITEM_FLAG_LIMITINWORLD )
			return CGameRules::CanHavePlayerItem( pPlayer, pItem );

		// check if the player already has this weapon
		for ( int i = 0 ; i < MAX_ITEM_TYPES ; i++ )
		{
			CBasePlayerItem *it = pPlayer->m_rgpPlayerItems[i];

			while ( it != NULL )
			{
				if ( it->m_iId == pItem->m_iId )
				{
					return FALSE;
				}

				it = it->m_pNext;
			}
		}
	}

	return CGameRules::CanHavePlayerItem( pPlayer, pItem );
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem )
{
	return TRUE;
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem )
{
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::ItemShouldRespawn( CItem *pItem )
{
	if ( pItem->pev->spawnflags & SF_NORESPAWN )
	{
		return GR_ITEM_RESPAWN_NO;
	}

	return GR_ITEM_RESPAWN_YES;
}


//=========================================================
// At what time in the future may this Item respawn?
//=========================================================
float CHalfLifeMultiplay::FlItemRespawnTime( CItem *pItem )
{
	return gpGlobals->time + ITEM_RESPAWN_TIME;
}

//=========================================================
// Where should this item respawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeMultiplay::VecItemRespawnSpot( CItem *pItem )
{
	return pItem->pev->origin;
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay::PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount )
{
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsAllowedToSpawn( CBaseEntity *pEntity )
{
//	if ( pEntity->pev->flags & FL_MONSTER )
//		return FALSE;

	return TRUE;
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	if ( pAmmo->pev->spawnflags & SF_NORESPAWN )
	{
		return GR_AMMO_RESPAWN_NO;
	}

	return GR_AMMO_RESPAWN_YES;
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay::FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo )
{
	return gpGlobals->time + AMMO_RESPAWN_TIME;
}

//=========================================================
//=========================================================
Vector CHalfLifeMultiplay::VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo )
{
	return pAmmo->pev->origin;
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay::FlHealthChargerRechargeTime( void )
{
	return 60;
}


float CHalfLifeMultiplay::FlHEVChargerRechargeTime( void )
{
	return 30;
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_ACTIVE;
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_ACTIVE;
}

edict_t *CHalfLifeMultiplay::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = CGameRules::GetPlayerSpawnSpot( pPlayer );	
	if ( IsMultiplayer() && pentSpawnSpot->v.target )
	{
		FireTargets( STRING(pentSpawnSpot->v.target), pPlayer, pPlayer, USE_TOGGLE, 0 );
	}

	return pentSpawnSpot;
}


//=========================================================
//=========================================================
int CHalfLifeMultiplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life deathmatch has only enemies
	return GR_NOTTEAMMATE;
}

BOOL CHalfLifeMultiplay :: PlayFootstepSounds( CBasePlayer *pl, float fvol )
{
	if ( g_footsteps && g_footsteps->value == 0 )
		return FALSE;

	if ( pl->IsOnLadder() || pl->pev->velocity.Length2D() > 220 )
		return TRUE;  // only make step sounds in multiplayer if the player is moving fast enough

	return FALSE;
}

BOOL CHalfLifeMultiplay :: FAllowFlashlight( void ) 
{ 
	return flashlight.value != 0; 
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay :: FAllowMonsters( void )
{
	return ( allowmonsters.value != 0 );
//	return TRUE;
}

//=========================================================
//======== CHalfLifeMultiplay private functions ===========
#define INTERMISSION_TIME		6

void CHalfLifeMultiplay :: GoToIntermission( void )
{
	if ( g_fGameOver )
		return;  // intermission has already been triggered, so ignore.

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();

	// bounds check
	int time = (int)CVAR_GET_FLOAT( "mp_chattime" );
	if ( time < 1 )
		CVAR_SET_STRING( "mp_chattime", "1" );
	else if ( time > MAX_INTERMISSION_TIME )
		CVAR_SET_STRING( "mp_chattime", UTIL_dtos1( MAX_INTERMISSION_TIME ) );

	m_flIntermissionEndTime = gpGlobals->time + ( (int)mp_chattime.value );
	g_flIntermissionStartTime = gpGlobals->time;

	g_fGameOver = TRUE;
	m_iEndIntermissionButtonHit = FALSE;
}

#define MAX_RULE_BUFFER 1024

typedef struct mapcycle_item_s
{
	struct mapcycle_item_s *next;

	char mapname[ 32 ];
	int  minplayers, maxplayers;
	char rulebuffer[ MAX_RULE_BUFFER ];
} mapcycle_item_t;

typedef struct mapcycle_s
{
	struct mapcycle_item_s *items;
	struct mapcycle_item_s *next_item;
} mapcycle_t;

/*
==============
DestroyMapCycle

Clean up memory used by mapcycle when switching it
==============
*/
void DestroyMapCycle( mapcycle_t *cycle )
{
	mapcycle_item_t *p, *n, *start;
	p = cycle->items;
	if ( p )
	{
		start = p;
		p = p->next;
		while ( p != start )
		{
			n = p->next;
			delete p;
			p = n;
		}
		
		delete cycle->items;
	}
	cycle->items = NULL;
	cycle->next_item = NULL;
}

//static char com_token[ 1500 ];
char com_token[ 1500 ];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char *data)
{
	int             c;
	int             len;
	
	len = 0;
	com_token[0] = 0;
	
	if (!data)
		return NULL;
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
			break;
	} while (c>32);
	
	com_token[len] = 0;
	return data;
}

/*
==============
COM_TokenWaiting

Returns 1 if additional data is waiting to be processed on this line
==============
*/
int COM_TokenWaiting( char *buffer )
{
	char *p;

	p = buffer;
	while ( *p && *p!='\n')
	{
		if ( !isspace( *p ) || isalnum( *p ) )
			return 1;

		p++;
	}

	return 0;
}



/*
==============
ReloadMapCycleFile


Parses mapcycle.txt file into mapcycle_t structure
==============
*/
int ReloadMapCycleFile( char *filename, mapcycle_t *cycle )
{
	char szBuffer[ MAX_RULE_BUFFER ];
	char szMap[ 32 ];
	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
	int hasbuffer;
	mapcycle_item_s *item, *newlist = NULL, *next;

	if ( pFileList && length )
	{
		// the first map name in the file becomes the default
		while ( 1 )
		{
			hasbuffer = 0;
			memset( szBuffer, 0, MAX_RULE_BUFFER );

			pFileList = COM_Parse( pFileList );
			if ( strlen( com_token ) <= 0 )
				break;

			strcpy( szMap, com_token );

			// Any more tokens on this line?
			if ( COM_TokenWaiting( pFileList ) )
			{
				pFileList = COM_Parse( pFileList );
				if ( strlen( com_token ) > 0 )
				{
					hasbuffer = 1;
					strcpy( szBuffer, com_token );
				}
			}

			// Check map
			if ( IS_MAP_VALID( szMap ) )
			{
				// Create entry
				char *s;

				item = new mapcycle_item_s;

				strcpy( item->mapname, szMap );

				item->minplayers = 0;
				item->maxplayers = 0;

				memset( item->rulebuffer, 0, MAX_RULE_BUFFER );

				if ( hasbuffer )
				{
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "minplayers" );
					if ( s && s[0] )
					{
						item->minplayers = atoi( s );
						item->minplayers = max( item->minplayers, 0 );
						item->minplayers = min( item->minplayers, gpGlobals->maxClients );
					}
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "maxplayers" );
					if ( s && s[0] )
					{
						item->maxplayers = atoi( s );
						item->maxplayers = max( item->maxplayers, 0 );
						item->maxplayers = min( item->maxplayers, gpGlobals->maxClients );
					}

					// Remove keys
					//
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "minplayers" );
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "maxplayers" );

					strcpy( item->rulebuffer, szBuffer );
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
			{
				ALERT( at_console, "Skipping %s from mapcycle, not a valid map\n", szMap );
			}

		}

		FREE_FILE( aFileList );
	}

	// Fixup circular list pointer
	item = cycle->items;

	// Reverse it to get original order
	while ( item )
	{
		next = item->next;
		item->next = newlist;
		newlist = item;
		item = next;
	}
	cycle->items = newlist;
	item = cycle->items;

	// Didn't parse anything
	if ( !item )
	{
		return 0;
	}

	while ( item->next )
	{
		item = item->next;
	}
	item->next = cycle->items;
	
	cycle->next_item = item->next;

	return 1;
}

/*
==============
CountPlayers

Determine the current # of active players on the server for map cycling logic
==============
*/
int CountPlayers( void )
{
	int	num = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt )
		{
			num = num + 1;
		}
	}

	return num;
}

/*
==============
ExtractCommandString

Parse commands/key value pairs to issue right after map xxx command is issued on server
 level transition
==============
*/
void ExtractCommandString( char *s, char *szCommand )
{
	// Now make rules happen
	char	pkey[512];
	char	value[512];	// use two buffers so compares
								// work without stomping on each other
	char	*o;
	
	if ( *s == '\\' )
		s++;

	while (1)
	{
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s )
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;

		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		strcat( szCommand, pkey );
		if ( strlen( value ) > 0 )
		{
			strcat( szCommand, " " );
			strcat( szCommand, value );
		}
		strcat( szCommand, "\n" );

		if (!*s)
			return;
		s++;
	}
}

/*
==============
ChangeLevel

Server is changing to a new level, check mapcycle.txt for map name and setup info
==============
*/
void CHalfLifeMultiplay :: ChangeLevel( void )
{
	static char szPreviousMapCycleFile[ 256 ];
	static mapcycle_t mapcycle;

	char szNextMap[32];
	char szFirstMapInList[32];
	char szCommands[ 1500 ];
	char szRules[ 1500 ];
	int minplayers = 0, maxplayers = 0;
	strcpy( szFirstMapInList, "hldm1" );  // the absolute default level is hldm1

	int	curplayers;
	BOOL do_cycle = TRUE;

	// find the map to change to
	char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
	ASSERT( mapcfile != NULL );

	szCommands[ 0 ] = '\0';
	szRules[ 0 ] = '\0';

	curplayers = CountPlayers();

	// Has the map cycle filename changed?
	if ( stricmp( mapcfile, szPreviousMapCycleFile ) )
	{
		strcpy( szPreviousMapCycleFile, mapcfile );

		DestroyMapCycle( &mapcycle );

		if ( !ReloadMapCycleFile( mapcfile, &mapcycle ) || ( !mapcycle.items ) )
		{
			ALERT( at_console, "Unable to load map cycle file %s\n", mapcfile );
			do_cycle = FALSE;
		}
	}

	if ( do_cycle && mapcycle.items )
	{
		BOOL keeplooking = FALSE;
		BOOL found = FALSE;
		mapcycle_item_s *item;

		// Assume current map
		strcpy( szNextMap, STRING(gpGlobals->mapname) );
		strcpy( szFirstMapInList, STRING(gpGlobals->mapname) );

		// Traverse list
		for ( item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next )
		{
			keeplooking = FALSE;

			ASSERT( item != NULL );

			if ( item->minplayers != 0 )
			{
				if ( curplayers >= item->minplayers )
				{
					found = TRUE;
					minplayers = item->minplayers;
				}
				else
				{
					keeplooking = TRUE;
				}
			}

			if ( item->maxplayers != 0 )
			{
				if ( curplayers <= item->maxplayers )
				{
					found = TRUE;
					maxplayers = item->maxplayers;
				}
				else
				{
					keeplooking = TRUE;
				}
			}

			if ( keeplooking )
				continue;

			found = TRUE;
			break;
		}

		if ( !found )
		{
			item = mapcycle.next_item;
		}			
		
		// Increment next item pointer
		mapcycle.next_item = item->next;

		// Perform logic on current item
		strcpy( szNextMap, item->mapname );

		ExtractCommandString( item->rulebuffer, szCommands );
		strcpy( szRules, item->rulebuffer );
	}

	if ( !IS_MAP_VALID(szNextMap) )
	{
		strcpy( szNextMap, szFirstMapInList );
	}

	g_fGameOver = TRUE;

	ALERT( at_console, "CHANGE LEVEL: %s\n", szNextMap );
	if ( minplayers || maxplayers )
	{
		ALERT( at_console, "PLAYER COUNT:  min %i max %i current %i\n", minplayers, maxplayers, curplayers );
	}
	if ( strlen( szRules ) > 0 )
	{
		ALERT( at_console, "RULES:  %s\n", szRules );
	}
	
	CHANGE_LEVEL( szNextMap, NULL );
	if ( strlen( szCommands ) > 0 )
	{
		SERVER_COMMAND( szCommands );
	}
}

#define MAX_MOTD_CHUNK	  60
#define MAX_MOTD_LENGTH   1536 // (MAX_MOTD_CHUNK * 4)

void CHalfLifeMultiplay :: SendMOTDToClient( edict_t *client )
{
	// read from the MOTD.txt file
	int length, char_count = 0;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( (char *)CVAR_GET_STRING( "motdfile" ), &length );

	// send the server name
	MESSAGE_BEGIN( MSG_ONE, gmsgServerName, NULL, client );
		WRITE_STRING( CVAR_GET_STRING("hostname") );
	MESSAGE_END();

	// Send the message of the day
	// read it chunk-by-chunk,  and send it in parts

	while ( pFileList && *pFileList && char_count < MAX_MOTD_LENGTH )
	{
		char chunk[MAX_MOTD_CHUNK+1];
		
		if ( strlen( pFileList ) < MAX_MOTD_CHUNK )
		{
			strcpy( chunk, pFileList );
		}
		else
		{
			strncpy( chunk, pFileList, MAX_MOTD_CHUNK );
			chunk[MAX_MOTD_CHUNK] = 0;		// strncpy doesn't always append the null terminator
		}

		char_count += strlen( chunk );
		if ( char_count < MAX_MOTD_LENGTH )
			pFileList = aFileList + char_count; 
		else
			*pFileList = 0;

		MESSAGE_BEGIN( MSG_ONE, gmsgMOTD, NULL, client );
			WRITE_BYTE( *pFileList ? FALSE : TRUE );	// FALSE means there is still more message to come
			WRITE_STRING( chunk );
		MESSAGE_END();
	}

	FREE_FILE( aFileList );
}
	

