#pragma once

class CPlayer {
private:
    char m_szName[MAX_NAME];
    std::vector<CPlayer*> m_streamedFor;

public:
    int m_iID;
    ENetPeer* m_pPeer = nullptr;
    CPackets::PlayerUpdateData m_updateData;

    CPlayer(int id, const char* name, ENetPeer* peer);
    ~CPlayer();

    const char* GetName() const;
    void SetName(const char* newName);

    bool IsStreamedIn(CVector3 position);
    bool IsStreamedFor(CPlayer* player);
    void AddStreamedFor(CPlayer* player);
    void RemoveStreamedFor(CPlayer* player);
    std::vector<CPlayer*> GetStreamedFor();

    void SetPosition(CVector3 position) { this->m_updateData.m_vCurrentPosition = position; }
    CVector3 GetPosition() { return this->m_updateData.m_vCurrentPosition; }

    CPackets::PlayerUpdatePacket BuildUpdatePacket();
};