

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MTA_CLIENT
#include "SharedUtil.h"

#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>
#include "shellapi.h"
#include <Wscapi.h>

#include <shlobj.h>
#include <Psapi.h>
#include <time.h>

#include "Main.h"
#include "MainFunctions.h"
#include "Install.h"
#include "Utils.h"
#include "..\version.h"
#include "CInstallManager.h"
#include "D3DStuff.h"
#include "CExePatchedStatus.h"

// Things to make source easier to backport
#define _ SStringX
#define _E(code) SString()
inline int MessageBoxUTF8 ( HWND hWnd, SString strText, SString strCaption, UINT uType )
{
    return MessageBox( hWnd, strText, strCaption, uType );
}
