/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/D3DStuff.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <windows.h>

void BeginD3DStuff();
void EndD3DStuff();
void LoaderResolveCrashHandlerExports(HMODULE coreModule) noexcept;
