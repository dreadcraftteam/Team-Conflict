//========= Copyright DREADCRAFT TEAM, All rights reserved. ====================//
//
// Purpose:
//      Created: CoaXion
//      Updated: DREADCRAFT
//
//==============================================================================//

#ifndef COPLAY_CONNECTION_H
#define COPLAY_CONNECTION_H
#pragma once

#include "coplay.h"
#include <tier0/threadtools.h>
#include "steam/isteamnetworkingsockets.h"
#include "SDL2/SDL_net.h"

//a single SDL/Steam connection pair, clients will only have 0 or 1 of these, one per remote player on the host
class CCoplayConnection : public CThread
{
public:
    CCoplayConnection(HSteamNetConnection hConn);
    void QueueForDeletion(int reason = k_ESteamNetConnectionEnd_App_ConnectionFinished){ m_deletionQueued = true; m_endReason = reason;}
    void ConnectToHost();

private:
    int Run();

public:
    // only check for inital messaging for passwords, if needed, a connecting client cant know for sure
    UDPsocket m_localSocket = NULL;
    uint16    m_port = 0;
    IPaddress m_sendbackAddress;

    HSteamNetConnection     m_hSteamConnection = 0;
    float                   m_timeStarted;

private:
    CInterlockedInt m_deletionQueued;
    bool            m_gameReady;
    // For when the steam connection is still being kept alive but there is no actual activity
    float m_lastPacketTime = 0; 
    int   m_endReason;
};
#endif
