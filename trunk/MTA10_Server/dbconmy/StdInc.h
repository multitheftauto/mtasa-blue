#ifdef WIN32
#pragma message("Compiling precompiled header.\n")

#include <windows.h>
#include <winsock.h>
#include <mmsystem.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>

// SDK includes
#include "MTAPlatform.h"
#define SHARED_UTIL_WITH_FAST_HASH_MAP
#include "SharedUtil.h"
