/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirect3D8.h
*  PURPOSE:     Header file for Direct3D 8 proxy class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPROXYDIRECT3D8_H
#define __CPROXYDIRECT3D8_H

#include <d3d8.h>

class CProxyDirect3D8 : public IDirect3D8
{
    public:

    // CProxy* specific methods.
                                    CProxyDirect3D8             ( IDirect3D8* pInterface );
                                   ~CProxyDirect3D8             ( );
    // IUnknown Methods.
    virtual HRESULT     __stdcall   QueryInterface              ( REFIID riid, void** ppvObj );
    virtual ULONG       __stdcall   AddRef                      ( VOID );
    virtual ULONG       __stdcall   Release                     ( VOID );

    // IDirect3D8 Methods.
    virtual HRESULT     __stdcall   RegisterSoftwareDevice      ( void* pInitializeFunction);
    virtual UINT        __stdcall   GetAdapterCount             ( VOID );
    virtual HRESULT     __stdcall   GetAdapterIdentifier        ( UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier );
    virtual UINT        __stdcall   GetAdapterModeCount         ( UINT Adapter );
    virtual HRESULT     __stdcall   EnumAdapterModes            ( UINT Adapter, UINT Mode, D3DDISPLAYMODE* pMode );
    virtual HRESULT     __stdcall   GetAdapterDisplayMode       ( UINT Adapter, D3DDISPLAYMODE* pMode );
    virtual HRESULT     __stdcall   CheckDeviceType             ( UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed );
    virtual HRESULT     __stdcall   CheckDeviceFormat           ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat );
    virtual HRESULT     __stdcall   CheckDeviceMultiSampleType  ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType );
    virtual HRESULT     __stdcall   CheckDepthStencilMatch      ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat );
    virtual HRESULT     __stdcall   GetDeviceCaps               ( UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8* pCaps );
    virtual HMONITOR    __stdcall   GetAdapterMonitor           ( UINT Adapter );
    virtual HRESULT     __stdcall   CreateDevice                ( UINT Adapter, D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface );

    private:

    IDirect3D8*     m_pDevice;
    DWORD           m_dwRefCount;

};

#endif