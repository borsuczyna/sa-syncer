#include "../stdafx.h"

bool CNetworking::m_bIsConnecting = false;
bool CNetworking::m_bIsConnected = false;

ENetHost* CNetworking::m_pClient = nullptr;
std::vector<CPacketListener*> CNetworking::m_listeners = std::vector<CPacketListener*>();

void CNetworking::SendPacket(CPackets::MessageId messageId, void* packet, int size)
{
	if (!CNetworking::m_bIsConnected) return;

	char* buffer = new char[size + sizeof(CPackets::MessageId)];
	memcpy(buffer, &messageId, sizeof(CPackets::MessageId));
	memcpy(buffer + sizeof(CPackets::MessageId), packet, size);

	ENetPacket* enetPacket = enet_packet_create(buffer, size + sizeof(CPackets::MessageId), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(m_pClient, 0, enetPacket);
}

void CNetworking::InitAsync(int port)
{
	if (enet_initialize() != 0) return CConsole::Print("Failed to initialize ENet!");
	CConsole::Print("Connecting to the server...");

	ENetAddress address;

	CNetworking::m_bIsConnecting = true;
	m_pClient = enet_host_create(nullptr, 1, 2, 0, 0);
	if (m_pClient == nullptr) {
		CNetworking::m_bIsConnecting = false;
		return CConsole::Print("Failed to create ENet client!");
	}

	enet_address_set_host(&address, "localhost");
	address.port = port;

	ENetPeer* peer = enet_host_connect(m_pClient, &address, 2, 0);
	if (peer == nullptr) {
		CNetworking::m_bIsConnecting = false;
		return CConsole::Print("Failed to connect to server!");
	}

	ENetEvent event;

	if (enet_host_service(m_pClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		enet_host_flush(m_pClient);
	}
	else
	{
		CNetworking::m_bIsConnecting = false;
		enet_peer_reset(peer);
		CConsole::Print("Failed to connect to server!");
		return;
	}

	CNetworking::m_bIsConnecting = false;
	CNetworking::m_bIsConnected = true;
	CCore::m_ulServerConnectTime = time(0);
	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();
	CPackets::HandshakePacket packet = CPackets::HandshakePacket(localPlayer->GetName(), localPlayer->GetPosition());
	SendPacket(CPackets::MessageId::HANDSHAKE, &packet, sizeof(CPackets::HandshakePacket));

	while (1)
	{
		if (enet_host_service(m_pClient, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				HandlePacket(event.packet->data, event.packet->dataLength);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				CConsole::Print("Disconnected from server!");
				break;
			}
		}
	}
}

void CNetworking::Connect(int port)
{
	if (CPlayerManager::GetLocalPlayer() == nullptr) return CConsole::Print("You must be in game to connect to the server!");
	if (CNetworking::m_bIsConnecting) return CConsole::Print("You are already connecting to the server!");
	if (CNetworking::m_bIsConnected) return CConsole::Print("You are already connected to the server!");

	std::thread t(InitAsync, port);
	t.detach();
}

void CNetworking::RegisterListener(CPackets::MessageId messageId, void(*callback)(void*, int)) {
	CPacketListener* listener = new CPacketListener(messageId, callback);
	m_listeners.push_back(listener);
}

void CNetworking::UnregisterListener(CPackets::MessageId messageId, void(*callback)(void*, int)) {
	for (size_t i = 0; i < m_listeners.size(); i++) {
		if (m_listeners[i]->m_callback == callback && m_listeners[i]->m_messageId == messageId) {
			m_listeners.erase(m_listeners.begin() + i);
			return;
		}
	}
}

void CNetworking::HandlePacket(void* packet, int size)
{
	CPackets::MessageId messageId;
	memcpy(&messageId, packet, sizeof(CPackets::MessageId));

	char* buffer = new char[size - sizeof(CPackets::MessageId)];
	memcpy(buffer, (char*)packet + sizeof(CPackets::MessageId), size - sizeof(CPackets::MessageId));

	for (size_t i = 0; i < m_listeners.size(); i++) {
		if (m_listeners[i]->m_messageId == messageId) {
			m_listeners[i]->m_callback(buffer, size - sizeof(CPackets::MessageId));
		}
	}
}