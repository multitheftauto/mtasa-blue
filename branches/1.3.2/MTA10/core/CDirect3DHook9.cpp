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
}

CDirect3DHook9::~CDirect3DHook9 ( )
{
    WriteDebugEvent ( "CDirect3DHook9::~CDirect3DHook9" );

    m_pfnDirect3DCreate9 = NULL;
}

bool CDirect3DHook9::ApplyHook ( )
{
    // Hook Direct3DCreate9.
    m_pfnDirect3DCreate9 = reinterpret_cast < pDirect3DCreate > ( DetourFunction ( DetourFindFunction ( "D3D9.DLL", "Direct3DCreate9" ), 
                                                                  reinterpret_cast < PBYTE > ( API_Direct3DCreate9 ) ) );

    WriteDebugEvent ( "Direct3D9 hook applied" );

    return true;
}

bool CDirect3DHook9::RemoveHook ( )
{
    // Make sure we should be doing this.
    if ( m_pfnDirect3DCreate9 != NULL )
    {
        // Unhook Direct3DCreate9.
        DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnDirect3DCreate9 ), 
                       reinterpret_cast < PBYTE > ( API_Direct3DCreate9  ) );

        // Unset our hook variable.
        m_pfnDirect3DCreate9 = NULL;
    }

    WriteDebugEvent ( "Direct3D9 hook removed." );

    return true;
}

IDirect3D9* CDirect3DHook9::API_Direct3DCreate9 ( UINT SDKVersion )
{
    // Get our self instance.
    CDirect3DHook9* pThis = CDirect3DHook9::GetSingletonPtr ( );
    assert( pThis && "API_Direct3DCreate9: No CDirect3DHook9" );

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

        MessageBox ( NULL, "Could not initialize Direct3D9.\n\n"
                           "Please ensure the DirectX End-User Runtime and\n"
                           "latest Windows Service Packs are installed correctly."
                           , "Error", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST  );
        return NULL;
    }

    GetServerCache ();

    // Create a proxy device.
    CProxyDirect3D9* pProxyDirect3D9 = new CProxyDirect3D9 ( pDirect3D9 );

    return pProxyDirect3D9;
}

