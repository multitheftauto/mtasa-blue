

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MTA_CLIENT
#define SHARED_UTIL_WITH_SYS_INFO
#include "SharedUtil.h"

#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>
#include "shellapi.h"
#include <Wscapi.h>

#include <shlobj.h>
#include <Psapi.h>
#include <time.h>

#include "..\sdk\core\CLocalizationInterface.h"
extern CLocalizationInterface* g_pLocalization;

#include "Main.h"
#include "MainFunctions.h"
#include "Install.h"
#include "Utils.h"
#include "Dialogs.h"
#include "..\version.h"
#include "CInstallManager.h"
#include "D3DStuff.h"
#include "CExePatchedStatus.h"
