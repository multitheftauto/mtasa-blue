/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirectXHook.h
*  PURPOSE:     Header file for global DirectX hooking declarations
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECTXHOOK_H
#define __CDIRECTXHOOK_H

#include <windows.h>

enum D3DHOOKTYPE
{
    D3DHOOK_D3D9,
    D3DHOOK_D3D8,
    D3DHOOK_UNKNOWN
};

enum DINPUTHOOKTYPE
{
    DINPUTHOOK_DINPUT8,
    DINPUTHOOK_UNKNOWN
};

typedef IUnknown * ( __stdcall * pDirect3DCreate )    ( int SDKVersion );

typedef HRESULT    ( __stdcall * pDirectInputCreate ) ( HINSTANCE   hinst, 
                                                        DWORD       dwVersion,
                                                        REFIID      riidltf,
                                                        LPVOID*     ppvOut,
                                                        LPUNKNOWN   punkOuter );

class CDirectXHook 
{
    public:

    // Hook utilities.
    virtual bool        ApplyHook           ( ) =0;
    virtual bool        RemoveHook          ( ) =0;

    IUnknown *  m_pDevice;
};

#endif
