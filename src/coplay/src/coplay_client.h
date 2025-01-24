#ifndef COPLAY_CLIENT_H
#define COPLAY_CLIENT_H
#pragma once

#include "coplay.h"
#include "steam/isteamnetworkingsockets.h"
#include "steam/isteamnetworkingutils.h"

class CCoplayConnection;
class CCoplayClient
{
public:
	CCoplayClient();

	void ConnectToHost(CSteamID host, std::string passcode = "");
	void CloseConnection(int reason = k_ESteamNetConnectionEnd_App_ConnectionFinished);
	bool ConnectionStatusUpdated(SteamNetConnectionStatusChangedCallback_t* pParam);
	bool IsConnected() const { return m_hConn != k_HSteamNetConnection_Invalid; }
	std::string GetPasscode(){return m_passcode;}

private:
	bool CreateConnection(HSteamNetConnection hConnection);

private:
	HSteamNetConnection m_hConn;
	CSteamID m_hostLobby;
	CCoplayConnection* m_pConnection;
	std::string m_passcode;
};
#endif
