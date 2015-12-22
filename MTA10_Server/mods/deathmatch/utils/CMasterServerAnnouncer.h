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
    bool bHideSuccess;
    uint uiReminderIntervalMins;
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
        m_uiPushInterval                 = 1000 * 60 * 10;       // 10 mins push interval
    }

protected:
    ~CMasterServer( void ) {}    // Must use Release()
public:

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
            if ( m_Stage == ANNOUNCE_STAGE_REMINDER && m_Definition.bDoReminders && llTickCountNow - m_llLastAnnounceTime > TICKS_FROM_MINUTES( m_Definition.uiReminderIntervalMins ) )
                bIsTimeForAnnounce = true;

            if ( bIsTimeForAnnounce )
            {
                m_llLastAnnounceTime = llTickCountNow;

                // Send request
                this->AddRef();     // Keep object alive
                m_bStatusBusy = true;
                GetDownloadManager()->QueueFile( m_Definition.strURL, NULL, 0, "", 0, false, this, StaticDownloadFinishedCallback, false, 2 );
            }
        }
        else
        {
            m_Stage = ANNOUNCE_STAGE_INITIAL;
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
    static void StaticDownloadFinishedCallback( char * data, size_t dataLength, void * obj, bool bSuccess, int iErrorCode )
    {
        CMasterServer* pMasterServer = (CMasterServer*)obj;
        pMasterServer->DownloadFinishedCallback( data, dataLength, bSuccess, iErrorCode );
        pMasterServer->Release();   // No need to keep object alive now
    }

    //
    // Process response from master server
    //
    void DownloadFinishedCallback( char * data, size_t dataLength, bool bSuccess, int iErrorCode )
    {
        m_bStatusBusy = false;

        if ( bSuccess )
        {
            if ( m_Stage < ANNOUNCE_STAGE_REMINDER )
            {
                m_Stage = ANNOUNCE_STAGE_REMINDER;
                if ( !m_Definition.bHideSuccess )
                {
                    CArgMap argMap;
                    argMap.SetFromString( data );
                    SString strOkMessage = argMap.Get( "ok_message" );

                    // Log successful initial announcement
                    if ( iErrorCode == 200 )
                        CLogger::LogPrintf( "%s success! %s\n", *m_Definition.strDesc, *strOkMessage );
                    else
                        CLogger::LogPrintf( "%s successish! (%u %s)\n", *m_Definition.strDesc, iErrorCode, GetDownloadManager()->GetError() );
                }
            }
        }
        else
        {
            bool bCanRetry = ( iErrorCode == 28 ); // We can retry if 'Timeout was reached'

            if ( m_Stage == ANNOUNCE_STAGE_INITIAL )
            {
                if ( bCanRetry )
                {
                    m_Stage = ANNOUNCE_STAGE_INITIAL_RETRY;
                    if ( !m_Definition.bHideProblems )
                    {
                        // Log initial failure
                        CLogger::LogPrintf( "%s no response. Retrying...\n", *m_Definition.strDesc );
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
                        CLogger::LogPrintf( "%s failed! (%u %s)\n", *m_Definition.strDesc, iErrorCode, GetDownloadManager()->GetError() );
                    }
                }
            }
        }
    }

    const SMasterServerDefinition& GetDefinition( void ) const
    {
        return m_Definition;
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
        AddServer( true, true, false, false, 60 * 24, "Querying MTA master server...", QUERY_URL_MTA_MASTER_SERVER );
    }

    void AddServer( bool bAcceptsPush, bool bDoReminders, bool bHideProblems, bool bHideSuccess, uint uiReminderIntervalMins, const SString& strDesc, const SString& strInUrl )
    {
        // Check if server is already present
        for( auto pMasterServer : m_MasterServerList )
        {
            if ( pMasterServer->GetDefinition().strURL.BeginsWithI( strInUrl.SplitLeft( "%" ) ) )
                return;
        }

        CMainConfig* pMainConfig = g_pGame->GetConfig();
        SString strServerIP      = pMainConfig->GetServerIP();
        ushort  usServerPort     = pMainConfig->GetServerPort();
        ushort  usHTTPPort       = pMainConfig->GetHTTPPort();
        uint    uiMaxPlayerCount = pMainConfig->GetMaxPlayers();
        bool    bPassworded      = pMainConfig->HasPassword();
        SString strAseMode       = pMainConfig->GetSetting( "ase" );
        bool    bAseLanListen    = pMainConfig->GetAseLanListenEnabled();

        SString strVersion( "%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD );
        SString strExtra( "%d_%d_%d_%s_%d", 0, uiMaxPlayerCount, bPassworded, *strAseMode, bAseLanListen );

        SString strUrl = strInUrl;
        strUrl = strUrl.Replace( "%GAME%", SString( "%u", usServerPort ) );
        strUrl = strUrl.Replace( "%ASE%", SString( "%u", usServerPort + 123 ) );
        strUrl = strUrl.Replace( "%HTTP%", SString( "%u", usHTTPPort ) );
        strUrl = strUrl.Replace( "%VER%", strVersion );
        strUrl = strUrl.Replace( "%EXTRA%", strExtra );
        strUrl = strUrl.Replace( "%IP%", strServerIP );

        SMasterServerDefinition masterServerDefinition = { bAcceptsPush, bDoReminders, bHideProblems, bHideSuccess, uiReminderIntervalMins, strDesc, strUrl };
        m_MasterServerList.push_back( new CMasterServer( masterServerDefinition ) );
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
