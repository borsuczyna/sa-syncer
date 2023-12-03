#include "../stdafx.h"

void CPlayerPackets::HandshakePacket(ENetPeer* peer, void* p, int size)
{
	if (size != sizeof(CPackets::HandshakePacket)) return;
	CPackets::HandshakePacket* packet = (CPackets::HandshakePacket*)p;
	if (packet->id != CPackets::MessageId::HANDSHAKE) return;

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
		CPlayerManager::KickPlayer(player, "Player with this nickname is already connected");
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
	CNetworking::SendPacket(newPlayer, &response, sizeof(CPackets::HandshakeResponsePacket));

	CPlayerPackets::SendPlayerConnected(newPlayer);
	CPlayerPackets::SendConnectedPlayers(newPlayer);
	CPlayerPackets::UpdateStreaming(newPlayer);
}

void CPlayerPackets::PlayerUpdatePacket(ENetPeer* peer, void* p, int size)
{
	if (size != sizeof(CPackets::PlayerUpdatePacket)) return;
	CPackets::PlayerUpdatePacket* packet = (CPackets::PlayerUpdatePacket*)p;
	if (packet->id != CPackets::MessageId::PLAYER_UPDATE) return;

	CPlayer* player = CPlayerManager::GetPlayer(peer);
	if (player == nullptr) return;

	player->m_updateData = packet->data;
}

void CPlayerPackets::SendPlayerConnected(CPlayer* player)
{
	CPackets::PlayerConnectedPacket packet = CPackets::PlayerConnectedPacket(player->m_iID, player->GetName());
	CNetworking::BroadcastPacket(player, &packet, sizeof(CPackets::PlayerConnectedPacket));
}

void CPlayerPackets::SendPlayerConnected(CPlayer* player, CPlayer* target)
{
	CPackets::PlayerConnectedPacket packet = CPackets::PlayerConnectedPacket(player->m_iID, player->GetName());
	CNetworking::SendPacket(target, &packet, sizeof(CPackets::PlayerConnectedPacket));
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
	CNetworking::BroadcastPacket(player, &packet, sizeof(CPackets::PlayerDisconnectedPacket));
}

void CPlayerPackets::SendPlayerStreamIn(CPlayer* player, CPlayer* target)
{
	if (player->IsStreamedFor(target)) return;

	player->AddStreamedFor(target);
	CPackets::PlayerStreamInPacket packet = CPackets::PlayerStreamInPacket(player->m_iID, player->GetPosition());
	CNetworking::SendPacket(target, &packet, sizeof(CPackets::PlayerStreamInPacket));
}

void CPlayerPackets::SendPlayerStreamOut(CPlayer* player, CPlayer* target)
{
	if (!player->IsStreamedFor(target)) return;

	player->RemoveStreamedFor(target);
	CPackets::PlayerStreamOutPacket packet = CPackets::PlayerStreamOutPacket(player->m_iID);
	CNetworking::SendPacket(target, &packet, sizeof(CPackets::PlayerStreamOutPacket));
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

CPackets::MassivePlayerUpdatePacket CPlayerPackets::BuildMassivePacketForPlayer(CPlayer* player)
{
	auto streamedFor = CPlayerManager::GetStreamedInPlayers(player);
	CPackets::MassivePlayerUpdatePacket packet = CPackets::MassivePlayerUpdatePacket(streamedFor.size());

	for (size_t i = 0; i < streamedFor.size(); i++)
	{
		packet.players[i] = streamedFor[i]->BuildUpdatePacket();
	}

	return packet;
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
		CPackets::MassivePlayerUpdatePacket packet = CPlayerPackets::BuildMassivePacketForPlayer(player);
		CNetworking::SendPacket(player, &packet, sizeof(CPackets::MassivePlayerUpdatePacket));
	}
}

void CPlayerPackets::Init()
{
	CNetworking::RegisterListener(CPlayerPackets::HandshakePacket);
	CNetworking::RegisterListener(CPlayerPackets::PlayerUpdatePacket);

	CTimers::CreateTimer((int)(1000 / CCore::tickRate), 0, CPlayerPackets::SendPlayersUpdate);
}