#pragma once

class CPlayerPackets
{
public:
    static void HandshakePacket(ENetPeer* peer, void* p, int size);
    static void PlayerUpdatePacket(ENetPeer* peer, void* p, int size);
    static void PlayerAimPacket(ENetPeer* peer, void* p, int size);
    static void PlayerTaskPacket(ENetPeer* peer, void* p, int size);

    static void SendPlayerConnected(CPlayer* player);
    static void SendPlayerConnected(CPlayer* player, CPlayer* target);
    static void SendConnectedPlayers(CPlayer* player);
    static void SendPlayerDisconnected(CPlayer* player);

    static void SendPlayerStreamIn(CPlayer* player, CPlayer* target);
    static void SendPlayerStreamOut(CPlayer* player, CPlayer* target);
    static void UpdateStreaming(CPlayer* player);

    static void SendMassivePacketForPlayer(CPlayer* player);
    static void SendPlayersUpdate();

    static void Init();
};