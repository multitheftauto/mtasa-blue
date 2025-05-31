/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/gamesa_init.cpp
 *  PURPOSE:     Game initialization interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include <net/CNet.h>
#include "gamesa_init.h"
#include "CGameSA.h"
#define DECLARE_PROFILER_SECTION_gamesa_init
#include "profiler/SharedUtil.Profiler.h"

CGameSA*        pGame = NULL;
CNet*           g_pNet = NULL;
CCoreInterface* g_pCore = NULL;

//-----------------------------------------------------------
// This function uses the initialized data sections of the executables
// to differentiate between versions.  MUST be called at least once
// in order for proper initialization to occur.
MTAEXPORT CGame* GetGameInterface(CCoreInterface* pCore)
{
    g_pNet = pCore->GetNetwork();
    assert(g_pNet);

    pGame = new CGameSA;
    g_pCore = pCore;

    return (CGame*)pGame;
}

//-----------------------------------------------------------

void MemSet(void* dwDest, int cValue, uint uiAmount)
{
    if (ismemset(dwDest, cValue, uiAmount))
        return;
    SMemWrite hMem = OpenMemWrite(dwDest, uiAmount);
    memset(dwDest, cValue, uiAmount);
    CloseMemWrite(hMem);
}

void MemCpy(void* dwDest, const void* dwSrc, uint uiAmount)
{
    if (memcmp(dwDest, dwSrc, uiAmount) == 0)
        return;
    SMemWrite hMem = OpenMemWrite(dwDest, uiAmount);
    memcpy(dwDest, dwSrc, uiAmount);
    CloseMemWrite(hMem);
}

bool GetDebugIdEnabled(uint uiDebugId)
{
    return g_pCore->GetDebugIdEnabled(uiDebugId);
}

void LogEvent(uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId)
{
    g_pCore->LogEvent(uiDebugId, szType, szContext, szBody, uiAddReportLogId);
}
