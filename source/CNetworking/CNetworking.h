#pragma once

class CPacketListener
{
public:
	CPackets::MessageId m_messageId;
	void(*m_callback)(void*, int);

	CPacketListener(CPackets::MessageId messageId, void(*callback)(void*, int))
	{
		m_messageId = messageId;
		m_callback = callback;
	}
};

class CNetworking
{
	static std::vector<CPacketListener*> m_listeners;
	static ENetHost* m_pClient;

public:
	static bool m_bIsConnecting;
	static bool m_bIsConnected;

	static void Connect(int port);
	static void InitAsync(int port);
	static void SendPacket(CPackets::MessageId messageId, void* packet, int size);

	static void RegisterListener(CPackets::MessageId messageId, void(*callback)(void*, int));
	static void UnregisterListener(CPackets::MessageId messageId, void(*callback)(void*, int));

	static void HandlePacket(void* packet, int size);
};