/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirectInputHook8.cpp
*  PURPOSE:     Function hooker for DirectInput 8
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "detours/include/detours.h"

template<> CDirectInputHook8 * CSingleton< CDirectInputHook8 >::m_pSingleton = NULL;
IDirectInput8* g_pDirectInput8 = NULL;

CDirectInputHook8::CDirectInputHook8 ( )
{
    WriteDebugEvent ( "CDirectInputHook8::CDirectInputHook8" );

    // Set our creation function pointer.
    m_pfnDirectInputCreate  = NULL;
    m_bIsUnicodeInterface   = false;
}

CDirectInputHook8::~CDirectInputHook8 ( )
{
    WriteDebugEvent ( "CDirectInputHook8::~CDirectInputHook8" );
}

HRESULT CDirectInputHook8::API_DirectInput8Create  ( HINSTANCE  hinst,
                                                     DWORD      dwVersion,
                                                     REFIID     riidltf,
                                                     LPVOID*    ppvOut,
                                                     LPUNKNOWN  punkOuter )
{
    // Get our self instance.
    CDirectInputHook8* pThis = CDirectInputHook8::GetSingletonPtr ( );
    assert( pThis && "API_DirectInput8Create: No CDirectInputHook8" );

    // Create our interface.
    HRESULT hResult = pThis->m_pfnDirectInputCreate ( hinst, dwVersion, riidltf, ppvOut, punkOuter );

    if ( FAILED( hResult ) )
    {
        WriteDebugEvent ( "DirectInput8Create failed." );
        return hResult;
    }

    // See if it is unicode or ansi.
    if ( riidltf == IID_IDirectInputW ) 
    {
        WriteDebugEvent ( "DirectInput8 UNICODE Interface Created." );

        pThis->m_bIsUnicodeInterface = true;
    }
    else
    {
        WriteDebugEvent ( "DirectInput8 ANSI Interface Created." );

        pThis->m_bIsUnicodeInterface = false;
    }

    // Get the real interface temporarily.
    IDirectInput8* pDirectInput8 = static_cast < IDirectInput8 * > ( *ppvOut );

    // Give the caller a proxy interface.
    CProxyDirectInput8* pProxyDirectInput8 = new CProxyDirectInput8 ( pDirectInput8 );
    pProxyDirectInput8->AddRef();

    g_pDirectInput8 = pProxyDirectInput8;
    *ppvOut = pProxyDirectInput8;

    return hResult;
}

bool CDirectInputHook8::ApplyHook ( )
{
    // Hook DirectInput8Create.
    m_pfnDirectInputCreate = reinterpret_cast < pDirectInputCreate > ( DetourFunction ( DetourFindFunction ( "DINPUT8.DLL", "DirectInput8Create" ), 
                                                                                        reinterpret_cast < PBYTE > ( API_DirectInput8Create ) ) );
    
    return true;
}

bool CDirectInputHook8::RemoveHook ( )
{
    // Make sure we should be doing this.
    if ( m_pfnDirectInputCreate != NULL )
    {
        // Unhook Direct3DCreate9.
        DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnDirectInputCreate ), 
                       reinterpret_cast < PBYTE > ( API_DirectInput8Create ) );

        // Unset our hook variable.
        m_pfnDirectInputCreate = NULL;
    }
    return true;
}
