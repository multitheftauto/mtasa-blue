
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

// SDK includes
#include <core/CCoreInterface.h>
#include <version.h>

// Multiplayer includes
#include "multiplayersa_init.h"
#include "multiplayer_keysync.h"
#include "multiplayer_hooksystem.h"
#include "..\game_sa\CCameraSA.h"
#include "..\game_sa\CEntitySA.h"
#include "..\game_sa\CPedSA.h"

extern CMultiplayerSA* pMultiplayer;
