/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCommunity.cpp
*  PURPOSE:     Community connector class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
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


CCommunity::~CCommunity ( void )
{
}


void CCommunity::Logout ( void )
{
    m_ulStartTime = 0;
    m_bLoggedIn = false;
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
    std::string strSerial;
    g_pCore->GetNetwork ()->GetSerial ( strSerial );

    // Create the URL
    std::string strURL =
        std::string ( VERIFICATION_URL ) +
        "?username=" + m_strUsername + 
        "&password=" + m_strPassword +
        "&serial=" + strSerial;

    // Perform the HTTP request
    memset ( m_szBuffer, 0, VERIFICATION_DATA_BUFFER_SIZE );
    m_HTTP.Get ( strURL, m_szBuffer, VERIFICATION_DATA_BUFFER_SIZE - 1 );

    // Store the start time
    m_ulStartTime = CClientTime::GetTime ();
}


void CCommunity::DoPulse ( void )
{
    char *szBuffer;
    eVerificationResult Status;
    unsigned int nDataLength;

    // Poll the HTTP client
    if ( m_ulStartTime && m_HTTP.GetData ( &szBuffer, nDataLength ) ) {
        // Get the returned status
        Status = (eVerificationResult)(szBuffer[0] - 48);
        m_bLoggedIn = Status == VERIFY_ERROR_SUCCESS;
        m_ulStartTime = 0;

        // Change GUI
        CLocalGUI::GetSingleton ().GetMainMenu()->ChangeCommunityState ( m_bLoggedIn, m_strUsername );
        CLocalGUI::GetSingleton ().GetMainMenu()->GetSettingsWindow()->OnLoginStateChange ( m_bLoggedIn );

        // Perform callback
        if ( m_pCallback ) {
            m_pCallback ( m_bLoggedIn, szVerificationMessages[Status], m_pVerificationObject );
            m_pCallback = NULL;
            m_pVerificationObject = NULL;
        }
    }
    // Check for timeout
    else if ( ( CClientTime::GetTime () - m_ulStartTime ) > VERIFICATION_DELAY ) {
        Logout ();
    }
}
