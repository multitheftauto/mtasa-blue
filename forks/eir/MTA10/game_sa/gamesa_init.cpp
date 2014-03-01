/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_init.cpp
*  PURPOSE:     Game initialization interface
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_gamesa_init
#include "profiler/SharedUtil.Profiler.h"

CGameSA* pGame = NULL;
CCoreInterface* g_pCore = NULL;
CCoreInterface* core = NULL;

//-----------------------------------------------------------
// This function uses the initialized data sections of the executables
// to differentiate between versions.  MUST be called at least once
// in order for proper initialization to occur.

extern "C" _declspec(dllexport)
CGame * GetGameInterface( CCoreInterface* pCore )
{
    DEBUG_TRACE("CGame * GetGameInterface()");

    g_pCore = core = pCore;

    pGame = new CGameSA;

    return (CGame *)pGame;
}

//-----------------------------------------------------------


void MemSet ( void* dwDest, int cValue, uint uiAmount )
{
    if ( ismemset( dwDest, cValue, uiAmount ) )
        return;
    SMemWrite hMem = OpenMemWrite( dwDest, uiAmount );
    memset ( dwDest, cValue, uiAmount );
    CloseMemWrite( hMem );
}

void MemCpy ( void* dwDest, const void* dwSrc, uint uiAmount )
{
    if ( memcmp( dwDest, dwSrc, uiAmount ) == 0 )
        return;
    SMemWrite hMem = OpenMemWrite( dwDest, uiAmount );
    memcpy ( dwDest, dwSrc, uiAmount );
    CloseMemWrite( hMem );
}

bool GetDebugIdEnabled ( uint uiDebugId )
{
    return g_pCore->GetDebugIdEnabled ( uiDebugId );  
}

void LogEvent ( uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId )
{
    g_pCore->LogEvent ( uiDebugId, szType, szContext, szBody, uiAddReportLogId );  
}

//-----------------------------------------------------------

extern "C"
{
BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
}

BOOL WINAPI __dllInit( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
    switch( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
        DbgHeap_Init();
        break;
    }

    BOOL ret = _DllMainCRTStartup( hinstDLL, fdwReason, lpReserved );

    switch( fdwReason )
    {
    case DLL_PROCESS_DETACH:
        DbgHeap_Shutdown();
        break;
    }

    return ret;
}