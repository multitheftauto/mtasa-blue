/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirectInput8.h
*  PURPOSE:     Header file for DirectInput 8 proxy class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPROXYDIRECTINPUT_H
#define __CPROXYDIRECTINPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "CProxyDirectInputDevice8.h"

class CProxyDirectInput8 : public IDirectInput8A
{
    public:

                                 CProxyDirectInput8 ( IDirectInput8A * pDevice );
                                ~CProxyDirectInput8 ( );
    /*** IUnknown methods ***/
    virtual HRESULT __stdcall    QueryInterface ( REFIID riid,  LPVOID * ppvObj );
    virtual ULONG   __stdcall    AddRef ( VOID );
    virtual ULONG   __stdcall    Release ( VOID );

    /*** IDirectInput8A methods ***/
    virtual HRESULT __stdcall    CreateDevice ( REFGUID, LPDIRECTINPUTDEVICE8A *, LPUNKNOWN );
    virtual HRESULT __stdcall    EnumDevices ( DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD );
    virtual HRESULT __stdcall    GetDeviceStatus ( REFGUID );
    virtual HRESULT __stdcall    RunControlPanel ( HWND, DWORD );
    virtual HRESULT __stdcall    Initialize ( HINSTANCE, DWORD );
    virtual HRESULT __stdcall    FindDevice ( REFGUID, LPCSTR, LPGUID );
    virtual HRESULT __stdcall    EnumDevicesBySemantics ( LPCSTR, LPDIACTIONFORMATA, LPDIENUMDEVICESBYSEMANTICSCBA, LPVOID, DWORD );
    virtual HRESULT __stdcall    ConfigureDevices ( LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMSA,DWORD,LPVOID );

    private:

    IDirectInput8A *    m_pDevice;
    DWORD               m_dwRefCount;
};

#endif