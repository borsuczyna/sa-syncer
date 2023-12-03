#include "../stdafx.h"

CPlayer* CPlayerManager::m_pLocalPlayer = nullptr;
CPlayerInfo CPlayerManager::Players[MAX_SERVER_PLAYERS];
CPad CPlayerManager::Pads[MAX_SERVER_PLAYERS];

std::vector<CPlayer*> CPlayerManager::m_pPlayers;

void CPlayerManager::InitLocalPlayer()
{
	if (m_pLocalPlayer) return;

	CPlayerPed* pPlayerPed = FindPlayerPed();
	if (pPlayerPed)
	{
		m_pLocalPlayer = new CPlayer(-1, CCore::m_szNickname);
		m_pLocalPlayer->SetPed(pPlayerPed);
	}

	CConsole::Print("Local player initialized");
}

CPlayerPed* CPlayerManager::CreatePlayerPed(int id, int model, CVector3 position)
{
	if (model < 0 || model > 300) return nullptr;

	CStreaming::RequestModel(model, eStreamingFlags::PRIORITY_REQUEST | eStreamingFlags::GAME_REQUIRED);
	CStreaming::LoadAllRequestedModels(true);

	CPlayerPed* player = new CPlayerPed(id + 1, 0);

	player->m_nPedType = ePedType::PED_TYPE_PLAYER1;
	CWorld::Add(player);
	player->SetModelIndex(model);
	player->SetPosn(CVector(position.x, position.y, position.z));
	player->SetOrientation(0.0f, 0.0f, 0.0f);

	return player;
}

void CPlayerManager::DeletePlayerPed(CPlayerPed* ped)
{
	if (ped == nullptr) return;

	CWorld::Remove(ped);
	delete ped;
}