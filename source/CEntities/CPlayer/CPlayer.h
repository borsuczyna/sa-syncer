#pragma once

class CPlayer
{
	CPlayerPed* m_pPed = nullptr;
	char m_szName[MAX_NAME];

public:
	CPackets::PlayerUpdateData m_updateData = {};
	CPackets::PlayerUpdateData m_lastUpdateData = {};
	bool m_bUpdateDataAvailable = false;
	ULONGLONG m_lastUpdateTick = 0;

	bool m_bIsStreamedIn;
	int m_iID = -1;
	int m_iSkin = 0;

	CPlayer(int id, const char* name);
	~CPlayer();

	CPlayerPed* GetPed() const;
	void SetPed(CPlayerPed* ped);

	CVector3 GetPosition() const;
	void SetPosition(CVector3 position);
	CVector3 GetVelocity() const;
	void SetVelocity(CVector3 velocity);

	const char* GetName() const;
	void SetName(const char* name);

	void StreamIn(CVector3 position);
	void StreamOut();

	float GetInterpolationTime();
	void Process();
	CPackets::PlayerUpdatePacket BuildUpdatePacket();
	void Update(CPackets::PlayerUpdatePacket packet);
};