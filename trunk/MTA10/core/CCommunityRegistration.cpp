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
#include <utils/CMD5Hasher.h>

extern CCore* g_pCore;

CCommunityRegistration::CCommunityRegistration ( void )
{
    m_ulStartTime = 0;
}


void CCommunityRegistration::CreateWindows ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Community Registration" ) );
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
    m_pEditCode->SetSize ( CVector2D ( 85.0f, 22.0f ), false );
    m_pEditCode->SetMaxLength ( 6 );

    m_pImageCode = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pWindow ) );
	m_pImageCode->SetFrameEnabled ( false );
	m_pImageCode->SetPosition ( CVector2D ( 205.0f, 180.0f ), false );

    m_pButtonRegister = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Register" ) );
    m_pButtonRegister->SetPosition ( CVector2D ( 100.0f, 270.0f ), false );
    m_pButtonRegister->SetSize ( CVector2D ( 90.0f, 20.0f ), false );

    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 200.0f, 270.0f ), false );
    m_pButtonCancel->SetSize ( CVector2D ( 70.0f, 20.0f ), false );

    m_pButtonRegister->SetOnClickHandler ( GUI_CALLBACK ( &CCommunityRegistration::OnButtonClick, this ) );
    m_pButtonCancel->SetOnClickHandler ( GUI_CALLBACK ( &CCommunityRegistration::OnButtonClick, this ) );

    m_pWindow->SetAlpha ( 0.9f );
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
        // Create the URL
        std::string strURL = std::string ( REGISTRATION_URL ) + "?action=request";

        // Perform the HTTP request
        memset ( m_szBuffer, 0, REGISTRATION_DATA_BUFFER_SIZE );
        m_HTTP.Get ( strURL, m_szBuffer, REGISTRATION_DATA_BUFFER_SIZE - 1 );

        // Store the start time
        m_ulStartTime = CClientTime::GetTime ();
    }
}


void CCommunityRegistration::DoPulse ( void )
{
    if ( m_ulStartTime > 0 )
    {
        char* szBuffer;
        unsigned int uiBufferLength;

        if ( m_HTTP.GetData ( &szBuffer, uiBufferLength ) && szBuffer[0] )
        {
            // Succeed, deal with the response
            m_ulStartTime = 0;

            // ID
            eRegistrationResult Result = (eRegistrationResult)(szBuffer[0] - 48);

            if ( Result == REGISTRATION_ERROR_REQUEST )
            {
                CGUI *pManager = g_pCore->GetGUI ();

                // Sure we have it all right?
                if ( uiBufferLength > 32 )
                {
                    // Get the hash
                    m_strCommunityHash = std::string ( &szBuffer[1], 32 );

                    // TODO: Load it without a temp file

                    // Create a temp file for the png
                    FILE * fp = fopen ( REGISTRATION_TEMP_FILE, "wb" );
                    if ( fp )
                    {
                        fwrite ( &szBuffer[33], uiBufferLength, 1, fp );
                        fclose ( fp );

                        m_pImageCode->LoadFromFile ( "temp.png" );
	                    m_pImageCode->SetSize ( CVector2D ( 65.0f, 20.0f ), false );
                        m_pWindow->SetVisible ( true );
                        m_pWindow->BringToFront ();

                        // Delete the temp file
                        remove ( REGISTRATION_TEMP_FILE );
                        return;
                    }
                }
                g_pCore->ShowMessageBox ( "Error", "Services currently unavaliable", MB_BUTTON_OK | MB_ICON_ERROR );
            }
            else if ( Result == REGISTRATION_ERROR_SUCCESS )
            {
                g_pCore->ShowMessageBox ( "Success", "Successfully registered!", MB_BUTTON_OK | MB_ICON_INFO );

                m_pWindow->SetVisible ( false );
                m_strCommunityHash.clear ();
                m_pImageCode->Clear ();
            }
            else if ( Result == REGISTRATION_ERROR_ERROR )
            {
                if ( strlen ( &szBuffer[1] ) > 0 )
                    g_pCore->ShowMessageBox ( "Error", &szBuffer[1], MB_BUTTON_OK | MB_ICON_ERROR );
                else
                    g_pCore->ShowMessageBox ( "Error", "Unexpected error", MB_BUTTON_OK | MB_ICON_ERROR );
            }
            else
            {
                g_pCore->ShowMessageBox ( "Error", "Services currently unavaliable", MB_BUTTON_OK | MB_ICON_ERROR );
            }
        }
        else if ( ( CClientTime::GetTime () - m_ulStartTime ) > REGISTRATION_DELAY )
        {
            g_pCore->ShowMessageBox ( "Error", "Services currently unavaliable", MB_BUTTON_OK | MB_ICON_ERROR );
            // Timed out
            m_ulStartTime = 0;
        }
    }
}


bool CCommunityRegistration::OnButtonClick ( CGUIElement* pElement )
{
    if ( pElement == m_pButtonCancel )
    {
        m_pWindow->SetVisible ( false );
        m_strCommunityHash.clear ();
        m_pImageCode->Clear ();
    }
    else if ( pElement == m_pButtonRegister )
    {
		if ( m_pEditUsername->GetText().empty() )
            g_pCore->ShowMessageBox ( "Error", "Username missing", MB_BUTTON_OK | MB_ICON_INFO );
        else if ( m_pEditEmail->GetText().empty() )
            g_pCore->ShowMessageBox ( "Error", "Email missing", MB_BUTTON_OK | MB_ICON_INFO );
        else if ( m_pEditPassword->GetText().empty() || m_pEditConfirm->GetText().empty() )
            g_pCore->ShowMessageBox ( "Error", "Password missing", MB_BUTTON_OK | MB_ICON_INFO );
        else if ( m_pEditPassword->GetText() != m_pEditConfirm->GetText() )
            g_pCore->ShowMessageBox ( "Error", "Passwords do not match", MB_BUTTON_OK | MB_ICON_INFO );
        else if ( m_pEditCode->GetText().empty() )
            g_pCore->ShowMessageBox ( "Error", "Validation code missing", MB_BUTTON_OK | MB_ICON_INFO );
        else
        {
            /*
            // Compare entered code to the hash we recieved
            std::string strCode;
            HashString ( m_pEditCode->GetText().c_str(), strCode );
            // case insesitive
            if ( stricmp ( strCode.c_str(), m_strCommunityHash.c_str() ) != 0 )
            {
                g_pCore->ShowMessageBox ( "Error", "Invalid validation code", MB_BUTTON_OK | MB_ICON_INFO );
                return true;
            }
            */
            if ( m_ulStartTime == 0 )
            {
                std::string strPassword;
                HashString ( m_pEditPassword->GetText().c_str(), strPassword );

                // Create the URL
                std::string strURL =
                    std::string ( REGISTRATION_URL ) +
                    "?action=register" +
                    "&username=" + m_pEditUsername->GetText() + 
                    "&password=" + strPassword +
                    "&email=" + m_pEditEmail->GetText() +
                    "&code=" + m_pEditCode->GetText() +
                    "&hash=" + m_strCommunityHash;

                // Perform the HTTP request
                memset ( m_szBuffer, 0, VERIFICATION_DATA_BUFFER_SIZE );
                m_HTTP.Get ( strURL, m_szBuffer, VERIFICATION_DATA_BUFFER_SIZE - 1 );

                // Store the start time
                m_ulStartTime = CClientTime::GetTime ();
            }
        }
    }
    return true;
}

bool CCommunityRegistration::HashString ( const char* szString, std::string& strHashString )
{
    char szHashed[33];
	if ( szString && strlen ( szString ) > 0 )
	{
		MD5 HashedStr;
		CMD5Hasher Hasher;
		Hasher.Calculate ( szString, strlen ( szString ), HashedStr );
		Hasher.ConvertToHex ( HashedStr, szHashed );
        strHashString = szHashed;
        return true;
	}
    return false;
}