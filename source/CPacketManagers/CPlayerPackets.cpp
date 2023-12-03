#include "../stdafx.h"

void CPlayerPackets::HandshakeResponsePacket(void* p, int size)
{
	CPackets::HandshakeResponsePacket* packet = (CPackets::HandshakeResponsePacket*)p;

	CCore::m_serverInfo = packet->serverInfo;
	CConsole::Print("Connected to the server %s! My ID is %d", packet->serverInfo.name, packet->playerId);
	CPlayerManager::GetLocalPlayer()->m_iID = packet->playerId;

	CCore::SetTickRate(packet->serverInfo.tickRate);
}

void CPlayerPackets::PlayerConnectedPacket(void* p, int size)
{
	CPackets::PlayerConnectedPacket* packet = (CPackets::PlayerConnectedPacket*)p;

	CPlayer* player = new CPlayer(packet->playerId, packet->nickname);
	CPlayerManager::AddPlayer(player);

	CConsole::Print("%s (%d) connected to the server!", packet->nickname, packet->playerId);
}

void CPlayerPackets::PlayerDisconnectedPacket(void* p, int size)
{
	CPackets::PlayerDisconnectedPacket* packet = (CPackets::PlayerDisconnectedPacket*)p;
	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == nullptr) return;

	CPlayerManager::RemovePlayer(player);
	delete player;
	
	CConsole::Print("%s (%d) disconnected from the server!", player->GetName(), player->m_iID);
}

void CPlayerPackets::PlayerStreamInPacket(void* p, int size)
{
	CPackets::PlayerStreamInPacket* packet = (CPackets::PlayerStreamInPacket*)p;
	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == nullptr) return;

	player->StreamIn(packet->data, packet->isDucked);
	CConsole::Print("%s (%d) streamed in!", player->GetName(), player->m_iID);
}

void CPlayerPackets::PlayerStreamOutPacket(void* p, int size)
{
	CPackets::PlayerStreamOutPacket* packet = (CPackets::PlayerStreamOutPacket*)p;
	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == nullptr) return;

	player->StreamOut();
	CConsole::Print("%s (%d) streamed out!", player->GetName(), player->m_iID);
}

void CPlayerPackets::PlayerTaskPacket(void* p, int size)
{
	CPackets::PlayerTaskPacket* packet = (CPackets::PlayerTaskPacket*)p;
	CPlayer* player = CPlayerManager::GetPlayer(packet->playerId);
	if (player == NULL) return;

	player->HandleTask(packet);
}

void CPlayerPackets::MassivePlayerUpdatePacket(void* p, int size)
{
	char* buffer = static_cast<char*>(p);

	size_t count;
	std::vector<CPackets::PlayerUpdatePacket> data = CMassSerializer::DeserializeMessage<CPackets::PlayerUpdatePacket>(buffer, count);

	for (auto item : data)
	{
		CPlayer* player = CPlayerManager::GetPlayer(item.playerId);
		if (player == nullptr) continue;

		player->Update(item.data);
	}
}

void CPlayerPackets::SendPlayerTaskPacket(int task, TaskData data)
{
	if (!CNetworking::m_bIsConnected) return;
	CPlayer* player = CPlayerManager::GetLocalPlayer();
	if (player == NULL) return;

	CPackets::PlayerTaskPacket packet = CPackets::PlayerTaskPacket(task, data);
	CNetworking::SendPacket(CPackets::MessageId::PLAYER_TASK, &packet, sizeof(CPackets::PlayerTaskPacket));
}

void CPlayerPackets::SendPlayerUpdatePacket()
{
	if (!CNetworking::m_bIsConnected) return;
	CPlayer* player = CPlayerManager::GetLocalPlayer();
	if (player == nullptr) return;

	CPackets::PlayerUpdatePacket packet = player->BuildUpdatePacket();
	CNetworking::SendPacket(CPackets::MessageId::PLAYER_UPDATE, &packet, sizeof(CPackets::PlayerUpdatePacket));
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
	CNetworking::RegisterListener(CPackets::MessageId::HANDSHAKE_RESPONSE, CPlayerPackets::HandshakeResponsePacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_CONNECTED, CPlayerPackets::PlayerConnectedPacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_DISCONNECTED, CPlayerPackets::PlayerDisconnectedPacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_STREAM_IN, CPlayerPackets::PlayerStreamInPacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_STREAM_OUT, CPlayerPackets::PlayerStreamOutPacket);
	CNetworking::RegisterListener(CPackets::MessageId::PLAYER_TASK, CPlayerPackets::PlayerTaskPacket);
	CNetworking::RegisterListener(CPackets::MessageId::MASSIVE_PLAYER_UPDATE, CPlayerPackets::MassivePlayerUpdatePacket);
}