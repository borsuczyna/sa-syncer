#include "../stdafx.h"

std::vector<CPlayer*> CPlayerManager::m_players;

void CPlayerManager::AddPlayer(CPlayer* player)
{
	m_players.push_back(player);
}

void CPlayerManager::RemovePlayer(CPlayer* player)
{
	for (size_t i = 0; i < m_players.size(); i++)
	{
		if (m_players[i] == player)
		{
			m_players.erase(m_players.begin() + i);
			break;
		}
	}
}

void CPlayerManager::KickPlayer(CPlayer* player, const char* reason)
{
	CNetworking::KickPeer(player->m_pPeer, reason);
	CConsole::Print("Player '%s' (%d) has been kicked from the server! Reason: %s", player->GetName(), player->m_iID, reason);
}

void CPlayerManager::PlayerDisconnected(CPlayer* player)
{
	CConsole::Print("Player '%s' (%d) has disconnected from the server!", player->GetName(), player->m_iID);
	CPlayerManager::RemovePlayer(player);
}

CPlayer* CPlayerManager::GetPlayer(const char* name)
{
	for (size_t i = 0; i < m_players.size(); i++)
	{
		if (strcmp(m_players[i]->GetName(), name) == 0)
		{
			return m_players[i];
		}
	}
	return nullptr;
}

CPlayer* CPlayerManager::GetPlayer(int id)
{
	for (size_t i = 0; i < m_players.size(); i++)
	{
		if (m_players[i]->m_iID == id)
		{
			return m_players[i];
		}
	}
	return nullptr;
}

CPlayer* CPlayerManager::GetPlayer(ENetPeer* peer)
{
	for (size_t i = 0; i < m_players.size(); i++)
	{
		if (m_players[i]->m_pPeer == peer)
		{
			return m_players[i];
		}
	}
	return nullptr;
}

int CPlayerManager::GetFreeID()
{
	for (int i = 0; i < CCore::maxPlayers; i++)
	{
		bool bFound = false;
		for (size_t j = 0; j < m_players.size(); j++)
		{
			if (m_players[j]->m_iID == i)
			{
				bFound = true;
				break;
			}
		}
		if (bFound == false)
		{
			return i;
		}
	}

	return -1;
}

std::vector<CPlayer*> CPlayerManager::GetStreamedInPlayers(CVector3 position)
{
	std::vector<CPlayer*> players;
	for (size_t i = 0; i < m_players.size(); i++)
	{
		if (m_players[i]->IsStreamedIn(position))
		{
			players.push_back(m_players[i]);
		}
	}

	return players;
}

std::vector<CPlayer*> CPlayerManager::GetStreamedInPlayers(CPlayer* player)
{
	auto streamedIn = CPlayerManager::GetStreamedInPlayers(player->GetPosition());
	for (size_t i = 0; i < streamedIn.size(); i++)
	{
		if (streamedIn[i] == player)
		{
			streamedIn.erase(streamedIn.begin() + i);
			break;
		}
	}

	return streamedIn;
}