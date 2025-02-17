//========= Copyright DREADCRAFT TEAM, All rights reserved. ====================//
//
// Purpose:
//      Created: CoaXion
//      Updated: DREADCRAFT
//
//==============================================================================//

#ifndef COPLAY_H
#define COPLAY_H
#pragma once

#include "SDL2/SDL_net.h"
#include "steam/steam_api.h"

#include "tier0/valve_minmax_off.h"	// GCC 4.2.2 headers screw up our min/max defs.
#include <string>
#include "tier0/valve_minmax_on.h"

#ifdef COPLAY_USE_LOBBIES
extern ConVar coplay_use_lobbies;
#define UseCoplayLobbies() coplay_use_lobbies.GetBool()
#else
#define UseCoplayLobbies() false
#endif

#define COPLAY_MSG_COLOR Color(255, 255, 255, 255)
#define COPLAY_DEBUG_MSG_COLOR Color(255, 255, 255, 255)

#define COPLAY_MAX_PACKETS 8 // max packets proccessed in a single loop of running the connection.

#define COPLAY_VERSION "1.0"

#define COPLAY_NETMSG_NEEDPASS "NeedPasscode"
#define COPLAY_NETMSG_OK "OK"


enum JoinFilter
{
    eP2PFilter_OFF = -1,
    eP2PFilter_CONTROLLED = 0,
    eP2PFilter_FRIENDS = 1,
    eP2PFilter_EVERYONE = 2,
};

enum ConnectionRole
{
    eConnectionRole_UNAVAILABLE = -1,// Waiting on Steam
    eConnectionRole_INACTIVE = 0, // Coplay isnt doing anything right now
    eConnectionRole_HOST,
    eConnectionRole_CLIENT
};

enum ConnectionEndReason // see the enum ESteamNetConnectionEnd in steamnetworkingtypes.h
{
    k_ESteamNetConnectionEnd_App_NotOpen = 1001,
    k_ESteamNetConnectionEnd_App_ServerFull,
    k_ESteamNetConnectionEnd_App_RemoteIssue,// Couldn't open a socket
    k_ESteamNetConnectionEnd_App_ClosedByPeer,
    k_ESteamNetConnectionEnd_App_ConnectionFinished,// Everything went as expected

    // incoming connection rejected
    k_ESteamNetConnectionEnd_App_NotFriend,
    k_ESteamNetConnectionEnd_App_BadPassword,
};

static bool IsUserInLobby(CSteamID LobbyID, CSteamID UserID)
{
    uint32 numMembers = SteamMatchmaking()->GetNumLobbyMembers(LobbyID);
    for (uint32 i = 0; i < numMembers; i++)
    {
        if (UserID.ConvertToUint64() == SteamMatchmaking()->GetLobbyMemberByIndex(LobbyID, i).ConvertToUint64())
            return true;
    }
    return false;
}
#endif
