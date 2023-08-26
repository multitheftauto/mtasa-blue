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
    IDirect3D9*            ms_pDirect3D = NULL;
    UINT                   ms_Adapter = 0;
    D3DDEVTYPE             ms_DeviceType = D3DDEVTYPE_HAL;
    HWND                   ms_hFocusWindow = NULL;
    DWORD                  ms_BehaviorFlags = 0;
    D3DPRESENT_PARAMETERS* ms_pPresentationParameters = NULL;
    IDirect3DDevice9**     ms_ppReturnedDeviceInterface = NULL;
}            // namespace

DWORD RESTORE_Addr_PreCreateDevice;
DWORD RESTORE_Size_PreCreateDevice;
BYTE  RESTORE_Bytes_PreCreateDevice[6];

////////////////////////////////////////////////////////////////
//
// OnPreCreateDevice
//
// Called before GTA creates the D3D device
//
////////////////////////////////////////////////////////////////
void _cdecl OnPreCreateDevice(IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD* BehaviorFlags,
                              D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    // Unpatch
    MemCpy((PVOID)RESTORE_Addr_PreCreateDevice, RESTORE_Bytes_PreCreateDevice, RESTORE_Size_PreCreateDevice);

    // g_pCore->OnPreCreateDevice( pDirect3D, Adapter, DeviceType, hFocusWindow, *BehaviorFlags, pPresentationParameters );
    ms_pDirect3D = pDirect3D;
    ms_Adapter = Adapter;
    ms_DeviceType = DeviceType;
    ms_hFocusWindow = hFocusWindow;
    ms_BehaviorFlags = *BehaviorFlags;
    ms_pPresentationParameters = pPresentationParameters;
    ms_ppReturnedDeviceInterface = ppReturnedDeviceInterface;
}

// Hook info
#define HOOKPOS_PreCreateDevice             0x007F675B
#define HOOKSIZE_PreCreateDevice            6
DWORD RETURN_PreCreateDevice = 0x07F6781;
void _declspec(naked) HOOK_PreCreateDevice()
{
    _asm
    {
        // Run replaced code
        mov     ecx,dword ptr ds:[0C97C20h]
        push    0C97C28h
        push    0C9C040h
        push    eax
        mov     eax,dword ptr ds:[00C97C1Ch]
        mov     edx,  [ecx]
        push    eax
        mov     eax,dword ptr ds:[008E2428h]
        push    eax

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
        jmp     RETURN_PreCreateDevice
    }
}

////////////////////////////////////////////////////////////////
//
// OnPostCreateDevice
//
// Called after GTA creates the D3D device
//
////////////////////////////////////////////////////////////////
HRESULT _cdecl OnPostCreateDevice(HRESULT hResult)
{
    return g_pCore->OnPostCreateDevice(hResult, ms_pDirect3D, ms_Adapter, ms_DeviceType, ms_hFocusWindow, ms_BehaviorFlags, ms_pPresentationParameters,
                                       ms_ppReturnedDeviceInterface);
}

// Hook info
#define HOOKPOS_PostCreateDevice            0x07F6784
#define HOOKSIZE_PostCreateDevice           6
DWORD RETURN_PostCreateDevice = 0x07F678A;
DWORD RETURN_PostCreateDeviceB = 0x07F6799;
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
        jmp     RETURN_PostCreateDevice

ok:
        jmp     RETURN_PostCreateDeviceB
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Direct3D
//
// Setup hook
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Direct3D()
{
    EZHookInstallRestore(PreCreateDevice);
    EZHookInstall(PostCreateDevice);
}
