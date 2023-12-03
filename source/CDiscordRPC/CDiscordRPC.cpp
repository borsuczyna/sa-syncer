#include "../stdafx.h"

const char* GeneratePartyId() // like "ae488379-351d-4a4f-ad32-2b9b01c91657"
{
	static char szPartyId[37] = { 0 };
	static const char* szCharacters = "abcdef0123456789";
	for (int i = 0; i < 36; ++i)
	{
		if (i == 8 || i == 13 || i == 18 || i == 23)
			szPartyId[i] = '-';
		else
			szPartyId[i] = szCharacters[rand() % 16];
	}
	return szPartyId;
}

void CDiscordRPC::UpdateInGame()
{
	static const char* partyId = GeneratePartyId();

	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();
	CPed* ped = localPlayer->GetPed();

	std::string playingOn;
	std::string playingAs;

	if (CNetworking::m_bIsConnected)
	{
		playingOn = "Playing on " + std::string(CCore::m_serverInfo.name);
		playingAs = std::string(CCore::m_szNickname) + " [ID: " + std::to_string(localPlayer->m_iID) + "]";
	}
	else
	{
		playingOn = "Singleplayer";
		playingAs = "Playing as " + std::string(CCore::m_szNickname);
	}

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.details = playingOn.c_str();
	discordPresence.state = playingAs.c_str();
	discordPresence.partyId = partyId;
	discordPresence.partySize = CPlayerManager::GetPlayers().size() + 1;
	discordPresence.partyMax = CCore::m_serverInfo.maxPlayers;
	discordPresence.startTimestamp = CCore::m_ulServerConnectTime;
	discordPresence.largeImageKey = "big";
	discordPresence.largeImageText = "San Andreas: Syncer";
	Discord_UpdatePresence(&discordPresence);
}

void CDiscordRPC::UpdateMainMenu()
{
	std::string playingAs = "Playing as " + std::string(CCore::m_szNickname);

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.details = playingAs.c_str();
	discordPresence.state = "In main menu";
	discordPresence.startTimestamp = CCore::m_ulGameLaunchTime;
	discordPresence.largeImageKey = "big";
	discordPresence.largeImageText = "San Andreas: Syncer";
	Discord_UpdatePresence(&discordPresence);
}

void CDiscordRPC::Update()
{
	CPlayer* pLocalPlayer = CPlayerManager::GetLocalPlayer();
	bool inMainMenu = pLocalPlayer == nullptr;

	if (inMainMenu)
		UpdateMainMenu();
	else
		UpdateInGame();
}

void InitAsync()
{
	DiscordEventHandlers Handler;
	memset(&Handler, 0, sizeof(Handler));
	Discord_Initialize("1180881295744712784", &Handler, 1, NULL);

	CConsole::Print("Discord RPC initialized");

	CTimers::CreateTimer(5000, 0, []()
	{
		CDiscordRPC::Update();
	});
}

void CDiscordRPC::Init()
{
	std::thread(InitAsync).detach();
}