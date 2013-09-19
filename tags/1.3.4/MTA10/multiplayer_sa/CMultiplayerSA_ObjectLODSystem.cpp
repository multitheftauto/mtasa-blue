/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_ObjectLODSystem.cpp
*  PORPOISE:    System for displaying low LOD objects
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility helpers
//
////////////////////////////////////////////////
namespace
{
    #define     ARRAY_ModelInfo                 0xA9B0C8
    #define     LOW_LOD_DRAW_DISTANCE_SCALE     5


    void SetGlobalDrawDistanceScale ( float fValue )
    {
        MemPut < float > ( 0x858FD8, 300.f * fValue );
    }

    float GetDrawDistanceSetting ( void )
    {
        return *(float*)0xB6F118;
    }

    float CalculateLowLodFadeAlpha ( CBaseModelInfoSAInterface* pModelInfo, float fDist )
    {
        float fModelDrawDistance = pModelInfo->fLodDistanceUnscaled * LOW_LOD_DRAW_DISTANCE_SCALE + 20.f;
        float fFadeWidth = 20.f + fModelDrawDistance / 50.f;
        float fFadeFar = fModelDrawDistance;
        float fFadeNear = fFadeFar - fFadeWidth;
        return UnlerpClamped ( fFadeFar, fDist, fFadeNear );
    }

    struct
    {
        bool bValid;
        float fLodDistanceUnscaled;
        CBaseModelInfoSAInterface* pModelInfo;
    } saved = { false, 0.f, NULL };
}


////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility
//
////////////////////////////////////////////////
void OnMY_CRenderer_SetupEntityVisibility_Pre( CEntitySAInterface* pEntity, float& fValue )
{
    if ( pEntity->IsLowLodEntity () )
    {
        SetGlobalDrawDistanceScale ( LOW_LOD_DRAW_DISTANCE_SCALE * 2 );
        saved.pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[pEntity->m_nModelIndex];
        saved.fLodDistanceUnscaled = saved.pModelInfo->fLodDistanceUnscaled;
        saved.pModelInfo->fLodDistanceUnscaled *= LOW_LOD_DRAW_DISTANCE_SCALE / GetDrawDistanceSetting ();
        saved.bValid = true;
    }
    else
        saved.bValid = false;
}

void OnMY_CRenderer_SetupEntityVisibility_Post( int result, CEntitySAInterface* pEntity, float& fDist )
{
    if ( saved.bValid )
    {
        SetGlobalDrawDistanceScale ( 1 );
        saved.pModelInfo->fLodDistanceUnscaled = saved.fLodDistanceUnscaled;
        saved.bValid = false;

        // Doing any distance fading?
        if ( result == 0 && CalculateLowLodFadeAlpha ( saved.pModelInfo, fDist ) < 1.0f )
            pEntity->bDistanceFade = 1;
        else
            pEntity->bDistanceFade = 0;

        pEntity->SetEntityVisibilityResult ( result );
    }
    else
    if ( pEntity->IsHighLodEntity () )
    {
        pEntity->SetEntityVisibilityResult ( result );
    }
}


// Hook info
#define HOOKPOS_CRenderer_SetupEntityVisibility         0x554230
#define HOOKSIZE_CRenderer_SetupEntityVisibility        8
DWORD RETURN_CRenderer_SetupEntityVisibility =          0x554238;
void _declspec(naked) HOOK_CRenderer_SetupEntityVisibility()
{
    _asm
    {
////////////////////
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_CRenderer_SetupEntityVisibility_Pre
        add     esp, 4*2
        popad

////////////////////
        push    [esp+4*2]
        push    [esp+4*2]
        call    second
        add     esp, 4*2

////////////////////
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    eax
        call    OnMY_CRenderer_SetupEntityVisibility_Post
        add     esp, 4*2+4
        popad

        retn

second:
        sub     esp,14h 
        push    esi  
        mov     esi,dword ptr [esp+1Ch] 
        jmp     RETURN_CRenderer_SetupEntityVisibility   // 0x554238
    }
}


////////////////////////////////////////////////
//
// CWorldScan_ScanWorld
//
////////////////////////////////////////////////
void OnMY_CWorldScan_ScanWorld_Pre( CVector2D* pVec, int iValue, void (__cdecl *func)(int, int) )
{
    // Scale scan area
    const CVector2D& vecOrigin = pVec[0];

    for ( uint i = 1 ; i < 5 ; i++ )
    {
        CVector2D vecDiff = pVec[i] - vecOrigin;
        vecDiff *= LOW_LOD_DRAW_DISTANCE_SCALE * 2;
        pVec[i] = vecDiff + vecOrigin;
    }
}

// Hook info
#define HOOKPOS_CWorldScan_ScanWorld                0x55555E
#define HOOKSIZE_CWorldScan_ScanWorld               5
DWORD RETURN_CWorldScan_ScanWorlda =                0x555563;
DWORD RETURN_CWorldScan_ScanWorldb =                0x72CAE0;
void _declspec(naked) HOOK_CWorldScan_ScanWorld()
{
    _asm
    {
        call    second
        jmp     RETURN_CWorldScan_ScanWorlda   // 555563

second:
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        call    OnMY_CWorldScan_ScanWorld_Pre
        add     esp, 4*3
        popad

        jmp     RETURN_CWorldScan_ScanWorldb   // 72CAE0
    }
}


////////////////////////////////////////////////
//
// CVisibilityPlugins_CalculateFadingAtomicAlpha
//
////////////////////////////////////////////////
int OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre( CBaseModelInfoSAInterface* pModelInfo, CEntitySAInterface* pEntity, float fDist )
{
    if ( pEntity->IsLowLodEntity () )
    {
        // Apply custom distance fade
        float fAlpha = CalculateLowLodFadeAlpha ( pModelInfo, fDist );
        fAlpha *= pModelInfo->ucAlpha;
        return Clamp < int > ( 0, (int)fAlpha, 255 );
    }
    return -1;
}


// Hook info
#define HOOKPOS_CVisibilityPlugins_CalculateFadingAtomicAlpha       0x732500
#define HOOKSIZE_CVisibilityPlugins_CalculateFadingAtomicAlpha      5
DWORD RETURN_CVisibilityPlugins_CalculateFadingAtomicAlpha =        0x732505;
void _declspec(naked) HOOK_CVisibilityPlugins_CalculateFadingAtomicAlpha()
{
    _asm
    {
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        call    OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre
        mov     [esp+0],eax
        add     esp, 4*3
        popad
        mov     eax,[esp-32-4*3]

        cmp     eax, 0xffffffff
        jne     usecustom

        // Use standard code
        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    second
        add     esp, 4*3

usecustom:
        retn

second:
        push    ecx
        mov     ecx, [esp+8]
        jmp     RETURN_CVisibilityPlugins_CalculateFadingAtomicAlpha
    }
}


////////////////////////////////////////////////
//
// CMultiplayerSA::EnableLODSystem
//
// Enable/disable hooks programmatically
//
////////////////////////////////////////////////
void CMultiplayerSA::SetLODSystemEnabled ( bool bEnable )
{
    // Switch
    if ( m_bEnabledLODSystem == bEnable )
        return;
    m_bEnabledLODSystem = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    SHookInfo hookInfoList[] = { MAKE_HOOK_INFO ( CRenderer_SetupEntityVisibility ),
                                 MAKE_HOOK_INFO ( CWorldScan_ScanWorld ),
                                 MAKE_HOOK_INFO ( CVisibilityPlugins_CalculateFadingAtomicAlpha ) };

    // Enable or not?
    if ( bEnable )
    {
        CBufferWriteStream stream ( savedMem );
        for ( uint i = 0 ; i < NUMELMS( hookInfoList ) ; i++ )
        {
            const SHookInfo& hookInfo = hookInfoList[i];
            // Save memory before we blat it
            stream.WriteBytes ( (void*)hookInfo.dwAddress, hookInfo.uiSize );
            // Add hook
            HookInstall ( hookInfo.dwAddress, hookInfo.dwHook, hookInfo.uiSize );
        }
    }
    else
    {
        // Restore memory
        CBufferReadStream stream ( savedMem );
        for ( uint i = 0 ; i < NUMELMS( hookInfoList ) ; i++ )
        {
            const SHookInfo& hookInfo = hookInfoList[i];
            BYTE temp[10];
            assert ( sizeof(temp) >= hookInfo.uiSize );
            stream.ReadBytes ( temp, hookInfo.uiSize );
            MemCpy ( (void*)hookInfo.dwAddress, temp, hookInfo.uiSize );
        }
    }
}

