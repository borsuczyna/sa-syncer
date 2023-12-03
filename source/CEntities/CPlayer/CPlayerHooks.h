#pragma once

class CPlayerHooks
{
public:
	static CPlayerInfo* __fastcall GetPlayerInfoForThisPlayerPed(CPlayerPed* This);
	static CVehicle* __cdecl FindPlayerVehicleHook(int playerid, bool bIncludeRemote);
	static int GetPlayerWorldId(CPlayerPed* This);
	static CPad* __cdecl GetPad(int number);
	static void __fastcall ProcessControl(CPlayerPed* This);
};