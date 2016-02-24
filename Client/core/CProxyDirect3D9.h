/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirect3D9.h
*  PURPOSE:     Header file for Direct3D 9 proxy class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPROXYDIRECT3D9_H
#define __CPROXYDIRECT3D9_H

#include <d3d9.h>
#include "CProxyDirect3DDevice9.h"

class CProxyDirect3D9 : public IDirect3D9
{
    public:
                                    CProxyDirect3D9             ( IDirect3D9 * pDevice );
                                   ~CProxyDirect3D9             ( );
    /*** IUnknown methods ***/
    virtual HRESULT __stdcall       QueryInterface              ( REFIID riid, void** ppvObj );
    virtual ULONG   __stdcall       AddRef                      ( VOID );
    virtual ULONG   __stdcall       Release                     ( VOID );

    /*** IDirect3D9 methods ***/
    virtual HRESULT  __stdcall      RegisterSoftwareDevice      ( void* pInitializeFunction );
    virtual UINT     __stdcall      GetAdapterCount             ( VOID );
    virtual HRESULT  __stdcall      GetAdapterIdentifier        ( UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier );
    virtual UINT     __stdcall      GetAdapterModeCount         ( UINT Adapter, D3DFORMAT Format );
    virtual HRESULT  __stdcall      EnumAdapterModes            ( UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode );
    virtual HRESULT  __stdcall      GetAdapterDisplayMode       ( UINT Adapter, D3DDISPLAYMODE* pMode );
    virtual HRESULT  __stdcall      CheckDeviceType             ( UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed );
    virtual HRESULT  __stdcall      CheckDeviceFormat           ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat );
    virtual HRESULT  __stdcall      CheckDeviceMultiSampleType  ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels );
    virtual HRESULT  __stdcall      CheckDepthStencilMatch      ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat );
    virtual HRESULT  __stdcall      CheckDeviceFormatConversion ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat );
    virtual HRESULT  __stdcall      GetDeviceCaps               ( UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps );
    virtual HMONITOR __stdcall      GetAdapterMonitor           ( UINT Adapter );
    virtual HRESULT  __stdcall      CreateDevice                ( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface );

    static HMONITOR                 StaticGetAdapterMonitor     ( UINT Adapter );
    static IDirect3D9*              StaticGetDirect3D           ( void );

    private:

    IDirect3D9 *    m_pDevice;
};

#endif