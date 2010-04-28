/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CQuickConnect.cpp
*  PURPOSE:     Quick connect window
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CQuickConnect::CQuickConnect ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "QUICK CONNECT" ) );
    m_pWindow->SetCloseButtonEnabled ( false );
    //m_pWindow->SetMovable ( false );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 280.0f / 2, resolution.fY / 2 - 120.0f / 2  ), false );

   // m_pWindow->SetPosition ( CVector2D ( 0.35f, 0.375f ), true );
    m_pWindow->SetSize ( CVector2D ( 280.0f, 120.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );

    // Create the controls
    //  Host label
    m_pLabelHost = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Host:" ) );
    m_pLabelHost->SetPosition ( CVector2D ( 0.028f, 0.25f ), true );
    m_pLabelHost->AutoSize ( "Host:" );

    //  Host edit
    m_pEditHost = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditHost->SetPosition ( CVector2D ( 0.17f, 0.2f ), true );
    m_pEditHost->SetSize ( CVector2D ( 154.0f, 24.0f ) );
    m_pEditHost->SetMaxLength ( 128 );          // Just to prevent entering a huge hostname size.. no-one has a hostname over 128 chars i believe
    m_pEditHost->SetTextAcceptedHandler( GUI_CALLBACK( &CQuickConnect::OnConnectButtonClick, this ) );

    //  Port label
    m_pLabelPort = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, ":" ) );
    m_pLabelPort->SetPosition ( CVector2D ( 0.725f, 0.25f ) );
    m_pLabelPort->AutoSize ( ":" );

    //  Port edit
    m_pEditPort = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditPort->SetPosition ( CVector2D ( 0.75f, 0.2f ), true );
    m_pEditPort->SetSize ( CVector2D ( 59.0f, 24.0f ) );
    m_pEditPort->SetMaxLength ( 5 );
    m_pEditPort->SetTextAcceptedHandler( GUI_CALLBACK( &CQuickConnect::OnConnectButtonClick, this ) );

    //  Pass label
    m_pLabelPass = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Pass:" ) );
    m_pLabelPass->SetPosition ( CVector2D ( 0.028f, 0.49f ), true );
    m_pLabelPass->AutoSize ( "Pass:" );

    //  Pass edit
    m_pEditPass = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditPass->SetPosition ( CVector2D ( 0.17f, 0.44f ), true );
    m_pEditPass->SetSize ( CVector2D ( 221.0f, 24.0f ) );
    m_pEditPass->SetMasked ( true );
    m_pEditPass->SetTextAcceptedHandler( GUI_CALLBACK( &CQuickConnect::OnConnectButtonClick, this ) );

    //  Connect button
    m_pButtonConnect = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Connect" ) );
    m_pButtonConnect->SetPosition ( CVector2D ( 0.04f, 0.71f ), true );
    m_pButtonConnect->SetSize ( CVector2D ( 112.0f, 24.0f ) );

    //  Cancel button
    m_pButtonBack = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Back" ) );
    m_pButtonBack->SetPosition ( CVector2D ( 0.56f, 0.71f ), true );
    m_pButtonBack->SetSize ( CVector2D ( 112.0f, 24.0f ) );

    // Set up event handlers
    m_pButtonConnect->SetClickHandler ( GUI_CALLBACK ( &CQuickConnect::OnConnectButtonClick, this ) );
    m_pButtonBack->SetClickHandler ( GUI_CALLBACK ( &CQuickConnect::OnBackButtonClick, this ) );
    m_pWindow->SetEnterKeyHandler ( GUI_CALLBACK ( &CQuickConnect::OnConnectButtonClick, this ) );
}


CQuickConnect::~CQuickConnect ( void )
{
    delete m_pButtonBack;
    delete m_pButtonConnect;
    delete m_pEditPort;
    delete m_pLabelPort;
    delete m_pEditHost;
    delete m_pLabelHost;
    delete m_pEditPass;
    delete m_pLabelPass;

    delete m_pWindow;
}


void CQuickConnect::SetVisible ( bool bVisible )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();
    if ( !pMainMenu ) return;

    m_pWindow->SetVisible ( bVisible );

    if ( bVisible )
    {
        pMainMenu->m_bIsInSubWindow = true;
        m_pWindow->Activate ();
        LoadData ();
    }
    else
    {
        pMainMenu->m_bIsInSubWindow = false;        
    }
}


bool CQuickConnect::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}


void CQuickConnect::Reset ( void )
{
    m_pEditHost->SetText ( "" );
    m_pEditPort->SetText ( "" );
    m_pEditPass->SetText ( "" );
}

bool CQuickConnect::OnConnectButtonClick ( CGUIElement* pElement )
{
    // No host specified?
    const std::string& strHost = m_pEditHost->GetText ();
    if ( strHost.length () == 0 )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Error", "No host specified!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // No/invalid port specified?
    const std::string& strPort = m_pEditPort->GetText ();
    int iPort = atoi ( strPort.c_str () );
    if ( strPort.length () == 0 )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Error", "No port specified!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }
    else if ( iPort == 0 || iPort > 0xFFFF )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Error", "Invalid port specified!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // Valid nick?
    std::string strNick;
    CVARS_GET ( "nick", strNick );
    if ( !CCore::GetSingleton ().IsValidNick ( strNick.c_str () ) )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Error", "Invalid nickname! Please go to Settings and set a new!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // Grab the host, port, nick and password
    unsigned short usPort = static_cast < unsigned short > ( iPort );
    const std::string& strPassword = m_pEditPass->GetText ();

    // Save the connection details into the config
    CVARS_SET ( "qc_host", strHost );
    CVARS_SET ( "qc_port", usPort );
    CVARS_SET ( "qc_password", strPassword );

    // Start the connect
    CCore::GetSingleton ().GetConnectManager ()->Connect ( strHost.c_str (), usPort, strNick.c_str (), strPassword.c_str () );

    return true;
}


bool CQuickConnect::OnBackButtonClick ( CGUIElement* pElement )
{
    SetVisible ( false );
    return true;
}


void CQuickConnect::LoadData ( void )
{
    std::string val;
    unsigned int uval;

    CVARS_GET ( "qc_host",      val );   m_pEditHost->SetText ( val.c_str () );

    CVARS_GET ( "qc_port",      uval );
    m_pEditPort->SetText ( SString ( "%u", uval ) );

    CVARS_GET ( "qc_password",  val );  m_pEditPass->SetText ( val.c_str () );
}
