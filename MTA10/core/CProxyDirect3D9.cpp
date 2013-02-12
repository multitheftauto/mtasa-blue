/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirect3D9.cpp
*  PURPOSE:     Direct3D 9 function hooking proxy
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
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

    DWORD BehaviorFlagsOrig = 0;
    D3DPRESENT_PARAMETERS presentationParametersOrig;
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
            WriteDebugEvent( SString( "  CreateDeviceInsist succeeded on retry #%d", uiRetryCount + 1 ) );
            break;
        }
        Sleep( 1 );
    }
    while( ++uiRetryCount < uiMinTries || retryTimer.Get() < uiTimeout );

    return hResult;
}


////////////////////////////////////////////////
//
// Hook CCore::OnPreCreateDevice
//
// Modify paramters
//
////////////////////////////////////////////////
void CCore::OnPreCreateDevice( IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD& BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters )
{
    WriteDebugEvent ( "CCore::OnPreCreateDevice" );

    uint uiPrevResult = GetApplicationSettingInt( "diagnostics", "last-create-device-result" );
    if ( uiPrevResult )
    {
        // Failed last time, so as a test for logging, try a create device with no modifications
        WriteDebugEvent ( SString( "Previous CreateDevice failed with: %08x", uiPrevResult ) );
        WriteDebugEvent ( "  Test unmodified:" );
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
        IDirect3DDevice9* pReturnedDeviceInterface = NULL;
        HRESULT hResult = pDirect3D->CreateDevice( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &pReturnedDeviceInterface );
        SAFE_RELEASE( pReturnedDeviceInterface );
        WriteDebugEvent ( SString( "  Unmodified result is: %08x", hResult ) );
    }

    // Save original values for later
    BehaviorFlagsOrig = BehaviorFlags;
    presentationParametersOrig = *pPresentationParameters;

    WriteDebugEvent ( "  Original paramters:" );
    WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );

    // Make sure DirectX Get...() calls will work
    BehaviorFlags &= ~D3DCREATE_PUREDEVICE;

    // Enable the auto depth stencil parameter
    pPresentationParameters->EnableAutoDepthStencil = true;

    GetVideoModeManager ()->PreCreateDevice ( pPresentationParameters );

    WriteDebugEvent ( "  Modified paramters:" );
    WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
}


////////////////////////////////////////////////
//
// Hook CCore::OnPostCreateDevice
//
// Wrap device or log failure
//
////////////////////////////////////////////////
HRESULT CCore::OnPostCreateDevice( HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    WriteDebugEvent ( "CCore::OnPostCreateDevice" );

    // Do diagnostic log if initial attempt failed or previous run failed
    bool bDiagnosticLog = ( hResult != D3D_OK ) || GetApplicationSettingInt( "diagnostics", "last-create-device-result" );

    // If failed, try again a few more times over the next second
    if ( hResult != D3D_OK )
        hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

    // Save result so we can do something different next time if needed
    SetApplicationSettingInt( "diagnostics", "last-create-device-result", hResult );

    if ( hResult != D3D_OK )
    {
        // Handle retry on failure
        WriteDebugEvent ( SString( "CreateDevice failed #1: %08x", hResult ) );

        // If create failed, try using original presentationParameters
        *pPresentationParameters = presentationParametersOrig;
        WriteDebugEvent ( "  Attempt #2 with orig pp:" );
        WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlags, *pPresentationParameters ) );
        hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

        if ( hResult != D3D_OK )
        {
            WriteDebugEvent ( SString( "CreateDevice failed #2: %08x", hResult ) );

            // If create failed, try using original BehaviorFlags as well
            WriteDebugEvent ( "  Attempt #3 with orig bf+pp:" );
            WriteDebugEvent ( ToString( Adapter, DeviceType, hFocusWindow, BehaviorFlagsOrig, *pPresentationParameters ) );
            hResult = CreateDeviceInsist( 2, 1000, pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlagsOrig, pPresentationParameters, ppReturnedDeviceInterface );

            if ( hResult != D3D_OK )
            {
                WriteDebugEvent ( SString( "CreateDevice failed #3: %08x", hResult ) );
            }
        }
    }

    // Change the window title to MTA: San Andreas
    #ifdef MTA_DEBUG
        SetWindowTextW ( hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas [DEBUG]").c_str() );
    #else
        SetWindowTextW ( hFocusWindow, MbUTF8ToUTF16("MTA: San Andreas").c_str() );
    #endif

    // Log graphic card name
    D3DADAPTER_IDENTIFIER9 AdapterIdent;
    pDirect3D->GetAdapterIdentifier ( Adapter, 0, &AdapterIdent );
    WriteDebugEvent ( ToString( AdapterIdent ) );

    // Store the rendering window in the direct 3d data
    CDirect3DData::GetSingleton ().StoreDeviceWindow ( pPresentationParameters->hDeviceWindow );

    // Apply input hook
    CMessageLoopHook::GetSingleton ( ).ApplyHook ( hFocusWindow );

    // Make sure the object was created successfully.
    if ( hResult == D3D_OK )
    {
        WriteDebugEvent ( "CreateDevice succeeded" );
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

        WriteDebugEvent( "   Used creation parameters:" );
        WriteDebugEvent ( SString ( "    Adapter:%d  DeviceType:%d  BehaviorFlags:0x%x  ReadableDepth:%s"
                                    ,parameters.AdapterOrdinal
                                    ,parameters.DeviceType
                                    ,parameters.BehaviorFlags
                                    ,ReadableDepthFormat ? std::string ( (char*)&ReadableDepthFormat, 4 ).c_str () : "None"
                                ) );
    }

    // Do diagnostic log now if needed
    if ( bDiagnosticLog )
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
