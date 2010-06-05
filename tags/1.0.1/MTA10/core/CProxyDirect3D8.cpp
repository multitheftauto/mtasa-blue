/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirect3D8.cpp
*  PURPOSE:     Direct3D 8 function hooking proxy
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#ifdef COMPILE_FOR_VC

#include "CProxyDirect3D8.h"
#include "CLogger.h"
#include <d3d8.h>

// CProxy* specific methods.
CProxyDirect3D8::CProxyDirect3D8 ( IDirect3D8* pInterface )
{
    WriteDebugEvent ( "CProxyDirect3D8::CProxyDirect3D8" );
    m_pDevice       = pInterface;
    m_dwRefCount    = 1;
}

CProxyDirect3D8::~CProxyDirect3D8 ( )
{
    WriteDebugEvent ( "CProxyDirect3D8::~CProxyDirect3D8" );
    m_pDevice       = NULL;
    m_dwRefCount    = 1;

}

// IUnknown Methods.
HRESULT     CProxyDirect3D8::QueryInterface              ( REFIID riid, void** ppvObj )
{
    return m_pDevice->QueryInterface ( riid, ppvObj ) ;
}

ULONG       CProxyDirect3D8::AddRef                      ( VOID )
{
    // Incrase refcount.
    m_dwRefCount++;

    return m_pDevice->AddRef ( );
}

ULONG       CProxyDirect3D8::Release                     ( VOID )
{
    ULONG ulRefCount;

    m_dwRefCount -= 1;

    // Check to see if we should self-destruct
    if ( m_dwRefCount == 0 )
    {
        WriteDebugEvent ( "Releasing IDirect3D8 Proxy..." );

        do
        {
            // Call device's release function and get refcount.
            ulRefCount = m_pDevice->Release ( );
        } while ( ulRefCount != 0 );

        delete this;
    }

    return m_dwRefCount;
}

// IDirect3D8 Methods.
HRESULT     CProxyDirect3D8::RegisterSoftwareDevice      ( void* pInitializeFunction)
{
    return m_pDevice->RegisterSoftwareDevice ( pInitializeFunction );
}

UINT        CProxyDirect3D8::GetAdapterCount             ( VOID )
{
    return m_pDevice->GetAdapterCount ( );
}

HRESULT     CProxyDirect3D8::GetAdapterIdentifier        ( UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier )
{
    return m_pDevice->GetAdapterIdentifier ( Adapter, Flags, pIdentifier );
}

UINT        CProxyDirect3D8::GetAdapterModeCount         ( UINT Adapter )
{
    return m_pDevice->GetAdapterModeCount ( Adapter );
}

HRESULT     CProxyDirect3D8::EnumAdapterModes            ( UINT Adapter, UINT Mode, D3DDISPLAYMODE* pMode )
{
    return m_pDevice->EnumAdapterModes ( Adapter, Mode, pMode );
}

HRESULT     CProxyDirect3D8::GetAdapterDisplayMode       ( UINT Adapter, D3DDISPLAYMODE* pMode )
{
    return m_pDevice->GetAdapterDisplayMode ( Adapter, pMode );
}

HRESULT     CProxyDirect3D8::CheckDeviceType             ( UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed )
{
    return m_pDevice->CheckDeviceType ( Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed );
}

HRESULT     CProxyDirect3D8::CheckDeviceFormat           ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat )
{
    return m_pDevice->CheckDeviceFormat ( Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat );
}

HRESULT     CProxyDirect3D8::CheckDeviceMultiSampleType  ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType )
{
    return m_pDevice->CheckDeviceMultiSampleType ( Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType );
}

HRESULT     CProxyDirect3D8::CheckDepthStencilMatch      ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat )
{
    return m_pDevice->CheckDepthStencilMatch ( Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat );
}

HRESULT     CProxyDirect3D8::GetDeviceCaps               ( UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8* pCaps )
{
    return m_pDevice->GetDeviceCaps ( Adapter, DeviceType, pCaps );
}

HMONITOR    CProxyDirect3D8::GetAdapterMonitor           ( UINT Adapter )
{
    return m_pDevice->GetAdapterMonitor ( Adapter );
}

HRESULT     CProxyDirect3D8::CreateDevice                ( UINT Adapter, D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface )
{   
    HRESULT hResult;

    // Create the new IDirect3DDevice8 object.
    hResult = m_pDevice->CreateDevice ( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

    // Make sure CreateDevice returned a valid object.
    if ( hResult == D3D_OK )
    {
        // Object created; feed the application our proxy interface.
        /* WTF: cannot instantiate abstract class!
        *ppReturnedDeviceInterface = new CProxyDirect3DDevice8 ( *ppReturnedDeviceInterface ); */
    }

    return hResult;
}

#endif
