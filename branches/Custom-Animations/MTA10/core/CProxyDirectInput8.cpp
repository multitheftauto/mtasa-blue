/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirectInput8.cpp
*  PURPOSE:     DirectInput 8 function hooking proxy
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CProxyDirectInput8::CProxyDirectInput8 ( IDirectInput8* pDevice )
{
    WriteDebugEvent ( "CProxyDirectInput8::CProxyDirectInput8" );

    // Initalize our local variable.
    m_pDevice       = pDevice;
    m_dwRefCount    = 0;
}

CProxyDirectInput8::~CProxyDirectInput8 ( )
{
    WriteDebugEvent ( "CProxyDirectInput8::~CProxyDirectInput8" );
}

/*** IUnknown methods ***/
HRESULT CProxyDirectInput8::QueryInterface ( REFIID riid,  LPVOID * ppvObj )
{
    return m_pDevice->QueryInterface ( riid, ppvObj );
}

ULONG   CProxyDirectInput8::AddRef ( VOID )
{
    // Incrase our refcount
    m_dwRefCount++;

    return m_pDevice->AddRef ( );
}

ULONG   CProxyDirectInput8::Release ( VOID )
{
    ULONG       ulRefCount;
    IUnknown *  pDestroyedDevice;

    // Determine if we should self destruct
    if ( m_dwRefCount-- == 0 )
    {
        WriteDebugEvent ( "Releasing IDirectInput8 Proxy..." );

        // Save device so we can destroy it after.
        pDestroyedDevice = m_pDevice;

        delete this;

        // Call the release routine and get the refcount.
        ulRefCount = pDestroyedDevice->Release ( );

        return ulRefCount;
    }

    // Call the release routine and get the refcount.
    ulRefCount = m_pDevice->Release ( );
    
    return ulRefCount;
}

/*** IDirectInput8A methods ***/
HRESULT CProxyDirectInput8::CreateDevice ( REFGUID a, LPDIRECTINPUTDEVICE8A *b, LPUNKNOWN c )
{
    HRESULT hResult;

    // Create the new IDirect3DDevice8 object.
    hResult = m_pDevice->CreateDevice ( a, b, c );

    // Make sure CreateDevice returned a valid object.
    if ( hResult == DI_OK )
    {
        // Object created; feed the application our proxy interface.
        *b = new CProxyDirectInputDevice8 ( *b );
    }

    return hResult;
}

HRESULT CProxyDirectInput8::EnumDevices ( DWORD a, LPDIENUMDEVICESCALLBACKA b, LPVOID c, DWORD d )
{
    return m_pDevice->EnumDevices ( a, b, c, d );
}

HRESULT CProxyDirectInput8::GetDeviceStatus ( REFGUID a )
{
    return m_pDevice->GetDeviceStatus ( a );
}

HRESULT CProxyDirectInput8::RunControlPanel ( HWND a, DWORD b )
{
    return m_pDevice->RunControlPanel ( a, b );
}

HRESULT CProxyDirectInput8::Initialize ( HINSTANCE a, DWORD b )
{
    return m_pDevice->Initialize ( a, b );
}

HRESULT CProxyDirectInput8::FindDevice ( REFGUID a, LPCSTR b, LPGUID c )
{
    return m_pDevice->FindDevice ( a, b, c );
}

HRESULT CProxyDirectInput8::EnumDevicesBySemantics ( LPCSTR a, LPDIACTIONFORMATA b, LPDIENUMDEVICESBYSEMANTICSCBA c, LPVOID d, DWORD e )
{
    return m_pDevice->EnumDevicesBySemantics ( a, b, c, d, e );
}

HRESULT CProxyDirectInput8::ConfigureDevices ( LPDICONFIGUREDEVICESCALLBACK a, LPDICONFIGUREDEVICESPARAMSA b, DWORD c, LPVOID d )
{
    return m_pDevice->ConfigureDevices ( a, b, c, d );
}