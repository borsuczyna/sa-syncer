#pragma once

class CPlayerManager {
private:
    static std::vector<CPlayer*> m_players;

public:
    static void AddPlayer(CPlayer* player);
    static void RemovePlayer(CPlayer* player);
    static void KickPlayer(CPlayer* player, const char* reason);
    static void KickPlayer(ENetPeer* peer, const char* reason);
    static void PlayerDisconnected(CPlayer* player);
    
    static CPlayer* GetPlayer(const char* name);
    static CPlayer* GetPlayer(int id);
    static CPlayer* GetPlayer(ENetPeer* peer);
    static int GetFreeID();

    static std::vector<CPlayer*> GetStreamedInPlayers(CVector3 position);
    static std::vector<CPlayer*> GetStreamedInPlayers(CPlayer* player);

    static std::vector<CPlayer*> GetPlayers() { return m_players; }
};