/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCommunityRegistration.cpp
*  PURPOSE:     Community registration dialog class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CCommunityRegistration::CCommunityRegistration ( void )
{
    m_ulStartTime = 0;
}


void CCommunityRegistration::CreateWindows ( void )
{
    __asm int 3;
    CGUI *pManager = g_pCore->GetGUI ();
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Login Request" ) );
    m_pWindow->SetCloseButtonEnabled ( false );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 300.0f / 2, resolution.fY / 2 - 300.0f / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( 300.0f, 300.0f ), false );
    m_pWindow->SetSizingEnabled ( false );
	m_pWindow->SetAlwaysOnTop ( true );

    m_pLabelUsername = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Username:" ) );
    m_pLabelUsername->SetPosition ( CVector2D ( 15.0f, 60.0f ), false );
    m_pLabelUsername->AutoSize ( "Username:" );

    m_pLabelEmail = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Email:" ) );
    m_pLabelEmail->SetPosition ( CVector2D ( 15.0f, 85.0f ), false );
    m_pLabelEmail->AutoSize ( "Email:" );

    m_pLabelPassword = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Password:" ) );
    m_pLabelPassword->SetPosition ( CVector2D ( 15.0f, 120.0f ), false );
    m_pLabelPassword->AutoSize ( "Password:" );

    m_pLabelConfirm = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Confirm:" ) );
    m_pLabelConfirm->SetPosition ( CVector2D ( 15.0f, 145.0f ), false );
    m_pLabelConfirm->AutoSize ( "Confirm:" );

    m_pLabelCode = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Validation code:" ) );
    m_pLabelCode->SetPosition ( CVector2D ( 15.0f, 180.0f ), false );
    m_pLabelCode->AutoSize ( "Validation code:" );

    m_pEditUsername = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditUsername->SetPosition ( CVector2D ( 90.0f, 59.0f ), false );
    m_pEditUsername->SetSize ( CVector2D ( 180.0f, 22.0f ), false );
    m_pEditUsername->SetMaxLength ( 32 );

    m_pEditEmail = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditEmail->SetPosition ( CVector2D ( 90.0f, 84.0f ), false );
    m_pEditEmail->SetSize ( CVector2D ( 180.0f, 22.0f ), false );

    m_pEditPassword = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditPassword->SetPosition ( CVector2D ( 90.0f, 119.0f ), false );
    m_pEditPassword->SetSize ( CVector2D ( 180.0f, 22.0f ), false );
    m_pEditPassword->SetMasked ( true );
    m_pEditPassword->SetMaxLength ( 32 );

    m_pEditConfirm = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditConfirm->SetPosition ( CVector2D ( 90.0f, 144.0f ), false );
    m_pEditConfirm->SetSize ( CVector2D ( 180.0f, 22.0f ), false );
    m_pEditConfirm->SetMasked ( true );
    m_pEditConfirm->SetMaxLength ( 32 );

    m_pEditCode = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow ) );
    m_pEditCode->SetPosition ( CVector2D ( 110.0f, 179.0f ), false );
    m_pEditCode->SetSize ( CVector2D ( 70.0f, 22.0f ), false );
    m_pEditCode->SetMaxLength ( 8 );

    m_pButtonRegister = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Register" ) );
    m_pButtonRegister->SetPosition ( CVector2D ( 100.0f, 270.0f ), false );
    m_pButtonRegister->SetSize ( CVector2D ( 90.0f, 20.0f ), false );

    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 200.0f, 270.0f ), false );
    m_pButtonCancel->SetSize ( CVector2D ( 70.0f, 20.0f ), false );

    m_pButtonRegister->SetOnClickHandler ( GUI_CALLBACK ( &CCommunityRegistration::OnButtonClick, this ) );
    m_pButtonCancel->SetOnClickHandler ( GUI_CALLBACK ( &CCommunityRegistration::OnButtonClick, this ) );
}


CCommunityRegistration::~CCommunityRegistration ( void )
{
    delete m_pButtonRegister;
    delete m_pButtonCancel;
    delete m_pEditUsername;
    delete m_pEditPassword;
    delete m_pLabelUsername;
    delete m_pLabelPassword;
    delete m_pWindow;
}


void CCommunityRegistration::Open ( void )
{
    if ( m_ulStartTime == 0 )
    {
        //g_pCore->GetConsole()->Printf ( "starting!" );
        // Create the URL
        std::string strURL = std::string ( REGISTRATION_URL ) + "?action=request";

        // Perform the HTTP request
        memset ( m_szBuffer, 0, REGISTRATION_DATA_BUFFER_SIZE );
        if ( m_HTTP.Get ( strURL, m_szBuffer, REGISTRATION_DATA_BUFFER_SIZE - 1 ) )
            g_pCore->GetConsole()->Printf ( "started!" );
        else
            g_pCore->GetConsole()->Printf ( "failed #1!" );

        // Store the start time
        m_ulStartTime = CClientTime::GetTime ();
    }
    //g_pCore->GetConsole()->Printf ( m_HTTP.GetStatusMessage().c_str() );
}


void CCommunityRegistration::DoPulse ( void )
{
    if ( m_ulStartTime > 0 )
    {
        char* szBuffer;
        unsigned int uiBufferLength;

        if ( m_HTTP.GetData ( &szBuffer, uiBufferLength ) )
        {
            // Succeed, deal with the response
            m_ulStartTime = 0;
            m_pWindow->SetVisible ( true );

            FILE * ff = fopen("c:\\temp.png","wb");
            fwrite(szBuffer,uiBufferLength,1,ff);
            fclose(ff);

            //g_pCore->GetConsole()->Printf ( "success!" );
        }
        else if ( ( CClientTime::GetTime () - m_ulStartTime ) > REGISTRATION_DELAY )
        {
            // Timed out
            //g_pCore->GetConsole()->Printf ( "failed #2!" );
            m_ulStartTime = 0;
        }
    }
}


bool CCommunityRegistration::OnButtonClick ( CGUIElement* pElement )
{
    Open ();
    return true;
}
