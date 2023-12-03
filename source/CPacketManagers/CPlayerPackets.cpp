#include "../stdafx.h"

void CPlayerPackets::HandshakeResponsePacket(void* p, int size)
{
	if (size != sizeof(CPackets::HandshakeResponsePacket)) return;
	CPackets::HandshakeResponsePacket* packet = (CPackets::HandshakeResponsePacket*)p;
	if (packet->id != CPackets::MessageId::HANDSHAKE_RESPONSE) return;

	CConsole::Print("Connected to the server! My ID is %d", packet->playerId);
	CPlayerManager::GetLocalPlayer()->m_iID = packet->playerId;

	CCore::SetTickRate(packet->tickRate);
}

void CPlayerPackets::PlayerConnectedPacket(void* p, int size)
{
	if (size != sizeof(CPackets::PlayerConnectedPacket)) return;
	CPackets::PlayerConnectedPacket* packet = (CPackets::PlayerConnectedPacket*)p;
	if (packet->id != CPackets::MessageId::PLAYER_CONNECTED) return;

	CConsole::Print("%s (%d) connected to the server!", packet->nickname, packet->playerId);

	CPlayer* player = new CPlayer(packet->playerId, packet->nickname);
	CPlayerManager::AddPlayer(player);
}

void CPlayerPackets::PlayerDisconnectedPacket(void* p, int size)
{
	if (size != sizeof(CPackets::PlayerDisconnectedPacket)) return;
	CPackets::PlayerDisconnectedPacket* packet = (CPackets::PlayerDisconnectedPacket*)p;
	if (packet->id != CPackets::MessageId::PLAYER_DISCONNECTED) return;

	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == nullptr) return;

	CConsole::Print("%s (%d) disconnected from the server!", player->GetName(), player->m_iID);
	
	CPlayerManager::RemovePlayer(player);
	delete player;
}

void CPlayerPackets::PlayerStreamInPacket(void* p, int size)
{
	if (size != sizeof(CPackets::PlayerStreamInPacket)) return;
	CPackets::PlayerStreamInPacket* packet = (CPackets::PlayerStreamInPacket*)p;
	if (packet->id != CPackets::MessageId::PLAYER_STREAM_IN) return;

	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == nullptr) return;

	CConsole::Print("%s (%d) streamed in!", player->GetName(), player->m_iID);
	player->StreamIn(packet->position);
}

void CPlayerPackets::PlayerStreamOutPacket(void* p, int size)
{
	if (size != sizeof(CPackets::PlayerStreamOutPacket)) return;
	CPackets::PlayerStreamOutPacket* packet = (CPackets::PlayerStreamOutPacket*)p;
	if (packet->id != CPackets::MessageId::PLAYER_STREAM_OUT) return;

	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == nullptr) return;

	CConsole::Print("%s (%d) streamed out!", player->GetName(), player->m_iID);
	player->StreamOut();
}

void CPlayerPackets::MassivePlayerUpdatePacket(void* p, int size)
{
	if (size != sizeof(CPackets::MassivePlayerUpdatePacket)) return;
	CPackets::MassivePlayerUpdatePacket* packet = (CPackets::MassivePlayerUpdatePacket*)p;
	if (packet->id != CPackets::MessageId::MASSIVE_PLAYER_UPDATE) return;

	for (size_t i = 0; i < packet->playerCount; i++)
	{
		CPlayer* player = CPlayerManager::GetPlayer(packet->players[i].playerId);
		if (player == nullptr) continue;

		player->Update(packet->players[i]);
	}
}

void CPlayerPackets::SendPlayerUpdatePacket()
{
	if (!CNetworking::m_bIsConnected) return;

	CPlayer* player = CPlayerManager::GetLocalPlayer();
	if (player == nullptr) return;

	CPackets::PlayerUpdatePacket packet = player->BuildUpdatePacket();
	CNetworking::SendPacket(&packet, sizeof(CPackets::PlayerUpdatePacket));
}

void CPlayerPackets::InitPlayerUpdateTimer()
{
	CTimers::KillTimer(CPlayerPackets::SendPlayerUpdatePacket);
	CTimers::CreateTimer((int)(1000 / CCore::m_iTickRate), 0, CPlayerPackets::SendPlayerUpdatePacket);
}

void CPlayerPackets::DestroyPlayerUpdateTimer()
{
	CTimers::KillTimer(CPlayerPackets::SendPlayerUpdatePacket);
}

void CPlayerPackets::Init()
{
	CNetworking::RegisterListener(CPlayerPackets::HandshakeResponsePacket);
	CNetworking::RegisterListener(CPlayerPackets::PlayerConnectedPacket);
	CNetworking::RegisterListener(CPlayerPackets::PlayerDisconnectedPacket);
	CNetworking::RegisterListener(CPlayerPackets::PlayerStreamInPacket);
	CNetworking::RegisterListener(CPlayerPackets::PlayerStreamOutPacket);
	CNetworking::RegisterListener(CPlayerPackets::MassivePlayerUpdatePacket);
}