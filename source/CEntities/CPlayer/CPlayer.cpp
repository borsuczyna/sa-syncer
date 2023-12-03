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

void CPlayer::StreamIn(CVector3 position)
{
	if (this->m_bIsStreamedIn) return;

	CPlayerPed* ped = CPlayerManager::CreatePlayerPed(this->m_iID, this->m_iSkin, position);
	if (ped == nullptr) return;

	this->SetPed(ped);
	this->m_bIsStreamedIn = true;
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

void CPlayer::Process()
{
	if (!this->m_bUpdateDataAvailable) return;

	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return;

	float interpolation = this->GetInterpolationTime();
	auto lData = this->m_lastUpdateData;
	auto nData = this->m_updateData;

	this->SetPosition(CInterpolate::Value(lData.m_vCurrentPosition, nData.m_vCurrentPosition, interpolation));
	this->SetVelocity(CInterpolate::Value(lData.m_vMoveSpeed, nData.m_vMoveSpeed, interpolation));
	
	// update aiming rotation only when difference is too big, otherwise player controls will handle it
	float aimingRotation = CInterpolate::Angle(lData.m_fAimingRotation, nData.m_fAimingRotation, interpolation);
	float difference = std::abs(CInterpolate::AngleDifference(ped->m_fAimingRotation, aimingRotation));
	if(difference > 45) ped->m_fAimingRotation = aimingRotation;
}

CPackets::PlayerUpdatePacket CPlayer::BuildUpdatePacket()
{
	CPlayerPed* ped = this->GetPed();
	if (ped == nullptr) return CPackets::PlayerUpdatePacket();

	CPad* pad = ped->GetPadFromPlayer();
	CControllerState state = pad->NewState;
	CPackets::CPlayerControls controls{
		state.LeftStickX,
		state.LeftStickY,
		state.RightShoulder1,
		state.m_bPedWalk,
		state.ButtonCircle,
		state.ButtonCross,
		state.ButtonSquare,
		state.ButtonTriangle,
	};

	CPackets::PlayerUpdatePacket packet(this->m_iID);
	CPackets::PlayerUpdateData data{
		this->GetPosition(),			// m_vCurrentPosition
		this->GetVelocity(),			// m_vMoveSpeed
		TheCamera.m_fOrientation,		// m_fCameraOrientation
		ped->m_fAimingRotation,			// m_fAimingRotation

		controls						// m_controls
	};

	packet.data = data;

	return packet;
}

void CPlayer::Update(CPackets::PlayerUpdatePacket packet)
{
	this->m_lastUpdateData = this->m_updateData;
	this->m_updateData = packet.data;
	this->m_bUpdateDataAvailable = true;
	this->m_lastUpdateTick = GetTickCount64();
}