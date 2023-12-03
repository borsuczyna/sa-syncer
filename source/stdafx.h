// includes
#include <enet/enet.h>
#include <vector>
#include <thread>
#include <ctime>
#include "Windows.h"

// discord rpc
#include "CDiscordRPC/include/discord_register.h"
#include "CDiscordRPC/include/discord_rpc.h"
#pragma comment(lib, "discord-rpc.lib")

// plugin sdk
#include "plugin.h"
#include "CPlayerInfo.h"
#include "CPad.h"
#include "CLoadingScreen.h"
#include "CWorld.h"
#include "CStreaming.h"
#include "CCamera.h"
#include "CTheZones.h"
#include "CTaskSimpleDuckToggle.h"
#include "CTaskComplexJump.h"

using namespace plugin;

// shared
#include "../shared/CPackets.h"
#include "../shared/CInterpolate.h"

// syncer
#include "CTimers/CTimers.h"
#include "CUtils/CNickGenerator.h"
#include "CHooks/CHooks.h"
#include "CHooks/CPatches.h"
#include "CNetworking/CNetworking.h"
#include "CEntities/CPlayer/CPlayer.h"
#include "CEntities/CPlayer/CPlayerHooks.h"
#include "CPacketManagers/CPlayerPackets.h"
#include "CManagers/CPlayerManager.h"
#include "CConsole/CConsole.h"
#include "CCore/CCore.h"
#include "CModelManager/CModelManager.h"
#include "CDiscordRPC/CDiscordRPC.h"