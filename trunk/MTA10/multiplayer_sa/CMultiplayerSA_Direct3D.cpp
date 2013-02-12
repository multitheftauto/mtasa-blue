/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_Direct3D.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
extern CCoreInterface* g_pCore;

namespace
{
    IDirect3D9* ms_pDirect3D    = NULL;
    UINT ms_Adapter             = 0;
    D3DDEVTYPE ms_DeviceType    = D3DDEVTYPE_HAL;
    HWND ms_hFocusWindow        = NULL;
    DWORD ms_BehaviorFlags      = 0;
    D3DPRESENT_PARAMETERS* ms_pPresentationParameters   = NULL;
    IDirect3DDevice9** ms_ppReturnedDeviceInterface     = NULL;
}

////////////////////////////////////////////////////////////////
//
// OnPreCreateDevice
//
// Called before GTA creates the D3D device
//
////////////////////////////////////////////////////////////////
void _cdecl OnPreCreateDevice( IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD* BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    g_pCore->OnPreCreateDevice( pDirect3D, Adapter, DeviceType, hFocusWindow, *BehaviorFlags, pPresentationParameters );
    ms_pDirect3D                 = pDirect3D;
    ms_Adapter                   = Adapter;
    ms_DeviceType                = DeviceType;
    ms_hFocusWindow              = hFocusWindow;
    ms_BehaviorFlags             = *BehaviorFlags;
    ms_pPresentationParameters   = pPresentationParameters;
    ms_ppReturnedDeviceInterface = ppReturnedDeviceInterface;
}

// Hook info
#define HOOKPOS_PreCreateDevice_US          0x07F677A
#define HOOKPOS_PreCreateDevice_EU          0x07F67BA
#define HOOKSIZE_PreCreateDevice_US         6
#define HOOKSIZE_PreCreateDevice_EU         6
DWORD RETURN_PreCreateDevice_US =           0x07F6781;
DWORD RETURN_PreCreateDevice_EU =           0x07F67C1;
DWORD RETURN_PreCreateDevice_BOTH =         0;
void _declspec(naked) HOOK_PreCreateDevice()
{
    _asm
    {
        // Replaced code
        mov     eax, ds:0x0C97C24       // __RwD3DAdapterIndex
        push    eax
        push    ecx

        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        lea     eax,[esp+32+4*6]    // Turn BehaviorFlags into a pointer so we can modify it
        push    eax
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        call    OnPreCreateDevice
        add     esp, 4*7
        popad

        // Continue
        jmp     RETURN_PreCreateDevice_BOTH
    }
}


////////////////////////////////////////////////////////////////
//
// OnPostCreateDevice
//
// Called after GTA creates the D3D device
//
////////////////////////////////////////////////////////////////
HRESULT _cdecl OnPostCreateDevice( HRESULT hResult )
{
    return g_pCore->OnPostCreateDevice( hResult, ms_pDirect3D, ms_Adapter, ms_DeviceType, ms_hFocusWindow, ms_BehaviorFlags, ms_pPresentationParameters, ms_ppReturnedDeviceInterface );
}

// Hook info
#define HOOKPOS_PostCreateDevice_US         0x07F6784
#define HOOKPOS_PostCreateDevice_EU         0x07F67C4
#define HOOKSIZE_PostCreateDevice_US        6
#define HOOKSIZE_PostCreateDevice_EU        6
DWORD RETURN_PostCreateDevice_US =          0x07F678A;
DWORD RETURN_PostCreateDevice_EU =          0x07F67CA;
DWORD RETURN_PostCreateDevice_BOTH =        0;
DWORD RETURN_PostCreateDeviceB_US =         0x07F6799;
DWORD RETURN_PostCreateDeviceB_EU =         0x07F67D9;
DWORD RETURN_PostCreateDeviceB_BOTH =       0;
void _declspec(naked) HOOK_PostCreateDevice()
{
    _asm
    {
        // Replaced code
        pushad
        push    eax
        call    OnPostCreateDevice
        mov     [esp+0],eax
        add     esp, 4*1
        popad
        mov     eax,[esp-32-4*1]

        // Continue
        test    eax, eax
        jge     ok
        xor     eax, eax
        jmp     RETURN_PostCreateDevice_BOTH

ok:
        jmp     RETURN_PostCreateDeviceB_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Direct3D
//
// Setup hook
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Direct3D ( void )
{
    EZHookInstall ( PreCreateDevice );
    EZHookInstall ( PostCreateDevice );
}
