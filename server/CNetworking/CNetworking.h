#pragma once

class CPacketListener
{
public:
	CPackets::MessageId m_messageId;
	void(*m_callback)(ENetPeer*, void*, int);

	CPacketListener(CPackets::MessageId messageId, void(*callback)(ENetPeer*, void*, int))
	{
		m_messageId = messageId;
		m_callback = callback;
	}
};

class CNetworking
{
	static std::vector<CPacketListener*> m_listeners;
	static ENetHost* m_pServer;

public:
	static void Init(int port);
	static void SendPacket(CPackets::MessageId messageId, ENetPeer* peer, void* packet, int size);
	static void SendPacket(CPackets::MessageId messageId, CPlayer* player, void* packet, int size);
	static void BroadcastPacket(CPackets::MessageId messageId, void* packet, int size);
	static void BroadcastPacket(CPackets::MessageId messageId, ENetPeer* exclude, void* packet, int size);
	static void BroadcastPacket(CPackets::MessageId messageId, CPlayer* exclude, void* packet, int size);
	static void ClientDisconnect(ENetPeer* peer);

	template<typename T>
	static void SendMassiveData(CPlayer* player, const std::vector<T>& data, CPackets::MessageId messageId);

	static void RegisterListener(CPackets::MessageId messageId, void(*callback)(ENetPeer*, void*, int));
	static void UnregisterListener(CPackets::MessageId messageId, void(*callback)(ENetPeer*, void*, int));

	static void HandlePacket(ENetPeer* peer, void* packet, int size);

	static void KickPeer(ENetPeer* peer, const char* reason);
};