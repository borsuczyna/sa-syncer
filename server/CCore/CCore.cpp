#include "../stdafx.h"

int CCore::maxPlayers = MAX_SERVER_PLAYERS;
int CCore::tickRate = TICK_RATE;
char CCore::m_szServerName[MAX_SERVER_NAME];

void CCore::Init()
{
	// Set server name to Freeroam
	strcpy_s(m_szServerName, "Freeroam");

	CConsole::Print("Starting SA:Syncer v%.2f...", VERSION);

	// Packet listeners
	CPlayerPackets::Init();

	// Timers and threads
	CTimers::Init();

	// Init server
	CNetworking::Init(8787);
}