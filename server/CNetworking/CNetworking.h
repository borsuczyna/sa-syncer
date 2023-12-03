#pragma once

class CPacketListener
{
public:
	void(*m_callback)(ENetPeer*, void*, int);

	CPacketListener(void(*callback)(ENetPeer*, void*, int))
	{
		m_callback = callback;
	}
};

class CNetworking
{
	static std::vector<CPacketListener*> m_listeners;
	static ENetHost* m_pServer;

public:
	static void Init(int port);
	static void SendPacket(ENetPeer* peer, void* packet, int size);
	static void SendPacket(CPlayer* player, void* packet, int size);
	static void BroadcastPacket(void* packet, int size);
	static void BroadcastPacket(ENetPeer* exclude, void* packet, int size);
	static void BroadcastPacket(CPlayer* exclude, void* packet, int size);
	static void ClientDisconnect(ENetPeer* peer);

	static void RegisterListener(void(*callback)(ENetPeer*, void*, int));
	static void UnregisterListener(void(*callback)(ENetPeer*, void*, int));

	static void HandlePacket(ENetPeer* peer, void* packet, int size);

	static void KickPeer(ENetPeer* peer, const char* reason);
};