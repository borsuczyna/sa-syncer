#include "../../stdafx.h"

CPlayerInfo* __fastcall CPlayerHooks::GetPlayerInfoForThisPlayerPed(CPlayerPed* This)
{
	if (This == FindPlayerPed(0)) return &CWorld::Players[0];
	for (int i = 0; i < MAX_SERVER_PLAYERS + 2; i++) // we don't use player id 0 and 1
	{
		if (This == CWorld::Players[i].m_pPed) return &CWorld::Players[i];
	}
	return nullptr;
}

CVehicle* __cdecl CPlayerHooks::FindPlayerVehicleHook(int playerid, bool bIncludeRemote)
{
	if (playerid < 0)
	{
		if (CWorld::Players[0].m_pPed && CWorld::Players[0].m_pPed->m_nPedFlags.bInVehicle)
		{
			if (!bIncludeRemote || !CWorld::Players[0].m_pRemoteVehicle) return CWorld::Players[0].m_pPed->m_pVehicle;
			else return CWorld::Players[0].m_pRemoteVehicle;
		}
		else return nullptr;
	}
	return FindPlayerVehicle(playerid, bIncludeRemote);
}

int CPlayerHooks::GetPlayerWorldId(CPlayerPed* This)
{
	if (This == FindPlayerPed(0)) return 0;
	for (int i = 0; i < MAX_SERVER_PLAYERS + 2; i++) // we don't use player id 0 and 1
	{
		if (This == CWorld::Players[i].m_pPed) return i;
	}
	return -1;
}

CPad* __cdecl CPlayerHooks::GetPad(int number)
{
	return &CPlayerManager::Pads[CWorld::PlayerInFocus];
}

void __fastcall CPlayerHooks::SetRealMoveAnim(CPlayerPed* _this)
{
	CPlayer* player = CPlayerManager::GetPlayer(_this);
	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();

	if (player != nullptr && localPlayer->GetPed() != _this)
	{
		auto lData = player->m_lastUpdateData;
		auto cData = player->m_updateData;
		float interpolation = player->GetInterpolationTime();

		_this->m_fAimingRotation = CInterpolate::Angle(lData.m_fAimingRotation, cData.m_fAimingRotation, interpolation);
		_this->m_nMoveState = cData.m_iMoveState;
	}

	plugin::CallMethod<0x60A9C0, CPlayerPed*>(_this);
}

void __fastcall CPlayerHooks::ProcessControl(CPlayerPed* This)
{
	static void(__thiscall * CPlayerPed__ProcessControl)(CPlayerPed * This) = decltype(CPlayerPed__ProcessControl)(0x60EA90);

	static bool bLocalPlayerInitialized = false;
	if (!bLocalPlayerInitialized) {
		bLocalPlayerInitialized = true;
		CPlayerManager::InitLocalPlayer();
	}

	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();

	if (This != localPlayer->GetPed())
	{
		CPlayer* player = CPlayerManager::GetPlayer(This);
		if (player == nullptr) {
			CConsole::Print("CPlayerHooks::ProcessControl: player == nullptr");
			return;
		}

		int worldId = CPlayerHooks::GetPlayerWorldId(This);
		if (worldId == -1) {
			CConsole::Print("CPlayerHooks::ProcessControl: worldId == -1");
			return;
		}

		// Set player in focus
		CWorld::PlayerInFocus = worldId;

		// Get all required data
		CPad* pad = This->GetPadFromPlayer();
		CPlayerInfo* playerInfo = This->GetPlayerInfoForThisPlayerPed();
		CPlayerControls controls = player->m_updateData.m_controls;

		// Save current camera state
		float cameraOrientation = TheCamera.m_fOrientation;

		// Set current camera state
		TheCamera.m_fOrientation = player->m_updateData.m_fCameraOrientation;

		// Save current keys state
		CControllerState oldState = pad->NewState;

		// Apply new keys state
		pad->NewState = CControllerState();
		pad->NewState.LeftStickX = controls.LeftStickX;
		pad->NewState.LeftStickY = controls.LeftStickY;
		pad->NewState.RightShoulder1 = controls.RightShoulder1;
		pad->NewState.m_bPedWalk = controls.m_bPedWalk;
		pad->NewState.ButtonCircle = controls.ButtonCircle;
		pad->NewState.ButtonCross = controls.ButtonCross;
		pad->NewState.ButtonSquare = controls.ButtonSquare;
		pad->NewState.ButtonTriangle = controls.ButtonTriangle;

		// Process control
		CPlayerPed__ProcessControl(This);

		// Restore keys state
		pad->NewState = oldState;

		// Restore player 0 as the player in focus
		CWorld::PlayerInFocus = 0;

		// Restore camera state
		TheCamera.m_fOrientation = cameraOrientation;

		return;
	}

	// Check ducking
	CPlayer* player = CPlayerManager::GetLocalPlayer();
	static bool networkDucked = false;
	if (player->IsDucked() != networkDucked) {

		bool isDucked = player->IsDucked();

		TaskData data;
		data.position = localPlayer->GetPosition();
		data.rotation = localPlayer->GetPed()->m_fCurrentRotation;
		data.toggle = isDucked;

		CPlayerPackets::SendPlayerTaskPacket(eTaskType::TASK_SIMPLE_DUCK, data);

		networkDucked = isDucked;
	}

	CPlayerPed__ProcessControl(This);
}

bool __fastcall CPlayerHooks::JumpJustDown(CPad* This)
{
	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();
	CPlayerPed* ped = localPlayer->GetPed();
	if (ped == nullptr) return false;

	if (This == ped->GetPadFromPlayer())
	{
		if (CPatches::IsWindowFocused())
		{
			if (!This->DisablePlayerControls && !This->bDisablePlayerDuck && This->NewState.ButtonSquare)
				return true;
		}
	}
	return false;
}

void __fastcall CPlayerHooks::SetTask(CTaskManager* This, DWORD EDX, CTask* task, int tasksId, bool unused)
{
	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();
	if (FindPlayerPed(0) == This->m_pPed)
	{
		if (task)
		{
			int taskID = task->GetId();
			if (taskID == eTaskType::TASK_COMPLEX_JUMP)
			{
				if (!CPatches::IsWindowFocused()) return; // dont process jump when game is not focused

				TaskData data;
				data.position = localPlayer->GetPosition();
				data.rotation = localPlayer->GetPed()->m_fCurrentRotation;

				CPlayerPackets::SendPlayerTaskPacket(eTaskType::TASK_COMPLEX_JUMP, data);
			}
		}
		else
		{
			CTask* _task = This->m_aPrimaryTasks[tasksId];
			//if (_task)printf("New task for player ped: nullptr %d: %s\n", _task->GetId(), TaskNames[_task->GetId()]);
		}
	}
	else
	{
		if (task)
		{
			int taskID = task->GetId();
			if (taskID == eTaskType::TASK_COMPLEX_JUMP) return;
		}
	}

	This->SetTask(task, tasksId, unused);
}

void __fastcall CPlayerHooks::SetTaskSecondary(CTaskManager* This, DWORD EDX, CTask* task, int tasksId)
{
	CPlayer* localPlayer = CPlayerManager::GetLocalPlayer();
	if (FindPlayerPed(0) == This->m_pPed)
	{
		if (task)
		{
			int taskID = task->GetId();
			//printf("New task for player ped: %d: %s\n", taskID, TaskNames[taskID]);
		}
		else
		{
			CTask* _task = This->m_aSecondaryTasks[tasksId];
			//if (_task)printf("New task for player ped: nullptr %d: %s\n", _task->GetId(), TaskNames[_task->GetId()]);
		}
	}
	This->SetTaskSecondary(task, tasksId);
}