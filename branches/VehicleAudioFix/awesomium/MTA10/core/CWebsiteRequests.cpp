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

    m_pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, _("The server requests the following websites in order to open them (later):") ) );
    m_pLabel->SetPosition ( CVector2D(10, 26), false );
    m_pLabel->SetSize ( CVector2D(456, 43), false );
    m_pLabel->SetFont ( "default-bold" );

    m_pAddressMemo = reinterpret_cast<CGUIMemo*> ( pManager->CreateMemo ( m_pWindow, "" ) );
    m_pAddressMemo->SetPosition ( CVector2D(10, 71), false );
    m_pAddressMemo->SetSize ( CVector2D(456, 182), false );
    m_pAddressMemo->SetFont ( "default-bold" );
    m_pAddressMemo->SetReadOnly ( true );

    m_pButtonAllow = reinterpret_cast<CGUIButton*> ( pManager->CreateButton ( m_pWindow, _("Allow") ) );
    m_pButtonAllow->SetPosition ( CVector2D ( 10, 259 ), false );
    m_pButtonAllow->SetSize ( CVector2D(204, 28), false );
    m_pButtonAllow->SetProperty ("NormalTextColour", "FF00FF00" );
    m_pButtonAllow->SetClickHandler ( GUI_CALLBACK ( &CWebsiteRequests::OnAllowButtonClick, this) );

    m_pButtonDeny = reinterpret_cast<CGUIButton*> ( pManager->CreateButton ( m_pWindow, _("Deny") ) );
    m_pButtonDeny->SetPosition ( CVector2D(262, 259), false );
    m_pButtonDeny->SetSize ( CVector2D(204, 28), false );
    m_pButtonDeny->SetProperty ( "NormalTextColour", "FFFF0000");
    m_pButtonDeny->SetClickHandler ( GUI_CALLBACK ( &CWebsiteRequests::OnDenyButtonClick, this ) );
}

CWebsiteRequests::~CWebsiteRequests ()
{
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

void CWebsiteRequests::SetPendingRequests ( const std::vector<SString>& pendingRequests )
{
    SString content = "";
    for ( std::vector<SString>::const_iterator iter = pendingRequests.begin(); iter != pendingRequests.end(); ++iter )
    {
        content += *iter + "\n";
    }

    // Apply text
    m_pAddressMemo->SetText ( content.c_str() );
}

void CWebsiteRequests::Clear ()
{
    m_pAddressMemo->SetText ("");
}

bool CWebsiteRequests::OnAllowButtonClick ( CGUIElement* pElement )
{
    g_pCore->GetWebCore ()->AllowPendingPages ();
    Hide ();
    return true;
}

bool CWebsiteRequests::OnDenyButtonClick ( CGUIElement* pElement )
{
    g_pCore->GetWebCore ()->DenyPendingPages ();
    Hide ();
    return true;
}
