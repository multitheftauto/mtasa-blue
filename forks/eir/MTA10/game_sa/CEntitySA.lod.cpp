/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.lod.cpp
*  PURPOSE:     Entity custom level of detail management
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility helpers
//
////////////////////////////////////////////////
namespace
{
    #define     ARRAY_ModelInfo                 0xA9B0C8
    #define     LOW_LOD_DRAW_DISTANCE_SCALE     5


    // This function actually modifies a read-only data value.
    // Hopefully, we are not causing bugs with this.
    void SetGlobalDrawDistanceScale ( float fValue )
    {
        EntityRender::SetGlobalDrawDistanceScale( fValue );
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


////////////////////////////////////////////////
//
// CWorldScan_ScanWorld
//
// TODO: rewrite the world scanning algorithm, so we know what it does.
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
int OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre( CBaseModelInfoSAInterface* pModelInfo, const CEntitySAInterface* pEntity, float fDist )
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

////////////////////////////////////////////////
//
// CGameSA::EnableLODSystem
//
// Enable/disable hooks programmatically
//
////////////////////////////////////////////////
void CGameSA::SetLODSystemEnabled ( bool bEnable )
{
    // Switch
    if ( m_bEnabledLODSystem == bEnable )
        return;
    m_bEnabledLODSystem = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    SHookInfo hookInfoList[] = { MAKE_HOOK_INFO ( CWorldScan_ScanWorld ) };

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