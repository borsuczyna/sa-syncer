#pragma once

class CPlayerManager
{
	static CPlayer* m_pLocalPlayer;

public:
	static CPlayerInfo Players[MAX_SERVER_PLAYERS + 2];
	static CPad Pads[MAX_SERVER_PLAYERS + 2];

	static std::vector<CPlayer*> m_pPlayers;

	static CPlayer* GetLocalPlayer() { return m_pLocalPlayer; }
	static void InitLocalPlayer();
	static CPlayerPed* CreatePlayerPed(int id, int model, CVector3 position);
	static void DeletePlayerPed(CPlayerPed* ped);

	static void ProcessPlayers();
	static void Init();

	static void AddPlayer(CPlayer* player) { m_pPlayers.push_back(player); }
	static void RemovePlayer(CPlayer* player) { m_pPlayers.erase(std::find(m_pPlayers.begin(), m_pPlayers.end(), player)); }
	static CPlayer* GetPlayer(int id) { for (auto player : m_pPlayers) { if (player->m_iID == id) return player; } return nullptr; }
	static CPlayer* GetPlayer(std::string name) { for (auto player : m_pPlayers) { if (player->GetName() == name) return player; } return nullptr; }
	static CPlayer* GetPlayer(CPlayerPed* ped) { for (auto player : m_pPlayers) { if (player->GetPed() == ped) return player; } return nullptr; }
	static CPlayer* GetPlayer(CPed* ped) { for (auto player : m_pPlayers) { if (player->GetPed() == ped) return player; } return nullptr; }
	static std::vector<CPlayer*> GetPlayers() { return m_pPlayers; }
};