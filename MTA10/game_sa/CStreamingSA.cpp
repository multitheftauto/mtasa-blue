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


void CStreamingSA::RequestModel( DWORD dwModelID, DWORD dwFlags )
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

void CStreamingSA::LoadAllRequestedModels ( BOOL bOnlyPriorityModels )
{
    // Keep looping if bLoadingBigModel is true
    // If still true after 1 second, force false and try again

    // Record before state
    SPassStats before;
    before.Record ();

    // Init loop vars
    SPassStats previousPass = before;
    SPassStats after;
    int iNumRepeats = 0;
    int iNumForces = 0;
    int iNumModelsRequestedLowest = before.numModelsRequested;
    CTickCount startTickCount = CTickCount::Now ();

    while ( true )
    {
        DWORD dwFunction = FUNC_LoadAllRequestedModels;
        DWORD dwOnlyPriorityModels = bOnlyPriorityModels;
        _asm
        {
            push    dwOnlyPriorityModels
            call    dwFunction
            add     esp, 4
        }

        // Record after state
        after.Record ();

        // Load worked ok?
        if ( after.bLoadingBigModel == false &&
             after.numPriorityRequests == 0 &&
             ( after.numModelsRequested == 0 || bOnlyPriorityModels ) )
        {
            break;
        }

        // Decide what to do on load fail
        if ( iNumRepeats > 3 && ( CTickCount::Now () - startTickCount ).ToLongLong () > 1000 )
        {
            if ( !after.bLoadingBigModel )
                break;

            MemPutFast < BYTE > ( VAR_CStreaming_bLoadingBigModel, 0 );
            iNumForces++;
        }

        previousPass = after;
        iNumRepeats++;
    }

    // Should log this?
    if ( iNumRepeats > 0 || before.bLoadingBigModel )
    {
        SString strType ( "LoadAllRequestedModels(%d): ", bOnlyPriorityModels );
        SString strStatus ( " Before[ bBig:%d numPri:%d numReq:%d mem:%d]"
                            " After[ bBig:%d numPri:%d numReq:%d mem:%d]"
                            " reps:%d"
                            " force:%d"
                            " time:%d"
                            , before.bLoadingBigModel
                            , before.numPriorityRequests
                            , before.numModelsRequested
                            , before.memoryUsed
                            , after.bLoadingBigModel
                            , after.numPriorityRequests
                            , after.numModelsRequested
                            , after.memoryUsed
                            , iNumRepeats
                            , iNumForces
                            , (int)( CTickCount::Now () - startTickCount ).ToLongLong ()
                        );
        AddReportLog ( 6640, strType + strStatus );
        LogEvent ( 640, strType, "", strStatus );
    }
}


BOOL CStreamingSA::HasModelLoaded ( DWORD dwModelID )
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

void CStreamingSA::RequestAnimations ( int iAnimationLibraryBlock, DWORD dwFlags )
{
    iAnimationLibraryBlock += 25575;
    RequestModel( iAnimationLibraryBlock, dwFlags );
}

BOOL CStreamingSA::HaveAnimationsLoaded ( int iAnimationLibraryBlock )
{
    iAnimationLibraryBlock += 25575;
    return HasModelLoaded( iAnimationLibraryBlock );
}

bool CStreamingSA::HasVehicleUpgradeLoaded ( int model )
{
    bool bReturn;
    DWORD dwFunc = FUNC_CStreaming__HasVehicleUpgradeLoaded;
    _asm
    {
        push    model
        call    dwFunc
        add     esp, 0x4
        mov     bReturn, al
    }
    return bReturn;
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