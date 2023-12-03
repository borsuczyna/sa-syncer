#include "../stdafx.h"

char CCore::m_szNickname[MAX_NAME];
int CCore::m_iTickRate = TICK_RATE;

void HandleInitLocalPlayer()
{
    static bool bLocalPlayerInitialized = false;

    if (!bLocalPlayerInitialized)
    {
        CPlayerManager::InitLocalPlayer();
    }
}

void HandleServerConnecting()
{
    static ULONGLONG lastKeyPress = 0;

    if (GetKeyState('L') & 0x800 && GetTickCount64() - lastKeyPress > 1000)
    {
		lastKeyPress = GetTickCount64();
		CNetworking::Connect(8787);
	}
}

void CCore::SetNickname(const char* nickName)
{
	strcpy_s(m_szNickname, nickName);
}

void CCore::InitPacketTimers()
{
    CPlayerPackets::InitPlayerUpdateTimer();
}

void CCore::SetTickRate(int rate)
{
    m_iTickRate = rate;
    CConsole::Print("Tick rate is set to %d", m_iTickRate);
    CCore::InitPacketTimers();
}

void CCore::GameLaunch()
{
    SetNickname(CNickGenerator::GetRandomNickname());

    CConsole::Init();
    CPatches::Init();
    CHooks::Init();

    // Timers
    CTimers::Init();

    // Packet listeners
    CPlayerPackets::Init();

    Events::drawingEvent += []()
    {
        HandleInitLocalPlayer();
		HandleServerConnecting();
	};
}