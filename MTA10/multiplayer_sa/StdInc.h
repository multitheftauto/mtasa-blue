#pragma message("Compiling precompiled header.\n")

// Pragmas
#pragma warning (disable:4250)

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>

// SDK includes
#include <game/CGame.h>
#include <CMatrix_Pad.h>
#include "SharedUtil.h"
#include "SString.h"

// Multiplayer includes
#include "multiplayersa_init.h"
#include "multiplayer_keysync.h"
#include "multiplayer_hooksystem.h"
#include "multiplayer_shotsync.h"
#include "CMultiplayerSA.h"
#include "COffsets.h"
#include "CPopulationSA.h"
#include "CRemoteDataSA.h"
