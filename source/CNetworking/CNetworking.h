#pragma once

class CPacketListener
{
public:
	void(*m_callback)(void*, int);

	CPacketListener(void(*callback)(void*, int))
	{
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
	static void SendPacket(void* packet, int size);

	static void RegisterListener(void(*callback)(void*, int));
	static void UnregisterListener(void(*callback)(void*, int));

	static void HandlePacket(void* packet, int size);
};