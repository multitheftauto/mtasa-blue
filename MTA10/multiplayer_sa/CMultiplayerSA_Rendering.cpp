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

namespace
{
    CEntitySAInterface* ms_Rendering = NULL;
    CEntitySAInterface* ms_RenderingOneNonRoad = NULL;
    GameEntityRenderHandler* pGameEntityRenderHandler = NULL;
    bool ms_bIsMinimizedAndNotConnected = false;
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

    if ( pGameEntityRenderHandler && ms_Rendering )
        pGameEntityRenderHandler ( ms_Rendering );
}

void OnMY_CEntity_Render_Post( CEntitySAInterface* pEntity )
{
    if ( ms_Rendering )
    {
        ms_Rendering = NULL;
        if ( pGameEntityRenderHandler )
            pGameEntityRenderHandler ( ms_RenderingOneNonRoad );
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

    if ( pGameEntityRenderHandler )
        pGameEntityRenderHandler ( ms_RenderingOneNonRoad );
}

void OnMY_CEntity_RenderOneNonRoad_Post( DWORD calledFrom, CEntitySAInterface* pEntity )
{
    if ( ms_RenderingOneNonRoad )
    {
        ms_RenderingOneNonRoad = NULL;
        if ( pGameEntityRenderHandler )
            pGameEntityRenderHandler ( ms_RenderingOneNonRoad );
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
    EZHookInstall ( WinLoop );
}
