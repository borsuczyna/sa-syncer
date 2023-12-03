#pragma once

class CCore
{
public:
    static char m_szNickname[MAX_NAME];
    static ServerInfo m_serverInfo;
    static int m_iTickRate;
    static time_t m_ulGameLaunchTime;
    static time_t m_ulServerConnectTime;

    static void InitPacketTimers();
    static void SetTickRate(int rate);
    static void SetNickname(const char* nickName);
    static void GameLaunch();
};