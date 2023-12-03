#include "../stdafx.h"

ENetHost* CNetworking::m_pServer = nullptr;
std::vector<CPacketListener*> CNetworking::m_listeners = std::vector<CPacketListener*>();

void CNetworking::SendPacket(ENetPeer* peer, void* packet, int size)
{
	ENetPacket* enetPacket = enet_packet_create(packet, size, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, enetPacket);
}

void CNetworking::SendPacket(CPlayer* player, void* packet, int size)
{
	CNetworking::SendPacket(player->m_pPeer, packet, size);
}

void CNetworking::BroadcastPacket(void* packet, int size)
{
    auto players = CPlayerManager::GetPlayers();
    for (auto player : players)
    {
        CNetworking::SendPacket(player->m_pPeer, packet, size);
    }
}

void CNetworking::BroadcastPacket(ENetPeer* exclude, void* packet, int size)
{
    auto players = CPlayerManager::GetPlayers();
    for (auto player : players)
    {
        if (player->m_pPeer == exclude) continue;
        
        CNetworking::SendPacket(player->m_pPeer, packet, size);
    }
}

void CNetworking::BroadcastPacket(CPlayer* exclude, void* packet, int size)
{
	return CNetworking::BroadcastPacket(exclude->m_pPeer, packet, size);
}

void CNetworking::ClientDisconnect(ENetPeer* peer)
{
    CPlayer* player = CPlayerManager::GetPlayer(peer);
    if (player == nullptr) return;

    CPlayerPackets::SendPlayerDisconnected(player);
    CPlayerManager::PlayerDisconnected(player);

	enet_peer_disconnect(peer, 0);
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

void CNetworking::RegisterListener(void(*callback)(ENetPeer*, void*, int))
{
	CPacketListener* listener = new CPacketListener(callback);
	m_listeners.push_back(listener);
}

void CNetworking::UnregisterListener(void(*callback)(ENetPeer*, void*, int)) {
    for (size_t i = 0; i < m_listeners.size(); i++) {
        if (m_listeners[i]->m_callback == callback) {
            m_listeners.erase(m_listeners.begin() + i);
            return;
        }
    }
}

void CNetworking::HandlePacket(ENetPeer* peer, void* packet, int size)
{
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
	    m_listeners[i]->m_callback(peer, packet, size);
	}
}

void CNetworking::KickPeer(ENetPeer* peer, const char* reason)
{
	ENetPacket* packet = enet_packet_create(reason, strlen(reason) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
	enet_peer_disconnect(peer, 0);
}