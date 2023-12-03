#include <enet/enet.h>
#include <vector>
#include <thread>
#include <ctime>
#include "Windows.h"

#include "plugin.h"
#include "CPlayerInfo.h"
#include "CPad.h"
#include "CLoadingScreen.h"
#include "CWorld.h"
#include "CStreaming.h"
#include "CCamera.h"

using namespace plugin;

#include "../shared/CPackets.h"
#include "../shared/CInterpolate.h"

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