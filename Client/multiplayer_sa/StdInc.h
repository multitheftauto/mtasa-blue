
// Pragmas
#pragma warning (disable:4250)

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
#include <game/CGame.h>
#include <CMatrix_Pad.h>
#include <version.h>
#include <hwbrk.h>

// Multiplayer includes
#include "multiplayersa_init.h"
#include "multiplayer_keysync.h"
#include "multiplayer_hooksystem.h"
#include "multiplayer_shotsync.h"
#include "CMultiplayerSA.h"
#include "COffsets.h"
#include "CPopulationSA.h"
#include "CRemoteDataSA.h"
#include "..\game_sa\CCameraSA.h"
#include "..\game_sa\CEntitySA.h"
#include "..\game_sa\CBuildingSA.h"
#include "..\game_sa\CPedSA.h"
#include "..\game_sa\common.h"
#include "..\core\CCrashDumpWriter.h"

extern CMultiplayerSA* pMultiplayer;
