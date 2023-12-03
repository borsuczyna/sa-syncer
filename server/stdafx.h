#pragma once

#include <iostream>
#include <cstdarg>
#include <vector>
#include <chrono>
#include <thread>

#include <enet/enet.h>

#include "../shared/CPackets.h"
#include "../shared/CMassSerializer.h"

#include "CConsole/CConsole.h"
#include "CEntities/CPlayer/CPlayer.h"
#include "CTimers/CTimers.h"
#include "CPacketManagers/CPlayerPackets.h"
#include "CManagers/CPlayerManager.h"
#include "CNetworking/CNetworking.h"
#include "CCore/CCore.h"