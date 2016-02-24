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
        m_strCrashLogFilename = g_pServerInterface->GetAbsolutePath( PathJoin( SERVER_DUMP_PATH, "server_pending_upload.log" ) );
        m_strCrashDumpMeta = g_pServerInterface->GetAbsolutePath( PathJoin( SERVER_DUMP_PATH, "server_pending_upload_filename" ) );
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

            CBitStream bitStream;
            bitStream->Write( (char)3 );    // Data version
            bitStream->WriteStr( g_pGame->GetConfig()->GetServerIP() );
            bitStream->Write( g_pGame->GetConfig()->GetServerPort() );
            bitStream->WriteStr( CStaticFunctionDefinitions::GetVersionSortable() );
            bitStream->Write( g_pGame->GetConfig()->GetMinClientVersionAutoUpdate() );
            bitStream->WriteStr( g_pGame->GetConfig()->GetMinClientVersion() );
            bitStream->Write( m_iPrevBadFileHashesRev );
            bitStream->Write( g_pGame->GetConfig()->GetHardMaxPlayers() );
            bitStream->Write( g_pGame->GetPlayerManager()->Count() );
            bitStream->Write( g_pGame->GetConfig()->GetAseInternetPushEnabled() ? 1 : 0 );
            bitStream->Write( g_pGame->GetConfig()->GetAseInternetListenEnabled() ? 1 : 0 );

            SString strCrashLog;
            FileLoad( m_strCrashLogFilename, strCrashLog, 50000 );
            bitStream->WriteStr( strCrashLog );

            // Latest crash dump
            SString strCrashDumpFilename, strCrashDumpContent;
            if ( FileExists( m_strCrashDumpMeta ) )
            {
                if ( g_pGame->GetConfig()->GetCrashDumpUploadEnabled() )
                {
                    FileLoad( m_strCrashDumpMeta, strCrashDumpFilename );
                    FileLoad( strCrashDumpFilename, strCrashDumpContent );
                }
                // Only attempt to send crashdump once
                FileDelete( m_strCrashDumpMeta );
                m_strCrashDumpMeta = "";
            }
            bitStream->WriteStr( ExtractFilename( strCrashDumpFilename ) );
            bitStream->WriteStr( strCrashDumpContent );

            bitStream->WriteStr( MTA_OS_STRING );
            bitStream->WriteStr( g_pGame->GetConfig()->GetServerIPList() );

            // Send request
            this->AddRef();     // Keep object alive
            GetDownloadManager()->QueueFile( m_strURL, NULL, 0, (const char*)bitStream->GetData(), bitStream->GetNumberOfBytesUsed(), true, this, StaticDownloadFinishedCallback, false, 2 );
        }
    }

    //
    // Process response from hq
    //
    static void StaticDownloadFinishedCallback( char * data, size_t dataLength, void * obj, bool bSuccess, int iErrorCode )
    {
        CHqComms* pHqComms = (CHqComms*)obj;
        pHqComms->DownloadFinishedCallback( data, dataLength, bSuccess, iErrorCode );
        pHqComms->Release();   // No need to keep object alive now
    }

    //
    // Process response from hq
    //
    void DownloadFinishedCallback( char * data, size_t dataLength, bool bSuccess, int iErrorCode )
    {
        if ( bSuccess )
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
            CBitStream bitStream( data, dataLength );

            // Process various parts of returned data
            ProcessPollInterval( bitStream );
            ProcessMinClientVersion( bitStream );
            ProcessMessage( bitStream );
            ProcessBadFileHashes( bitStream );
            ProcessCrashInfo( bitStream );
            ProcessAseServers( bitStream );
        }
        else
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
        }
    }

    // Interval until next HQ check
    void ProcessPollInterval( CBitStream& bitStream )
    {
        int iPollInterval = 0;
        bitStream->Read( iPollInterval );
        if ( iPollInterval )
            m_iPollInterval = Max( TICKS_FROM_MINUTES( 5 ), iPollInterval );
    }

    // Auto update of min client check
    void ProcessMinClientVersion( CBitStream& bitStream )
    {
        int iForceSetting = 0;
        SString strResultMinClientVersion;

        bitStream->Read( iForceSetting );
        bitStream->ReadStr( strResultMinClientVersion );
        SString strSetttingsMinClientVersion = g_pGame->GetConfig ()->GetMinClientVersion();
        if ( strResultMinClientVersion > strSetttingsMinClientVersion || iForceSetting )
        {
            g_pGame->GetConfig ()->SetSetting ( "minclientversion", strResultMinClientVersion, true );
        }
    }

    // Messsage for this server from HQ
    void ProcessMessage( CBitStream& bitStream )
    {
        int iMessageAlwaysPrint = 0;
        SString strMessage;

        bitStream->Read( iMessageAlwaysPrint );
        bitStream->ReadStr( strMessage );
        if ( !strMessage.empty() && ( strMessage != m_strPrevMessage || iMessageAlwaysPrint ) )
        {
            m_strPrevMessage = strMessage;
            CLogger::LogPrintNoStamp( strMessage );
        }
    }

    // Block script hashes
    void ProcessBadFileHashes( CBitStream& bitStream )
    {
        int iBadFileHashesRev = 0;
        uint uiNumHashes = 0;
        struct SHashItem { SString strHash, strReason; };
        std::vector < SHashItem > itemList;

        bitStream->Read( iBadFileHashesRev );
        bitStream->Read( uiNumHashes );
        for( uint i = 0 ; i < uiNumHashes ; i++ )
        {
            SString strHash, strReason;
            bitStream->ReadStr( strHash );
            if ( !bitStream->ReadStr( strReason ) )
                break;
            itemList.push_back( { strHash, strReason } );
        }

        if ( iBadFileHashesRev && ( iBadFileHashesRev == 1 || iBadFileHashesRev != m_iPrevBadFileHashesRev ) )
        {
            m_iPrevBadFileHashesRev = iBadFileHashesRev;
            g_pGame->GetResourceManager()->ClearBlockedFileReason( "" );
            for ( auto item : itemList )
            {
                g_pGame->GetResourceManager()->AddBlockedFileReason( item.strHash, item.strReason );
            }
            g_pGame->GetResourceManager()->SaveBlockedFileReasons();
        }
    }

    // Got crashinfo recpt
    void ProcessCrashInfo( CBitStream& bitStream )
    {
        int iGotCrashInfo = 0;
        bitStream->Read( iGotCrashInfo );
        if ( iGotCrashInfo )
        {
            FileDelete( m_strCrashLogFilename );
        }
    }

    // Extra ASE servers
    void ProcessAseServers( CBitStream& bitStream )
    {
        uint uiNumServers = 0;
        bitStream->Read( uiNumServers );
        for( uint i = 0 ; i < uiNumServers ; i++ )
        {
            char bAcceptsPush, bDoReminders, bHideProblems, bHideSuccess;
            uint uiReminderIntervalMins;
            SString strDesc, strUrl;
            bitStream->Read( bAcceptsPush );
            bitStream->Read( bDoReminders );
            bitStream->Read( bHideProblems );
            bitStream->Read( bHideSuccess );
            bitStream->Read( uiReminderIntervalMins );
            bitStream->ReadStr( strDesc );
            if ( !bitStream->ReadStr( strUrl ) )
                break;
            g_pGame->GetMasterServerAnnouncer()->AddServer( bAcceptsPush != 0, bDoReminders != 0, bHideProblems != 0, bHideSuccess != 0, Max( 5U, uiReminderIntervalMins ), strDesc, strUrl );
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
    SString         m_strCrashLogFilename;
    SString         m_strCrashDumpMeta;     // Filename of file which contains the latest crash dump filename
};
