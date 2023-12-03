#pragma once

class CCore
{
public:
	static int maxPlayers;
	static int tickRate;
	static char m_szServerName[MAX_SERVER_NAME];

	static void Init();
};