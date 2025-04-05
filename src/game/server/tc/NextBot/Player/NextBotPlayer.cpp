//========= Copyright Valve Corporation, All rights reserved. ============//
//
// 
//
//========================================================================//

#include "cbase.h"

#include "nav_mesh.h"

#include "tc/NextBot/NextBot.h"
#include "NextBotPlayer.h"

#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar NextBotPlayerStop( "nb_player_stop", "0", FCVAR_CHEAT, "Stop all NextBotPlayers from updating" );
ConVar NextBotPlayerWalk( "nb_player_walk", "0", FCVAR_CHEAT, "Force bots to walk" );
ConVar NextBotPlayerCrouch( "nb_player_crouch", "0", FCVAR_CHEAT, "Force bots to crouch" );
ConVar NextBotPlayerMove( "nb_player_move", "1", FCVAR_CHEAT, "Prevents bots from moving" );

