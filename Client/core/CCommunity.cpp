/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCommunity.cpp
*  PURPOSE:     Community connector class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

template<> CCommunity * CSingleton< CCommunity >::m_pSingleton = NULL;

char *szVerificationMessages[] = {
    "Unexpected error",
    "Success",
    "Your account does not seem to exist",
    "Computer already in use",
    "You have reached your serials quota",
    "Invalid username/password",
    "Account provided is already in use",
    "Version error",
    "Account is banned",
    "Serial is banned",
    "Could not connect to master server"
};

CCommunity::CCommunity ( void )
{
    m_ulStartTime = 0;
    m_pVerificationObject = 0;
    m_pCallback = NULL;
    m_bLoggedIn = false;
}


void CCommunity::Initialize ( void )
{
    CVARS_GET ( "community_username", m_strUsername );
    CVARS_GET ( "community_password", m_strPassword );

    if ( !m_strUsername.empty() && !m_strPassword.empty() )
        Login ();
}

CCommunity::~CCommunity ( void )
{
}


void CCommunity::Logout ( void )
{
    m_ulStartTime = 0;
    m_bLoggedIn = false;

    CVARS_SET ( "community_username", std::string () );
    CVARS_SET ( "community_password", std::string () );

    // Change GUI
    CLocalGUI::GetSingleton ().GetMainMenu()->ChangeCommunityState ( false, "" );
    CLocalGUI::GetSingleton ().GetMainMenu()->GetSettingsWindow()->OnLoginStateChange ( false );
}


void CCommunity::Login ( VERIFICATIONCALLBACK pCallBack, void* pObject )
{
    m_bLoggedIn = false;

    if ( pCallBack && pObject )
    {
        m_pCallback = pCallBack;
        m_pVerificationObject = pObject;
    }

    // Get our serial number
    char szSerial [ 64 ];
    g_pCore->GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

    // Create the URL
    SString strURL ( VERIFICATION_URL "?username=%s&password=%s&serial=%s",
                     m_strUsername.c_str(),
                     m_strPassword.c_str(),
                     szSerial );

    // Perform the HTTP request
    GetHTTP()->Reset();
    GetHTTP()->QueueFile( strURL, NULL, 0, NULL, 0, false, this, StaticDownloadFinished, false, 1/*uiConnectionAttempts*/ );

    // Store the start time
    m_ulStartTime = CClientTime::GetTime ();
}


void CCommunity::DoPulse ( void )
{
    if ( m_ulStartTime )
    {
        GetHTTP()->ProcessQueuedFiles();
    }
}


// Get the HTTP download manager used for community stuff
CNetHTTPDownloadManagerInterface* CCommunity::GetHTTP( void )
{
    return g_pCore->GetNetwork()->GetHTTPDownloadManager( EDownloadMode::CORE_UPDATER );
}


// Handle server response
void CCommunity::StaticDownloadFinished ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode )
{
    if ( bSuccess )
        ((CCommunity*)pObj)->DownloadSuccess( pCompletedData, completedLength );
    else
        ((CCommunity*)pObj)->DownloadFailed( iErrorCode );
}

void CCommunity::DownloadSuccess( char* pCompletedData, size_t completedLength )
{
    // Get the returned status
    eVerificationResult Status = VERIFY_ERROR_UNEXPECTED;
    if ( completedLength > 0 )
    {
        Status = (eVerificationResult)(pCompletedData[0] - 48);
    }
            
    m_bLoggedIn = Status == VERIFY_ERROR_SUCCESS;
    m_ulStartTime = 0;

    // Change GUI
    CLocalGUI::GetSingleton ().GetMainMenu()->ChangeCommunityState ( m_bLoggedIn, m_strUsername );
    CLocalGUI::GetSingleton ().GetMainMenu()->GetSettingsWindow()->OnLoginStateChange ( m_bLoggedIn );

    // Perform callback
    if ( m_pCallback )
    {
        m_pCallback ( m_bLoggedIn, szVerificationMessages[Status], m_pVerificationObject );
        m_pCallback = NULL;
        m_pVerificationObject = NULL;
    }
}

void CCommunity::DownloadFailed( int iErrorCode )
{
    m_ulStartTime = 0;
    g_pCore->ShowMessageBox ( _("Error")+_E("CC01"), _("Services currently unavailable"), MB_BUTTON_OK | MB_ICON_ERROR );
    Logout ();
}
