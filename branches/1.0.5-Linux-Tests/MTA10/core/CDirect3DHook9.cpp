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
    m_bHookingEnabled = true;
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

IUnknown * CDirect3DHook9::API_Direct3DCreate9 ( UINT SDKVersion )
{
    CDirect3DHook9 *    pThis;
    CProxyDirect3D9 *   pNewProxy;
    static bool         bLoadedModules;

    // Get our self instance.
    pThis = CDirect3DHook9::GetSingletonPtr ( );

    if ( pThis->m_bHookingEnabled == false ) {
        // Use the original function
        pThis->m_pDevice = pThis->m_pfnDirect3DCreate9 ( SDKVersion );
        return pThis->m_pDevice;
    }

    // A little hack to get past the loading time required to decrypt the gta 
    // executable into memory...
    if ( !bLoadedModules )
    {
        CCore::GetSingleton ( ).CreateNetwork ( );
        CCore::GetSingleton ( ).CreateGame ( );
        CCore::GetSingleton ( ).CreateMultiplayer ( );
        CCore::GetSingleton ( ).CreateXML ( );
        CCore::GetSingleton ( ).CreateGUI ( );
        bLoadedModules = true;
    }

    // Create our interface.
    pThis->m_pDevice = pThis->m_pfnDirect3DCreate9 ( SDKVersion );

    if ( !pThis->m_pDevice )
    {
        MessageBox ( NULL, "Could not initialize Direct3D9.\n\n"
                           "Please ensure the DirectX End-User Runtime and\n"
                           "latest Windows Service Packs are installed correctly."
                           , "Error", MB_OK|MB_ICONEXCLAMATION );
        return NULL;
    }

    // Create a proxy device.
    pNewProxy = new CProxyDirect3D9 ( static_cast < IDirect3D9 * > ( pThis->m_pDevice ) );

    return pNewProxy;
}

