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
		PLAYER_TASK = 7,

		MASSIVE_PLAYER_UPDATE = 8,
	};

    struct Packet
	{
		MessageId id;
	};

    struct HandshakePacket : public Packet
	{
		HandshakePacket(const char* nickname, CVector3 position)
		{
			id = HANDSHAKE;
			strcpy_s(this->nickname, nickname);
			strcpy_s(this->secret, SERVER_SECRET);
			this->position = position;
		}

		char nickname[MAX_NAME] = {};
		char secret[sizeof(SERVER_SECRET)] = {};
		CVector3 position = {};
	};

	struct HandshakeResponsePacket : public Packet
	{
		HandshakeResponsePacket(int playerId, int tickRate, const char* serverName, int maxPlayers)
		{
			id = HANDSHAKE_RESPONSE;
			this->playerId = playerId;
			this->serverInfo.tickRate = tickRate;
			strcpy_s(this->serverInfo.name, serverName);
			this->serverInfo.maxPlayers = maxPlayers;
		}

		int playerId;
		ServerInfo serverInfo = {};
	};

	struct PlayerConnectedPacket : public Packet
	{
		PlayerConnectedPacket(int playerId, const char* nickname)
		{
			id = PLAYER_CONNECTED;
			this->playerId = playerId;
			strcpy_s(this->nickname, nickname);
		}

		int playerId;
		char nickname[MAX_NAME] = {};
	};

	struct PlayerDisconnectedPacket : public Packet
	{
		PlayerDisconnectedPacket(int playerId)
		{
			id = PLAYER_DISCONNECTED;
			this->playerId = playerId;
		}

		int playerId;
	};

	struct PlayerStreamInPacket : public Packet
	{
		PlayerStreamInPacket(int playerId)
		{
			id = PLAYER_STREAM_IN;
			this->playerId = playerId;
		}

		int playerId;
		PlayerUpdateData data = {};
		bool isDucked = false;
	};

	struct PlayerStreamOutPacket : public Packet
	{
		PlayerStreamOutPacket(int playerId)
		{
			id = PLAYER_STREAM_OUT;
			this->playerId = playerId;
		}

		int playerId;
	};

	struct PlayerUpdatePacket : public Packet
	{
		PlayerUpdatePacket() = default;

		PlayerUpdatePacket(int playerId)
		{
			id = PLAYER_UPDATE;
			this->playerId = playerId;
		}

		int playerId;
		PlayerUpdateData data = {};
	};

	struct PlayerTaskPacket : public Packet
	{
		PlayerTaskPacket(int taskType, TaskData taskData = {})
		{
			id = PLAYER_TASK;
			this->taskType = taskType;
			this->taskData = taskData;
		}

		int playerId;
		int taskType;
		TaskData taskData;
	};

	struct MassivePlayerUpdatePacket : public Packet
	{
		MassivePlayerUpdatePacket(int playersCount)
		{
			id = MASSIVE_PLAYER_UPDATE;
			this->playerCount = playersCount;
		}

		size_t playerCount;
		PlayerUpdatePacket players[MAX_SERVER_PLAYERS] = {};
	};
};