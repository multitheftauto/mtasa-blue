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
HRESULT HandleCreateDeviceResult( HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface );
uint ms_uiInstanceCount = 0;

CProxyDirect3D9::CProxyDirect3D9 ( IDirect3D9* pInterface )
{
    ms_uiInstanceCount++;
    WriteDebugEvent ( SString( "CProxyDirect3D9::CProxyDirect3D9 #%d", ms_uiInstanceCount ) );
    if ( ms_uiInstanceCount > 1 )
        AddReportLog( 9401,  SString( "CProxyDirect3D9::CProxyDirect3D9 #%d", ms_uiInstanceCount ) );

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
    return m_pDevice->GetDeviceCaps ( Adapter, DeviceType, pCaps );
}

HMONITOR   CProxyDirect3D9::GetAdapterMonitor           ( UINT Adapter )
{
    return m_pDevice->GetAdapterMonitor ( Adapter );
}

HRESULT    CProxyDirect3D9::CreateDevice                ( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
// Do not change the code at the start of this function.
// Some graphic card drivers seem sensitive to the content of the CreateDevice function.
    HRESULT hResult;

    WriteDebugEvent ( "CProxyDirect3D9::CreateDevice" );

    WriteDebugEvent ( SString ( "    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x"
                                ,Adapter
                                ,DeviceType
                                ,BehaviorFlags
                            ) );

    // Make sure DirectX Get calls will work
    BehaviorFlags &= ~D3DCREATE_PUREDEVICE;

    WriteDebugEvent ( SString ( "    BackBufferWidth:%d  Height:%d  Format:%d  Count:%d"
                                ,pPresentationParameters->BackBufferWidth
                                ,pPresentationParameters->BackBufferHeight
                                ,pPresentationParameters->BackBufferFormat
                                ,pPresentationParameters->BackBufferCount
                           ) );

    WriteDebugEvent ( SString ( "    MultiSampleType:%d  Quality:%d"
                                ,pPresentationParameters->MultiSampleType
                                ,pPresentationParameters->MultiSampleQuality
                           ) );

    WriteDebugEvent ( SString ( "    SwapEffect:%d  Windowed:%d  EnableAutoDepthStencil:%d  AutoDepthStencilFormat:%d  Flags:0x%x"
                                ,pPresentationParameters->SwapEffect
                                ,pPresentationParameters->Windowed
                                ,pPresentationParameters->EnableAutoDepthStencil
                                ,pPresentationParameters->AutoDepthStencilFormat
                                ,pPresentationParameters->Flags
                           ) );

    WriteDebugEvent ( SString ( "    FullScreen_RefreshRateInHz:%d  PresentationInterval:0x%08x"
                                ,pPresentationParameters->FullScreen_RefreshRateInHz
                                ,pPresentationParameters->PresentationInterval
                           ) );


    // Change the window title to MTA: San Andreas
    #ifdef MTA_DEBUG
        SetWindowTextW ( hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas [DEBUG]").c_str() );
    #else
        SetWindowTextW ( hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas").c_str() );
    #endif

    // Enable the auto depth stencil parameter
    pPresentationParameters->EnableAutoDepthStencil = true;
    
    GetVideoModeManager ()->PreCreateDevice ( pPresentationParameters );

    // Create our object.
    hResult = m_pDevice->CreateDevice ( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

    // Store the rendering window in the direct 3d data
    CDirect3DData::GetSingleton ().StoreDeviceWindow ( pPresentationParameters->hDeviceWindow );

    // Make sure the object was created successfully.
    if ( hResult == D3D_OK )
    {
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

    hResult = HandleCreateDeviceResult( hResult, m_pDevice, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

    return hResult;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::LockRect
//
// For stats
//
/////////////////////////////////////////////////////////////
SString GUIDToString ( const GUID& g );

#define CREATE_DEVICE_FAIL          1
#define CREATE_DEVICE_RETRY_SUCCESS 2
#define CREATE_DEVICE_SUCCESS       3

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
    WriteDebugEvent ( "  Pass #2 as before" );
    WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
    HRESULT hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
    if ( hResult == D3D_OK )
        return hResult;
    WriteDebugEvent ( SString( "  CreateDevice failed #2: %08x", hResult ) );

    // If create failed, try removing multisampling if enabled
    if ( pPresentationParameters->MultiSampleType )
    {
        pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_NONE;
        WriteDebugEvent ( "    Pass #3 with D3DMULTISAMPLE_NONE:" );
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
        hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
        if ( hResult == D3D_OK )
            return hResult;
        WriteDebugEvent ( SString( "    CreateDevice failed #3: %08x", hResult ) );
    }

    // If create failed, try using pure device
    WriteDebugEvent ( "      Pass #4 with D3DCREATE_PUREDEVICE:" );
    BehaviorFlags |= D3DCREATE_PUREDEVICE;
    WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
    hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );
    if ( hResult == D3D_OK )
        return hResult;
    WriteDebugEvent ( SString( "      CreateDevice failed #4: %08x", hResult ) );

    // If create failed, do a test create for logging only
    {
        D3DPRESENT_PARAMETERS pp = *pPresentationParameters;
        pp.BackBufferWidth = 640;
        pp.BackBufferHeight = 480;
        BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        WriteDebugEvent ( "        Test with 640x480" );
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pp ) );
        HRESULT hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, &pp, ppReturnedDeviceInterface );
        WriteDebugEvent ( SString( "        Test result: %08x", hResult ) );
    }

    return hResult;
}


////////////////////////////////////////////////
//
// HandleCreateDeviceResult
//
// Log result and possibly fix everything
//
////////////////////////////////////////////////
HRESULT HandleCreateDeviceResult( HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    // Log graphic card name
    D3DADAPTER_IDENTIFIER9 AdapterIdent;
    pDirect3D->GetAdapterIdentifier ( Adapter, 0, &AdapterIdent );
    WriteDebugEvent ( ToString( AdapterIdent ) );

    uint uiCurrentStatus = 0;   //  0-unknown  1-fail  2-success after retry  3-success

    if ( hResult == D3D_OK )
    {
        // Log success and creation parameters
        WriteDebugEvent ( "CreateDevice success" );
        uiCurrentStatus = CREATE_DEVICE_SUCCESS;
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
    }

    if ( hResult != D3D_OK )
    {
        // Handle failure of initial create device call
        WriteDebugEvent ( SString( "CreateDevice failed #0: %08x", hResult ) );

        // Try create device again
        hResult = DoCreateDevice ( pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

        // Handle retry result
        if ( hResult != D3D_OK )
        {
            WriteDebugEvent ( "--CreateDevice failed after retry" );
            uiCurrentStatus = CREATE_DEVICE_FAIL;
        }
        else
        {
            WriteDebugEvent ( "++CreateDevice succeeded after retry" );
            uiCurrentStatus = CREATE_DEVICE_RETRY_SUCCESS;

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
    }

    uint uiLastStatus = GetApplicationSettingInt( "diagnostics", "createdevice-last-status" );
    SetApplicationSettingInt( "diagnostics", "createdevice-last-status", uiCurrentStatus );

    // Calc log level to use
    uint uiDiagnosticLogLevel = 0;
    if ( uiLastStatus == CREATE_DEVICE_FAIL && uiCurrentStatus != CREATE_DEVICE_FAIL )
        uiDiagnosticLogLevel = 1;   // Log and continue - If changing from fail status
    if ( uiCurrentStatus == CREATE_DEVICE_FAIL )
        uiDiagnosticLogLevel = 2;   // Log and wait - If fail status

    if ( uiDiagnosticLogLevel )
    {
        // Prevent statup warning in loader
        WatchDogCompletedSection ( "L3" );
        CCore::GetSingleton().GetNetwork()->ResetStub( 'dia3', *ms_strExtraLogBuffer, uiDiagnosticLogLevel );
    }
    ms_strExtraLogBuffer.clear();

    if ( hResult != D3D_OK )
    {
        // Handle fatal error
        SString strMessage;
        strMessage += "There was a problem starting MTA:SA\n\n";
        strMessage += SString( "Direct3D CreateDevice error: %08x", hResult );
        BrowseToSolution( "d3dcreatedevice-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, strMessage );
    }

    return hResult;
}
