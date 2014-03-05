#pragma message("Compiling precompiled header.\n")

// Pragmas
#pragma warning (disable:4250)

#define NOMINMAX
#include <windows.h>
#define MTA_CLIENT
#define SHARED_UTIL_WITH_HASH_MAP
#define SHARED_UTIL_WITH_FAST_HASH_MAP
#define SHARED_UTIL_WITH_SYS_INFO
#include "SharedUtil.h"
#include "SharedUtil.MemAccess.h"
#include <stdio.h>

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>

// SDK includes
#include <core/CCoreInterface.h>
#include <net/CNet.h>
#include <game/Common.h>
#include "../game_sa/RenderWare.h"
#include <game/CGame.h>
#include <CMatrix_Pad.h>
#include <../version.h>
#include "../game_sa/Common.h"

// Multiplayer includes
#include "multiplayersa_init.h"
#include "multiplayer_keysync.h"
#include "multiplayer_hooksystem.h"
#include "multiplayer_shotsync.h"
#include "CMultiplayerSA.h"
#include "COffsets.h"
#include "CPopulationSA.h"
#include "CRemoteDataSA.h"

extern CMultiplayerSA* pMultiplayer;
