#pragma once

class CPlayerPackets
{
public:
    static void HandshakeResponsePacket(void* p, int size);
    static void PlayerConnectedPacket(void* p, int size);
    static void PlayerDisconnectedPacket(void* p, int size);
    static void PlayerStreamInPacket(void* p, int size);
    static void PlayerStreamOutPacket(void* p, int size);
    static void PlayerTaskPacket(void* p, int size);
    static void MassivePlayerUpdatePacket(void* p, int size);
    static void MassivePlayerAimPacket(void* p, int size);

    static void SendPlayerTaskPacket(int task, TaskData data);
    static void SendPlayerUpdatePacket();

    static void InitPlayerUpdateTimer();
    static void DestroyPlayerUpdateTimer();

    static void Init();
};