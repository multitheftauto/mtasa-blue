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
    CDirectInputHook8 *    pThis;
    HRESULT                hResult;

    // Get our self instance.
    pThis = CDirectInputHook8::GetSingletonPtr ( );

    // Create our interface.
    hResult = pThis->m_pfnDirectInputCreate ( hinst, dwVersion, riidltf, ppvOut, punkOuter );

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

    // Save the real interface temporarily.
    pThis->m_pDevice = static_cast < IUnknown * > ( *ppvOut );

    // Give the caller a proxy interface.
    *ppvOut = new CProxyDirectInput8 ( static_cast < IDirectInput8 * > ( pThis->m_pDevice ) );

    g_pDirectInput8 = static_cast < IDirectInput8 * > ( pThis->m_pDevice );

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
