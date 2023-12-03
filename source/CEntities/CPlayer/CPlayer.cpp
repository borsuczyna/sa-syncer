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

CPackets::PlayerUpdatePacket CPlayer::BuildUpdatePacket()
{
	CPackets::PlayerUpdatePacket packet(this->m_iID);
	CPackets::PlayerUpdateData data{
		this->GetPosition(),			// m_vCurrentPosition
	};

	packet.data = data;

	return packet;
}

void CPlayer::Update(CPackets::PlayerUpdatePacket packet)
{
	this->SetPosition(packet.data.m_vCurrentPosition);
}