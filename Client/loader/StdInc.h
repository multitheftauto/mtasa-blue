/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/StdInc.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <shellapi.h>
#include <Wscapi.h>
#include <shlobj.h>
#include <Psapi.h>
#include <winternl.h>

#define MTA_CLIENT
#define SHARED_UTIL_WITH_SYS_INFO
#include "SharedUtil.h"

#include <core/CLocalizationInterface.h>
extern CLocalizationInterface* g_pLocalization;
