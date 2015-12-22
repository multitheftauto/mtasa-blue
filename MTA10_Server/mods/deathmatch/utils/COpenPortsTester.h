/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/COpenPortsTester.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class COpenPortsTester
{
public:
    ZERO_ON_NEW

    //
    // Start a new test
    //
    void Start ( void )
    {
        if ( m_iPortTestStage != 0 )
            return;

        // Construct URL
        SString strServerIP = g_pGame->GetConfig ()->GetServerIP ();
        ushort usServerPort = g_pGame->GetConfig ()->GetServerPort ();
        ushort usHTTPPort   = g_pGame->GetConfig ()->GetHTTPPort ();
        SString strURL ( PORT_TESTER_URL "?simple=1&g=%u", usServerPort );

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
        GetDownloadManager()->QueueFile ( strURL, NULL, 0, "", 0, true, this, DownloadFinishedCallback, false, 1, 15000 );

        CLogger::LogPrintfNoStamp ( "Testing ports...\n" );

        // And now we wait...
        m_iPortTestStage = 1;
    }

    //
    // Process response from remote
    //
    static void DownloadFinishedCallback( char * data, size_t dataLength, void * obj, bool bSuccess, int iErrorCode )
    {
        COpenPortsTester* pOpenPortsTester = (COpenPortsTester*)obj;
        if ( bSuccess )
        {
            pOpenPortsTester->m_iPortTestStage = 0;
            if ( iErrorCode != 200 )
            {
                CLogger::LogPrintfNoStamp ( "Port testing service unavailable! (%u: %s)\n", iErrorCode, GetDownloadManager()->GetError() );
            }
            else
            {
                // Parse each test result
                uint uiLinesOutput = 0;
                SStringX strResult( data, dataLength );
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
        }
        else
        {
            pOpenPortsTester->m_iPortTestStage = 0;
            CLogger::LogPrintfNoStamp ( "Port testing service unavailable! (%u %s)\n", iErrorCode, GetDownloadManager()->GetError() );
        }
    }

    //
    // Get http downloader used for port testing
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager( void )
    {
        return g_pNetServer->GetHTTPDownloadManager( EDownloadMode::ASE );
    }

protected:
    int                 m_iPortTestStage;
};
