#include "../stdafx.h"

int CCore::maxPlayers = MAX_SERVER_PLAYERS;
int CCore::tickRate = TICK_RATE;

void CCore::Init()
{
	CConsole::Print("Starting SA:Syncer v%.2f...", VERSION);

	// Packet listeners
	CPlayerPackets::Init();

	// Timers and threads
	CTimers::Init();

	// Init server
	CNetworking::Init(8787);
}