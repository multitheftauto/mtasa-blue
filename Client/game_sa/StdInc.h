
// Pragmas

// illegal instruction size
// The instruction did not have a form with the specified size. The smallest legal size was used.
#pragma warning (disable:4409)

// 'class1' : inherits 'class2::member' via dominance
// Two or more members have the same name. The one in class2 is inherited because it is a base class for the other classes that contained this member.
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
#include <ijsify.h>

// Game includes
#include "HookSystem.h"
#include "gamesa_init.h"
