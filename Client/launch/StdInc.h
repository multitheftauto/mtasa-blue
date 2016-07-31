

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MTA_CLIENT
#include "SharedUtil.h"

#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>
#include "shellapi.h"

#include <shlobj.h>

#if _MSC_VER == 1500        // MSVC++ 9.0 (Visual Studio 2008)
    #define MSVCR_DLL "msvcr90.dll"
#elif _MSC_VER == 1800      // MSVC++ 12.0 (Visual Studio 2013)
    #define MSVCR_DLL "msvcr120.dll"
#else
    #error "Insert VCR info"
#endif
