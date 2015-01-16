/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_Rendering.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
extern CCoreInterface* g_pCore;
GameEntityRenderHandler* pGameEntityRenderHandler = NULL;
PreRenderSkyHandler* pPreRenderSkyHandlerHandler = NULL;

#define VAR_CCullZones_NumMirrorAttributeZones  0x0C87AC4   // int
#define VAR_CMirrors_d3dRestored                0x0C7C729   // uchar

namespace
{
    CEntitySAInterface* ms_Rendering = NULL;
    CEntitySAInterface* ms_RenderingOneNonRoad = NULL;
    bool ms_bIsMinimizedAndNotConnected = false;
    int ms_iSavedNumMirrorZones = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CallIdle
//
// Profile call to function 'Idle'
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CallIdle_Pre( DWORD calledFrom )
{
    TIMING_CHECKPOINT( "+CallIdle1" );
}

void OnMY_CallIdle_Post( RwGeometry* pGeometry, DWORD calledFrom )
{
    TIMING_CHECKPOINT( "-CallIdle2" );
}

// Hook info
#define HOOKPOS_CallIdle                         0x53ECBD
#define HOOKSIZE_CallIdle                        5
DWORD RETURN_CallIdle =                          0x53ECC2;
DWORD DO_CallIdle =                          0x53E920;
void _declspec(naked) HOOK_CallIdle()
{
    _asm
    {
        pushad
        call    OnMY_CallIdle_Pre
        popad

        push    [esp+4*1]
        call    DO_CallIdle
        add     esp, 4*1

        pushad
        call    OnMY_CallIdle_Post
        popad

        jmp     RETURN_CallIdle
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::Render
//
// Detect entity rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CEntity_Render_Pre( CEntitySAInterface* pEntity )
{
    ms_Rendering = pEntity;

    if ( ms_Rendering )
        CallGameEntityRenderHandler ( ms_Rendering );
}

void OnMY_CEntity_Render_Post( void )
{
    if ( ms_Rendering )
    {
        ms_Rendering = NULL;
        CallGameEntityRenderHandler ( ms_RenderingOneNonRoad ); // restore value set in RenderOneNonRoad
    }
}

// Hook info
#define HOOKPOS_CEntity_Render                         0x534310
#define HOOKSIZE_CEntity_Render                        6
DWORD RETURN_CEntity_Render =                          0x534317;
void _declspec(naked) HOOK_CEntity_Render()
{
    _asm
    {
        pushad
        push    ecx
        call    OnMY_CEntity_Render_Pre
        add     esp, 4*1
        popad

        call inner

        pushad
        call    OnMY_CEntity_Render_Post
        popad
        retn

inner:
        push    ecx  
        push    esi  
        mov     esi,ecx 
        mov     eax,dword ptr [esi+18h] 
        jmp     RETURN_CEntity_Render
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::RenderOneNonRoad
//
// Detect entity rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CEntity_RenderOneNonRoad_Pre( CEntitySAInterface* pEntity )
{
    ms_RenderingOneNonRoad = pEntity;
    CallGameEntityRenderHandler ( ms_RenderingOneNonRoad );
}

void OnMY_CEntity_RenderOneNonRoad_Post( CEntitySAInterface* pEntity )
{
    if ( ms_RenderingOneNonRoad )
    {
        ms_RenderingOneNonRoad = NULL;
        CallGameEntityRenderHandler ( ms_RenderingOneNonRoad );
    }
}

// Hook info
#define HOOKPOS_CEntity_RenderOneNonRoad                         0x553260
#define HOOKSIZE_CEntity_RenderOneNonRoad                        5
DWORD RETURN_CEntity_RenderOneNonRoad =                          0x553265;
void _declspec(naked) HOOK_CEntity_RenderOneNonRoad()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        call    OnMY_CEntity_RenderOneNonRoad_Pre
        add     esp, 4*1
        popad

        push    [esp+4*1]
        call inner
        add     esp, 4*1

        pushad
        push    [esp+32+4*1]
        call    OnMY_CEntity_RenderOneNonRoad_Post
        add     esp, 4*1
        popad
        retn

inner:
        push    esi  
        mov     esi, [esp+08h]
        jmp     RETURN_CEntity_RenderOneNonRoad
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVisibilityPlugin::RenderWeaponPedsForPC_Mid
//
// Detect next ped weapon rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_Mid( CPedSAInterface* pEntity )
{
    CallGameEntityRenderHandler( pEntity );
}

// Hook info
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC_Mid                0x733080
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC_Mid               6
DWORD RETURN_CVisibilityPlugins_RenderWeaponPedsForPC_Mid =                 0x733086;
void _declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC_Mid()
{
    _asm
    {
        pushad
        push    ebx
        call    OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_Mid
        add     esp, 4*1
        popad

        // Continue original code
        mov     ecx,dword ptr [ebx+4F4h]
        jmp     RETURN_CVisibilityPlugins_RenderWeaponPedsForPC_Mid
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVisibilityPlugin::RenderWeaponPedsForPC_End
//
// End of all ped weapon rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_End( void )
{
    CallGameEntityRenderHandler( NULL );
}

// Hook info
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC_End                0x73314D
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC_End               5
void _declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC_End()
{
    _asm
    {
        pushad
        call    OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_End
        popad

        // Continue original code
        pop         esi  
        add         esp,0Ch 
        ret
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Check_NoOfVisibleLods
//
// Apply render limits
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_Check_NoOfVisibleLods                         0x5534F9
#define HOOKSIZE_Check_NoOfVisibleLods                        6
DWORD RETURN_Check_NoOfVisibleLods =                          0x5534FF;
void _declspec(naked) HOOK_Check_NoOfVisibleLods()
{
    _asm
    {
        cmp     eax, 999            // Array limit is 1000
        jge     limit
        inc     eax
limit:
        mov     dword ptr ds:[00B76840h],eax        // NoOfVisibleLods
        jmp     RETURN_Check_NoOfVisibleLods
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Check_NoOfVisibleEntities
//
// Apply render limits
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_Check_NoOfVisibleEntities                         0x55352D
#define HOOKSIZE_Check_NoOfVisibleEntities                        6
DWORD RETURN_Check_NoOfVisibleEntities =                          0x553533;
void _declspec(naked) HOOK_Check_NoOfVisibleEntities()
{
    _asm
    {
        cmp     eax, 999        // Array limit is 1000
        jge     limit
        inc     eax
limit:
        mov     dword ptr ds:[00B76844h],eax        // NoOfVisibleEntities
        jmp     RETURN_Check_NoOfVisibleEntities
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// WinLoop
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_WinLoop( void )
{
    if ( ms_bIsMinimizedAndNotConnected )
        Sleep ( 10 );
}

// Hook info
#define HOOKPOS_WinLoop_US                         0x748A93
#define HOOKPOS_WinLoop_EU                         0x748AE3
#define HOOKSIZE_WinLoop_US                        5
#define HOOKSIZE_WinLoop_EU                        5
DWORD RETURN_WinLoop_US =                          0x748A98;
DWORD RETURN_WinLoop_EU =                          0x748AE8;
DWORD RETURN_WinLoop_BOTH =                        0;
void _declspec(naked) HOOK_WinLoop ()
{
    _asm
    {
        pushad
        call    OnMY_WinLoop
        popad

        mov     eax, ds:0x0C8D4C0
        jmp     RETURN_WinLoop_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Photograph screen grab in windowed mode
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_psGrabScreen_GetRect( HWND hWnd, LPRECT pRect )
{
    // Get the window client area
    GetClientRect( hWnd, pRect );
    LPPOINT pPoints = (LPPOINT)pRect;
    ClientToScreen( hWnd, pPoints );
    ClientToScreen( hWnd, pPoints + 1 );

    // Clip to desktop
    RECT desktopRect;
    GetWindowRect( GetDesktopWindow(), &desktopRect );
    pRect->left = Max( pRect->left, desktopRect.left );
    pRect->top = Max( pRect->top, desktopRect.top );
    pRect->right = Min( pRect->right, desktopRect.right );
    pRect->bottom = Min( pRect->bottom, desktopRect.bottom );

    // Ensure at least 1 pixel 
    pRect->bottom = Max( pRect->bottom, pRect->top + 1 );
    pRect->right = Max( pRect->right, pRect->left + 1 );
}

bool OnMY_psGrabScreen_ShouldUseRect( void )
{
    bool bWindowed;
    g_pCore->GetCVars()->Get( "display_windowed", bWindowed );
    return bWindowed;
}

// Hook info
#define HOOKPOS_psGrabScreen                        0x7452FC
#define HOOKSIZE_psGrabScreen                       5
DWORD RETURN_psGrabScreen_YesChange =               0x745311;
DWORD RETURN_psGrabScreen_NoChange =                0x745336;
void _declspec(naked) HOOK_psGrabScreen ()
{
    _asm
    {
        pushad
        call    OnMY_psGrabScreen_ShouldUseRect
        test    al, al
        jnz     use_rect
        popad
        jmp     RETURN_psGrabScreen_NoChange

use_rect:
        popad
        mov     edx, [eax]      // hwnd
        lea     ecx, [esp+24h]  // pRect result
        pushad
        push    ecx
        push    edx
        call    OnMY_psGrabScreen_GetRect
        add     esp, 4*2
        popad

        jmp     RETURN_psGrabScreen_YesChange
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CClouds::RenderSkyPolys
//
// This is the first thing drawn by GTA
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CClouds_RenderSkyPolys( void )
{
    if ( pPreRenderSkyHandlerHandler )
        pPreRenderSkyHandlerHandler();
}

// Hook info
#define HOOKCHECK_CClouds_RenderSkyPolys            0xA1
#define HOOKPOS_CClouds_RenderSkyPolys              0x714650
#define HOOKSIZE_CClouds_RenderSkyPolys             5
DWORD RETURN_CClouds_RenderSkyPolys =               0x714655;
void _declspec(naked) HOOK_CClouds_RenderSkyPolys ()
{
    _asm
    {
        pushad
        call    OnMY_CClouds_RenderSkyPolys
        popad

        mov     eax, ds:0x0B6F03C
        jmp     RETURN_CClouds_RenderSkyPolys
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwCameraSetNearClipPlane
//
// Called multiple times, with varying distances from 2.0f down to 0.05f
//  - The lower values are used when near walls and peds
//  - The higher values are used to reduce z-flicker when flying etc
//
//////////////////////////////////////////////////////////////////////////////////////////
float OnMY_RwCameraSetNearClipPlane( DWORD dwCalledFrom, void* pUnknown, float fDistance )
{
    float fSetting = pMultiplayer->GetNearClipDistance();
    if ( fSetting == DEFAULT_NEAR_CLIP_DISTANCE )
    {
        // Do nothing if setting is default value
        return fDistance;
    }

    // Don't process calls from RenderScene as they are restoring saved values (which have already been processed here)
    if ( dwCalledFrom > 0x53DF40 && dwCalledFrom < 0x53E160 )
    {
        return fDistance;
    }

    if ( fSetting < DEFAULT_NEAR_CLIP_DISTANCE )
    {
        // If required setting is lower than default, ensure value used is not higher.
        return Min( fSetting, fDistance );
    }
    else
    {
        // If required setting is higher than default, converge value towards it.
        float fAlpha = UnlerpClamped( DEFAULT_NEAR_CLIP_DISTANCE, fSetting, DEFAULT_NEAR_CLIP_DISTANCE * 3 );
        return Lerp( fDistance, fAlpha, fSetting );
    }
}

// Hook info
#define HOOKCHECK_RwCameraSetNearClipPlane_US       0xD9
#define HOOKCHECK_RwCameraSetNearClipPlane_EU       0xD9
#define HOOKPOS_RwCameraSetNearClipPlane_US         0x7EE1D0
#define HOOKPOS_RwCameraSetNearClipPlane_EU         0x7EE210
#define HOOKSIZE_RwCameraSetNearClipPlane_US        5
#define HOOKSIZE_RwCameraSetNearClipPlane_EU        5
DWORD RETURN_RwCameraSetNearClipPlane_US =          0x7EE1D5;
DWORD RETURN_RwCameraSetNearClipPlane_EU =          0x7EE215;
DWORD RETURN_RwCameraSetNearClipPlane_BOTH =        0;
void _declspec(naked) HOOK_RwCameraSetNearClipPlane ()
{
    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_RwCameraSetNearClipPlane
        add     esp, 4*3
        popad

        // Result is on fp stack
        //fld     [esp+8]
        push    esi
        jmp     RETURN_RwCameraSetNearClipPlane_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetGameEntityRenderHandler
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetGameEntityRenderHandler ( GameEntityRenderHandler * pHandler )
{
    pGameEntityRenderHandler = pHandler;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetPreRenderSkyHandler
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetPreRenderSkyHandler ( PreRenderSkyHandler * pHandler )
{
    pPreRenderSkyHandlerHandler = pHandler;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetIsMinimizedAndNotConnected
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetIsMinimizedAndNotConnected ( bool bIsMinimizedAndNotConnected )
{
    ms_bIsMinimizedAndNotConnected = bIsMinimizedAndNotConnected;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetMirrorsEnabled
//
// Stop mirrors by skipping the frame setup and rendering code.
// Forces mirror render buffer recreation when enabling.
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetMirrorsEnabled ( bool bEnabled )
{
    int& iNumMirrorZones = *(int*)VAR_CCullZones_NumMirrorAttributeZones;
    uchar& bResetMirror = *(uchar*)VAR_CMirrors_d3dRestored;

    if ( !bEnabled )
    {
        // Remove mirror zones
        ms_iSavedNumMirrorZones += iNumMirrorZones;
        iNumMirrorZones = 0;
    }
    else
    {
        if ( ms_iSavedNumMirrorZones )
        {
            // Restore mirror zones
            iNumMirrorZones += ms_iSavedNumMirrorZones;
            ms_iSavedNumMirrorZones = 0;

            // Recreate render buffers
            bResetMirror = true;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Rendering
//
// Setup hook
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Rendering ( void )
{
    EZHookInstall ( CallIdle );
    EZHookInstall ( CEntity_Render );
    EZHookInstall ( CEntity_RenderOneNonRoad );
    EZHookInstall ( CVisibilityPlugins_RenderWeaponPedsForPC_Mid );
    EZHookInstall ( CVisibilityPlugins_RenderWeaponPedsForPC_End );
    EZHookInstall ( Check_NoOfVisibleLods );
    EZHookInstall ( Check_NoOfVisibleEntities );
    EZHookInstall ( WinLoop );
    EZHookInstall ( psGrabScreen );
    EZHookInstallChecked ( CClouds_RenderSkyPolys );
    EZHookInstallChecked ( RwCameraSetNearClipPlane );
}
