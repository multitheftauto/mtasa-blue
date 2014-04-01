/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DHook9.cpp
*  PURPOSE:     Function hooker for Direct3D 9
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "detours/include/detours.h"

template<> CDirect3DHook9 * CSingleton< CDirect3DHook9 >::m_pSingleton = NULL;

CDirect3DHook9::CDirect3DHook9 (  )
{
    WriteDebugEvent ( "CDirect3DHook9::CDirect3DHook9" );

    m_pfnDirect3DCreate9 = NULL;
    m_bDirect3DCreate9Suspended = false;
}

CDirect3DHook9::~CDirect3DHook9 ( )
{
    WriteDebugEvent ( "CDirect3DHook9::~CDirect3DHook9" );

    m_pfnDirect3DCreate9 = NULL;
}

bool CDirect3DHook9::ApplyHook ( )
{
    if ( UsingAltD3DSetup() )
        return true;

    // Hook Direct3DCreate9.
    if ( !m_pfnDirect3DCreate9 )
    {
        m_pfnDirect3DCreate9 = reinterpret_cast < pDirect3DCreate > ( DetourFunction ( DetourFindFunction ( "D3D9.DLL", "Direct3DCreate9" ), 
                                                                      reinterpret_cast < PBYTE > ( API_Direct3DCreate9 ) ) );

        WriteDebugEvent ( SString( "Direct3D9 hook applied %08x", m_pfnDirect3DCreate9 ) );
    }
    else
    {
        WriteDebugEvent ( "Direct3D9 hook resumed." );
        m_bDirect3DCreate9Suspended = false;
    }
    return true;
}

bool CDirect3DHook9::RemoveHook ( )
{
    if ( UsingAltD3DSetup() )
        return true;

    m_bDirect3DCreate9Suspended = true;
    WriteDebugEvent ( "Direct3D9 hook suspended." );
    return true;
}


extern HINSTANCE g_hInstance;
SString ToString( const D3DADAPTER_IDENTIFIER9& a );
SString ToString( const D3DCAPS9& a );
HRESULT DoCreateDevice( IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface );
SString GetByteDiffDesc( const void* pData1, const void* pData2, uint uiSize );

namespace
{
    SString ms_strLogPrepend;

    //
    // Hacky log interception
    //
    void WriteDebugEventTest( const SString& strInText )
    {
        SString strText = ms_strLogPrepend + strInText;
        WriteDebugEvent ( strText );
        AddReportLog( 8762, strText.Replace( "\n", " " ) + "\n" );
    }
    #define WriteDebugEvent WriteDebugEventTest

}

////////////////////////////////////////////////////////////////
//
// CreateWindowForD3D
//
//
//
////////////////////////////////////////////////////////////////
HWND CreateWindowForD3D( void )
{
    DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    int X = 10;
    int Y = 10;
    int XL = 100;
    int YL = 100;
    HWND Parent = NULL;
    HINSTANCE hInstance = g_hInstance;

    // Register class if required
    const TCHAR* lpszClassName = "D3D window class";
    {
        WNDCLASS wcex;

        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = (WNDPROC)DefWindowProc;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = hInstance;
        wcex.hIcon          = LoadIcon( NULL, IDI_APPLICATION );
        wcex.hCursor        = LoadCursor( NULL, IDC_ARROW );
        wcex.hbrBackground  = (HBRUSH)( COLOR_WINDOW + 1 );
        wcex.lpszMenuName   = NULL;
        wcex.lpszClassName  = lpszClassName;

        // Returns null if already registered
        RegisterClass(&wcex);
    }

    HWND hWnd = CreateWindowEx( WS_EX_APPWINDOW, lpszClassName, "D3D window", dwStyle, X, Y, XL, YL, Parent, NULL, hInstance, NULL );

    if ( !hWnd )
      return FALSE;

   ShowWindow( hWnd, SW_SHOW );
   UpdateWindow( hWnd );

   return hWnd;
}


//
// Get device using a different method
//
IDirect3DDevice9* CreateDeviceForD3D( HWND hWnd, IDirect3D9* pInitialDirect3D9 )
{
    uint Adapter = 0;
    HRESULT hr;

    DWORD BehaviorFlags = 0x40;
    D3DPRESENT_PARAMETERS pp;
    pp.BackBufferWidth = 800;
    pp.BackBufferHeight = 600;
    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.BackBufferCount = 1;
    pp.MultiSampleType = D3DMULTISAMPLE_NONE;
    pp.MultiSampleQuality = 0;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = hWnd;
    pp.Windowed = true;
    pp.EnableAutoDepthStencil = true;
    pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    pp.Flags = 0;
    pp.FullScreen_RefreshRateInHz = 0;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    IDirect3D9* pAnotherDirect3D9 = NULL;
    IDirect3DDevice9* pD3DDevice9 = NULL;
    do
    {
        hr = pInitialDirect3D9->CreateDevice( Adapter, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &pp, &pD3DDevice9 );
        if ( hr != D3D_OK )
        {
            // Handle failure of initial create device call
            WriteDebugEvent ( SString( "CreateDevice failed #0: %08x", hr ) );

            // Try create device again
            hr = DoCreateDevice ( pInitialDirect3D9, Adapter, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &pp, &pD3DDevice9 );

            // Handle retry result
            if ( hr != D3D_OK )
            {
                WriteDebugEvent ( "-- CreateDevice failed after retry" );
                break;
            }
            else
            {
                WriteDebugEvent ( "++ CreateDevice succeeded after retry" );
            }
        }

        hr = pD3DDevice9->GetDirect3D( &pAnotherDirect3D9 );
        if ( FAILED( hr ) )
        {
            WriteDebugEvent ( SString( "GetDirect3D failed %08x", hr ) );
            break;
        }

        // Log graphic card name
        D3DADAPTER_IDENTIFIER9 InitialAdapterIdent;
        pInitialDirect3D9->GetAdapterIdentifier ( Adapter, 0, &InitialAdapterIdent );
        WriteDebugEvent ( "pInitialDirect3D9:" );
        WriteDebugEvent ( ToString( InitialAdapterIdent ) );

        // Get caps from pInitialDirect3D9
        D3DCAPS9 InitialD3DCaps9;
        hr = pInitialDirect3D9->GetDeviceCaps( Adapter, D3DDEVTYPE_HAL, &InitialD3DCaps9 );
        WriteDebugEvent( SString( "pInitialDirect3D9 CapsReport Caps9 - %s", *ToString( InitialD3DCaps9 ) ) );

        if ( pInitialDirect3D9 == pAnotherDirect3D9 )
        {
            WriteDebugEvent( SString( "pAnotherDirect3D9 same: %x %x", pInitialDirect3D9, pAnotherDirect3D9 ) );
        }
        else
        {
            WriteDebugEvent( SString( "AnotherDirect3D9 differs: %x %x", pInitialDirect3D9, pAnotherDirect3D9 ) );

            // Log graphic card name
            D3DADAPTER_IDENTIFIER9 AnotherAdapterIdent;
            pAnotherDirect3D9->GetAdapterIdentifier ( Adapter, 0, &AnotherAdapterIdent );
            WriteDebugEvent ( "pAnotherDirect3D9:" );
            WriteDebugEvent ( ToString( AnotherAdapterIdent ) );

            // Get caps from pAnotherDirect3D9
            D3DCAPS9 AnotherD3DCaps9;
            hr = pAnotherDirect3D9->GetDeviceCaps( Adapter, D3DDEVTYPE_HAL, &AnotherD3DCaps9 );
            WriteDebugEvent( SString( "pAnotherDirect3D9 CapsReport Caps9 - %s ", *ToString( AnotherD3DCaps9 ) ) );

            // Choose which device to use
            // Use better caps
            // Use NVidia over Intel

            bool InitialCapsSameAsAnotherCaps = memcmp( &InitialD3DCaps9, &AnotherD3DCaps9, sizeof( D3DCAPS9 ) ) == 0;

            WriteDebugEvent( SString( "InitialCaps==AnotherCaps:%d", InitialCapsSameAsAnotherCaps ) );

            SString strDiffDesc = GetByteDiffDesc( &InitialD3DCaps9, &AnotherD3DCaps9, sizeof( D3DCAPS9 ) );
            if ( !strDiffDesc.empty() )
                WriteDebugEvent( SString( "InitialCaps==AnotherD3DCaps9 diff:%s", *strDiffDesc.Left( 500 ) ) );

            if ( AnotherD3DCaps9.DeclTypes < InitialD3DCaps9.DeclTypes )
            {
                WriteDebugEvent( "Don't use pAnotherDirect3D9 as pInitialDirect3D9 Caps is better" );
                SAFE_RELEASE( pAnotherDirect3D9 );
            }
            else
            if ( ToString( InitialAdapterIdent ).ContainsI( "NVidia" ) )
            {
                WriteDebugEvent( "Don't use pAnotherDirect3D9 as pInitialDirect3D9 is NVidia" );
                SAFE_RELEASE( pAnotherDirect3D9 );
            }
            else
            if ( !ToString( AnotherAdapterIdent ).ContainsI( "NVidia" ) )
            {
                WriteDebugEvent( "Don't use pAnotherDirect3D9 as not NVidia" );
                SAFE_RELEASE( pAnotherDirect3D9 );
            }
        }

        // Get caps from pD3DDevice9
        D3DCAPS9 DeviceCaps9;
        hr = pD3DDevice9->GetDeviceCaps( &DeviceCaps9 );
        WriteDebugEvent( SString( "pD3DDevice9 CapsReport Caps9 - %s", *ToString( DeviceCaps9 ) ) );

        SString strDiffDesc = GetByteDiffDesc( &InitialD3DCaps9, &DeviceCaps9, sizeof( D3DCAPS9 ) );
        if ( !strDiffDesc.empty() )
            WriteDebugEvent( SString( "InitialCaps==DeviceCaps9 diff:%s", *strDiffDesc.Left( 500 ) ) );

        // If device caps better, use them
        if ( DeviceCaps9.DeclTypes > InitialD3DCaps9.DeclTypes )
        {
            WriteDebugEvent( "Would use DeviceCaps as better" );
        }

    }
    while( false );

    return pD3DDevice9;
}


//
// Get device using a different method
//
IDirect3D9* GetAnotherThing( IDirect3D9* pInitialDirect3D9, UINT SDKVersion )
{
    static uint uiCallCount = 0;
    uiCallCount += 1;
    ms_strLogPrepend = SString( "GetAnotherThing(%d) ", uiCallCount );
    uint Adapter = 0;
    HRESULT hr;

    // Log graphic card name
    D3DADAPTER_IDENTIFIER9 InitialAdapterIdent;
    pInitialDirect3D9->GetAdapterIdentifier ( Adapter, 0, &InitialAdapterIdent );
    WriteDebugEvent ( "pInitialDirect3D9:" );
    WriteDebugEvent ( ToString( InitialAdapterIdent ) );

    // Get caps from pInitialDirect3D9
    D3DCAPS9 InitialD3DCaps9;
    hr = pInitialDirect3D9->GetDeviceCaps( Adapter, D3DDEVTYPE_HAL, &InitialD3DCaps9 );
    WriteDebugEvent( SString( "pInitialDirect3D9 CapsReport Caps9 - %s ", *ToString( InitialD3DCaps9 ) ) );

    SString strDriver = InitialAdapterIdent.Driver;
    if ( !strDriver.ContainsI( "shim.dll" ) || uiCallCount != 1 )
        return pInitialDirect3D9;

    // Optimus tests only

    ms_strLogPrepend = SString( "GetAnotherThing(%d)A ", uiCallCount );
    HWND hWndA = CreateWindowForD3D();
    IDirect3DDevice9* pD3DDevice9A = CreateDeviceForD3D( hWndA, pInitialDirect3D9 );

    ms_strLogPrepend = SString( "GetAnotherThing(%d)B ", uiCallCount );
    IDirect3D9* pDirect3D9B = CDirect3DHook9::GetSingleton().m_pfnDirect3DCreate9 ( D3D_SDK_VERSION );
    HWND hWndB = CreateWindowForD3D();
    IDirect3DDevice9* pD3DDevice9B = CreateDeviceForD3D( hWndB, pDirect3D9B );

    DestroyWindow( hWndA );
    DestroyWindow( hWndB );

    ms_strLogPrepend = "";
    return pDirect3D9B;
}


IDirect3D9* CDirect3DHook9::API_Direct3DCreate9 ( UINT SDKVersion )
{
    // Get our self instance.
    CDirect3DHook9* pThis = CDirect3DHook9::GetSingletonPtr ( );
    assert( pThis && "API_Direct3DCreate9: No CDirect3DHook9" );

    if ( pThis->m_bDirect3DCreate9Suspended )
        return pThis->m_pfnDirect3DCreate9( SDKVersion );

    // A little hack to get past the loading time required to decrypt the gta 
    // executable into memory...
    if ( !CCore::GetSingleton ( ).AreModulesLoaded ( ) )
    {
        CCore::GetSingleton ( ).CreateNetwork ( );
        CCore::GetSingleton ( ).CreateGame ( );
        CCore::GetSingleton ( ).CreateMultiplayer ( );
        CCore::GetSingleton ( ).CreateXML ( );
        CCore::GetSingleton ( ).CreateGUI ( );
        CCore::GetSingleton ( ).SetModulesLoaded ( true );
    }

    // Create our interface.
    IDirect3D9* pDirect3D9 = pThis->m_pfnDirect3DCreate9 ( SDKVersion );

    if ( !pDirect3D9 )
    {
        WriteDebugEvent ( "Direct3DCreate9 failed" );

        MessageBoxUTF8 ( NULL, _("Could not initialize Direct3D9.\n\n"
                           "Please ensure the DirectX End-User Runtime and\n"
                           "latest Windows Service Packs are installed correctly.")
                           , _("Error")+_E("CC50"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST  ); // Could not initialize Direct3D9.  Please ensure the DirectX End-User Runtime and latest Windows Service Packs are installed correctly.
        return NULL;
    }

    pDirect3D9 = GetAnotherThing( pDirect3D9, SDKVersion );

    GetServerCache ();

    // Create a proxy device.
    CProxyDirect3D9* pProxyDirect3D9 = new CProxyDirect3D9 ( pDirect3D9 );

    return pProxyDirect3D9;
}

