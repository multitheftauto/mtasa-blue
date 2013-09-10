/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CMasterServerAnnouncer.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct SAnnounceInfo
{
    SAnnounceInfo( const SString& strDesc, bool bIsInitialAnnounce, bool bHideProblems )
        : strDesc( strDesc )
        , bIsInitialAnnounce( bIsInitialAnnounce )
        , bHideProblems( bHideProblems )
    {}
    SString strDesc;
    bool    bIsInitialAnnounce;
    bool    bHideProblems;
};


class CMasterServerAnnouncer
{
public:
    ZERO_ON_NEW

    //
    // Remind master server once every 24 hrs
    //
    void Pulse( bool bIsInitialAnnounce = false )
    {
        long long llTickCountNow = GetTickCount64_ ();
        bool bIsTimeForAnnounce  = false;
        bool bIsTimeForPush      = false;
        CMainConfig* pMainConfig = g_pGame->GetConfig();

        if ( pMainConfig->GetAseInternetListenEnabled() &&
            ( bIsInitialAnnounce || m_llLastAnnounceTime == 0 || llTickCountNow - m_llLastAnnounceTime > 1000 * 60 * 60 * 24 ) )   // 24 hrs
        {
            bIsTimeForAnnounce = true;
            m_llLastAnnounceTime = llTickCountNow;
        }
        else
        if ( pMainConfig->GetAseInternetPushEnabled() &&
            ( m_llLastPushTime == 0 || llTickCountNow - m_llLastPushTime > 1000 * 60 * 10 ) ) // 10 mins
        {
            bIsTimeForPush = true;
            m_llLastPushTime = llTickCountNow;
        }
        else
            return;


        SString strServerIP      = pMainConfig->GetServerIP ();
        ushort  usServerPort     = pMainConfig->GetServerPort ();
        ushort  usHTTPPort       = pMainConfig->GetHTTPPort ();
        uint    uiPlayerCount    = g_pGame->GetPlayerManager()->Count ();
        uint    uiMaxPlayerCount = pMainConfig->GetMaxPlayers();
        bool    bPassworded      = pMainConfig->HasPassword ();
        SString strAseMode       = pMainConfig->GetSetting( "ase" );
        bool    bAseLanListen    = pMainConfig->GetAseLanListenEnabled();

        SString strVersion ( "%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD );
        SString strExtra ( "%d_%d_%d_%s_%d", uiPlayerCount, uiMaxPlayerCount, bPassworded, *strAseMode, bAseLanListen );

        struct {
            bool bAcceptsPush;
            bool bDoReminders;
            bool bHideProblems;
            SString strDesc;
            SString strURL;
        } masterServerList[] = {
                                 { false, false, true,  "Querying game-monitor.com master server... ", SString ( QUERY_URL_GAME_MONITOR, usServerPort + 123 ) },
                                 { true,  true,  false, "Querying MTA master server... ", SString ( QUERY_URL_MTA_MASTER_SERVER, usServerPort, usServerPort + 123, usHTTPPort, *strVersion, *strExtra, *strServerIP ) },
                               };

        for ( uint i = 0 ; i < NUMELMS( masterServerList ) ; i++ )
        {
            bool bServerAcceptsPush      = masterServerList[i].bAcceptsPush;
            bool bServerAcceptsReminders = masterServerList[i].bDoReminders;
            bool bHideProblems           = masterServerList[i].bHideProblems;
            const SString& strDesc       = masterServerList[i].strDesc;
            const SString& strURL        = masterServerList[i].strURL;

            if ( bIsTimeForAnnounce )
            {
                // Don't repeat announce for some servers
                if ( !bIsInitialAnnounce && !bServerAcceptsReminders )
                    continue;

                // Send request
                SAnnounceInfo* pAnnounceInfo = new SAnnounceInfo( strDesc, bIsInitialAnnounce, bHideProblems );
                GetDownloadManager()->QueueFile ( strURL, NULL, 0, "", 0, false, pAnnounceInfo, ProgressCallback, false, 1 );
            }

            // Send extra data if required
            if ( bIsTimeForPush && bServerAcceptsPush )
            {
                SString strPostContent = ASE::GetInstance()->QueryLight ();
                bool bPostContentBinary = true;

                GetDownloadManager()->QueueFile ( strURL, NULL, 0, &strPostContent.at ( 0 ), strPostContent.length (), bPostContentBinary, NULL, NULL, false, 1 );
            }
        }
    }

    //
    // Process response from master server
    //
    static bool ProgressCallback( double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool bComplete, int iError )
    {
        SAnnounceInfo* pInfo = (SAnnounceInfo*)obj;

        if ( bComplete )
        {
            if ( pInfo->bIsInitialAnnounce )
            {
                if ( !pInfo->bHideProblems || iError == 200 )
                {
                    CLogger::LogPrint ( pInfo->strDesc );
                    if ( iError == 200 )
                        CLogger::LogPrintfNoStamp ( "success!\n");
                    else
                        CLogger::LogPrintfNoStamp ( "successish! (%u %s)\n", iError, GetDownloadManager()->GetError() );
                }
            }
        }
        else
        if ( iError )
        {
            if ( pInfo->bIsInitialAnnounce )
            {
                if ( !pInfo->bHideProblems )
                {
                    CLogger::LogPrint ( pInfo->strDesc );
                    CLogger::LogPrintfNoStamp ( "failed! (%u %s)\n", iError, GetDownloadManager()->GetError() );
                }
            }
        }

        delete pInfo;
        return true;
    }

    //
    // Get http downloader used for master server comms etc.
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager( void )
    {
        return g_pNetServer->GetHTTPDownloadManager( EDownloadMode::ASE );
    }

protected:
    long long           m_llLastAnnounceTime;
    long long           m_llLastPushTime;
};
