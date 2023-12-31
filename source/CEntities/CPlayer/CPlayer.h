#pragma once

class CPlayer
{
	CPlayerPed* m_pPed = nullptr;
	char m_szName[MAX_NAME];
	bool m_bIsAiming = false;
	CVector3 m_lastAimAt;
	CVector3 m_vecAimAt;

public:
	PlayerUpdateData m_updateData = {};
	PlayerUpdateData m_lastUpdateData = {};
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
	bool IsDucked() const;
	void SetDucked(bool isDucked);
	void AimAt(CVector3 position);
	void StopAiming();
	void GiveWeapon(eWeaponType weaponType, unsigned int ammo, bool armed);
	void SetWeapon(eWeaponType weaponType, unsigned int ammoInClip, unsigned int ammoTotal);
	bool IsAiming() const;
	CVector3 GetAimTarget() const;

	const char* GetName() const;
	void SetName(const char* name);

	void StreamIn(PlayerUpdateData data, bool isDucked);
	void StreamOut();

	float GetInterpolationTime();
	CPackets::PlayerUpdatePacket BuildUpdatePacket();
	CPackets::PlayerAimPacket BuildAimPacket();
	void HandleTask(CPackets::PlayerTaskPacket* packet);
	void Update(PlayerUpdateData data);
	void UpdateAim(CVector3 aimTarget);
	void Process();
};