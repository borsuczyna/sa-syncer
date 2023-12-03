#pragma once

class CHooks
{
	static bool SetBranchPointer(uintptr_t currentAddress, uintptr_t src, injector::memory_pointer dest, bool vp = true);
	static bool AdjustBranchPointer(uintptr_t Address, uintptr_t src, injector::memory_pointer dest, bool vp = true)
	{
		return SetBranchPointer(GetGlobalAddress(Address), GetGlobalAddress(src), dest, vp);
	}

public:
	static std::vector<bool(*)(unsigned int)> Patch_Funcs;

	static void Init();
};