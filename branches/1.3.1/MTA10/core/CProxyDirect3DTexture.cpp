/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:  D3DOCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CProxyDirect3DTexture.h"


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::CProxyDirect3DTexture
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DTexture::CProxyDirect3DTexture ( IDirect3DDevice9* InD3DDevice9,IDirect3DTexture9* pOriginal, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool )
    : m_stats ( Usage & D3DUSAGE_DYNAMIC ? g_pDeviceState->MemoryState.DynamicTexture : g_pDeviceState->MemoryState.StaticTexture )
{
	m_pOriginal = pOriginal;
    m_iMemUsed = CRenderItemManager::CalcD3DTextureMemoryKBUsage ( pOriginal ) * 1024;
    m_dwUsage = Usage;

    m_stats.iCurrentCount++;
    m_stats.iCurrentBytes += m_iMemUsed;
    m_stats.iCreatedCount++;
    m_stats.iCreatedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::~CProxyDirect3DTexture
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DTexture::~CProxyDirect3DTexture ( void )
{
    m_stats.iCurrentCount--;
    m_stats.iCurrentBytes -= m_iMemUsed;
    m_stats.iDestroyedCount++;
    m_stats.iDestroyedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DTexture::QueryInterface ( REFIID riid, void** ppvObj )
{
	*ppvObj = NULL;

    // Looking for me?
	if( riid == CProxyDirect3DTexture_GUID )
	{
		*ppvObj = this;
		return S_OK;
	}

	return m_pOriginal->QueryInterface ( riid, ppvObj ); 
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DTexture::Release ( void )
{
	// Call original function
	ULONG count = m_pOriginal->Release ();
		
    if ( count == 0 )
    {
		// now, the Original Object has deleted itself, so do we here
		delete this ;  // destructor will be called automatically
	}

	return count;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::LockRect
//
// For stats
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DTexture::LockRect ( UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
    m_stats.iLockedCount++;
    return m_pOriginal->LockRect ( Level, pLockedRect, pRect, Flags );
}


SString GUIDToString ( const GUID& g );

namespace
{
    // Debugging helpers
    SString ToString( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, const D3DPRESENT_PARAMETERS& pp )
    {
        return SString(
                        "    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x\n"
                        "    BackBufferWidth:%d  Height:%d  Format:%d  Count:%d\n"
                        "    MultiSampleType:%d  Quality:%d\n"
                        "    SwapEffect:%d  Windowed:%d  EnableAutoDepthStencil:%d  AutoDepthStencilFormat:%d  Flags:0x%x\n"
                        "    FullScreen_RefreshRateInHz:%d  PresentationInterval:0x%08x"
                        , Adapter
                        , DeviceType
                        , BehaviorFlags
                        , pp.BackBufferWidth
                        , pp.BackBufferHeight
                        , pp.BackBufferFormat
                        , pp.BackBufferCount
                        , pp.MultiSampleType
                        , pp.MultiSampleQuality
                        , pp.SwapEffect
                        , pp.Windowed
                        , pp.EnableAutoDepthStencil
                        , pp.AutoDepthStencilFormat
                        , pp.Flags
                        , pp.FullScreen_RefreshRateInHz
                        , pp.PresentationInterval
                    );
    }

    SString ToString( const D3DADAPTER_IDENTIFIER9& a )
    {
        return SString(
                        "    Driver:%s\n"
                        "    Description:%s\n"
                        "    DeviceName:%s\n"
                        "    DriverVersion:0x%08x 0x%08x\n"
                        "    VendorId:0x%08x  DeviceId:0x%08x  SubSysId:0x%08x  Revision:0x%08x  WHQLLevel:0x%08x\n"
                        "    DeviceIdentifier:%s"
                        , a.Driver
                        , a.Description
                        , a.DeviceName
                        , a.DriverVersion.HighPart
                        , a.DriverVersion.LowPart
                        , a.VendorId
                        , a.DeviceId
                        , a.SubSysId
                        , a.Revision
                        , a.WHQLLevel
                        , *GUIDToString( a.DeviceIdentifier )
                    );
    }

    //
    // Hacky log interception
    //
    SString ms_strExtraLogBuffer;
    void WriteDebugEventTest( const SString& strText )
    {
        ms_strExtraLogBuffer += strText.Replace( "\n", " " ) + "\n";
        WriteDebugEvent ( strText );
    }
    #define WriteDebugEvent WriteDebugEventTest

}


////////////////////////////////////////////////
//
// CreateDeviceInsist
//
// Keep trying create device for a little bit
//
////////////////////////////////////////////////
HRESULT CreateDeviceInsist( uint uiMinTries, uint uiTimeout, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    HRESULT hResult;
    CElapsedTime retryTimer;
    uint uiRetryCount = 0;
    do
    {
        hResult = pDirect3D->CreateDevice( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
        if ( hResult == D3D_OK )
        {
            WriteDebugEvent( SString( "   -- CreateDeviceInsist succeeded on try #%d", uiRetryCount + 1 ) );
            break;
        }
        Sleep( 1 );
    }
    while( ++uiRetryCount < uiMinTries || retryTimer.Get() < uiTimeout );

    return hResult;
}


////////////////////////////////////////////////
//
// DoCreateDevice
//
// In various ways
//
////////////////////////////////////////////////
HRESULT DoCreateDevice( IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    // If initial attempt failed, enable diagnostic log and try again a few more times over the next second
    WriteDebugEvent ( "  Attempt #2:" );
    WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
    HRESULT hResult = CreateDeviceInsist( 4, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
    if ( hResult == D3D_OK )
        return hResult;
    WriteDebugEvent ( SString( "CreateDevice failed #2: %08x", hResult ) );

    // If create failed, try removing multisampling if enabled
    if ( pPresentationParameters->MultiSampleType )
    {
        pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_NONE;
        WriteDebugEvent ( "  Attempt #3 with D3DMULTISAMPLE_NONE:" );
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
        hResult = CreateDeviceInsist( 4, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
        if ( hResult == D3D_OK )
            return hResult;
        WriteDebugEvent ( SString( "CreateDevice failed #3: %08x", hResult ) );
    }

    // If create failed, try using pure device
    WriteDebugEvent ( "  Attempt #4 with D3DCREATE_PUREDEVICE:" );
    BehaviorFlags |= D3DCREATE_PUREDEVICE;
    WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
    hResult = CreateDeviceInsist( 4, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
    if ( hResult == D3D_OK )
        return hResult;
    WriteDebugEvent ( SString( "CreateDevice failed #4: %08x", hResult ) );

    // If create failed, do a test create for logging only
    {
        D3DPRESENT_PARAMETERS pp = *pPresentationParameters;
        pp.BackBufferWidth = 640;
        pp.BackBufferHeight = 480;
        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        WriteDebugEvent ( "  Test with 640x480" );
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pp ) );
        HRESULT hResult = CreateDeviceInsist( 4, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, &pp, ppReturnedDeviceInterface );
        WriteDebugEvent ( SString( "  Test result: %08x", hResult ) );
    }

    return hResult;
}


////////////////////////////////////////////////
//
// DoSomethingSpecialFarAway
//
// Fix everything
//
////////////////////////////////////////////////
HRESULT DoSomethingSpecialFarAway( HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    if ( hResult == D3D_OK )
    {
        // Just log success and card type
        WriteDebugEvent ( "CreateDevice success" );

        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );

        D3DADAPTER_IDENTIFIER9 AdapterIdent;
        pDirect3D->GetAdapterIdentifier ( Adapter, 0, &AdapterIdent );
        WriteDebugEvent ( ToString( AdapterIdent ) );

        ms_strExtraLogBuffer.clear();
        return hResult;
    }

    WriteDebugEvent ( SString( "CreateDevice failed #0: %08x", hResult ) );

    // Log graphic card name
    D3DADAPTER_IDENTIFIER9 AdapterIdent;
    pDirect3D->GetAdapterIdentifier ( Adapter, 0, &AdapterIdent );
    WriteDebugEvent ( ToString( AdapterIdent ) );

    hResult = DoCreateDevice ( pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

    // Make sure the object was created successfully.
    if ( hResult == D3D_OK )
    {
        WriteDebugEvent ( "CreateDevice2 succeeded" );

        // Apply input hook
        CMessageLoopHook::GetSingleton ( ).ApplyHook ( hFocusWindow );

        GetVideoModeManager ()->PostCreateDevice ( *ppReturnedDeviceInterface, pPresentationParameters );

        // We must first store the presentation values.
        CDirect3DData::GetSingleton ( ).StoreViewport ( 0, 0,
                                                        pPresentationParameters->BackBufferWidth,
                                                        pPresentationParameters->BackBufferHeight );
        
        // Calc and store readable depth format for shader use
        ERenderFormat ReadableDepthFormat = CDirect3DEvents9::DiscoverReadableDepthFormat ( *ppReturnedDeviceInterface, pPresentationParameters->MultiSampleType, pPresentationParameters->Windowed != 0 );
        CGraphics::GetSingleton ().GetRenderItemManager ()->SetDepthBufferFormat ( ReadableDepthFormat );

        // Now create the proxy device.
        *ppReturnedDeviceInterface = new CProxyDirect3DDevice9 ( *ppReturnedDeviceInterface );

        // Debug output
        D3DDEVICE_CREATION_PARAMETERS parameters;
        (*ppReturnedDeviceInterface)->GetCreationParameters ( &parameters );

        WriteDebugEvent ( SString ( "    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x  ReadableDepth:%s"
                                    ,parameters.AdapterOrdinal
                                    ,parameters.DeviceType
                                    ,parameters.BehaviorFlags
                                    ,ReadableDepthFormat ? std::string ( (char*)&ReadableDepthFormat, 4 ).c_str () : "None"
                                ) );
    }

    // Do diagnostic log now if needed
    if ( true )
    {
        // Prevent statup warning in loader
        WatchDogCompletedSection ( "L3" );

        // Run diagnostic
        CCore::GetSingleton().GetNetwork()->ResetStub( 'dia2', *ms_strExtraLogBuffer );
    }
    ms_strExtraLogBuffer.clear();

    // Handle fatal error
    if ( hResult != D3D_OK )
    {
        // Inform user
        SString strMessage;
        strMessage += "There was a problem starting MTA:SA\n\n";
        strMessage += SString( "Direct3D CreateDevice error: %08x", hResult );
        BrowseToSolution( "d3dcreatedevice-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, strMessage );
    }

    return hResult;
}
