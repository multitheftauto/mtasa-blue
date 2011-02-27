

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>
#include "shellapi.h"

#include <shlobj.h>
#include <Psapi.h>

#define MTA_CLIENT
#include "SharedUtil.h"
#include "Main.h"
#include "Install.h"
#include "Utils.h"
#include "..\version.h"
#include "CInstallManager.h"
