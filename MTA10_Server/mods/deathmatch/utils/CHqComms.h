/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHqComms.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

enum
{
    HQCOMMS_STAGE_NONE,
    HQCOMMS_STAGE_TIMER,
    HQCOMMS_STAGE_QUERY,
};

////////////////////////////////////////////////////////////////////
//
// Check with MTA HQ for status updates
//
////////////////////////////////////////////////////////////////////
class CHqComms : public CRefCountable
{
public:
    ZERO_ON_NEW

    CHqComms( void )
    {
        m_iPollInterval = TICKS_FROM_MINUTES( 60 );
        m_strURL = HQCOMMS_URL;
        m_strCrashInfoFilename = "dumps/server_pending_upload.log";
    }

    //
    // Send query if it's time
    //
    void Pulse( void )
    {
        // Check for min client version info
        if ( m_Stage == HQCOMMS_STAGE_NONE || ( m_Stage == HQCOMMS_STAGE_TIMER && m_CheckTimer.Get() > (uint)m_iPollInterval ) )
        {
            m_CheckTimer.Reset();
            m_Stage = HQCOMMS_STAGE_QUERY;

            SString strUrlParams;
            strUrlParams += SString( "?ip=%s", *g_pGame->GetConfig()->GetServerIP() );
            strUrlParams += SString( "&gport=%d", g_pGame->GetConfig()->GetServerPort() );
            strUrlParams += SString( "&version=%s", *CStaticFunctionDefinitions::GetVersionSortable() );
            strUrlParams += SString( "&minclientautoupdate=%d", g_pGame->GetConfig()->GetMinClientVersionAutoUpdate() );
            strUrlParams += SString( "&minclientversion=%s", *g_pGame->GetConfig()->GetMinClientVersion() );
            strUrlParams += SString( "&badscriptrev=%d", m_iPrevBadFileHashesRev );
            strUrlParams += SString( "&maxplayers=%d", g_pGame->GetConfig()->GetHardMaxPlayers() );
            strUrlParams += SString( "&numplayers=%d", g_pGame->GetPlayerManager()->Count() );
            strUrlParams += SString( "&asepush=%d", g_pGame->GetConfig()->GetAseInternetPushEnabled() );
            strUrlParams += SString( "&aselisten=%d", g_pGame->GetConfig()->GetAseInternetListenEnabled() );

            SString strCrashInfo;
            FileLoad( m_strCrashInfoFilename, strCrashInfo );
            strUrlParams += SString( "&crashinfosize=%d", strCrashInfo.length() );

            // Send request
            this->AddRef();     // Keep object alive
            GetDownloadManager()->QueueFile( m_strURL + strUrlParams, NULL, 0, strCrashInfo, strCrashInfo.length(), true, this, StaticProgressCallback, false, 1 );
        }
    }

    //
    // Process response from hq
    //
    static bool StaticProgressCallback( double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool bComplete, int iError )
    {
        CHqComms* pHqComms = (CHqComms*)obj;
        pHqComms->ProgressCallback( sizeJustDownloaded, totalDownloaded, data, dataLength, bComplete, iError );
        if ( bComplete || iError )
            pHqComms->Release();   // No need to keep object alive now
        return true;
    }

    //
    // Process response from hq
    //
    void ProgressCallback( double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, bool bComplete, int iError )
    {
        if ( bComplete )
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
            CArgMap argMap;
            argMap.SetFromString( data );

            // Process various parts of returned data
            ProcessPollInterval( argMap );
            ProcessMinClientVersion( argMap );
            ProcessMessage( argMap );
            ProcessBadFileHashes( argMap );
            ProcessCrashInfo( argMap );
        }
        else
        if ( iError )
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
        }
    }

    // Interval until next HQ check
    void ProcessPollInterval( const CArgMap& argMap )
    {
        int iPollInterval;
        argMap.Get( "PollInterval", iPollInterval, m_iPollInterval );
        if ( iPollInterval )
            m_iPollInterval = Max( TICKS_FROM_MINUTES( 5 ), iPollInterval );
    }

    // Auto update of min client check
    void ProcessMinClientVersion( const CArgMap& argMap )
    {
        int iForceSetting;
        argMap.Get( "ForceMinClientVersion", iForceSetting );
        SString strResultMinClientVersion = argMap.Get( "AutoMinClientVersion" );
        SString strSetttingsMinClientVersion = g_pGame->GetConfig ()->GetMinClientVersion();
        if ( strResultMinClientVersion > strSetttingsMinClientVersion || iForceSetting )
        {
            g_pGame->GetConfig ()->SetSetting ( "minclientversion", strResultMinClientVersion, true );
        }
    }

    // Messsage for this server from HQ
    void ProcessMessage( const CArgMap& argMap )
    {
        int iMessageAlwaysPrint;
        argMap.Get( "MessageAlwaysPrint", iMessageAlwaysPrint );
        SString strMessage = argMap.Get( "Message" );
        if ( !strMessage.empty() && ( strMessage != m_strPrevMessage || iMessageAlwaysPrint ) )
        {
            m_strPrevMessage = strMessage;
            CLogger::LogPrintNoStamp( strMessage );
        }
    }

    // Block script hashes
    void ProcessBadFileHashes( const CArgMap& argMap )
    {
        int iBadFileHashesRev;
        argMap.Get( "BadFileHashesRev", iBadFileHashesRev );
        if ( iBadFileHashesRev && ( iBadFileHashesRev == 1 || iBadFileHashesRev != m_iPrevBadFileHashesRev ) )
        {
            m_iPrevBadFileHashesRev = iBadFileHashesRev;
            g_pGame->GetResourceManager()->ClearBlockedFileReason( "" );
            std::vector < SString > itemList;
            argMap.Get( "BadFileHashes" ).Split( ",", itemList );
            for ( uint i = 0 ; i < itemList.size() ; i++ )
            {
                SString strHash, strReason;
                itemList[i].Split( "|", &strHash, &strReason );
                g_pGame->GetResourceManager()->AddBlockedFileReason( strHash, strReason );
            }
            g_pGame->GetResourceManager()->SaveBlockedFileReasons();
        }
    }

    // Got crashinfo recpt
    void ProcessCrashInfo( const CArgMap& argMap )
    {
        int iGotCrashInfo;
        argMap.Get( "GotCrashInfo", iGotCrashInfo );
        if ( iGotCrashInfo )
        {
            FileDelete( m_strCrashInfoFilename );
        }
    }


    //
    // Get http downloader used for hq comms etc.
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager( void )
    {
        return g_pNetServer->GetHTTPDownloadManager( EDownloadMode::ASE );
    }

protected:
    ~CHqComms( void ) {}    // Must use Release()

    int             m_iPollInterval;
    int             m_iPrevBadFileHashesRev;
    uint            m_Stage;
    CElapsedTime    m_CheckTimer;
    SString         m_strURL;
    SString         m_strPrevMessage;
    SString         m_strCrashInfoFilename;
};
