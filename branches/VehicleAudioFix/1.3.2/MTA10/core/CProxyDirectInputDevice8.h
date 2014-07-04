/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirectInputDevice8.h
*  PURPOSE:     Header file for DirectInput 8 device proxy class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPROXYDIRECTINPUTDEVICE_H
#define __CPROXYDIRECTINPUTDEVICE_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class CProxyDirectInputDevice8 : public IDirectInputDevice8A
{
    public:

                                 CProxyDirectInputDevice8   ( IDirectInputDevice8A * pDevice );
                                ~CProxyDirectInputDevice8   ( );
    /*** IUnknown methods ***/
    virtual HRESULT __stdcall    QueryInterface             ( REFIID riid,  LPVOID * ppvObj );
    virtual ULONG   __stdcall    AddRef                     ( VOID );
    virtual ULONG   __stdcall    Release                    ( VOID );

    /*** IDirectInputDevice8A methods ***/
    virtual HRESULT __stdcall    GetCapabilities            ( LPDIDEVCAPS a );
    virtual HRESULT __stdcall    EnumObjects                ( LPDIENUMDEVICEOBJECTSCALLBACKA a, LPVOID b, DWORD c );
    virtual HRESULT __stdcall    GetProperty                ( REFGUID a, LPDIPROPHEADER b );
    virtual HRESULT __stdcall    SetProperty                ( REFGUID a, LPCDIPROPHEADER b );
    virtual HRESULT __stdcall    Acquire                    ( VOID );
    virtual HRESULT __stdcall    Unacquire                  ( VOID );
    virtual HRESULT __stdcall    GetDeviceState             ( DWORD a, LPVOID b );
    virtual HRESULT __stdcall    GetDeviceData              ( DWORD a, LPDIDEVICEOBJECTDATA b, LPDWORD c, DWORD d );
    virtual HRESULT __stdcall    SetDataFormat              ( LPCDIDATAFORMAT a );
    virtual HRESULT __stdcall    SetEventNotification       ( HANDLE a );
    virtual HRESULT __stdcall    SetCooperativeLevel        ( HWND a, DWORD b );
    virtual HRESULT __stdcall    GetObjectInfo              ( LPDIDEVICEOBJECTINSTANCEA a, DWORD b, DWORD c );
    virtual HRESULT __stdcall    GetDeviceInfo              ( LPDIDEVICEINSTANCEA a );
    virtual HRESULT __stdcall    RunControlPanel            ( HWND a, DWORD b );
    virtual HRESULT __stdcall    Initialize                 ( HINSTANCE a, DWORD b, REFGUID c );
    virtual HRESULT __stdcall    CreateEffect               ( REFGUID a, LPCDIEFFECT b, LPDIRECTINPUTEFFECT * c, LPUNKNOWN d );
    virtual HRESULT __stdcall    EnumEffects                ( LPDIENUMEFFECTSCALLBACKA a, LPVOID b, DWORD c );
    virtual HRESULT __stdcall    GetEffectInfo              ( LPDIEFFECTINFOA a, REFGUID b );
    virtual HRESULT __stdcall    GetForceFeedbackState      ( LPDWORD a );
    virtual HRESULT __stdcall    SendForceFeedbackCommand   ( DWORD a );
    virtual HRESULT __stdcall    EnumCreatedEffectObjects   ( LPDIENUMCREATEDEFFECTOBJECTSCALLBACK a, LPVOID b, DWORD c );
    virtual HRESULT __stdcall    Escape                     ( LPDIEFFESCAPE a );
    virtual HRESULT __stdcall    Poll                       ( VOID );
    virtual HRESULT __stdcall    SendDeviceData             ( DWORD a, LPCDIDEVICEOBJECTDATA b, LPDWORD c, DWORD d );
    virtual HRESULT __stdcall    EnumEffectsInFile          ( LPCSTR a, LPDIENUMEFFECTSINFILECALLBACK b, LPVOID c, DWORD d );
    virtual HRESULT __stdcall    WriteEffectToFile          ( LPCSTR a, DWORD b, LPDIFILEEFFECT c, DWORD d );
    virtual HRESULT __stdcall    BuildActionMap             ( LPDIACTIONFORMATA a, LPCSTR b, DWORD c );
    virtual HRESULT __stdcall    SetActionMap               ( LPDIACTIONFORMATA a, LPCSTR b, DWORD c );
    virtual HRESULT __stdcall    GetImageInfo               ( LPDIDEVICEIMAGEINFOHEADERA a);

    private:

    IDirectInputDevice8A *   m_pDevice;
    bool                     m_bDropDataIfInputGoesToGUI;
};

#endif


