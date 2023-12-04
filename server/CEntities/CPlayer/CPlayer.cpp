#include "../../stdafx.h"

CPlayer::CPlayer(int id, const char* name, ENetPeer* peer)
{
	this->m_pPeer = peer;
	this->m_iID = id;
	this->SetName(name);
}

CPlayer::~CPlayer()
{
    this->m_pPeer = nullptr;
    this->m_iID = -1;
    this->m_szName[0] = '\0';
}

const char* CPlayer::GetName() const
{
    return this->m_szName;
}

void CPlayer::SetName(const char* newName)
{
    strncpy_s(this->m_szName, newName, sizeof(this->m_szName) - 1);
    this->m_szName[sizeof(this->m_szName) - 1] = '\0';
}

bool CPlayer::IsStreamedIn(CVector3 position)
{
	return (this->GetPosition().Distance(position) <= STREAM_DISTANCE);
}

bool CPlayer::IsStreamedFor(CPlayer* player)
{
	return (std::find(this->m_streamedFor.begin(), this->m_streamedFor.end(), player) != this->m_streamedFor.end());
}

void CPlayer::AddStreamedFor(CPlayer* player)
{
	this->m_streamedFor.push_back(player);
}

void CPlayer::RemoveStreamedFor(CPlayer* player)
{
	this->m_streamedFor.erase(std::remove(this->m_streamedFor.begin(), this->m_streamedFor.end(), player), this->m_streamedFor.end());
}

std::vector<CPlayer*> CPlayer::GetStreamedFor()
{
	return this->m_streamedFor;
}

CPackets::PlayerUpdatePacket CPlayer::BuildUpdatePacket()
{
	CPackets::PlayerUpdatePacket packet;
	packet.playerId = this->m_iID;
	packet.data = this->m_updateData;

	return packet;
}

CPackets::PlayerAimPacket CPlayer::BuildAimPacket()
{
	CPackets::PlayerAimPacket packet;
	packet.playerId = this->m_iID;
	packet.data = this->m_updateData;
	packet.aimTarget = this->m_vAimTarget;

	return packet;
}