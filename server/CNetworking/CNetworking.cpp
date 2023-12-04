#include "../stdafx.h"

ENetHost* CNetworking::m_pServer = nullptr;
std::vector<CPacketListener*> CNetworking::m_listeners = std::vector<CPacketListener*>();

template void CNetworking::SendMassiveData<CPackets::PlayerAimPacket>(CPlayer*, const std::vector<CPackets::PlayerAimPacket>&, CPackets::MessageId);
template void CNetworking::SendMassiveData<CPackets::PlayerUpdatePacket>(CPlayer*, const std::vector<CPackets::PlayerUpdatePacket>&, CPackets::MessageId);

void CNetworking::SendPacket(CPackets::MessageId messageId, ENetPeer* peer, void* packet, int size)
{
    char* buffer = new char[size + sizeof(CPackets::MessageId)];
    memcpy(buffer, &messageId, sizeof(CPackets::MessageId));
    memcpy(buffer + sizeof(CPackets::MessageId), packet, size);

    ENetPacket* enetPacket = enet_packet_create(buffer, size + sizeof(CPackets::MessageId), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, enetPacket);
}

void CNetworking::SendPacket(CPackets::MessageId messageId, CPlayer* player, void* packet, int size)
{
	CNetworking::SendPacket(messageId, player->m_pPeer, packet, size);
}

void CNetworking::BroadcastPacket(CPackets::MessageId messageId, void* packet, int size)
{
    auto players = CPlayerManager::GetPlayers();
    for (auto player : players)
    {
        CNetworking::SendPacket(messageId, player->m_pPeer, packet, size);
    }
}

void CNetworking::BroadcastPacket(CPackets::MessageId messageId, ENetPeer* exclude, void* packet, int size)
{
    auto players = CPlayerManager::GetPlayers();
    for (auto player : players)
    {
        if (player->m_pPeer == exclude) continue;
        
        CNetworking::SendPacket(messageId, player->m_pPeer, packet, size);
    }
}

void CNetworking::BroadcastPacket(CPackets::MessageId messageId, CPlayer* exclude, void* packet, int size)
{
	return CNetworking::BroadcastPacket(messageId, exclude->m_pPeer, packet, size);
}

void CNetworking::ClientDisconnect(ENetPeer* peer)
{
    CPlayer* player = CPlayerManager::GetPlayer(peer);
    if (player == nullptr) return;

    CPlayerPackets::SendPlayerDisconnected(player);
    CPlayerManager::PlayerDisconnected(player);

	enet_peer_disconnect(peer, 0);
}

template<typename T>
void CNetworking::SendMassiveData(CPlayer* player, const std::vector<T>& data, CPackets::MessageId messageId)
{
    size_t size = 0;
    char* serializedData = CMassSerializer::SerializeMessage(data.size(), data, size);
    CNetworking::SendPacket(messageId, player, serializedData, size);
    delete[] serializedData;
}

void CNetworking::Init(int port)
{
    if (enet_initialize() != 0) return CConsole::Print("Error initializing ENet.");

    ENetAddress address;
    ENetHost* server;

    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == nullptr) {
        CConsole::Print("Error creating ENet server.");
        enet_deinitialize();
        return;
    }

    CConsole::Print("Server started on port %d", address.port);

    ENetEvent event;
    while (1)
    {
        if (enet_host_service(server, &event, 1) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    /*CConsole::Print("Client connected from %x:%u.",
                        event.peer->address.host,
                        event.peer->address.port);*/
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    HandlePacket(event.peer, event.packet->data, event.packet->dataLength);
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    CNetworking::ClientDisconnect(event.peer);
                    break;

                default:
                    break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
}

void CNetworking::RegisterListener(CPackets::MessageId messageId, void(*callback)(ENetPeer*, void*, int))
{
	CPacketListener* listener = new CPacketListener(messageId, callback);
	m_listeners.push_back(listener);
}

void CNetworking::UnregisterListener(CPackets::MessageId messageId, void(*callback)(ENetPeer*, void*, int)) {
    for (size_t i = 0; i < m_listeners.size(); i++) {
        if (m_listeners[i]->m_callback == callback && m_listeners[i]->m_messageId == messageId) {
            m_listeners.erase(m_listeners.begin() + i);
            return;
        }
    }
}

void CNetworking::HandlePacket(ENetPeer* peer, void* packet, int size)
{
    CPackets::MessageId messageId;
    memcpy(&messageId, packet, sizeof(CPackets::MessageId));

    char* buffer = new char[size - sizeof(CPackets::MessageId)];
    memcpy(buffer, (char*)packet + sizeof(CPackets::MessageId), size - sizeof(CPackets::MessageId));

    for (size_t i = 0; i < m_listeners.size(); i++) {
        if (m_listeners[i]->m_messageId == messageId) {
			m_listeners[i]->m_callback(peer, buffer, size - sizeof(CPackets::MessageId));
		}
	}
}

void CNetworking::KickPeer(ENetPeer* peer, const char* reason)
{
	ENetPacket* packet = enet_packet_create(reason, strlen(reason) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
	enet_peer_disconnect(peer, 0);
}