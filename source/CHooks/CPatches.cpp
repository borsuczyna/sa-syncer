#include "../stdafx.h"

static bool g_bFullscreen = false;
static RwVideoMode g_videoMode = { 0 };
static HWND g_hWnd = nullptr;
RwV2d* lastMousePos = nullptr;

void ApplyWindowMode()
{
    if (g_bFullscreen)
    {
        // Force fullscreen mode
        g_videoMode.flags = rwVIDEOMODEEXCLUSIVE;
        RsGlobal.maximumWidth = g_videoMode.width;
        RsGlobal.maximumHeight = g_videoMode.height;
        RsGlobal.ps->fullScreen = TRUE;
        SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(g_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        RwD3D9EngineSetRefreshRate(g_videoMode.refRate);
    }
    else
    {
        // Force windowed mode
        g_videoMode.flags = RwVideoModeFlag(0);
        RECT rect = { 0 };
        GetClientRect(g_hWnd, &rect);
        RsGlobal.maximumWidth = rect.right;
        RsGlobal.maximumHeight = rect.bottom;
        int x = (g_videoMode.width - rect.right) / 2;
        int y = (g_videoMode.height - rect.bottom) / 2;
        SetWindowPos(g_hWnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
}

bool Hooked_PSelectDevice()
{
    RwInt32 const numSubSystems = std::min<RwInt32>(RwEngineGetNumSubSystems(), 16);

    if (!numSubSystems)
        return FALSE;

    auto subSystemInfo = reinterpret_cast<RwSubSystemInfo*>(0xC8CFC0);

    for (RwInt32 i = 0; i < numSubSystems; ++i)
        RwEngineGetSubSystemInfo(&subSystemInfo[i], i);

    RwInt32 const subSystem = RwEngineGetCurrentSubSystem();
    RwInt32 const videoMode = RwEngineGetCurrentVideoMode();

    g_videoMode = { 0 };
    RwEngineGetVideoModeInfo(&g_videoMode, videoMode);
    g_hWnd = RsGlobal.ps->window;

    ApplyWindowMode();

    RwD3D9EngineSetMultiSamplingLevels(*reinterpret_cast<RwUInt32*>(0xBA6810));
    return TRUE;
}

void WindowPatches()
{
    DWORD originalProtect;
    auto const address = reinterpret_cast<void*>(0x74887B);
    unsigned char const instructions[2] = { 0xEB, 0x5D };
    VirtualProtect(address, 6, PAGE_READWRITE, &originalProtect);
    std::memset(address, 0x90, 6);
    std::memcpy(address, instructions, 2);
    VirtualProtect(address, 6, originalProtect, &originalProtect);

    patch::RedirectJump(0x746190, Hooked_PSelectDevice);

    RsGlobal.maxFPS = 100;
}

void SimulateCopyrightScreen()
{
    CLoadingScreen::m_currDisplayedSplash = 0;
    CLoadingScreen::m_timeSinceLastScreen -= 1000.f;
    CLoadingScreen::m_bFadeInNextSplashFromBlack = 1;
}

void LoadScreenPatches()
{
    // Start the game at state 5
    // Disable gGameState = 0 setting
    patch::Nop(0x747483, 6);

    // Put the game where the user wants (default's to the copyright screen)
    // GS_INIT_ONCE:5
    patch::Set(0xC8D4C0, 5);

    // Disable Copyright screen
    // Hook the copyright screen fading in/out and simulates that it has happened
    patch::Nop(0x748C2B, 5);
    patch::ReplaceFunctionCall(0x748C9A, SimulateCopyrightScreen);
}

void GameCorePatches()
{
    // Patch semaphore, allow more than one game instances
    static char cdstream[9];
    sprintf(cdstream, "%I64d", GetTickCount64());
    patch::SetRaw(0x858AD4, cdstream, 9);

    // Set streaming memory to 128MB
    //patch::SetUInt(0x5B8E6A, 134217728);

    // Dont print aspect ratios
    patch::Nop(0x745997, 5);

    // Dont print soundmanager text
    patch::Nop(0x5B97C9, 5);

    // Use our icon
    patch::SetUChar(0x7486A5, 1);

    // Patch IsAlreadyRunning 
    patch::PutRetn0(0x7468E0);

    // Don't catch WM_SYSKEYDOWN and WM_SYSKEYUP (fixes Alt+F4)
    patch::RedirectJump(0x748220, (void*)0x748446);
    patch::SetUChar(0x7481E3, 0x5C); // esi -> ebx
    patch::SetUChar(0x7481EA, 0x53); // esi -> ebx
    patch::SetUChar(0x74820D, 0xFB); // esi -> ebx
    patch::SetChar(0x7481EF, 0x54 - 0x3C); // use stack space for new lParam
    patch::SetChar(0x748200, 0x4C - 0x3C); // use stack space for new lParam
    patch::SetChar(0x748214, 0x4C - 0x3C); // use stack space for new lParam

    patch::RedirectJump(0x74826A, (void*)0x748446);
    patch::SetUChar(0x74822D, 0x5C); // esi -> ebx
    patch::SetUChar(0x748234, 0x53); // esi -> ebx
    patch::SetUChar(0x748257, 0xFB); // esi -> ebx
    patch::SetChar(0x748239, 0x54 - 0x3C); // use stack space for new lParam
    patch::SetChar(0x74824A, 0x4C - 0x3C); // use stack space for new lParam
    patch::SetChar(0x74825E, 0x4C - 0x3C); // use stack space for new lParam
}

void InputPatches()
{
    // Disable re-initialization of DirectInput mouse device by the game
    patch::SetUChar(0x576CCC, 0xEB);
    patch::SetUChar(0x576EBA, 0xEB);
    patch::SetUChar(0x576F8A, 0xEB);

    // Make sure DirectInput mouse device is set non-exclusive (may not be needed?)
    patch::SetUInt(0x7469A0, 0x909000B0);

    //CPed::Say patch
    patch::SetUInt(0x5EFFE0, 0x900018C2);
}

void CrashfixHooks()
{
    //Anim crash in CPlayerPed::ProcessControl
    patch::Nop(0x609C08, 39);

    // No DirectPlay dependency
    // Increase compatibility for Windows 8+
    patch::SetUChar(0x74754A, 0xB8);
    patch::SetUInt(0x74754B, 0x900);

    // Don't create a ped group on player creation (Fixes a crash)
    // TODO: Proper CPlayerPed creation
    patch::Nop(0x60D64D, 1);
    patch::SetUChar(0x60D64E, 0xE9);

    // Disable the call to FxSystem_c::GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters
    // Which was causing a crash. The crash happens if you create 40 or
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    patch::Nop(0x4DCF87, 6);

    // Fixed a crash (race condition triggered when jacking a vehicle)
    patch::Nop(0x6485AC, 6);

    // Fix mirror crash
    patch::SetUChar(0x7271CB + 0, 0x85); // test eax, eax
    patch::SetUChar(0x7271CB + 1, 0xC0);
    patch::SetUChar(0x7271CB + 2, 0x74); // je 0x727203
    patch::SetUChar(0x7271CB + 3, 0x34);
    patch::SetUChar(0x7271CB + 4, 0x83); // add esp, 04
    patch::SetUChar(0x7271CB + 5, 0xC4);
    patch::SetUChar(0x7271CB + 6, 0x04);
    patch::SetUChar(0x7271CB + 7, 0xC6); // mov byte ptr [00C7C728],01

    // No FxMemoryPool_c::Optimize (causes heap corruption)
    patch::Nop(0x5C25D3, 5);

    // Satchel charge crash fix
    patch::Nop(0x738F3A, 83);
}

void psMouseSetPos(RwV2d* pos)
{
    bool isWindowFocused = GetForegroundWindow() == g_hWnd;
    if (!isWindowFocused)
        return;

    POINT point = { LONG(pos->x), LONG(pos->y) };
    ::ClientToScreen(g_hWnd, &point);
    ::SetCursorPos(point.x, point.y);
    lastMousePos = pos;
}

void MenuPatches()
{
    // No background texture drawing in menu
    patch::RedirectShortJump(0x57B9CA);

    // Allow widescreen resolutions
    patch::SetUInt(0x745B81, 0x9090587D);
    patch::SetUInt(0x74596C, 0x9090127D);
    patch::Nop(0x745970, 2);
    patch::Nop(0x745B85, 2);
    patch::Nop(0x7459E1, 2);

    // Allow Alt+Tab without pausing the game
    int patchAddress = NULL;
    if (*(BYTE*)0x748ADD == 0xFF && *(BYTE*)0x748ADE == 0x53)
        patchAddress = 0x748A8D;
    else
        patchAddress = 0x748ADD;

    patch::Nop(patchAddress, 6);

    // Disable MENU AFTER alt + tab
    patch::SetUChar(0x53BC78, 0x00);

    // Disable menu after focus loss
    patch::PutRetn(0x53BC60);

    // Dont lock mouse on focus loss
    patch::ReplaceFunction(0x7453F0, psMouseSetPos);
}

void LimitPatches()
{
    //Inc task pool
    patch::SetUChar(0x551140, 0xFF);

    //Inc ped pool pool
    patch::SetUChar(0x550FF2, 1000);

    //Inc intelligence pool
    patch::SetUChar(0x551283, 210);

    //Inc event pool
    patch::SetUChar(0x551178, 0x01);

    //Inc matrices pool
    patch::SetUChar(0x54F3A2, 0x10);

    //Inc ccolmodel pool
    patch::SetUChar(0x551108, 0x77);

    //Inc dummies pool
    patch::SetUChar(0x5510D0, 0x0F);

    //Inc objects pool
    patch::SetUChar(0x551098, 0x02);
}

bool CPatches::IsWindowFocused()
{
    return GetForegroundWindow() == g_hWnd;
}

void CPatches::Init()
{
    WindowPatches();
    LoadScreenPatches();
    GameCorePatches();
    InputPatches();
    CrashfixHooks();
    LimitPatches();
    MenuPatches();
}