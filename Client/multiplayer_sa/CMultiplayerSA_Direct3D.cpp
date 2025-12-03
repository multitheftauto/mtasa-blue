/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Direct3D.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    bool                   ms_hasDeviceArgs = false;
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
    // Safely unpatch with validation
    if (RESTORE_Addr_PreCreateDevice && 
        RESTORE_Size_PreCreateDevice > 0 && 
        RESTORE_Size_PreCreateDevice <= sizeof(RESTORE_Bytes_PreCreateDevice))
    {
        MemCpy((PVOID)RESTORE_Addr_PreCreateDevice, RESTORE_Bytes_PreCreateDevice, RESTORE_Size_PreCreateDevice);
    }

    // Validate critical parameters before dereferencing
    if (BehaviorFlags && pPresentationParameters && ppReturnedDeviceInterface)
    {
        ms_pDirect3D = pDirect3D;
        ms_Adapter = Adapter;
        ms_DeviceType = DeviceType;
        ms_hFocusWindow = hFocusWindow;
        ms_BehaviorFlags = *BehaviorFlags;
        ms_pPresentationParameters = pPresentationParameters;
        ms_ppReturnedDeviceInterface = ppReturnedDeviceInterface;
        ms_hasDeviceArgs = true;
    }
    else
    {
        ms_hasDeviceArgs = false;
        SString message;
        message = "OnPreCreateDevice: missing device arguments for alt startup path";
        AddReportLog(8740, message);
    }
}

// Hook info
#define HOOKPOS_PreCreateDevice             0x007F675B
#define HOOKSIZE_PreCreateDevice            6
DWORD RETURN_PreCreateDevice = 0x07F6781;
void _declspec(naked) HOOK_PreCreateDevice()
{
    _asm
    {
        // Run replaced code - these pushes create the original function parameters
        mov     ecx,dword ptr ds:[0C97C20h]        // pDirect3D
        push    0C97C28h                           // ppReturnedDeviceInterface
        push    0C9C040h                           // pPresentationParameters  
        push    eax                                // BehaviorFlags (original eax)
        mov     eax,dword ptr ds:[00C97C1Ch]       
        mov     edx,  [ecx]
        push    eax                                // hFocusWindow
        mov     eax,dword ptr ds:[008E2428h]
        push    eax                                // DeviceType

        mov     eax, ds:0x0C97C24                  // __RwD3DAdapterIndex
        push    eax                                // Adapter
        push    ecx                                // pDirect3D

        // Now we have 7 parameters on stack (28 bytes)
        // Stack layout at ESP: [pDirect3D][Adapter][DeviceType][hFocusWindow][BehaviorFlags][pPresentationParameters][ppReturnedDeviceInterface]

        pushad  // Save all registers (32 bytes), ESP now at ESP-32
        
        // Pass parameters to OnPreCreateDevice
        // After pushad, params start at ESP+32. Each push decreases ESP by 4,
        // so [esp+32+4*6] effectively walks backward through the params:
        // 1st access: ESP+56 = ppReturnedDeviceInterface, then ESP -= 4
        // 2nd access: ESP+56 = pPresentationParameters (was at ESP+52), etc.
        push    [esp+32+4*6]                       // ppReturnedDeviceInterface
        push    [esp+32+4*6]                       // pPresentationParameters
        lea     eax,[esp+32+4*6]                   // BehaviorFlags as pointer
        push    eax
        push    [esp+32+4*6]                       // hFocusWindow
        push    [esp+32+4*6]                       // DeviceType
        push    [esp+32+4*6]                       // Adapter
        push    [esp+32+4*6]                       // pDirect3D
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
    if (!g_pCore)
    {
        ms_hasDeviceArgs = false;
        return hResult;
    }

    if (!ms_hasDeviceArgs)
    {
        SString message;
        message = "OnPostCreateDevice: device arguments were not captured; skipping alt startup logic";
        AddReportLog(8741, message);
        return hResult;
    }

    if (!SharedUtil::IsReadablePointer(ms_ppReturnedDeviceInterface, sizeof(*ms_ppReturnedDeviceInterface)))
    {
        SString message;
        message.Format("OnPostCreateDevice: invalid device pointer reference %p", ms_ppReturnedDeviceInterface);
        AddReportLog(8742, message);
        ms_hasDeviceArgs = false;
        return hResult;
    }

    if (!SharedUtil::IsReadablePointer(ms_pPresentationParameters, sizeof(*ms_pPresentationParameters)))
    {
        SString message;
        message.Format("OnPostCreateDevice: invalid presentation parameters pointer %p", ms_pPresentationParameters);
        AddReportLog(8743, message);
        ms_hasDeviceArgs = false;
        return hResult;
    }

    HRESULT result = g_pCore->OnPostCreateDevice(hResult, ms_pDirect3D, ms_Adapter, ms_DeviceType, ms_hFocusWindow, ms_BehaviorFlags, ms_pPresentationParameters,
                                                 ms_ppReturnedDeviceInterface);
    ms_hasDeviceArgs = false;
    return result;
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
