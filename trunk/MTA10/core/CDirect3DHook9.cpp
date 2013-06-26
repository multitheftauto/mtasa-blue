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

    GetServerCache ();

    // Create a proxy device.
    CProxyDirect3D9* pProxyDirect3D9 = new CProxyDirect3D9 ( pDirect3D9 );

    return pProxyDirect3D9;
}

