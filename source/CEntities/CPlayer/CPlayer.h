#pragma once

class CPlayer
{
public:
	bool m_bIsStreamedIn;
	int m_iID = -1;
	int m_iSkin = 0;
	char m_szName[MAX_NAME];
	CPlayerPed* m_pPed = nullptr;

	CPlayer(int id, const char* name);
	~CPlayer();

	CPlayerPed* GetPed() const;
	void SetPed(CPlayerPed* ped);

	CVector3 GetPosition() const;
	void SetPosition(CVector3 position);

	const char* GetName() const;
	void SetName(const char* name);

	void StreamIn(CVector3 position);
	void StreamOut();

	CPackets::PlayerUpdatePacket BuildUpdatePacket();
	void Update(CPackets::PlayerUpdatePacket packet);
};