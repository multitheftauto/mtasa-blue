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
    // Ignore buildings
    if ( pEntity->vtbl == (CEntitySAInterfaceVTBL*)0x08585c8 )
        return;
    // Ignore dummy objects
    if ( pEntity->vtbl == (CEntitySAInterfaceVTBL*)0x0866E78 )
        return;

    ms_Rendering = pEntity;

    if ( ms_Rendering )
        CallGameEntityRenderHandler ( ms_Rendering );
}

void OnMY_CEntity_Render_Post( CEntitySAInterface* pEntity )
{
    if ( ms_Rendering )
    {
        ms_Rendering = NULL;
        CallGameEntityRenderHandler ( ms_RenderingOneNonRoad );
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
        push    ecx
        call    OnMY_CEntity_Render_Post
        add     esp, 4*1
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
void OnMY_CEntity_RenderOneNonRoad_Pre( DWORD calledFrom, CEntitySAInterface* pEntity )
{
    // Ignore buildings
    if ( pEntity->vtbl == (CEntitySAInterfaceVTBL*)0x08585c8 )
        return;
    // Ignore dummy objects
    if ( pEntity->vtbl == (CEntitySAInterfaceVTBL*)0x0866E78 )
        return;

    ms_RenderingOneNonRoad = pEntity;
    CallGameEntityRenderHandler ( ms_RenderingOneNonRoad );
}

void OnMY_CEntity_RenderOneNonRoad_Post( DWORD calledFrom, CEntitySAInterface* pEntity )
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
        push    [esp+32+4*1]
        call    OnMY_CEntity_RenderOneNonRoad_Pre
        add     esp, 4*2
        popad

        push    [esp+4*1]
        call inner
        add     esp, 4*1

        pushad
        push    [esp+32+4*1]
        push    [esp+32+4*1]
        call    OnMY_CEntity_RenderOneNonRoad_Post
        add     esp, 4*2
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
    EZHookInstall ( Check_NoOfVisibleLods );
    EZHookInstall ( Check_NoOfVisibleEntities );
    EZHookInstall ( WinLoop );
    EZHookInstall ( psGrabScreen );
}
