/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/COpenPortsTester.h
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class COpenPortsTester
{
public:
    COpenPortsTester ( void )
    {
        m_pTCP = NULL;
        m_pRequest = NULL;
        m_pSocket = NULL;
        m_iPortTestStage = 0;
        m_llPortTestStartTime = 0;
    }

    //
    // Start a new test
    //
    void Start ( void )
    {
        if ( m_iPortTestStage != 0 )
            return;

        // Construct URL
        const SString strServerIP = g_pGame->GetConfig ()->GetServerIP ();
        ushort usServerPort = g_pGame->GetConfig ()->GetServerPort ();
        ushort usHTTPPort = g_pGame->GetConfig ()->GetHTTPPort ();
        SString strURL ( "http://nightly.mtasa.com/ports/?simple=1&g=%u", usServerPort );

        if ( ASE::GetInstance() )
        {
            strURL += SString ( "&a=%u", usServerPort + 123 );
        }
        else
        {
            CLogger::LogPrintfNoStamp ( "ASE is not enabled, so port UDP port %u will not be tested\n", usServerPort + 123 );
        }

        if ( g_pGame->GetHTTPD () )
        {
            strURL += SString ( "&h=%u", usHTTPPort );
        }
        else
        {
            CLogger::LogPrintfNoStamp ( "HTTP server is not enabled, so port TCP port %u will not be tested\n", usHTTPPort );
        }

        // Send request
        m_pTCP = new CTCPImpl ();
        m_pTCP->Initialize ();
        m_pRequest = new CHTTPRequest ( strURL );
        m_pRequest->SetLocalIP ( strServerIP );
        m_pSocket = m_pRequest->SendRequest ( m_pTCP );

        if ( !m_pSocket )
        {
            CLogger::LogPrintfNoStamp ( "Port testing service unavailable! (Not reachable)\n" );
            SAFE_DELETE ( m_pRequest );
            SAFE_DELETE ( m_pTCP );
            return;
        }

        CLogger::LogPrintfNoStamp ( "Testing ports...\n" );

        // And now we wait...
        m_llPortTestStartTime = GetTickCount64_ ();
        m_iPortTestStage = 1;
    }


    //
    // Update any running test
    //
    void Poll ( void )
    {
        if ( m_iPortTestStage != 1 )
            return;

        // Wait for 10 seconds to minimize server stalling
        if ( GetTickCount64_ () - m_llPortTestStartTime < 10 * 1000 )
            return;

        m_iPortTestStage = 0;

        // Get result
        CHTTPResponse* pResponse = m_pRequest->GetResponse ( m_pSocket );
        m_pSocket = NULL;

        if ( !pResponse )
        {
            CLogger::LogPrintfNoStamp ( "Port testing service unavailable! (Not available)\n" );
        }
        else if ( pResponse->GetErrorCode () != 200 )
        {
            CLogger::LogPrintfNoStamp ( "Port testing service unavailable! (%u: %s)\n", pResponse->GetErrorCode (), pResponse->GetErrorDescription () );
        }
        else
        {
            // Parse each test result
            uint uiLinesOutput = 0;
            SString strResult = pResponse->GetData ();
            std::vector < SString > parts;
            strResult.Split( "&", parts );
            for ( uint i = 0 ; i < parts.size () ; i++ )
            {
                // Parse one test result
                ushort usPort = 0;
                SString strType, strValue, strProtocol, strAction;
                parts[i].Split( "=", &strType, &strValue );

                if ( strType == "a" )
                {
                    usPort = g_pGame->GetConfig ()->GetServerPort () + 123;
                    strProtocol = "UDP";
                    strAction = "browse";
                }
                else
                if ( strType == "g" )
                {
                    usPort = g_pGame->GetConfig ()->GetServerPort ();
                    strProtocol = "UDP";
                    strAction = "join";
                }
                else
                if ( strType == "h" )
                {
                    usPort = g_pGame->GetConfig ()->GetHTTPPort ();
                    strProtocol = "TCP";
                    strAction = "download";
                }
                else
                    continue;

                SString strStatus;
                if ( strValue == "1" )
                    strStatus = SString ( "Port %u %s is open.", usPort, *strProtocol );
                else
                    strStatus = SString ( "Port %u %s is closed. Players can not %s!", usPort, *strProtocol, *strAction );

                // Output test result
                CLogger::LogPrintfNoStamp ( "%s\n", *strStatus );
                uiLinesOutput++;
            }

            if ( uiLinesOutput == 0 )
                CLogger::LogPrintfNoStamp ( "Unexpected response:\n%s\n", *strResult );
        }

        SAFE_DELETE ( pResponse );
        SAFE_DELETE ( m_pRequest );
        SAFE_DELETE ( m_pTCP );
    }

protected:
    CTCPImpl*           m_pTCP;
    CHTTPRequest*       m_pRequest;
    CTCPClientSocket*   m_pSocket;
    int                 m_iPortTestStage;
    long long           m_llPortTestStartTime;
};
