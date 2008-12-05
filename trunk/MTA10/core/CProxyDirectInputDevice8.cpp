/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirectInputDevice8.cpp
*  PURPOSE:     DirectInput 8 device function hooking proxy
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CProxyDirectInputDevice8::CProxyDirectInputDevice8 ( IDirectInputDevice8A* pDevice )
{
    GetJoystickManager ( )->AddDevice ( pDevice );
    WriteDebugEvent ( "CProxyDirectInputDevice8::CProxyDirectInputDevice8" );
    
    // Notify the event handler
    CDirectInputEvents8::OnDirectInputDeviceCreate ( pDevice );

    // Initialize our device member variable.
    m_pDevice       = pDevice;

	// Get the current refcount.
	pDevice->AddRef ( );
	m_dwRefCount = pDevice->Release ( );
}

CProxyDirectInputDevice8::~CProxyDirectInputDevice8 ( )
{
    GetJoystickManager ( )->RemoveDevice ( m_pDevice );
    WriteDebugEvent ( "CProxyDirectInputDevice8::~CProxyDirectInputDevice8" );
}

/*** IUnknown methods ***/
HRESULT CProxyDirectInputDevice8::QueryInterface             ( REFIID riid,  LPVOID * ppvObj )
{
    return m_pDevice->QueryInterface ( riid, ppvObj );
}

ULONG   CProxyDirectInputDevice8::AddRef                     ( VOID )
{
    // Incrase our refcount
    m_dwRefCount++;

    return m_pDevice->AddRef ( );
}

ULONG   CProxyDirectInputDevice8::Release                    ( VOID )
{
    ULONG       ulRefCount;
    IUnknown *  pDestroyedDevice;

    // Determine if we should self destruct
    if ( --m_dwRefCount == 0 )
	{
        WriteDebugEvent ( "Releasing IDirectInputDevice8 Proxy..." );

		// Notify the event handler
		CDirectInputEvents8::OnDirectInputDeviceDestroy ( m_pDevice );

        // Save device so we can destroy it after.
        pDestroyedDevice = m_pDevice;

		// Destroy object.
        delete this;

		// Call the release routine and get the refcount.
		ulRefCount = pDestroyedDevice->Release ( );

        return ulRefCount;
    }

	ulRefCount = m_pDevice->Release ( );

    return ulRefCount;
}

/*** IDirectInputDevice8A methods ***/
HRESULT CProxyDirectInputDevice8::GetCapabilities            ( LPDIDEVCAPS a )
{
    return m_pDevice->GetCapabilities ( a );
}

HRESULT CProxyDirectInputDevice8::EnumObjects                ( LPDIENUMDEVICEOBJECTSCALLBACKA a, LPVOID b, DWORD c )
{
    return m_pDevice->EnumObjects ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::GetProperty                ( REFGUID a, LPDIPROPHEADER b )
{
    return m_pDevice->GetProperty ( a, b );
}

HRESULT CProxyDirectInputDevice8::SetProperty                ( REFGUID a, LPCDIPROPHEADER b )
{
    return m_pDevice->SetProperty ( a, b );
}

HRESULT CProxyDirectInputDevice8::Acquire                    ( VOID )
{
    return m_pDevice->Acquire ( );
}

HRESULT CProxyDirectInputDevice8::Unacquire                  ( VOID )
{
    return m_pDevice->Unacquire ( );
}

HRESULT CProxyDirectInputDevice8::GetDeviceState             ( DWORD a, LPVOID b )
{
    HRESULT hResult     = 0;
    DWORD   dwNumItems  = INFINITE;

    // If the GUI has focus, we don't let the game grab any input
    if ( CLocalGUI::GetSingletonPtr () ) 
    {
        if ( CLocalGUI::GetSingleton().InputGoesToGUI () )
        {
            // Need to call GetDeviceState here to drain the input buffer.
            // Otherwise, once we let go of our hook, the game will get all the
            // movements on the device.
            hResult = m_pDevice->GetDeviceState ( a, b );

            // Clear the buffer so the game won't get any events.
            memset ( b, 0, a );

            m_pDevice->GetDeviceData ( sizeof ( DIDEVICEOBJECTDATA ), NULL, &dwNumItems, 0 );

            return hResult;
        }
    }

    return m_pDevice->GetDeviceState ( a, b ); 
}

HRESULT CProxyDirectInputDevice8::GetDeviceData              ( DWORD a, LPDIDEVICEOBJECTDATA b, LPDWORD c, DWORD d )
{
    HRESULT hResult     = 0;
    DWORD   dwNumItems  = INFINITE;
    // If the GUI has focus, we don't let the game grab any input
    if ( CLocalGUI::GetSingletonPtr () ) 
    {
        if ( CLocalGUI::GetSingleton().InputGoesToGUI () )
        {
            // Record result.
            hResult = m_pDevice->GetDeviceData ( a, b, c, d );
    
            // Flush data buffer.
            if ( d != DIGDD_PEEK )
            {
                m_pDevice->GetDeviceData ( sizeof ( DIDEVICEOBJECTDATA ), 
                                           NULL,
                                           &dwNumItems,
                                           0 );
            }

            // Clear strucutre(s).
            memset ( b, 0, a * (*c) );
            return hResult;
        }
    }

    return m_pDevice->GetDeviceData ( a, b, c, d );
}

HRESULT CProxyDirectInputDevice8::SetDataFormat              ( LPCDIDATAFORMAT a )
{
    GetJoystickManager ( )->SetDeviceDataFormat ( m_pDevice, a );
    return m_pDevice->SetDataFormat ( a );
}

HRESULT CProxyDirectInputDevice8::SetEventNotification       ( HANDLE a )
{
    return m_pDevice->SetEventNotification ( a );
}

HRESULT CProxyDirectInputDevice8::SetCooperativeLevel        ( HWND a, DWORD b )
{
    HRESULT hResult;

    hResult = m_pDevice->SetCooperativeLevel ( a, b/*&(~DISCL_EXCLUSIVE|DISCL_NONEXCLUSIVE)*/ );

    return hResult;
}

HRESULT CProxyDirectInputDevice8::GetObjectInfo              ( LPDIDEVICEOBJECTINSTANCEA a, DWORD b, DWORD c )
{
    return m_pDevice->GetObjectInfo ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::GetDeviceInfo              ( LPDIDEVICEINSTANCEA a )
{
    return m_pDevice->GetDeviceInfo ( a );
}

HRESULT CProxyDirectInputDevice8::RunControlPanel            ( HWND a, DWORD b )
{
    return m_pDevice->RunControlPanel ( a, b );
}

HRESULT CProxyDirectInputDevice8::Initialize                 ( HINSTANCE a, DWORD b, REFGUID c )
{
    return m_pDevice->Initialize ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::CreateEffect               ( REFGUID a, LPCDIEFFECT b, LPDIRECTINPUTEFFECT * c, LPUNKNOWN d )
{
    return m_pDevice->CreateEffect ( a, b, c, d );
}

HRESULT CProxyDirectInputDevice8::EnumEffects                ( LPDIENUMEFFECTSCALLBACKA a, LPVOID b, DWORD c )
{
    return m_pDevice->EnumEffects ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::GetEffectInfo              ( LPDIEFFECTINFOA a, REFGUID b )
{
    return m_pDevice->GetEffectInfo ( a, b );
}

HRESULT CProxyDirectInputDevice8::GetForceFeedbackState      ( LPDWORD a )
{
    return m_pDevice->GetForceFeedbackState ( a );
}

HRESULT CProxyDirectInputDevice8::SendForceFeedbackCommand   ( DWORD a )
{
    return m_pDevice->SendForceFeedbackCommand ( a );
}

HRESULT CProxyDirectInputDevice8::EnumCreatedEffectObjects   ( LPDIENUMCREATEDEFFECTOBJECTSCALLBACK a, LPVOID b, DWORD c )
{
    return m_pDevice->EnumCreatedEffectObjects ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::Escape                     ( LPDIEFFESCAPE a )
{
    return m_pDevice->Escape ( a );
}

HRESULT CProxyDirectInputDevice8::Poll                       ( VOID )
{
    return m_pDevice->Poll ( );
}

HRESULT CProxyDirectInputDevice8::SendDeviceData             ( DWORD a, LPCDIDEVICEOBJECTDATA b, LPDWORD c, DWORD d )
{
    return m_pDevice->SendDeviceData ( a, b, c, d );
}

HRESULT CProxyDirectInputDevice8::EnumEffectsInFile          ( LPCSTR a, LPDIENUMEFFECTSINFILECALLBACK b, LPVOID c, DWORD d )
{
    return m_pDevice->EnumEffectsInFile ( a, b, c, d );
}

HRESULT CProxyDirectInputDevice8::WriteEffectToFile          ( LPCSTR a, DWORD b, LPDIFILEEFFECT c, DWORD d )
{
    return m_pDevice->WriteEffectToFile ( a, b, c, d );
}

HRESULT CProxyDirectInputDevice8::BuildActionMap             ( LPDIACTIONFORMATA a, LPCSTR b, DWORD c )
{
    return m_pDevice->BuildActionMap ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::SetActionMap               ( LPDIACTIONFORMATA a, LPCSTR b, DWORD c )
{
    return m_pDevice->SetActionMap ( a, b, c );
}

HRESULT CProxyDirectInputDevice8::GetImageInfo               ( LPDIDEVICEIMAGEINFOHEADERA a )
{
    return m_pDevice->GetImageInfo ( a );
}
