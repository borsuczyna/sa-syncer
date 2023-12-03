#include "../../stdafx.h"

CPlayer::CPlayer(int id, const char* name)
{
	this->m_bIsStreamedIn = false;
	this->m_iID = id;
	this->SetName(name);
}

CPlayer::~CPlayer()
{
	this->m_iID = -1;
	this->m_szName[0] = '\0';

	this->StreamOut();
}

void CPlayer::SetPed(CPlayerPed* ped)
{
	this->m_pPed = ped;
	this->m_bIsStreamedIn = true;
}

CPlayerPed* CPlayer::GetPed() const
{
	if (!this->m_bIsStreamedIn) return nullptr;
	return this->m_pPed;
}

const char* CPlayer::GetName() const
{
	return this->m_szName;
}

void CPlayer::SetName(const char* name)
{
	strcpy_s(this->m_szName, name);
}

CVector3 CPlayer::GetPosition() const
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return CVector3();

	CVector position = ped->GetPosition();
	return CVector3(position.x, position.y, position.z);
}

void CPlayer::SetPosition(CVector3 position)
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	ped->m_matrix->pos = CVector(position.x, position.y, position.z);
}

CVector3 CPlayer::GetVelocity() const
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return CVector3();

	CVector velocity = ped->m_vecMoveSpeed;
	return CVector3(velocity.x, velocity.y, velocity.z);
}

void CPlayer::SetVelocity(CVector3 velocity)
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	ped->m_vecMoveSpeed = CVector(velocity.x, velocity.y, velocity.z);
}

bool CPlayer::IsDucked() const
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return false;

	CTaskSimpleDuck* duckTask = ped->m_pIntelligence->GetTaskDuck(true);
	if (duckTask == nullptr) return false;
	if (duckTask->m_bIsFinished || duckTask->m_bIsAborting) return false;

	return true;
}

void CPlayer::SetDucked(bool isDucked)
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;
	if (this->IsDucked() == isDucked) return;

	CTaskSimpleDuckToggle duckToggle(isDucked ? 1 : 0);
	duckToggle.ProcessPed(ped);
}

void CPlayer::GiveWeapon(eWeaponType weaponType, unsigned int ammo, bool armed)
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	CWeaponInfo* info = CWeaponInfo::GetWeaponInfo(weaponType, 1);

	bool requestModelSuccess = true;
	if (info->m_nModelId1 > 0) requestModelSuccess = CModelManager::LoadModel(info->m_nModelId1);
	if (info->m_nModelId2 > 0 && requestModelSuccess) requestModelSuccess = CModelManager::LoadModel(info->m_nModelId2);
	if (!requestModelSuccess) return;

	ped->GiveWeapon(weaponType, ammo, false);
	ped->SetCurrentWeapon(weaponType);
}

void CPlayer::SetWeapon(eWeaponType weaponType, unsigned int ammoInClip, unsigned int ammoTotal)
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	if (ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType != weaponType)
	{
		if (weaponType != 0) this->GiveWeapon(weaponType, 1000, true);
		else ped->SetCurrentWeapon(weaponType);
	}

	CWeapon* weapon = &ped->m_aWeapons[ped->m_nActiveWeaponSlot];
	if (weapon->m_eWeaponType == weaponType && weapon->m_nAmmoInClip > 0)
	{
		weapon->m_nAmmoInClip = ammoInClip;
		weapon->m_nTotalAmmo = ammoTotal;
	}
}

void CPlayer::StreamIn(PlayerUpdateData data, bool isDucked)
{
	if (this->m_bIsStreamedIn) return;

	CPlayerPed* ped = CPlayerManager::CreatePlayerPed(this->m_iID, this->m_iSkin, data.m_vCurrentPosition);
	if (ped == nullptr) return;

	this->SetPed(ped);
	ped->m_fCurrentRotation = data.m_fAimingRotation;
	ped->m_fAimingRotation = data.m_fAimingRotation;
	this->m_bIsStreamedIn = true;

	this->Update(data);
	this->SetDucked(isDucked);
}

void CPlayer::StreamOut()
{
	if (!this->m_bIsStreamedIn) return;

	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	CPlayerManager::DeletePlayerPed(ped);
	this->m_bIsStreamedIn = false;
}

float CPlayer::GetInterpolationTime()
{
	float fTime = (GetTickCount64() - this->m_lastUpdateTick) / (float)(1000.0f / CCore::m_iTickRate);

	fTime = std::clamp(fTime, 0.0f, 2.0f); // let's clamp it to max 2 ticks
	return fTime;
}

CPackets::PlayerUpdatePacket CPlayer::BuildUpdatePacket()
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return CPackets::PlayerUpdatePacket();

	CPad* pad = ped->GetPadFromPlayer();
	CControllerState state = pad->NewState;
	CPlayerControls controls{
		state.LeftStickX,
		state.LeftStickY,
		state.RightShoulder1,
		state.m_bPedWalk,
		state.ButtonCircle,
		state.ButtonCross,
		state.ButtonSquare,
		state.ButtonTriangle,
	};

	CWeapon activeWeapon = ped->m_aWeapons[ped->m_nActiveWeaponSlot];

	CPackets::PlayerUpdatePacket packet(this->m_iID);
	PlayerUpdateData data{
		this->GetPosition(),					// m_vCurrentPosition
		this->GetVelocity(),					// m_vMoveSpeed
		TheCamera.m_fOrientation,				// m_fCameraOrientation
		ped->m_fAimingRotation,					// m_fAimingRotation
		ped->m_pPlayerData->m_fMoveBlendRatio,	// m_fMoveBlendRatio
		ped->m_nMoveState,						// m_iMoveState

		activeWeapon.m_eWeaponType,				// m_iCurrentWeapon
		activeWeapon.m_nAmmoInClip,				// m_iAmmoInClip
		activeWeapon.m_nTotalAmmo,				// m_iAmmoTotal

		controls								// m_controls
	};

	packet.data = data;

	return packet;
}

void CPlayer::HandleTask(CPackets::PlayerTaskPacket* packet)
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	eTaskType activeTask = ped->m_pIntelligence->m_TaskMgr.GetActiveTask()->GetId();
	switch (packet->taskType)
    {
        case eTaskType::TASK_COMPLEX_JUMP:
        {
            if (activeTask == eTaskType::TASK_COMPLEX_JUMP) break;
            if (this->IsDucked()) this->SetDucked(false);

            CVector3 pos = packet->taskData.position;
            ped->m_matrix->pos = CVector(pos.x, pos.y, pos.z);
            ped->m_fCurrentRotation = packet->taskData.rotation;
            ped->m_pIntelligence->m_TaskMgr.SetTask(new CTaskComplexJump(0), 3, false);

            break;
        }

        case eTaskType::TASK_SIMPLE_DUCK:
        {
            this->SetDucked(packet->taskData.toggle);
            break;
        }

        default:
        {
            printf("Unknown task received %d", packet->taskType);
            break;
        }
    }
}

void CPlayer::Update(PlayerUpdateData data)
{
	this->m_lastUpdateData = this->m_updateData;
	this->m_updateData = data;
	this->m_bUpdateDataAvailable = true;
	this->m_lastUpdateTick = GetTickCount64();

	this->SetWeapon((eWeaponType)data.m_iCurrentWeapon, data.m_iAmmoInClip, data.m_iAmmoTotal);
}

void CPlayer::Process()
{
	if (!this->m_bUpdateDataAvailable) return;

	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	eTaskType activeTask = ped->m_pIntelligence->m_TaskMgr.GetActiveTask()->GetId();

	float interpolation = this->GetInterpolationTime();
	auto lData = this->m_lastUpdateData;
	auto nData = this->m_updateData;

	this->SetPosition(CInterpolate::Value(lData.m_vCurrentPosition, nData.m_vCurrentPosition, interpolation));

	// dont apply velocity when player is jumping
	if (activeTask != eTaskType::TASK_COMPLEX_JUMP)
		this->SetVelocity(CInterpolate::Value(lData.m_vMoveSpeed, nData.m_vMoveSpeed, interpolation));

	// update aiming rotation only when difference is too big, otherwise player controls will handle it
	float aimingRotation = CInterpolate::Angle(lData.m_fAimingRotation, nData.m_fAimingRotation, interpolation);
	float difference = std::abs(CInterpolate::AngleDifference(ped->m_fAimingRotation, aimingRotation));
	if (difference > 45) ped->m_fAimingRotation = aimingRotation;
}