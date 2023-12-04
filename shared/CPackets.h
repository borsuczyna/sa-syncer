#pragma once

#include "CShared.h"
#include "CVector3.h"
#include <string.h>

struct ServerInfo
{
	char name[MAX_SERVER_NAME] = {};
	int tickRate;
	int maxPlayers;
};

struct CPlayerControls
{
	short LeftStickX;
	short LeftStickY;
	short RightShoulder1;
	short m_bPedWalk;
	short ButtonCircle;
	short ButtonCross;
	short ButtonSquare;
	short ButtonTriangle;
};

struct PlayerUpdateData {
	CVector3 m_vCurrentPosition;
	CVector3 m_vMoveSpeed;
	float m_fCameraOrientation;
	float m_fAimingRotation;
	float m_fMoveBlendRatio;
	int m_iMoveState;

	unsigned int m_iCurrentWeapon;
	unsigned int m_iAmmoInClip;
	unsigned int m_iAmmoTotal;

	CPlayerControls m_controls;
};

struct TaskData
{
	TaskData() = default;

	CVector3 position;
	float rotation = 0.0f;
	bool toggle = false;
};

class CPackets
{
public:
    enum MessageId
	{
		HANDSHAKE = 0,
		HANDSHAKE_RESPONSE = 1,

		PLAYER_CONNECTED = 2,
		PLAYER_DISCONNECTED = 3,
		PLAYER_STREAM_IN = 4,
		PLAYER_STREAM_OUT = 5,

		PLAYER_UPDATE = 6,
		PLAYER_AIM = 7,
		PLAYER_TASK = 8,

		MASSIVE_PLAYER_UPDATE = 9,
		MASSIVE_PLAYER_AIM = 10,
	};

    struct HandshakePacket
	{
		HandshakePacket(const char* nickname, CVector3 position)
		{
			strcpy_s(this->nickname, nickname);
			strcpy_s(this->secret, SERVER_SECRET);
			this->position = position;
		}

		char nickname[MAX_NAME] = {};
		char secret[sizeof(SERVER_SECRET)] = {};
		CVector3 position = {};
	};

	struct HandshakeResponsePacket
	{
		HandshakeResponsePacket(int playerId, int tickRate, const char* serverName, int maxPlayers)
		{
			this->playerId = playerId;
			this->serverInfo.tickRate = tickRate;
			strcpy_s(this->serverInfo.name, serverName);
			this->serverInfo.maxPlayers = maxPlayers;
		}

		int playerId;
		ServerInfo serverInfo = {};
	};

	struct PlayerConnectedPacket
	{
		PlayerConnectedPacket(int playerId, const char* nickname)
		{
			this->playerId = playerId;
			strcpy_s(this->nickname, nickname);
		}

		int playerId;
		char nickname[MAX_NAME] = {};
	};

	struct PlayerDisconnectedPacket
	{
		PlayerDisconnectedPacket(int playerId)
		{
			this->playerId = playerId;
		}

		int playerId;
	};

	struct PlayerStreamInPacket
	{
		PlayerStreamInPacket(int playerId)
		{
			this->playerId = playerId;
		}

		int playerId;
		PlayerUpdateData data = {};
		bool isDucked = false;
	};

	struct PlayerStreamOutPacket
	{
		PlayerStreamOutPacket(int playerId)
		{
			this->playerId = playerId;
		}

		int playerId;
	};

	struct PlayerUpdatePacket
	{
		PlayerUpdatePacket() = default;
		PlayerUpdatePacket(int playerId)
		{
			this->playerId = playerId;
		}

		int playerId;
		PlayerUpdateData data = {};
	};

	struct PlayerAimPacket
	{
		PlayerAimPacket() = default;
		PlayerAimPacket(int playerId, CVector3 aimTarget)
		{
			this->playerId = playerId;
			this->aimTarget = aimTarget;
		}

		int playerId;
		PlayerUpdateData data = {};
		CVector3 aimTarget = {};
	};

	struct PlayerTaskPacket
	{
		PlayerTaskPacket(int taskType, TaskData taskData = {})
		{
			this->taskType = taskType;
			this->taskData = taskData;
		}

		int playerId;
		int taskType;
		TaskData taskData;
	};
};