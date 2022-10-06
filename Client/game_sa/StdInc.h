
// Pragmas
#pragma warning (disable:4409)
#pragma warning (disable:4250)

#include <windows.h>
#define MTA_CLIENT
#define SHARED_UTIL_WITH_HASH_MAP
#define SHARED_UTIL_WITH_FAST_HASH_MAP
#include "SharedUtil.h"
#include "SharedUtil.MemAccess.h"
#include <stdio.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

// SDK includes
#include <core/CCoreInterface.h>
#include <net/CNet.h>
#include <version.h>
#include <ijsify.h>

// Game includes
#include "HookSystem.h"
#include "gamesa_init.h"
