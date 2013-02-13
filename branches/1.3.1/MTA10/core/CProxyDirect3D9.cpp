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
#include "SharedUtil.Hooks.h"
#include "SharedUtil.Hooks.hpp"
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
            WriteDebugEvent( SString( "   -- CreateDeviceInsist succeeded on retry #%d", uiRetryCount + 1 ) );
            break;
        }
        Sleep( 1 );
    }
    while( ++uiRetryCount < uiMinTries || retryTimer.Get() < uiTimeout );

    return hResult;
}



////////////////////////////////////////////////
//
// InDepthMegaTestDirect3D9
//
// Do various things
//
////////////////////////////////////////////////
void InDepthMegaTestDirect3D9( UINT SdkVersion, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pInPresentationParameters )
{
    WriteDebugEvent ( "InDepthMegaTest" );

    HRESULT hResult = -2;
    IDirect3DDevice9* pReturnedDeviceInterface = NULL;
    IDirect3D9* pDirect3D9 = Direct3DCreate9( SdkVersion );

    if ( !pDirect3D9 )
        WriteDebugEvent( SString( "  Could not create Direct3DCreate9( %d )", SdkVersion ) );
    else
    {
        WriteDebugEvent( SString( "  Created pDirect3D9:%08x from Direct3DCreate9( %d )", pDirect3D9, SdkVersion ) );

        // First try to create with our new interface
        hResult = CreateDeviceInsist( 2, 1000, pDirect3D9, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pInPresentationParameters, &pReturnedDeviceInterface );
        WriteDebugEvent( SString( "  (A) hResult:#08x  pReturnedDeviceInterface:%08x  ", hResult, pReturnedDeviceInterface ) );
        SAFE_RELEASE( pReturnedDeviceInterface );

        // Then try with some changes
        D3DPRESENT_PARAMETERS pp = *pInPresentationParameters;
        pp.MultiSampleQuality = 0;
        pp.MultiSampleType = D3DMULTISAMPLE_NONE;
        pp.BackBufferHeight = 640;
        pp.BackBufferWidth = 480;

        hResult = CreateDeviceInsist( 2, 1000, pDirect3D9, Adapter, DeviceType, hFocusWindow, BehaviorFlags, &pp, &pReturnedDeviceInterface );
        WriteDebugEvent( SString( "  (B) hResult:#08x  pReturnedDeviceInterface:%08x  ", hResult, pReturnedDeviceInterface ) );
        SAFE_RELEASE( pReturnedDeviceInterface );

        // And some more changes
        pp.Windowed = true;
        pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        hResult = CreateDeviceInsist( 2, 1000, pDirect3D9, Adapter, DeviceType, hFocusWindow, BehaviorFlags, &pp, &pReturnedDeviceInterface );
        WriteDebugEvent( SString( "  (C) hResult:#08x  pReturnedDeviceInterface:%08x  ", hResult, pReturnedDeviceInterface ) );
        SAFE_RELEASE( pReturnedDeviceInterface );

        SAFE_RELEASE( pDirect3D9 );
    }
}


////////////////////////////////////////////////
//
// InDepthMegaTest
//
// Do various things
//
////////////////////////////////////////////////
void InDepthMegaTest( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pInPresentationParameters )
{
    WriteDebugEvent ( "InDepthMegaTest" );
    InDepthMegaTestDirect3D9( D3D_SDK_VERSION, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pInPresentationParameters );
    InDepthMegaTestDirect3D9( D3D9b_SDK_VERSION, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pInPresentationParameters );
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
                InDepthMegaTest( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters );
            }
        }
    }

    CCore::GetSingleton ( ).CreateNetwork ( );
    CCore::GetSingleton ( ).CreateGame ( );
    CCore::GetSingleton ( ).CreateMultiplayer ( );
    CCore::GetSingleton ( ).CreateXML ( );
    CCore::GetSingleton ( ).CreateGUI ( );

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





namespace
{
    IDirect3D9* ms_pDirect3D    = NULL;
    UINT ms_Adapter             = 0;
    D3DDEVTYPE ms_DeviceType    = D3DDEVTYPE_HAL;
    HWND ms_hFocusWindow        = NULL;
    DWORD ms_BehaviorFlags      = 0;
    D3DPRESENT_PARAMETERS* ms_pPresentationParameters   = NULL;
    IDirect3DDevice9** ms_ppReturnedDeviceInterface     = NULL;
}

////////////////////////////////////////////////////////////////
//
// OnPreCreateDevice
//
// Called before GTA creates the D3D device
//
////////////////////////////////////////////////////////////////
void _cdecl OnPreCreateDevice( IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD* BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    g_pCore->OnPreCreateDevice( pDirect3D, Adapter, DeviceType, hFocusWindow, *BehaviorFlags, pPresentationParameters );
    ms_pDirect3D                 = pDirect3D;
    ms_Adapter                   = Adapter;
    ms_DeviceType                = DeviceType;
    ms_hFocusWindow              = hFocusWindow;
    ms_BehaviorFlags             = *BehaviorFlags;
    ms_pPresentationParameters   = pPresentationParameters;
    ms_ppReturnedDeviceInterface = ppReturnedDeviceInterface;
}

// Hook info
#define HOOKPOS_PreCreateDevice_US          0x07F677A
#define HOOKPOS_PreCreateDevice_EU          0x07F67BA
#define HOOKSIZE_PreCreateDevice_US         6
#define HOOKSIZE_PreCreateDevice_EU         6
DWORD RETURN_PreCreateDevice_US =           0x07F6781;
DWORD RETURN_PreCreateDevice_EU =           0x07F67C1;
DWORD RETURN_PreCreateDevice_BOTH =         0;
void _declspec(naked) HOOK_PreCreateDevice()
{
    _asm
    {
        // Replaced code
        mov     eax, ds:0x0C97C24       // __RwD3DAdapterIndex
        push    eax
        push    ecx

        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        lea     eax,[esp+32+4*6]    // Turn BehaviorFlags into a pointer so we can modify it
        push    eax
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        call    OnPreCreateDevice
        add     esp, 4*7
        popad

        // Continue
        jmp     RETURN_PreCreateDevice_BOTH
    }
}


////////////////////////////////////////////////////////////////
//
// OnPostCreateDevice
//
// Called after GTA creates the D3D device
//
////////////////////////////////////////////////////////////////
HRESULT _cdecl OnPostCreateDevice( HRESULT hResult )
{
    return g_pCore->OnPostCreateDevice( hResult, ms_pDirect3D, ms_Adapter, ms_DeviceType, ms_hFocusWindow, ms_BehaviorFlags, ms_pPresentationParameters, ms_ppReturnedDeviceInterface );
}

// Hook info
#define HOOKPOS_PostCreateDevice_US         0x07F6784
#define HOOKPOS_PostCreateDevice_EU         0x07F67C4
#define HOOKSIZE_PostCreateDevice_US        6
#define HOOKSIZE_PostCreateDevice_EU        6
DWORD RETURN_PostCreateDevice_US =          0x07F678A;
DWORD RETURN_PostCreateDevice_EU =          0x07F67CA;
DWORD RETURN_PostCreateDevice_BOTH =        0;
DWORD RETURN_PostCreateDeviceB_US =         0x07F6799;
DWORD RETURN_PostCreateDeviceB_EU =         0x07F67D9;
DWORD RETURN_PostCreateDeviceB_BOTH =       0;
void _declspec(naked) HOOK_PostCreateDevice()
{
    _asm
    {
        // Replaced code
        pushad
        push    eax
        call    OnPostCreateDevice
        mov     [esp+0],eax
        add     esp, 4*1
        popad
        mov     eax,[esp-32-4*1]

        // Continue
        test    eax, eax
        jge     ok
        xor     eax, eax
        jmp     RETURN_PostCreateDevice_BOTH

ok:
        jmp     RETURN_PostCreateDeviceB_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CCore::InitHooks_Direct3D
//
// Setup hook
//
//////////////////////////////////////////////////////////////////////////////////////////
void CCore::InitHooks_Direct3D ( void )
{
    DWORD oldProt;
    VirtualProtect((LPVOID)0x401000, 0x4A3000, PAGE_EXECUTE_READWRITE, &oldProt);
    EZHookInstall ( PreCreateDevice );
    EZHookInstall ( PostCreateDevice );
}
