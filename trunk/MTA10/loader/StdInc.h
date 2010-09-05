

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>
#include "shellapi.h"

#include <shlobj.h>
#include <Psapi.h>

#include "SharedUtil.h"
#include "Main.h"
#include "Utils.h"

int     HasUsersGotFullAccess   ( const char* argv1 );
char*   ParseSid                ( const TCHAR * trustee );
int     DoSetOnFile             ( bool bYesInherit, const std::string& strFilename, const std::string& strTrustee, const std::string& strPermissions );
