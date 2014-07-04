/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCommunityLogin.cpp
*  PURPOSE:     Community login dialog class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <utils/CMD5Hasher.h>

extern CCore* g_pCore;

CCommunityLogin::CCommunityLogin ( void )
{
    m_pCallBack = NULL;

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

    m_pButtonLogin->SetClickHandler ( GUI_CALLBACK ( &CCommunityLogin::OnButtonLoginClick, this ) );
    m_pButtonCancel->SetClickHandler ( GUI_CALLBACK ( &CCommunityLogin::OnButtonCancelClick, this ) );
}

CCommunityLogin::~CCommunityLogin ( void )
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

void CCommunityLogin::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( bVisible );
    if ( bVisible )
        m_pWindow->BringToFront ();
}

bool CCommunityLogin::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}

bool CCommunityLogin::OnButtonLoginClick ( CGUIElement* pElement )
{
    if ( m_pEditUsername->GetText().empty() ||
        m_pEditPassword->GetText().empty() )
    {
        g_pCore->ShowMessageBox ( "Serial Error", "Invalid username/password", MB_BUTTON_OK | MB_ICON_ERROR );
        return true;
    }
    // Hash password
    char szPassword[33];
    std::string strPassword;
    MD5 Password;
    CMD5Hasher Hasher;
    Hasher.Calculate ( m_pEditPassword->GetText ().c_str(), m_pEditPassword->GetText().length(), Password );
    Hasher.ConvertToHex ( Password, szPassword );
    strPassword = std::string ( szPassword );

    // Store the user/pass and log in using community
    CCommunity *pCommunity = CCommunity::GetSingletonPtr ();
    pCommunity->SetUsername ( m_pEditUsername->GetText () );
    pCommunity->SetPassword ( strPassword );
    CVARS_SET ( "community_username", m_pEditUsername->GetText () );
    CVARS_SET ( "community_password", strPassword );
    pCommunity->Login ( OnLoginCallback, this );
    return true;
}

bool CCommunityLogin::OnButtonCancelClick ( CGUIElement* pElement )
{
    SetVisible ( false );
    return true;
}

void CCommunityLogin::OnLoginCallback ( bool bResult, char* szError, void *obj )
{
    if ( !bResult )
    {
        g_pCore->ShowMessageBox ( "Serial Error", szError, MB_BUTTON_OK | MB_ICON_ERROR );
    }
    else
    {
        CCommunityLogin* pLogin = reinterpret_cast < CCommunityLogin* > ( obj );
        // Succeed, connect
        pLogin->SetVisible ( false );
        pLogin->GetCallback()();
    }
}


void CCommunityLogin::SetCallback ( BROWSERLOGINCALLBACK pCallBack )
{
    m_pCallBack = pCallBack;
}

BROWSERLOGINCALLBACK CCommunityLogin::GetCallback ( void )
{
    return m_pCallBack;
}
