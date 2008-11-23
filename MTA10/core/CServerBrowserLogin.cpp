/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFavouritesAddByIP.h
*  PURPOSE:     Header file for add to favourites by IP dialog
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CServerBrowserLogin::CServerBrowserLogin ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Login Request" ) );
    m_pWindow->SetCloseButtonEnabled ( false );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 300.0f / 2, resolution.fY / 2 - 150.0f / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( 300.0f, 150.0f ), false );
    m_pWindow->SetSizingEnabled ( false );
	m_pWindow->SetAlwaysOnTop ( true );

    m_pLabelText = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "This server requires you to be logged in with\n your MTA Community account, please login:" ) );
    m_pLabelText->SetPosition ( CVector2D ( 15.0f, 20.0f ), false );
    m_pLabelText->SetSize ( CVector2D ( 270.0f, 40.0f ), false );

    m_pLabelUsername = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Username:" ) );
    m_pLabelUsername->SetPosition ( CVector2D ( 15.0f, 60.0f ), false );
    m_pLabelUsername->AutoSize ( "Username:" );

    m_pLabelPassword = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Password:" ) );
    m_pLabelPassword->SetPosition ( CVector2D ( 15.0f, 85.0f ), false );
    m_pLabelPassword->AutoSize ( "Password:" );

    m_pEditUsername = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditUsername->SetPosition ( CVector2D ( 90.0f, 59.0f ), false );
    m_pEditUsername->SetSize ( CVector2D ( 180.0f, 22.0f ), false );
    m_pEditUsername->SetMaxLength ( 32 );

    m_pEditPassword = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditPassword->SetPosition ( CVector2D ( 90.0f, 84.0f ), false );
    m_pEditPassword->SetSize ( CVector2D ( 180.0f, 22.0f ), false );
    m_pEditPassword->SetMasked ( true );
    m_pEditPassword->SetMaxLength ( 32 );

    m_pButtonLogin = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Login" ) );
    m_pButtonLogin->SetPosition ( CVector2D ( 120.0f, 120.0f ), false );
    m_pButtonLogin->SetSize ( CVector2D ( 70.0f, 20.0f ), false );

    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 200.0f, 120.0f ), false );
    m_pButtonCancel->SetSize ( CVector2D ( 70.0f, 20.0f ), false );

    m_pButtonLogin->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowserLogin::OnButtonBackClick, this ) );
    m_pButtonCancel->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowserLogin::OnButtonBackClick, this ) );
}

CServerBrowserLogin::~CServerBrowserLogin ( void )
{
    delete m_pButtonLogin;
    delete m_pButtonCancel;
    delete m_pEditUsername;
    delete m_pEditPassword;
    delete m_pLabelUsername;
    delete m_pLabelPassword;
    delete m_pLabelText;
    delete m_pWindow;
}

void CServerBrowserLogin::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( bVisible );
    if ( bVisible )
        m_pWindow->BringToFront ();
}

bool CServerBrowserLogin::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}

bool CServerBrowserLogin::OnButtonBackClick ( CGUIElement* pElement )
{
    if ( pElement == m_pButtonCancel )
    {
        SetVisible ( false );
    }
    else if ( pElement == m_pButtonLogin )
    {

    }
    return true;
}

void CServerBrowserLogin::SetCallback ( BROWSERLOGINCALLBACK )
{

}