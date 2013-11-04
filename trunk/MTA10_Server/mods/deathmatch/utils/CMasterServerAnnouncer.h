/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CMasterServerAnnouncer.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct SMasterServerDefinition
{
    bool bAcceptsPush;
    bool bDoReminders;
    bool bHideProblems;
    SString strDesc;
    SString strURL;
};

enum
{
    ANNOUNCE_STAGE_NONE,
    ANNOUNCE_STAGE_INITIAL,
    ANNOUNCE_STAGE_INITIAL_RETRY,
    ANNOUNCE_STAGE_REMINDER,
};

////////////////////////////////////////////////////////////////////
//
// A remote master server to announce our existence to
//
////////////////////////////////////////////////////////////////////
class CMasterServer : public CRefCountable
{
public:
    ZERO_ON_NEW

    CMasterServer( const SMasterServerDefinition& definition )
        : m_Definition( definition )
    {
        m_Stage = ANNOUNCE_STAGE_INITIAL;
        m_uiInitialAnnounceRetryAttempts = 5;
        m_uiInitialAnnounceRetryInterval = 1000 * 60 * 5;        // 5 mins initial announce retry interval
        m_uiReminderAnnounceInterval     = 1000 * 60 * 60 * 24;  // 24 hrs reminder announce interval
        m_uiPushInterval                 = 1000 * 60 * 10;       // 10 mins push interval
    }

    //
    // Pulse this master server
    //
    void Pulse( void )
    {
        if ( m_bStatusBusy )
            return;

        long long llTickCountNow = GetTickCount64_();

        // Do announce?
        if ( g_pGame->GetConfig()->GetAseInternetListenEnabled() )
        {
            bool bIsTimeForAnnounce  = false;
            if ( m_Stage == ANNOUNCE_STAGE_INITIAL )
                bIsTimeForAnnounce = true;
            if ( m_Stage == ANNOUNCE_STAGE_INITIAL_RETRY && m_Definition.bDoReminders && llTickCountNow - m_llLastAnnounceTime > m_uiInitialAnnounceRetryInterval )
                bIsTimeForAnnounce = true;
            if ( m_Stage == ANNOUNCE_STAGE_REMINDER && m_Definition.bDoReminders && llTickCountNow - m_llLastAnnounceTime > m_uiReminderAnnounceInterval )
                bIsTimeForAnnounce = true;

            if ( bIsTimeForAnnounce )
            {
                m_llLastAnnounceTime = llTickCountNow;

                // Send request
                this->AddRef();     // Keep object alive
                m_bStatusBusy = true;
                GetDownloadManager()->QueueFile( m_Definition.strURL, NULL, 0, "", 0, false, this, StaticProgressCallback, false, 1 );
            }
        }

        // Do push?
        if ( g_pGame->GetConfig()->GetAseInternetPushEnabled() && m_Definition.bAcceptsPush )
        {
            if ( m_llLastPushTime == 0 || llTickCountNow - m_llLastPushTime > m_uiPushInterval )
            {
                m_llLastPushTime = llTickCountNow;
                SString strPostContent = ASE::GetInstance()->QueryLight();
                bool bPostContentBinary = true;
                GetDownloadManager()->QueueFile( m_Definition.strURL, NULL, 0, &strPostContent.at( 0 ), strPostContent.length(), bPostContentBinary, NULL, NULL, false, 1 );
            }
        }
    }

    //
    // Process response from master server
    //
    static bool StaticProgressCallback( double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool bComplete, int iError )
    {
        CMasterServer* pMasterServer = (CMasterServer*)obj;
        pMasterServer->ProgressCallback( sizeJustDownloaded, totalDownloaded, data, dataLength, bComplete, iError );
        if ( bComplete || iError )
            pMasterServer->Release();   // No need to keep object alive now
        return true;
    }

    //
    // Process response from master server
    //
    void ProgressCallback( double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, bool bComplete, int iError )
    {
        if ( bComplete || iError )
        {
            m_bStatusBusy = false;
        }

        if ( bComplete )
        {
            if ( m_Stage < ANNOUNCE_STAGE_REMINDER )
            {
                m_Stage = ANNOUNCE_STAGE_REMINDER;
                if ( !m_Definition.bHideProblems || iError == 200 )
                {
                    // Log successful initial announcement
                    if ( iError == 200 )
                        CLogger::LogPrintf( "%s success!\n", *m_Definition.strDesc );
                    else
                        CLogger::LogPrintf( "%s successish! (%u %s)\n", *m_Definition.strDesc, iError, GetDownloadManager()->GetError() );
                }
            }
        }
        else
        if ( iError )
        {
            bool bCanRetry = ( iError == 28 ); // We can retry if 'Timeout was reached'

            if ( m_Stage == ANNOUNCE_STAGE_INITIAL )
            {
                if ( bCanRetry )
                {
                    m_Stage = ANNOUNCE_STAGE_INITIAL_RETRY;
                    if ( !m_Definition.bHideProblems )
                    {
                        // Log initial failure
                        CLogger::LogPrintf( "%s problem (%u). Retrying...\n", *m_Definition.strDesc, iError );
                    }
                }
            }

            if ( m_Stage < ANNOUNCE_STAGE_REMINDER )
            {
                if ( !bCanRetry || m_uiInitialAnnounceRetryAttempts-- == 0 )
                {
                    // Give up initial retries
                    m_Stage = ANNOUNCE_STAGE_REMINDER;
                    if ( !m_Definition.bHideProblems )
                    {
                        // Log final failure
                        CLogger::LogPrintf( "%s failed! (%u %s)\n", *m_Definition.strDesc, iError, GetDownloadManager()->GetError() );
                    }
                }
            }
        }
    }

    //
    // Get http downloader used for master server comms etc.
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager( void )
    {
        return g_pNetServer->GetHTTPDownloadManager( EDownloadMode::ASE );
    }

protected:
    bool                            m_bStatusBusy;
    uint                            m_Stage;
    uint                            m_uiInitialAnnounceRetryAttempts;
    uint                            m_uiInitialAnnounceRetryInterval;
    uint                            m_uiReminderAnnounceInterval;
    uint                            m_uiPushInterval;
    long long                       m_llLastAnnounceTime;
    long long                       m_llLastPushTime;
    const SMasterServerDefinition   m_Definition;
};


////////////////////////////////////////////////////////////////////
//
// A list of remote master servers to announce our existence to
//
////////////////////////////////////////////////////////////////////
class CMasterServerAnnouncer
{
public:
    ZERO_ON_NEW

    ~CMasterServerAnnouncer( void )
    {
        while ( !m_MasterServerList.empty() )
        {
            m_MasterServerList.back()->Release();
            m_MasterServerList.pop_back();
        }
    }

    //
    // Make list of master servers to contact
    //
    void InitServerList( void )
    {
        assert( m_MasterServerList.empty() );

        CMainConfig* pMainConfig = g_pGame->GetConfig();
        SString strServerIP      = pMainConfig->GetServerIP();
        ushort  usServerPort     = pMainConfig->GetServerPort();
        ushort  usHTTPPort       = pMainConfig->GetHTTPPort();
        uint    uiPlayerCount    = g_pGame->GetPlayerManager()->Count ();
        uint    uiMaxPlayerCount = pMainConfig->GetMaxPlayers();
        bool    bPassworded      = pMainConfig->HasPassword();
        SString strAseMode       = pMainConfig->GetSetting( "ase" );
        bool    bAseLanListen    = pMainConfig->GetAseLanListenEnabled();

        SString strVersion( "%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD );
        SString strExtra( "%d_%d_%d_%s_%d", uiPlayerCount, uiMaxPlayerCount, bPassworded, *strAseMode, bAseLanListen );

        SMasterServerDefinition masterServerDefinitionList[] = {
                                         { false, false, true,  "Querying game-monitor.com master server...", SString( QUERY_URL_GAME_MONITOR, usServerPort + 123 ) },
                                         { true,  true,  false, "Querying MTA master server...", SString( QUERY_URL_MTA_MASTER_SERVER, usServerPort, usServerPort + 123, usHTTPPort, *strVersion, *strExtra, *strServerIP ) },
                                       };

        for ( uint i = 0 ; i < NUMELMS( masterServerDefinitionList ) ; i++ )
        {
            m_MasterServerList.push_back( new CMasterServer( masterServerDefinitionList[i] ) );
        }
    }

    //
    // Pulse each master server in our list
    //
    void Pulse( void )
    {
        if ( m_MasterServerList.empty() )
            InitServerList();

        for ( uint i = 0 ; i < m_MasterServerList.size() ; i++ )
        {
            m_MasterServerList[i]->Pulse();
        }
    }

protected:
    std::vector < CMasterServer* > m_MasterServerList;
};
