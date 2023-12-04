#include "../stdafx.h"

void CPlayerPackets::HandshakePacket(ENetPeer* peer, void* p, int size)
{
	CPackets::HandshakePacket* packet = (CPackets::HandshakePacket*)p;

	auto clientIp = peer->address.host;
	auto clientPort = peer->address.port;

	if (strcmp(packet->secret, SERVER_SECRET) != 0)
	{
		CNetworking::KickPeer(peer, "Wrong server secret");
		CConsole::Print("Player '%s' tried to connect, but server secret is wrong", packet->nickname);
		return;
	}

	CPlayer* player = CPlayerManager::GetPlayer(packet->nickname);
	if (player != nullptr)
	{
		CPlayerManager::KickPlayer(peer, "Player with this nickname is already connected");
		CConsole::Print("Player '%s' tried to connect, but player with this nickname is already connected", packet->nickname);
		return;
	}

	int id = CPlayerManager::GetFreeID();
	if (id == -1)
	{
		CNetworking::KickPeer(peer, "Server is full");
		CConsole::Print("Player '%s' tried to connect, but server is full", packet->nickname);
		return;
	}

	CConsole::Print("Player '%s' (%d) connected to the server!", packet->nickname, id);

	CPlayer* newPlayer = new CPlayer(id, packet->nickname, peer);
	newPlayer->SetPosition(packet->position);
	CPlayerManager::AddPlayer(newPlayer);

	CPackets::HandshakeResponsePacket response = CPackets::HandshakeResponsePacket(id, CCore::tickRate, CCore::m_szServerName, CCore::maxPlayers);
	CNetworking::SendPacket(CPackets::MessageId::HANDSHAKE_RESPONSE, newPlayer, &response, sizeof(CPackets::HandshakeResponsePacket));

	CPlayerPackets::SendPlayerConnected(newPlayer);
	CPlayerPackets::SendConnectedPlayers(newPlayer);
	CPlayerPackets::UpdateStreaming(newPlayer);
}

void CPlayerPackets::PlayerUpdatePacket(ENetPeer* peer, void* p, int size)
{
	CPackets::PlayerUpdatePacket* packet = (CPackets::PlayerUpdatePacket*)p;

	CPlayer* player = CPlayerManager::GetPlayer(peer);
	if (player == nullptr) return;

	player->m_updateData = packet->data;
	player->StopAiming();
}

void CPlayerPackets::PlayerAimPacket(ENetPeer* peer, void* p, int size)
{
	CPackets::PlayerAimPacket* packet = (CPackets::PlayerAimPacket*)p;

	CPlayer* player = CPlayerManager::GetPlayer(peer);
	if (player == nullptr) return;

	player->m_updateData = packet->data;
	player->AimAt(packet->aimTarget);
}

void CPlayerPackets::PlayerTaskPacket(ENetPeer* peer, void* p, int size)
{
	CPackets::PlayerTaskPacket* packet = (CPackets::PlayerTaskPacket*)p;

	CPlayer* player = CPlayerManager::GetPlayer(peer);
	if (player == nullptr) return;

	// Save ducking
	if (packet->taskType == 415) // TASK_SIMPLE_DUCK
	{
		player->m_bIsDucked = packet->taskData.toggle;
	}

	packet->playerId = player->m_iID;

	auto streamedFor = CPlayerManager::GetStreamedInPlayers(player);
	for (auto p : streamedFor)
	{
		CNetworking::SendPacket(CPackets::MessageId::PLAYER_TASK, p, packet, sizeof(CPackets::PlayerTaskPacket));
	}
}

void CPlayerPackets::SendPlayerConnected(CPlayer* player)
{
	CPackets::PlayerConnectedPacket packet = CPackets::PlayerConnectedPacket(player->m_iID, player->GetName());
	CNetworking::BroadcastPacket(CPackets::MessageId::PLAYER_CONNECTED, player, &packet, sizeof(CPackets::PlayerConnectedPacket));
}

void CPlayerPackets::SendPlayerConnected(CPlayer* player, CPlayer* target)
{
	CPackets::PlayerConnectedPacket packet = CPackets::PlayerConnectedPacket(player->m_iID, player->GetName());
	CNetworking::SendPacket(CPackets::MessageId::PLAYER_CONNECTED, target, &packet, sizeof(CPackets::PlayerConnectedPacket));
}

void CPlayerPackets::SendConnectedPlayers(CPlayer* player)
{
	auto players = CPlayerManager::GetPlayers();
	for (auto p : players)
	{
		if (p->m_iID == player->m_iID) continue;

		CPlayerPackets::SendPlayerConnected(p, player);
	}
}

void CPlayerPackets::SendPlayerDisconnected(CPlayer* player)
{
	CPackets::PlayerDisconnectedPacket packet = CPackets::PlayerDisconnectedPacket(player->m_iID);
	CNetworking::BroadcastPacket(CPackets::MessageId::PLAYER_DISCONNECTED, player, &packet, sizeof(CPackets::PlayerDisconnectedPacket));
}

void CPlayerPackets::SendPlayerStreamIn(CPlayer* player, CPlayer* target)
{
	if (player->IsStreamedFor(target)) return;

	player->AddStreamedFor(target);
	CPackets::PlayerStreamInPacket packet = CPackets::PlayerStreamInPacket(player->m_iID);
	packet.data = player->BuildUpdatePacket().data;
	packet.isDucked = player->m_bIsDucked;
	CNetworking::SendPacket(CPackets::MessageId::PLAYER_STREAM_IN, target, &packet, sizeof(CPackets::PlayerStreamInPacket));
}

void CPlayerPackets::SendPlayerStreamOut(CPlayer* player, CPlayer* target)
{
	if (!player->IsStreamedFor(target)) return;

	player->RemoveStreamedFor(target);
	CPackets::PlayerStreamOutPacket packet = CPackets::PlayerStreamOutPacket(player->m_iID);
	CNetworking::SendPacket(CPackets::MessageId::PLAYER_STREAM_OUT, target, &packet, sizeof(CPackets::PlayerStreamOutPacket));
}

void CPlayerPackets::UpdateStreaming(CPlayer* player)
{
	auto streamedFor = CPlayerManager::GetStreamedInPlayers(player);

	for (auto p : streamedFor)
	{
		CPlayerPackets::SendPlayerStreamIn(player, p);
		CPlayerPackets::SendPlayerStreamIn(p, player);
	}

	auto actuallyStreamedFor = player->GetStreamedFor();
	for (auto p : actuallyStreamedFor)
	{
		if (std::find(streamedFor.begin(), streamedFor.end(), p) == streamedFor.end())
		{
			CPlayerPackets::SendPlayerStreamOut(player, p);
			CPlayerPackets::SendPlayerStreamOut(p, player);
		}
	}
}

void CPlayerPackets::SendMassivePacketForPlayer(CPlayer* player)
{
	size_t dataSize;
	auto streamedFor = CPlayerManager::GetStreamedInPlayers(player);
	if (streamedFor.size() == 0) return;

	std::vector<CPackets::PlayerUpdatePacket> playersUpdate;
	std::vector<CPackets::PlayerAimPacket> playersAim;

	for (auto p : streamedFor)
	{
		if (p->m_bIsAiming)
		{
			CPackets::PlayerAimPacket packet = p->BuildAimPacket();
			playersAim.push_back(packet);
		}
		else
		{
			CPackets::PlayerUpdatePacket packet = p->BuildUpdatePacket();
			playersUpdate.push_back(packet);
		}
	}

	CNetworking::SendMassiveData(player, playersUpdate, CPackets::MessageId::MASSIVE_PLAYER_UPDATE);
	CNetworking::SendMassiveData(player, playersAim, CPackets::MessageId::MASSIVE_PLAYER_AIM);
}

void CPlayerPackets::SendPlayersUpdate()
{
	auto players = CPlayerManager::GetPlayers();
	for (auto player : players)
	{
		CPlayerPackets::UpdateStreaming(player);
	}

	for (auto player : players)
	{
		CPlayerPackets::SendMassivePacketForPlayer(player);
	}
}

void CPlayerPackets::Init()
{
	CNetworking::RegisterListener(CPackets::MessageId::HANDSHAKE, CPlayerPackets::HandshakePacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_UPDATE, CPlayerPackets::PlayerUpdatePacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_AIM, CPlayerPackets::PlayerAimPacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_TASK, CPlayerPackets::PlayerTaskPacket);

	CTimers::CreateTimer((int)(1000 / CCore::tickRate), 0, CPlayerPackets::SendPlayersUpdate);
}