/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSerialManager.cpp
*  PURPOSE:     Serial verification manager class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

const char *szSerialErrorMessages[] = {
	"This server requires you to be logged in. Please make sure your username/password details are correct and that you are logged in.",
	"Success.",
    "This server requires you to be logged in. Please make sure your username/password details are correct and that you are logged in.",
	"This server requires you to be logged in. Please make sure your username/password details are correct and that you are logged in.",
	"This server requires you to be logged in. Please make sure your username/password details are correct and that you are logged in.",
	"This server requires you to be logged in. Please make sure your username/password details are correct and that you are logged in.",
	"The account that you are using to log in to this server is already in use.",
	"An error occurred during the validation of your account.",
	"Your account has been rejected. You cannot access servers with serial verification turned on.",
    "Your account has been rejected. You cannot access servers with serial verification turned on.",
};

CSerialManager::CSerialManager ( void )
{

}

CSerialManager::~CSerialManager ( void )
{
    list< CSerialVerification* >::iterator iter = m_calls.begin ();
    for ( ; iter != m_calls.end (); iter++ )
    {
        delete (*iter);
    }

    m_calls.clear();
}

void CSerialManager::Verify ( CPlayer* pPlayer, SERIALVERIFICATIONCALLBACK pCallBack )
{
	m_calls.push_back ( new CSerialVerification ( pPlayer, pCallBack ) );
}

bool CSerialManager::IsVerified ( CSerialVerification * call )
{
    list< CSerialVerification* >::iterator iter = m_calls.begin ();
    for ( ; iter != m_calls.end (); iter++ )
    {
        if ( (*iter) == call )
            return true;
    }
    return false;
}

void CSerialManager::DoPulse ( void )
{
    list< CSerialVerification* >::iterator iter = m_calls.begin ();
    for ( ; iter != m_calls.end (); iter++ )
    {
        (*iter)->DoPulse();
		if ( (*iter)->IsFinished() )
		{
			delete *iter;
			m_calls.remove ( *iter );
			iter = m_calls.begin ();
		}
    }
}

void CSerialManager::Remove ( CPlayer* pPlayer )
{
    list< CSerialVerification* >::iterator iter = m_calls.begin ();
    while ( iter != m_calls.end () )
    {
		if ( (*iter)->GetPlayer() == pPlayer )
		{
            delete *iter;
            iter = m_calls.erase ( iter );
		}
        else
            ++iter;
    }
}

void CSerialManager::Remove ( CSerialVerification * pCall )
{
    if ( pCall && IsVerified ( pCall ) )
    {
        m_calls.remove ( pCall );
        delete pCall;
    }
}



CSerialVerification::CSerialVerification ( CPlayer* pPlayer, SERIALVERIFICATIONCALLBACK pCallBack )
{
    char buf[32] = {0};
    CLogger::LogPrintf ( "VERIFY: Querying %s\n", SERIAL_VERIFICATION_SERVER );

	m_pCallBack = pCallBack;
	m_pPlayer = pPlayer;
	m_ulStartTime = GetTickCount();
	m_bFinished = false;

	if ( pPlayer->GetSerialUser().length() > 0 && pPlayer->GetSerial().length() > 0 )
	{
        // Create the POST URL
        std::string strPostData = "[\"" + pPlayer->GetSerialUser () +
                                  "\",\"" + pPlayer->GetSerial () +
                                  "\"," + itoa ( g_pGame->GetConfig ()->GetServerPort (), buf, 10 ) + "]";

        // Use CURL to perform the POST
        CNetHTTPDownloadManagerInterface * pHTTP = g_pNetServer->GetHTTPDownloadManager ();
		pHTTP->QueueFile ( SERIAL_VERIFICATION_URL, NULL, 0, strPostData.c_str (), this, ProgressCallback );
		if ( !pHTTP->IsDownloading () )
			pHTTP->StartDownloadingQueuedFiles ();
	}
	else
	{
		m_pCallBack ( m_pPlayer, false, szSerialErrorMessages[SERIAL_ERROR_INVALID_ACCOUNT] );
		m_bFinished = true;
	}
}

CSerialVerification::~CSerialVerification ( void )
{
}

void CSerialVerification::DoPulse ( void )
{
	if ( GetTickCount() > m_ulStartTime + SERIAL_VERIFICATION_TIMEOUT && !m_bFinished )
	{
		m_pCallBack ( m_pPlayer, true, NULL );

		m_bFinished = true;
	}
}

void CSerialVerification::ProgressCallback ( double nJustDownloaded, double nTotalDownloaded, char * szData, size_t nDataLength, void * pObject, bool bComplete, int iError )
{
    #define HTTP_HEADER_SKIP    48
    CSerialVerification * pCall = (CSerialVerification*) pObject;
	if ( g_pGame->GetSerialManager ()->IsVerified ( pCall ) )
    {
        if ( bComplete )
        {
            if ( nDataLength > 0 )
            {
			    eSerialVerificationResult Result = static_cast < eSerialVerificationResult > ( szData[0] - HTTP_HEADER_SKIP );
			    if ( Result >= 0 && Result < SERIAL_ERROR_LAST )
			    {
                    if ( Result == SERIAL_ERROR_SUCCESS ) {
                        CLogger::LogPrintf ( "VERIFY: Player was accepted by master server\n" );
                        // Store the community ID
				        pCall->GetPlayer ()->SetCommunityID ( &szData[2] );
                        // Complete the connect packet (allowed)
                        pCall->GetCallBack ()( pCall->GetPlayer (), true, NULL );
                    } else {
                        CLogger::LogPrintf ( "VERIFY: Player was rejected by master server (%s)\n",szSerialErrorMessages[Result]  );
                        // Complete the connect packet (rejected)
                        pCall->GetCallBack ()( pCall->GetPlayer (), false, szSerialErrorMessages[Result] );
                    }
                    // Delete ourselves and return
		            g_pGame->GetSerialManager ()->Remove ( pCall );
                    return;
			    }
            }
        }
        if ( bComplete || iError )
        {
            // An error occurred, let the player in
            CLogger::LogPrintf ( "VERIFY: Error with master server occurred, allowing player\n" );
            // Complete the connect packet (allowed)
            pCall->GetCallBack ()( pCall->GetPlayer (), true, NULL );
            // Delete ourselves
		    g_pGame->GetSerialManager ()->Remove ( pCall );
        }
    }
}
