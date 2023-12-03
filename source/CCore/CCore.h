#pragma once

class CCore
{
public:
    static char m_szNickname[MAX_NAME];
    static int m_iTickRate;

    static void InitPacketTimers();
    static void SetTickRate(int rate);
    static void SetNickname(const char* nickName);
    static void GameLaunch();
};