#include "../stdafx.h"

std::vector<bool(*)(unsigned int)> CHooks::Patch_Funcs;

bool CHooks::SetBranchPointer(uintptr_t currentAddress, uintptr_t src, injector::memory_pointer dest, bool vp)
{
    auto inst = patch::Get<uint8_t>(currentAddress, false);
    if (inst == 0xE8 || inst == 0xE9)
    {
        uintptr_t funcptr = injector::ReadRelativeOffset(currentAddress + 1, 4, vp).as_int();

        if (funcptr == src)
        {
            injector::MakeRelativeOffset(currentAddress + 1, dest, 4, vp);
            return true;
        }
    }
    return false;
}

void PlayersPatches()
{
    // Unlock pads count
    patch::SetUChar(0x84E1FA + 1, MAX_SERVER_PLAYERS);
    patch::SetUChar(0x856465 + 1, MAX_SERVER_PLAYERS);

    CHooks::Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        uint32_t Content = patch::GetUInt(Address, false);
        if (Content == 0xB73458)
        {
            patch::Set(Address, CPlayerManager::Pads, false);
        }
        else if (Content > 0xB73458 && Content <= 0xB73458 + sizeof(CPad) && (*(unsigned char*)(Address) != 0xE8))
        {
            uint32_t offset = (Content - 0xB73458);
            patch::Set(Address, uint32_t(CPlayerManager::Pads) + offset, false);
        }
        else return false;

        return true;
    });

    CHooks::Patch_Funcs.push_back([](uint32_t Address) -> bool
	{
		return CHooks::AdjustBranchPointer(Address, 0x53FB70, CPlayerHooks::GetPad, false);
	});

    // Unlock players count
    patch::SetUChar(0x84E98A + 1, MAX_SERVER_PLAYERS);
    patch::SetUChar(0x856505 + 1, MAX_SERVER_PLAYERS);

    CWorld::Players = CPlayerManager::Players;

    CHooks::Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        
        uint32_t Content = patch::Get<uint32_t>(Address, false);
        
        if (Content == 0xB7CD98)
        {
            patch::Set(Address, CPlayerManager::Players, false);
        }
        else if (Content > 0xB7CD98 && Content <= (0xB7CD98 + sizeof(CPlayerInfo)))
        {
            uint32_t offset = (Content - 0xB7CD98);
            patch::Set(Address, uint32_t(CPlayerManager::Players) + offset, false);
        }
        else return false;

        return true;
    });
}

void PlayerControlsHooks()
{
    CHooks::Patch_Funcs.push_back([](uint32_t Address) -> bool
	{
		return CHooks::AdjustBranchPointer(Address, 0x609FF0, CPlayerHooks::GetPlayerInfoForThisPlayerPed, false);
	});

    CHooks::Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        return CHooks::AdjustBranchPointer(Address, 0x56E0D0, CPlayerHooks::FindPlayerVehicleHook, false);
    });

    patch::SetPointer(0x86D190, CPlayerHooks::ProcessControl);
}

void ExecuteEXEPatches()
{
    auto starttime = GetTickCount();
    injector::scoped_unprotect xprotect(0x400000, 0x1576ff0 - 0x400000);
    for (uint32_t address = 0x400000; address < 0x1576ff0; address++)
    {
        if (address >= 0x856E00 && address <= 0x1564F01) address = 0x1564F01;
        for (auto& func : CHooks::Patch_Funcs)
        {
            if (func(address)) address += 3;
        }
    }

    CConsole::Print("Executed All EXEPatches - %dms", (GetTickCount() - starttime));
}

void CHooks::Init()
{
    PlayersPatches();
    PlayerControlsHooks();
    ExecuteEXEPatches();
}