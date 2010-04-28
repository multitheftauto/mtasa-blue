/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Utils.h
*  PURPOSE:     Loading utilities
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce executable size
//#define _WIN32_WINNT 0x0400     // So we can use IsDebuggerPresent()
#include <windows.h>
#endif

// Loads the given dll into hProcess. Returns 0 on failure or the handle to the
// remote dll module on success.
HMODULE RemoteLoadLibrary(HANDLE hProcess, const char* szLibPath);

#endif
