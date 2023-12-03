#include "../../stdafx.h"

CPlayerInfo* __fastcall CPlayerHooks::GetPlayerInfoForThisPlayerPed(CPlayerPed* This)
{
	if (This == FindPlayerPed(0)) return &CWorld::Players[0];
	for (int i = 0; i < MAX_SERVER_PLAYERS; i++)
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
	for (int i = 0; i < MAX_SERVER_PLAYERS; i++)
	{
		if (This == CWorld::Players[i].m_pPed) return i;
	}
	return -1;
}

CPad* __cdecl CPlayerHooks::GetPad(int number)
{
	return &CPlayerManager::Pads[CWorld::PlayerInFocus];
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
		CPackets::CPlayerControls controls = player->m_updateData.m_controls;

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

	CPlayerPed__ProcessControl(This);
}