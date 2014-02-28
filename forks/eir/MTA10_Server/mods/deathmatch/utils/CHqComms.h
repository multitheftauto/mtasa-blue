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
    }

    //
    // Send query if it's time
    //
    void Pulse( void )
    {
        // Check for min client version info
        if ( m_Stage == HQCOMMS_STAGE_NONE || ( m_Stage == HQCOMMS_STAGE_TIMER && m_CheckTimer.Get() > m_iPollInterval ) )
        {
            m_CheckTimer.Reset();
            m_Stage = HQCOMMS_STAGE_QUERY;
            int iMinClientAutoUpdate     = g_pGame->GetConfig()->GetMinClientVersionAutoUpdate();

            if ( iMinClientAutoUpdate > 0 )
            {
                SString strUrlParams;
                strUrlParams += SString( "?ip=%s", *g_pGame->GetConfig()->GetServerIP() );
                strUrlParams += SString( "&gport=%d", g_pGame->GetConfig()->GetServerPort() );
                strUrlParams += SString( "&version=%s", *CStaticFunctionDefinitions::GetVersionSortable() );
                strUrlParams += SString( "&minclientautoupdate=%d", iMinClientAutoUpdate );
                strUrlParams += SString( "&minclientversion=%s", *g_pGame->GetConfig ()->GetMinClientVersion() );

                // Send request
                this->AddRef();     // Keep object alive
                GetDownloadManager()->QueueFile( m_strURL + strUrlParams, NULL, 0, "", 0, false, this, StaticProgressCallback, false, 1 );
            }
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
            int iForceSetting;
            argMap.Get( "ForceMinClientVersion", iForceSetting );
            argMap.Get( "PollInterval", m_iPollInterval, m_iPollInterval );
            SString strResultMinClientVersion = argMap.Get( "AutoMinClientVersion" );
            SString strMessage = argMap.Get( "Message" );

            // Process received data
            m_iPollInterval = Max( TICKS_FROM_MINUTES( 5 ), m_iPollInterval );
            SString strSetttingsMinClientVersion = g_pGame->GetConfig ()->GetMinClientVersion();
            if ( strResultMinClientVersion > strSetttingsMinClientVersion || iForceSetting )
            {
                g_pGame->GetConfig ()->SetSetting ( "minclientversion", strResultMinClientVersion, true );
            }
            if ( !strMessage.empty() && strMessage != m_strPrevMessage )
            {
                m_strPrevMessage = strMessage;
                CLogger::LogPrintNoStamp( strMessage );
            }
        }
        else
        if ( iError )
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
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
    uint            m_Stage;
    CElapsedTime    m_CheckTimer;
    SString         m_strURL;
    SString         m_strPrevMessage;
};
