#pragma once

class CPlayerPackets
{
public:
    static void HandshakePacket(ENetPeer* peer, void* p, int size);
    static void PlayerUpdatePacket(ENetPeer* peer, void* p, int size);
    static void PlayerTaskPacket(ENetPeer* peer, void* p, int size);

    static void SendPlayerConnected(CPlayer* player);
    static void SendPlayerConnected(CPlayer* player, CPlayer* target);
    static void SendConnectedPlayers(CPlayer* player);
    static void SendPlayerDisconnected(CPlayer* player);

    static void SendPlayerStreamIn(CPlayer* player, CPlayer* target);
    static void SendPlayerStreamOut(CPlayer* player, CPlayer* target);
    static void UpdateStreaming(CPlayer* player);

    static char* BuildMassivePacketForPlayer(CPlayer* player, size_t& size);
    static void SendPlayersUpdate();

    static void Init();
};