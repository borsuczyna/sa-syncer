#pragma once

class CPlayerHooks
{
public:
	static CPlayerInfo* __fastcall GetPlayerInfoForThisPlayerPed(CPlayerPed* This);
	static CVehicle* __cdecl FindPlayerVehicleHook(int playerid, bool bIncludeRemote);
	static int GetPlayerWorldId(CPlayerPed* This);
	static CPad* __cdecl GetPad(int number);
	static CPad* __cdecl GetPadFromPlayer(CPlayerPed* This);
	static void __fastcall SetRealMoveAnim(CPlayerPed* _this);
	static void __fastcall ProcessControl(CPlayerPed* This);
	static bool __fastcall JumpJustDown(CPad* This);
	static void __fastcall SetTask(CTaskManager* This, DWORD EDX, CTask* task, int tasksId, bool unused);
	static void __fastcall SetTaskSecondary(CTaskManager* This, DWORD EDX, CTask* task, int tasksId);
};