/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebsiteRequests.cpp
*  PURPOSE:     Website requests GUI class
*
*****************************************************************************/

#include "StdInc.h"
#include "CWebsiteRequests.h"

CWebsiteRequests::CWebsiteRequests()
{
    CGUI* pManager = g_pCore->GetGUI ();
    CVector2D vecResolution = pManager->GetResolution ();

    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, _("Website requests") ) );
    m_pWindow->SetMovable ( true );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetPosition ( CVector2D ( vecResolution.fX / 2 - WEBSITEREQUESTS_WINDOW_DEFAULTWIDTH / 2, vecResolution.fY / 2 - WEBSITEREQUESTS_WINDOW_DEFAULTHEIGHT / 2), false );
    m_pWindow->SetSize ( CVector2D ( WEBSITEREQUESTS_WINDOW_DEFAULTWIDTH, WEBSITEREQUESTS_WINDOW_DEFAULTHEIGHT) );
    m_pWindow->SetAlwaysOnTop ( true );

    m_pLabel1 = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, _("The server requests the following websites in order to load them (later):") ) );
    m_pLabel1->SetPosition ( CVector2D ( 10, 26 ), false );
    m_pLabel1->SetSize ( CVector2D ( 456, 25 ), false );
    m_pLabel1->SetFont ( "default-bold" );

    m_pLabel2 = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, _("NEVER ENTER SENSITIVE DATA TO PROTECT THEM FROM BEING STOLEN") ) );
    m_pLabel2->SetPosition ( CVector2D ( 10, 45 ), false );
    m_pLabel2->SetSize ( CVector2D ( 456, 25 ), false );
    m_pLabel2->SetFont ( "default-bold" );
    m_pLabel2->SetTextColor ( 255, 0, 0 );

    m_pAddressMemo = reinterpret_cast < CGUIMemo* > ( pManager->CreateMemo ( m_pWindow, "" ) );
    m_pAddressMemo->SetPosition ( CVector2D ( 10, 71 ), false );
    m_pAddressMemo->SetSize ( CVector2D ( 456, 160 ), false );
    m_pAddressMemo->SetFont ( "default-bold" );
    m_pAddressMemo->SetReadOnly ( true );

    m_pCheckRemember = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pWindow, _("Remember decision") ) );
    m_pCheckRemember->SetPosition ( CVector2D ( 10, 229 ) );
    m_pCheckRemember->SetSize ( CVector2D ( 456, 30 ) );
    m_pCheckRemember->SetFont ( "default-bold" );

    m_pButtonAllow = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, _("Allow") ) );
    m_pButtonAllow->SetPosition ( CVector2D ( 10, 259 ), false );
    m_pButtonAllow->SetSize ( CVector2D ( 204, 28 ), false );
    m_pButtonAllow->SetProperty ( "NormalTextColour", "FF40A62E" );
    m_pButtonAllow->SetClickHandler ( GUI_CALLBACK ( &CWebsiteRequests::OnAllowButtonClick, this ) );

    m_pButtonDeny = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, _("Deny") ) );
    m_pButtonDeny->SetPosition ( CVector2D ( 262, 259 ), false );
    m_pButtonDeny->SetSize ( CVector2D ( 204, 28 ), false );
    m_pButtonDeny->SetProperty ( "NormalTextColour", "FFC4020F");
    m_pButtonDeny->SetClickHandler ( GUI_CALLBACK ( &CWebsiteRequests::OnDenyButtonClick, this ) );
}

CWebsiteRequests::~CWebsiteRequests ()
{
    delete m_pLabel1;
    delete m_pLabel2;
    delete m_pAddressMemo;
    delete m_pCheckRemember;
    delete m_pButtonAllow;
    delete m_pButtonDeny;
    delete m_pWindow;
}

void CWebsiteRequests::Show ()
{
    m_pWindow->SetVisible ( true );
    m_pWindow->BringToFront ();
}

void CWebsiteRequests::Hide ()
{
    m_pWindow->SetVisible ( false );
}

bool CWebsiteRequests::IsVisible ()
{
    return m_pWindow->IsVisible ();
}

void CWebsiteRequests::SetPendingRequests ( const std::vector<SString>& pendingRequests, WebRequestCallback* pCallback )
{
    SString content = "";
    for ( std::vector<SString>::const_iterator iter = pendingRequests.begin(); iter != pendingRequests.end(); ++iter )
    {
        content += *iter + "\n";
    }

    // Apply text
    m_pAddressMemo->SetText ( content.c_str() );
    if ( pCallback )
        m_Callbacks.push_back ( *pCallback );
}

void CWebsiteRequests::Clear ()
{
    m_pAddressMemo->SetText ("");
    m_Callbacks.clear ();
}

void CWebsiteRequests::Callback ( bool bAllow )
{
    // Call callbacks and clear list
    const auto& pendingRequests = g_pCore->GetWebCore ()->GetPendingRequests ();
    for ( auto&& callback : m_Callbacks )
    {
        callback ( bAllow, pendingRequests );
    }
    m_Callbacks.clear ();
}

bool CWebsiteRequests::OnAllowButtonClick ( CGUIElement* pElement )
{
    Callback ( true );
    g_pCore->GetWebCore ()->AllowPendingPages ( m_pCheckRemember->GetSelected () );
    Hide ();

    return true;
}

bool CWebsiteRequests::OnDenyButtonClick ( CGUIElement* pElement )
{
    Callback ( false );
    g_pCore->GetWebCore ()->DenyPendingPages ();
    Hide ();

    return true;
}
