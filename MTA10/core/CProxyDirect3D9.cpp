/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirect3D9.cpp
*  PURPOSE:     Direct3D 9 function hooking proxy
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CProxyDirect3D9::CProxyDirect3D9 ( IDirect3D9* pInterface )
{
    WriteDebugEvent ( "CProxyDirect3D9::CProxyDirect3D9" );
    m_pDevice       = pInterface;

	// Give ourself a matching refcount.
	pInterface->AddRef ( );
	m_dwRefCount = pInterface->Release ( );
}

CProxyDirect3D9::~CProxyDirect3D9             ( )
{
    WriteDebugEvent ( "CProxyDirect3D9::~CProxyDirect3D9" );
    m_pDevice       = NULL;
    m_dwRefCount    = 0;
}

/*** IUnknown methods ***/
HRESULT    CProxyDirect3D9::QueryInterface              ( REFIID riid, void** ppvObj )
{
    return m_pDevice->QueryInterface ( riid, ppvObj );
}

ULONG      CProxyDirect3D9::AddRef                      ( VOID )
{
    // Increment our refcount.
    m_dwRefCount++;

    return m_pDevice->AddRef ( );
}

ULONG      CProxyDirect3D9::Release                     ( VOID )
{
    ULONG       ulRefCount;
    IUnknown *  pDestroyedDevice;

    // Check to see if we should self-destruct
    if ( --m_dwRefCount == 0 )
    {
        // Save device so we can destroy it after.
        pDestroyedDevice = m_pDevice;

        delete this;  

        // Call device's AddRef routine and get the current refcount.
        ulRefCount = pDestroyedDevice->Release( );

        return ulRefCount;
    }

    // Call device's AddRef routine and get the current refcount.
    ulRefCount = m_pDevice->Release( );

    return ulRefCount;          
}

/*** IDirect3D9 methods ***/
HRESULT    CProxyDirect3D9::RegisterSoftwareDevice      ( void* pInitializeFunction )
{
    return m_pDevice->RegisterSoftwareDevice ( pInitializeFunction );
}

UINT       CProxyDirect3D9::GetAdapterCount             ( VOID )
{
    return m_pDevice->GetAdapterCount ( );
}

HRESULT    CProxyDirect3D9::GetAdapterIdentifier        ( UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier )
{
    return m_pDevice->GetAdapterIdentifier ( Adapter, Flags, pIdentifier );
}

UINT       CProxyDirect3D9::GetAdapterModeCount         ( UINT Adapter, D3DFORMAT Format )
{
    return m_pDevice->GetAdapterModeCount ( Adapter, Format );
}

HRESULT    CProxyDirect3D9::EnumAdapterModes            ( UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode )
{
    return m_pDevice->EnumAdapterModes ( Adapter, Format, Mode, pMode );
}

HRESULT    CProxyDirect3D9::GetAdapterDisplayMode       ( UINT Adapter, D3DDISPLAYMODE* pMode )
{
    return m_pDevice->GetAdapterDisplayMode ( Adapter, pMode );
}

HRESULT    CProxyDirect3D9::CheckDeviceType             ( UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed )
{
    return m_pDevice->CheckDeviceType ( Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed );
}

HRESULT    CProxyDirect3D9::CheckDeviceFormat           ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat )
{
    return m_pDevice->CheckDeviceFormat ( Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat );
}

HRESULT    CProxyDirect3D9::CheckDeviceMultiSampleType  ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels )
{
    return m_pDevice->CheckDeviceMultiSampleType ( Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels );
}

HRESULT    CProxyDirect3D9::CheckDepthStencilMatch      ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat )
{
    return m_pDevice->CheckDepthStencilMatch ( Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat );
}

HRESULT    CProxyDirect3D9::CheckDeviceFormatConversion ( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat )
{
    return m_pDevice->CheckDeviceFormatConversion ( Adapter, DeviceType, SourceFormat, TargetFormat );
}

HRESULT    CProxyDirect3D9::GetDeviceCaps               ( UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps )
{	
	HRESULT hResult;

	hResult = m_pDevice->GetDeviceCaps ( Adapter, DeviceType, pCaps );

	return hResult;
}

HMONITOR   CProxyDirect3D9::GetAdapterMonitor           ( UINT Adapter )
{
    return m_pDevice->GetAdapterMonitor ( Adapter );
}

HRESULT    CProxyDirect3D9::CreateDevice                ( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    HRESULT hResult;

	WriteDebugEvent ( "CProxyDirect3D9::CreateDevice" );

    // Change the window title to MTA: San Andreas
    #ifdef MTA_DEBUG
        SetWindowText ( hFocusWindow, "MTA: San Andreas [DEBUG]" );
    #else
        SetWindowText ( hFocusWindow, "MTA: San Andreas" );
    #endif

	// Enable the auto depth stencil parameter
	pPresentationParameters->EnableAutoDepthStencil = true;
	
    GetVideoModeManager ()->PreCreateDevice ( pPresentationParameters );

    // Create our object.
    hResult = m_pDevice->CreateDevice ( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

    GetVideoModeManager ()->PostCreateDevice ( *ppReturnedDeviceInterface, pPresentationParameters );

	// Pass the device to the core and store the rendering window in the direct 3d data
	CCore::GetSingleton ().SetRenderDevice ( *ppReturnedDeviceInterface );
    CDirect3DData::GetSingleton ().StoreDeviceWindow ( pPresentationParameters->hDeviceWindow );

	// Apply input hook
	CMessageLoopHook::GetSingleton ( ).ApplyHook ( hFocusWindow );

    // Make sure the object was created successfully.
    if ( hResult == D3D_OK )
    {
        // We must first store the presentation values.
        CDirect3DData::GetSingleton ( ).StoreViewport ( 0, 0,
                                                        pPresentationParameters->BackBufferWidth,
                                                        pPresentationParameters->BackBufferHeight );
        
        // Now create the proxy device.
        *ppReturnedDeviceInterface = new CProxyDirect3DDevice9 ( *ppReturnedDeviceInterface );
    }
    return hResult;
}