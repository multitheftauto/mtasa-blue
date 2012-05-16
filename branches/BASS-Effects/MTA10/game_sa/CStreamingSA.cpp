/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.cpp
*  PURPOSE:     Data streamer
*  DEVELOPERS:  Jax <>
*               jenksta <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    //
    // Used in LoadAllRequestedModels to record state
    //
    struct SPassStats
    {
        bool bLoadingBigModel;
        DWORD numPriorityRequests;
        DWORD numModelsRequested;
        DWORD memoryUsed;

        void Record ( void )
        {
            #define VAR_CStreaming_bLoadingBigModel         0x08E4A58
            #define VAR_CStreaming_numPriorityRequests      0x08E4BA0
            #define VAR_CStreaming_numModelsRequested       0x08E4CB8
            #define VAR_CStreaming_memoryUsed               0x08E4CB4

            bLoadingBigModel    = *(BYTE*)VAR_CStreaming_bLoadingBigModel != 0;
            numPriorityRequests = *(DWORD*)VAR_CStreaming_numPriorityRequests;
            numModelsRequested  = *(DWORD*)VAR_CStreaming_numModelsRequested;
            memoryUsed          = *(DWORD*)VAR_CStreaming_memoryUsed;
        }
    };
}


bool IsUpgradeModelId ( DWORD dwModelID )
{
    return dwModelID >= 1000 && dwModelID <= 1193;
}


void CStreamingSA::RequestModel( DWORD dwModelID, DWORD dwFlags )
{
    if ( IsUpgradeModelId ( dwModelID ) )
    {
        DWORD dwFunc = FUNC_RequestVehicleUpgrade;
        _asm
        {
            push    dwFlags
            push    dwModelID
            call    dwFunc
            add     esp, 8
        }
    }
    else
    {
        DWORD dwFunction = FUNC_CStreaming__RequestModel;
        _asm
        {
            push    dwFlags
            push    dwModelID
            call    dwFunction
            add     esp, 8
        }
    }
}


void CStreamingSA::LoadAllRequestedModels ( BOOL bOnlyPriorityModels, const char* szTag )
{
    TIMEUS startTime = GetTimeUs ();

    DWORD dwFunction = FUNC_LoadAllRequestedModels;
    DWORD dwOnlyPriorityModels = bOnlyPriorityModels;
    _asm
    {
        push    dwOnlyPriorityModels
        call    dwFunction
        add     esp, 4
    }

    if ( g_pCore->GetDiagnosticDebug () == EDiagnosticDebug::FPS_6934 )
    {
        uint deltaTimeMs = ( GetTimeUs () - startTime ) / 1000;
        if ( deltaTimeMs > 2 )
            TIMING_DETAIL_FORCE( SString ( "LoadAllRequestedModels( %d, %s ) took %d ms", bOnlyPriorityModels, szTag, deltaTimeMs ) );
    }
}


BOOL CStreamingSA::HasModelLoaded ( DWORD dwModelID )
{
    if ( IsUpgradeModelId ( dwModelID ) )
    {
        bool bReturn;
        DWORD dwFunc = FUNC_CStreaming__HasVehicleUpgradeLoaded;
        _asm
        {
            push    dwModelID
            call    dwFunc
            add     esp, 0x4
            mov     bReturn, al
        }
        return bReturn;
    }
    else
    {
        DWORD dwFunc = FUNC_CStreaming__HasModelLoaded;
        BOOL bReturn = 0;
        _asm
        {
            push    dwModelID
            call    dwFunc
            movzx   eax, al
            mov     bReturn, eax
            pop     eax
        }

        return bReturn;
    }
}

void CStreamingSA::RequestSpecialModel ( DWORD model, const char * szTexture, DWORD channel )
{
    DWORD dwFunc = FUNC_CStreaming_RequestSpecialModel;
    _asm
    {
        push    channel
        push    szTexture
        push    model
        call    dwFunc
        add     esp, 0xC
    }
}