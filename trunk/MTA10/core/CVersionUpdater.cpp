/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVersionUpdater.cpp
*  PURPOSE:     Version update check and message dialog class
*  DEVELOPERS:  You're only supposed to blow the bloody doors off.
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <utils/CMD5Hasher.h>
#include "CVersionUpdater.Util.hpp"
#include "CNewsBrowser.h"


///////////////////////////////////////////////////////////////
//
//
// CVersionUpdater
//
//
///////////////////////////////////////////////////////////////
class CVersionUpdater : public CVersionUpdaterInterface
{
public:
    ZERO_ON_NEW

    // CVersionUpdaterInterface interface
                        CVersionUpdater                     ( void );
    virtual             ~CVersionUpdater                    ( void );
    virtual bool        SaveConfigToXML                     ( void );
    virtual void        EnableChecking                      ( bool bOn );
    virtual void        DoPulse                             ( void );
    virtual void        InitiateUpdate                      ( const SString& strType, const SString& strData, const SString& strHost );
    virtual bool        IsOptionalUpdateInfoRequired        ( const SString& strHost );
    virtual void        InitiateDataFilesFix                ( void );
    virtual void        InitiateManualCheck                 ( void );
    virtual void        GetAseServerList                    ( std::vector < SString >& outResult );
    virtual void        InitiateSidegradeLaunch             ( const SString& strVersion, const SString& strHost, ushort usPort, const SString& strName, const SString& strPassword );
    virtual void        GetBlockedVersionMap                ( std::map < SString, int >& outBlockedVersionMap );
    virtual void        GetNewsSettings                     ( SString& strOutOldestPost, uint& uiOutMaxHistoryLength );

    // CVersionUpdater functions
    bool                EnsureLoadedConfigFromXML           ( void );

    // Command lists
    void                InitPrograms                        ( void );
    void                CheckPrograms                       ( void );
    void                RunProgram                          ( const SString& strProgramName );

    // Util
    CXMLNode*           GetXMLConfigNode                    ( bool bCreateIfRequired );
    bool                IsBusy                              ( void );
    void                ResetEverything                     ( void );
    CReportWrap*        GetReportWrap                       ( void );
    static std::vector < SString > MakeServerList           ( const CDataInfoSet& infoMap );
    void                UpdateTroubleURL                    ( void );
    void                OnPossibleConfigProblem             ( void );

    // Commands
    void                _UseMasterFetchURLs                 ( void );
    void                _UseVersionQueryURLs                ( void );
    void                _UseProvidedURLs                    ( void );
    void                _UseDataFilesURLs                   ( void );
    void                _UseReportLogURLs                   ( void );
    void                _UseCrashDumpURLs                   ( void );
    void                _UseSidegradeURLs                   ( void );
    void                _UseNewsUpdateURLs                  ( void );
    void                _UseReportLogPostContent            ( void );
    void                _UseCrashDumpPostContent            ( void );
    void                _ActionReconnect                    ( void );
    void                _DialogHide                         ( void );
    void                _End                                ( void );
    void                _ExitGame                           ( void );
    void                _ResetVersionCheckTimer             ( void );
    void                _ResetNewsCheckTimer                ( void );
    void                _ResetMasterCheckTimer              ( void );
    void                _ResetManualCheckTimer              ( void );
    void                _PollQuestionNoYes                  ( void );
    void                _PollAnyButton                      ( void );
    void                _DialogConnectingWait               ( void );
    void                _DialogChecking                     ( void );
    void                _DialogNoUpdate                     ( void );
    void                _DialogDownloading                  ( void );
    void                _DialogServerSaysUpdateQuestion     ( void );
    void                _DialogServerSaysRecommendQuestion  ( void );
    void                _DialogUpdateQuestion               ( void );
    void                _DialogUpdateQueryError             ( void );
    void                _DialogUpdateResult                 ( void );
    void                _QUpdateResult                      ( void );
    void                _QUpdateNewsResult                  ( void );
    void                _DialogDataFilesQuestion            ( void );
    void                _DialogDataFilesQueryError          ( void );
    void                _DialogDataFilesResult              ( void );
    void                _StartDownload                      ( void );
    void                _PollDownload                       ( void );
    void                _StartSendPost                      ( void );
    void                _PollSendPost                       ( void );
    void                _CheckSidegradeRequirements         ( void );
    void                _DoSidegradeLaunch                  ( void );
    void                _DialogSidegradeDownloadQuestion    ( void );
    void                _DialogSidegradeLaunchQuestion      ( void );
    void                _DialogSidegradeQueryError          ( void );

    void                _ProcessMasterFetch                 ( void );
    void                _ProcessPatchFileQuery              ( void );
    void                _ProcessPatchFileDownload           ( void );

    // Doers
    int                 DoSendDownloadRequestToNextServer   ( void );
    int                 DoPollDownload                      ( void );
    int                 DoSendPostToNextServer              ( void );
    int                 DoPollPost                          ( void );

    std::vector < PFNVOIDVOID >         m_Stack;
    std::map < SString, CProgram >      m_ProgramMap;
    CProgram                            m_CurrentProgram;
    SJobInfo                            m_JobInfo;
    CHTTPClient                         m_HTTP;
    long long                           m_llTimeStart;
    CConditionMap                       m_ConditionMap;
    SString                             m_strServerSaysType;
    SString                             m_strServerSaysData;
    SString                             m_strServerSaysHost;

    SString                             m_strSidegradeVersion;
    SString                             m_strSidegradeHost;
    ushort                              m_usSidegradePort;
    SString                             m_strSidegradeName;
    SString                             m_strSidegradePassword;
    SString                             m_strSidegradePath;

    bool                                m_bCheckedTimeForVersionCheck;
    bool                                m_bCheckedTimeForNewsUpdate;
    bool                                m_bCheckedTimeForMasterFetch;
    bool                                m_bLoadedConfig;
    bool                                m_bSentCrashDump;
    bool                                m_bSentReportLog;
    std::map < SString, int >           m_DoneOptionalMap;

    long long                           m_llTimeLastManualCheck;
    SString                             m_strLastManualCheckBuildType;

    CReportWrap*                        m_pReportWrap;
    SString                             m_strLastQueryURL;
    bool                                m_bEnabled;
    uint                                m_uiFrameCounter;
    int                                 m_iForceMasterConfigRefreshCount;

    SUpdaterMasterConfig                m_MasterConfig;
    SUpdaterVarConfig                   m_VarConfig;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CVersionUpdater* g_pVersionUpdater = NULL;

CVersionUpdaterInterface* GetVersionUpdater ()
{
    if ( !g_pVersionUpdater )
        g_pVersionUpdater = new CVersionUpdater ();
    return g_pVersionUpdater;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::CVersionUpdater
//
//
//
///////////////////////////////////////////////////////////////
CVersionUpdater::CVersionUpdater ( void )
{
    InitPrograms ();
    CheckPrograms ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::~CVersionUpdater
//
//
//
///////////////////////////////////////////////////////////////
CVersionUpdater::~CVersionUpdater ( void )
{
    SAFE_DELETE ( m_pReportWrap );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetXMLConfigNode
//
// Get the main node for load/saving settings
//
///////////////////////////////////////////////////////////////
CXMLNode* CVersionUpdater::GetXMLConfigNode ( bool bCreateIfRequired )
{
    // Get the root node
    CXMLNode *pRoot = CCore::GetSingleton ().GetConfig ();
    if ( !pRoot )
        return NULL;

    // Get the top config node
    CXMLNode* pSectionNode = pRoot->FindSubNode ( CONFIG_NODE_UPDATER );
    if ( !pSectionNode )
    {
        if ( !bCreateIfRequired )
            return NULL;

        // Non-existent, create a new node
        pSectionNode = pRoot->CreateSubNode ( CONFIG_NODE_UPDATER );
    }

    return pSectionNode;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::EnsureLoadedConfigFromXML
//
// Load settings
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::EnsureLoadedConfigFromXML ( void )
{
    if ( m_bLoadedConfig )
        return false;

    m_bLoadedConfig = true;

    // Reset
    m_MasterConfig = SUpdaterMasterConfig ();

    // Try load
    CXMLNode* pMainNode = GetXMLConfigNode ( false );

    if ( !pMainNode )
        return false;

    if ( CXMLNode* pVar = pMainNode->FindSubNode ( "var" ) )
    {
        CXMLAccess XMLAccess ( pVar );
        // Stuff for here
        XMLAccess.GetSubNodeValue ( "version_lastchecktime",        m_VarConfig.version_lastCheckTime );
        XMLAccess.GetSubNodeValue ( "master_lastchecktime",         m_VarConfig.master_lastCheckTime );
        XMLAccess.GetSubNodeValue ( "master_highestnotifyrevision", m_VarConfig.master_highestNotifyRevision );
        XMLAccess.GetSubNodeValue ( "news_lastchecktime",           m_VarConfig.news_lastCheckTime );
        XMLAccess.GetSubNodeValue ( "news_lastnewsdate",            m_VarConfig.news_lastNewsDate );
        XMLAccess.GetSubNodeValue ( "crashdump_historylist",        m_VarConfig.crashdump_history );
    }

    if ( CXMLNode* pMaster = pMainNode->FindSubNode ( "mastercache" ) )
    {
        CXMLAccess XMLAccess ( pMaster );
        // Stuff from master
        XMLAccess.GetSubNodeValue ( "master.revision",              m_MasterConfig.master.strRevision );
        XMLAccess.GetSubNodeValue ( "master.serverlist",            m_MasterConfig.master.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "master.interval",              m_MasterConfig.master.interval );
        XMLAccess.GetSubNodeValue ( "version.serverlist",           m_MasterConfig.version.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "version.interval",             m_MasterConfig.version.interval );
        XMLAccess.GetSubNodeValue ( "report.serverlist",            m_MasterConfig.report.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "report.interval",              m_MasterConfig.report.interval );
        XMLAccess.GetSubNodeValue ( "crashdump.serverlist",         m_MasterConfig.crashdump.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "crashdump.duplicates",         m_MasterConfig.crashdump.iDuplicates );
        XMLAccess.GetSubNodeValue ( "crashdump.maxhistorylength",   m_MasterConfig.crashdump.iMaxHistoryLength );
        XMLAccess.GetSubNodeValue ( "gtadatafiles.serverlist",      m_MasterConfig.gtadatafiles.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "trouble.serverlist",           m_MasterConfig.trouble.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "ase.serverlist",               m_MasterConfig.ase.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "sidegrade.serverlist",         m_MasterConfig.sidegrade.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "sidegrade.nobrowselist",       m_MasterConfig.sidegrade.nobrowseInfoMap );
        XMLAccess.GetSubNodeValue ( "news.serverlist",              m_MasterConfig.news.serverInfoMap );
        XMLAccess.GetSubNodeValue ( "news.interval",                m_MasterConfig.news.interval );
        XMLAccess.GetSubNodeValue ( "news.oldestpost",              m_MasterConfig.news.strOldestPost );
        XMLAccess.GetSubNodeValue ( "news.maxhistorylength",        m_MasterConfig.news.iMaxHistoryLength );

        UpdateTroubleURL ();
    }

    return true;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::SaveConfigToXML
//
// Save setting
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::SaveConfigToXML ( void )
{
    if ( !m_bLoadedConfig )
        return false;

    CXMLNode* pMainNode = GetXMLConfigNode ( true );

    if ( !pMainNode )
        return false;

    pMainNode->DeleteAllSubNodes ();

    if ( CXMLNode* pVar = pMainNode->CreateSubNode ( "var" ) )
    {
        CXMLAccess XMLAccess ( pVar );
        // Stuff for here
        XMLAccess.SetSubNodeValue ( "version_lastchecktime",        m_VarConfig.version_lastCheckTime );
        XMLAccess.SetSubNodeValue ( "master_lastchecktime",         m_VarConfig.master_lastCheckTime );
        XMLAccess.SetSubNodeValue ( "master_highestnotifyrevision", m_VarConfig.master_highestNotifyRevision );
        XMLAccess.SetSubNodeValue ( "news_lastchecktime",           m_VarConfig.news_lastCheckTime );
        XMLAccess.SetSubNodeValue ( "news_lastnewsdate",            m_VarConfig.news_lastNewsDate );
        XMLAccess.SetSubNodeValue ( "crashdump_historylist",        m_VarConfig.crashdump_history );
    }

    if ( CXMLNode* pMaster = pMainNode->CreateSubNode ( "mastercache" ) )
    {
        CXMLAccess XMLAccess ( pMaster );
        // Stuff from master
        XMLAccess.SetSubNodeValue ( "master.revision",              m_MasterConfig.master.strRevision );
        XMLAccess.SetSubNodeValue ( "master.serverlist",            m_MasterConfig.master.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "master.interval",              m_MasterConfig.master.interval );
        XMLAccess.SetSubNodeValue ( "version.serverlist",           m_MasterConfig.version.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "version.interval",             m_MasterConfig.version.interval );
        XMLAccess.SetSubNodeValue ( "report.serverlist",            m_MasterConfig.report.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "report.interval",              m_MasterConfig.report.interval );
        XMLAccess.SetSubNodeValue ( "crashdump.serverlist",         m_MasterConfig.crashdump.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "crashdump.duplicates",         m_MasterConfig.crashdump.iDuplicates );
        XMLAccess.SetSubNodeValue ( "crashdump.maxhistorylength",   m_MasterConfig.crashdump.iMaxHistoryLength );
        XMLAccess.SetSubNodeValue ( "gtadatafiles.serverlist",      m_MasterConfig.gtadatafiles.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "trouble.serverlist",           m_MasterConfig.trouble.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "ase.serverlist",               m_MasterConfig.ase.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "sidegrade.serverlist",         m_MasterConfig.sidegrade.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "sidegrade.nobrowselist",       m_MasterConfig.sidegrade.nobrowseInfoMap );
        XMLAccess.SetSubNodeValue ( "news.serverlist",              m_MasterConfig.news.serverInfoMap );
        XMLAccess.SetSubNodeValue ( "news.interval",                m_MasterConfig.news.interval );
        XMLAccess.SetSubNodeValue ( "news.oldestpost",              m_MasterConfig.news.strOldestPost );
        XMLAccess.SetSubNodeValue ( "news.maxhistorylength",        m_MasterConfig.news.iMaxHistoryLength );
    }

    return true;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::EnableChecking
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::EnableChecking( bool bOn )
{
    m_bEnabled = bOn;
    EnsureLoadedConfigFromXML ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetReportWrap
//
//
//
///////////////////////////////////////////////////////////////
CReportWrap* CVersionUpdater::GetReportWrap()
{
    if ( !m_pReportWrap )
        m_pReportWrap = new CReportWrap();
    return m_pReportWrap;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::DoPulse ( void )
{
    if ( !m_bEnabled )
        return;

    EnsureLoadedConfigFromXML ();

    //
    // Time for master check?
    //
    if ( !m_bCheckedTimeForMasterFetch && !IsBusy () )
    {
        m_bCheckedTimeForMasterFetch = true;

        // Do update from master now if our config is wrong
        if ( !m_MasterConfig.IsValid () )
            m_VarConfig.master_lastCheckTime.SetFromSeconds ( 0 );

        // Master update interval maximum is 7 days
        if ( m_MasterConfig.master.interval.ToSeconds () > CTimeSpan( "7d" ).ToSeconds () )
            m_MasterConfig.master.interval.SetFromString ( "7d" );

        time_t secondsSinceCheck = CDateTime::Now ().ToSeconds () - m_VarConfig.master_lastCheckTime.ToSeconds ();
        OutputDebugLine ( SString ( "master timeSinceCheck: %d  time till next check: %d", (int)(secondsSinceCheck), (int)(m_MasterConfig.master.interval.ToSeconds () - secondsSinceCheck) ) );

        // Only check once a week
        if ( secondsSinceCheck > m_MasterConfig.master.interval.ToSeconds () || secondsSinceCheck < 0 )
        {
            RunProgram ( "MasterFetch" );
        }
    }

    //
    // Time for periodic check?
    //
    if ( !m_bCheckedTimeForVersionCheck && !IsBusy () )
    {
        m_bCheckedTimeForVersionCheck = true;

        // Do update check now if game did not stop cleanly last time
        if ( WatchDogWasUncleanStop () )
            m_VarConfig.version_lastCheckTime.SetFromSeconds ( 0 );

        time_t secondsSinceCheck = CDateTime::Now ().ToSeconds () - m_VarConfig.version_lastCheckTime.ToSeconds ();
        OutputDebugLine ( SString ( "version timeSinceCheck: %d  time till next check: %d", (int)(secondsSinceCheck), (int)(m_MasterConfig.version.interval.ToSeconds () - secondsSinceCheck) ) );

        // Only check once a day
        if ( secondsSinceCheck > m_MasterConfig.version.interval.ToSeconds () || secondsSinceCheck < 0 )
        {
            RunProgram ( "VersionCheck" );
        }
    }

    //
    // Time for news update?
    //
    if ( !m_bCheckedTimeForNewsUpdate && !IsBusy () )
    {
        m_bCheckedTimeForNewsUpdate = true;

        time_t secondsSinceCheck = CDateTime::Now ().ToSeconds () - m_VarConfig.news_lastCheckTime.ToSeconds ();
        OutputDebugLine ( SString ( "news timeSinceCheck: %d  time till next check: %d", (int)(secondsSinceCheck), (int)(m_MasterConfig.news.interval.ToSeconds () - secondsSinceCheck) ) );

        // Only check once an interval
        if ( secondsSinceCheck > m_MasterConfig.news.interval.ToSeconds () || secondsSinceCheck < 0 )
        {
            RunProgram ( "NewsUpdate" );
        }
    }

    //
    // Should send previous crash dump?
    //
    if ( !m_bSentCrashDump && !IsBusy () )
    {
        m_bSentCrashDump = true;
        RunProgram ( "SendCrashDump" );
    }

    //
    // Should send report on how new features are functioning?
    //
    if ( !m_bSentReportLog && !IsBusy () )
    {
        m_bSentReportLog = true;
        RunProgram ( "SendReportLog" );
    }

    //
    // Step through update sequence
    //
    if ( m_Stack.size () )
    {
        // Call top function on stack
        PFNVOIDVOID pfnCmdFunc = m_Stack.back ();
        m_Stack.pop_back ();
        ( this->*pfnCmdFunc ) ();
    }
    else
    {
        // If no top function on stack, process next instruction
        if ( const CInstruction* pInstruction = m_CurrentProgram.GetNextInstruction () )
        {
            if ( pInstruction->IsFunction () )
                m_Stack.push_back ( pInstruction->pfnCmdFunc );
            else
            if ( pInstruction->IsConditionalGoto () )
                if ( m_ConditionMap.IsConditionTrue ( pInstruction->strCondition ) )
                    m_CurrentProgram.GotoLabel ( pInstruction->strGoto );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateUpdate
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateUpdate ( const SString& strType, const SString& strData, const SString& strHost )
{
    if ( strType == "Mandatory" )
    {
        CCore::GetSingleton ().RemoveMessageBox ();
        RunProgram ( "ServerSaysUpdate" );
    }
    else
    if ( strType == "Optional" || strType == "Recommended" )
    {
        CCore::GetSingleton ().RemoveMessageBox ();
        MapSet ( m_DoneOptionalMap, strHost, 1 );
        RunProgram ( "ServerSaysRecommend" );
    }

    m_strServerSaysType = strType;
    m_strServerSaysData = strData;
    m_strServerSaysHost = strHost;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::IsOptionalUpdateInfoRequired
//
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::IsOptionalUpdateInfoRequired ( const SString& strHost )
{
    // Should this server tell us about recommended updates?
    return !MapContains ( m_DoneOptionalMap, strHost );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateDataFilesFix
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateDataFilesFix ( void )
{
    RunProgram ( "ServerSaysDataFilesWrong" );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateManualCheck
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateManualCheck ( void )
{
    if ( GetQuestionBox ().IsVisible () )
    {
        // Bring to the front
        GetQuestionBox ().Show ();
        return;
    }

    if ( IsBusy () )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Busy", "Can't check for updates right now", MB_BUTTON_OK | MB_ICON_ERROR );
        return;
    }

    SString strUpdateBuildType;
    CVARS_GET ( "update_build_type", strUpdateBuildType );

    if ( GetTickCount64_ () - m_llTimeLastManualCheck > 10000 || strUpdateBuildType != m_strLastManualCheckBuildType )
    {
        RunProgram ( "ManualCheck" );
    }
    else
    {
        // Pretend to do a check if less than a minute since last check
        RunProgram ( "ManualCheckSim" );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateSidegradeLaunch
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateSidegradeLaunch ( const SString& strVersion, const SString& strHost, ushort usPort, const SString& strName, const SString& strPassword )
{
    m_strSidegradeVersion = strVersion;
    m_strSidegradeHost = strHost;
    m_usSidegradePort = usPort;
    m_strSidegradeName = strName;
    m_strSidegradePassword = strPassword;

    m_strServerSaysHost = SString ( "%s:%d", *strHost, usPort );
    RunProgram ( "SidegradeLaunch" );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetAseServerList
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetAseServerList ( std::vector < SString >& outResult )
{
    EnsureLoadedConfigFromXML ();

    outResult = MakeServerList ( m_MasterConfig.ase.serverInfoMap );

    // Backup plan if list is empty
    if ( outResult.empty () )
        outResult.push_back ( "http://1mgg.com/affil/mta" );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetBlockedVersionMap
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetBlockedVersionMap ( std::map < SString, int >& outBlockedVersionMap )
{
    EnsureLoadedConfigFromXML ();

    outBlockedVersionMap.clear ();

    //
    // Find list for our version
    //
    const CDataInfoSet& dataInfoSet = m_MasterConfig.sidegrade.nobrowseInfoMap;
    for ( uint i = 0 ; i < dataInfoSet.size () ; i++ )
    {
        const SDataInfoItem& line = dataInfoSet[i];
        const SString* pstrVersion = MapFind ( line.attributeMap, "version" );
        if ( pstrVersion && *pstrVersion == MTA_DM_ASE_VERSION )
        {
            std::vector < SString > blockedVersionList;
            line.strValue.Split ( ";", blockedVersionList );

            for ( uint i = 0 ; i < blockedVersionList.size () ; i++ )
            {
                outBlockedVersionMap[ blockedVersionList[i] ] = 1;
            }

            break;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::ResetEverything
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::ResetEverything ()
{
    m_Stack.clear ();
    m_JobInfo = SJobInfo();
    m_CurrentProgram = CProgram();
    m_HTTP.Get ("");
    m_llTimeStart = 0;
    m_ConditionMap.clear ();
    m_strServerSaysType = "";
    m_strServerSaysData = "";
    m_strServerSaysHost = "";
    GetQuestionBox ().Reset ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::IsBusy
//
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::IsBusy ( void )
{
    return m_Stack.size () || ( m_CurrentProgram.IsValid () );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::MakeServerList
//
// Make ordered server list from config map
//
///////////////////////////////////////////////////////////////
std::vector < SString > CVersionUpdater::MakeServerList ( const CDataInfoSet& dataInfoSetIn )
{
    std::vector < SString > serverList;

    //
    // Sort out server order
    //
    CDataInfoSet dataInfoSet = dataInfoSetIn;
    if ( int iSize = dataInfoSet.size () )
    {
        // randomize list
        for ( int i = 0 ; i < iSize ; i++ )
        {
            int iOther = rand () % iSize;
            std::swap ( dataInfoSet[i], dataInfoSet[iOther] );
        }

        // bubble sort based on the priority attribute
        for ( int i = 0 ; i < iSize - 1 ; i++ )
        {
            SDataInfoItem& a = dataInfoSet[i];
            SDataInfoItem& b = dataInfoSet[i + 1];
            int iPriorityA = MapFind ( a.attributeMap, "priority" ) ? atoi ( *MapFind ( a.attributeMap, "priority" ) ) : 5;
            int iPriorityB = MapFind ( b.attributeMap, "priority" ) ? atoi ( *MapFind ( b.attributeMap, "priority" ) ) : 5;
            if ( iPriorityA > iPriorityB )
            {
                std::swap ( a, b );
                i = Max ( i - 2, -1 );
            }
        }

        // Output values into the result list
        for ( int i = 0 ; i < iSize ; i++ )
            serverList.push_back ( dataInfoSet[i].strValue );
    }

    return serverList;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::OnPossibleConfigProblem
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::OnPossibleConfigProblem ( void )
{
    // Limit number of times this can be done
    if ( m_iForceMasterConfigRefreshCount < 1 )
    {
        m_iForceMasterConfigRefreshCount++;

        // Queue master config refresh
        m_VarConfig.master_lastCheckTime.SetFromSeconds ( 0 );
        m_bCheckedTimeForMasterFetch = false;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// Run'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitPrograms
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitPrograms ()
{
    //
    // VersionCheck
    //
    {
        SString strProgramName = "VersionCheck";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionQueryURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _StartDownload );                        // Fetch update info from update server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if ProcessResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if ProcessResponse.silent goto silentdload");  // If update server says 'silent' then goto silentdload:
        ADDCOND ( "if ProcessResponse.noupdate goto noupdate");   // If update server says 'noupdate' then goto noupdate:
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogUpdateQuestion );                    // Show "Update available" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                        // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "silentdload:" );
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                        // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _QUpdateResult );                           // Maybe set OnRestartCommand
        ADDINST (   _End );

        ADDLABL ( "noupdate:" );
        ADDINST (   _ResetVersionCheckTimer );                 // Wait 24hrs before checking again
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }


    //
    // Manual update check
    //
    {
        SString strProgramName = "ManualCheck";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionQueryURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _DialogChecking );                          // Show "Checking..." message
        ADDINST (   _StartDownload );                          // Fetch update info from update server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if ProcessResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if ProcessResponse.silent goto dload");        // If update server says 'silent' then goto dload:
        ADDCOND ( "if ProcessResponse.noupdate goto noupdate");   // If update server says 'noupdate' then goto noupdate:
        ADDCOND ( "if ProcessResponse.cancel goto end");
        ADDINST (   _DialogUpdateQueryError );
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogUpdateQuestion );                    // Show "Update available" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                       // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "noupdate:" );
        ADDINST (   _ResetManualCheckTimer );                   // Wait 1min before checking again
        ADDINST (   _DialogNoUpdate );                          // Show "No update available" dialog
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }


    //
    // Manual update simulation
    //
    {
        SString strProgramName = "ManualCheckSim";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _DialogNoUpdate );                          // Show "No update available" dialog
        ADDINST (   _End );
    }


    //
    // ServerSaysUpdate
    //
    {
        SString strProgramName = "ServerSaysUpdate";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionQueryURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _DialogServerSaysUpdateQuestion );          // Show "Server says update" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogChecking );                          // Show "Checking..." message
        ADDINST (   _StartDownload );                          // Fetch update info from update server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if ProcessResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if ProcessResponse.silent goto dload");        // If update server says 'silent' then goto dload:
        ADDINST (   _DialogUpdateQueryError );
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                       // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }

    //
    // ServerSaysRecommend
    //
    {
        SString strProgramName = "ServerSaysRecommend";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseVersionQueryURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _DialogConnectingWait );                    // Show "Please wait..." message
        ADDINST (   _StartDownload );                          // Fetch update info from update server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.update goto dload");        // If update server says 'update' then goto dload:
        ADDCOND ( "if ProcessResponse.files goto dload");         // If update server says 'files' then goto dload:
        ADDCOND ( "if ProcessResponse.silent goto silentdload");  // If update server says 'silent' then goto silentdload:
        ADDINST (   _ActionReconnect );
        ADDINST (   _End );

        ADDLABL ( "dload:" );
        ADDINST (   _DialogServerSaysRecommendQuestion );       // Show "Server says update" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto reconnect" );  // If user says 'No', then goto reconnect:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                       // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _DialogUpdateResult );                      // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "silentdload:" );
        ADDINST (   _DialogHide );                              // Don't show downloading progress
        ADDINST (   _ActionReconnect );                         // Reconnect to game
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                       // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _QUpdateResult );                           // Maybe set OnRestartCommand
        ADDINST (   _End );

        ADDLABL ( "reconnect:" );
        ADDINST (   _ActionReconnect );
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }


    //
    // ServerSaysDataFilesWrong
    //
    {
        SString strProgramName = "ServerSaysDataFilesWrong";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseDataFilesURLs );                        // Use DATA_FILES_URL*
        ADDINST (   _DialogDataFilesQuestion );                 // Show "Data files wrong" dialog
        ADDCOND ( "if QuestionResponse.!Yes goto end" );        // If user says 'No', then goto end:
        ADDINST (   _DialogChecking );                          // Show "Checking..." message
        ADDINST (   _StartDownload );                          // Fetch file info from server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.!datafiles goto error1" );  // If server says 'No files' then goto error1:
        ADDINST (   _DialogDownloading );                       // Show "Downloading..." message
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                       // Fetch file binary from mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _DialogDataFilesResult );                   // Show "Update ok/failed" message
        ADDINST (   _End );

        ADDLABL ( "error1:" );
        ADDINST (   _DialogUpdateQueryError );
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (     _End );
    }


    //
    // MasterFetch
    //
    {
        SString strProgramName = "MasterFetch";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseMasterFetchURLs );                     // Use VERSION_CHECKER_URL*
        ADDINST (   _StartDownload );                          // Fetch file info from server
        ADDINST (   _ProcessMasterFetch );
        ADDCOND ( "if ProcessResponse.!ok goto error1" );
        ADDINST (   _ResetMasterCheckTimer );
        ADDINST (   _End );

        ADDLABL ( "error1:" );
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }

    //
    // SendCrashDump
    //
    {
        SString strProgramName = "SendCrashDump";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseCrashDumpURLs );                        // Use CRASH_DUMP_URL*
        ADDINST (   _UseCrashDumpPostContent );                 // Use crash dump source
        ADDINST (   _StartSendPost );                           // Send data
        ADDLABL ( "end:" );
        ADDINST (     _End );
    }


    //
    // SendReportLog
    //
    {
        SString strProgramName = "SendReportLog";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseReportLogURLs );                        // Use REPORT_LOG_URL*
        ADDINST (   _UseReportLogPostContent );                 // Use report log source
        ADDINST (   _StartSendPost );                           // Send data
        ADDLABL ( "end:" );
        ADDINST (     _End );
    }


    //
    // SidegradeLaunch
    //
    {
        SString strProgramName = "SidegradeLaunch";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _CheckSidegradeRequirements );                      // Check if other version already installed
        ADDCOND ( "if ProcessResponse.!installed goto NOTINSTALLED" );  // Other version present and valid?
            ADDINST (   _DialogSidegradeLaunchQuestion );                   // Does user want to launch and connect using the other version?
            ADDCOND ( "if QuestionResponse.!Yes goto NOLAUNCH" );
                ADDINST (   _DoSidegradeLaunch );                                   // If user says 'Yes', then launch
            ADDLABL ( "NOLAUNCH:" );
            ADDINST (   _End );
       
        ADDLABL ( "NOTINSTALLED:" );                                    // If not installed, check if it is downloadable
        ADDINST (   _UseSidegradeURLs );                                // Use sidegrade URLs
        ADDINST (   _StartDownload );                                   // Fetch file info from server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.update goto HASFILE" );           // Does server have the required file?
            ADDINST (   _DialogSidegradeQueryError );                       // If no download available, show message
            ADDINST (   _End );

        ADDLABL ( "HASFILE:" );
        ADDINST (   _DialogSidegradeDownloadQuestion );                 // If it is downloadable, ask user what to do
        ADDCOND ( "if QuestionResponse.Yes goto YESDOWNLOAD" );
            ADDINST (     _End );                                           // If user says 'No', then finish

        ADDLABL ( "YESDOWNLOAD:" );                                     // If user says yes, download and run installer
        ADDINST (   _DialogDownloading );                               // Show "Downloading..." message
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                                   // Fetch file binary from mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _DialogDataFilesResult );                           // Show "ok/failed" message
        ADDINST (   _End );
    }


    //
    // NewsUpdate
    //
    {
        SString strProgramName = "NewsUpdate";
        MapSet ( m_ProgramMap, strProgramName, CProgram () );
        CProgram& program = *MapFind ( m_ProgramMap, strProgramName );

        ADDINST (   _UseNewsUpdateURLs );                       // Use news serverlist
        ADDINST (   _StartDownload );                           // Fetch update info from update server
        ADDINST (   _ProcessPatchFileQuery );
        ADDCOND ( "if ProcessResponse.silent goto silentdload");  // If update server says 'silent' then goto silentdload:
        ADDCOND ( "if ProcessResponse.noupdate goto noupdate");   // If update server says 'noupdate' then goto noupdate:
        ADDINST (   _End );

        ADDLABL ( "silentdload:" );
        ADDINST (   _UseProvidedURLs );
        ADDINST (   _StartDownload );                           // Fetch update binary from update mirror
        ADDINST (   _ProcessPatchFileDownload );
        ADDINST (   _QUpdateNewsResult );                       // Maybe update news install queue
        ADDINST (   _End );

        ADDLABL ( "noupdate:" );
        ADDINST (   _ResetNewsCheckTimer );                 // Wait interval before checking again
        ADDINST (   _End );

        ADDLABL ( "end:" );
        ADDINST (   _End );
    }

}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::CheckPrograms
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::CheckPrograms ()
{
    // Check each program
    for ( std::map < SString, CProgram >::iterator it = m_ProgramMap.begin (); it != m_ProgramMap.end (); ++it )
    {
        CProgram program = it->second;

        // Check each instruction
        while ( const CInstruction* pInstruction = program.GetNextInstruction () )
        {
            assert ( pInstruction->IsFunction () || pInstruction->IsConditionalGoto () || pInstruction->IsLabel () );

            if ( pInstruction->IsConditionalGoto () )
                assert ( program.FindLabel ( pInstruction->strGoto ) != -1 );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::RunProgram
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::RunProgram ( const SString& strProgramName )
{
    OutputDebugLine ( SString ( "RunProgram %s", strProgramName.c_str () ) );
    ResetEverything ();
    CProgram* pProgram = MapFind ( m_ProgramMap, strProgramName );
    if ( pProgram )
    {
        m_CurrentProgram = *pProgram;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// '_'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_CheckSidegradeRequirements
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_CheckSidegradeRequirements ( void )
{
    // Is version allowed to be launched?

    // Find reg settings for other version
    SString strRegPath ( "..\\%s", *m_strSidegradeVersion );

    m_strSidegradePath = GetRegistryValue ( strRegPath, "Last Run Path" );
    SString strLaunchHash = GetRegistryValue ( strRegPath, "Last Run Path Hash" );
    SString strLaunchVersion = GetRegistryValue ( strRegPath, "Last Run Path Version" );

    // Validate strLaunchVersion
    bool bVersionMatch = ( strLaunchVersion == m_strSidegradeVersion );

    // Validate strLaunchPath
    bool bLaunchPathValid = false;
    {
        MD5 md5;
        CMD5Hasher Hasher;
        if ( Hasher.Calculate ( m_strSidegradePath, md5 ) )
        {
            char szHashResult[33];
            Hasher.ConvertToHex ( md5, szHashResult );

            if ( strLaunchHash == szHashResult )
                bLaunchPathValid = true;
        }
    }

    if ( bVersionMatch && bLaunchPathValid )
    {
        m_ConditionMap.SetCondition ( "ProcessResponse", "installed" );
    }
    else
    {
        m_ConditionMap.SetCondition ( "ProcessResponse", "notinstalled" );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DoSidegradeLaunch
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DoSidegradeLaunch ( void )
{
    SString strURL ( "mtasa://%s:%s@%s:%d", *m_strSidegradeName, *m_strSidegradePassword, *m_strSidegradeHost, m_usSidegradePort );
    SetOnQuitCommand ( "open", m_strSidegradePath, strURL );
    Push ( _ExitGame );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogSidegradeDownloadQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogSidegradeDownloadQuestion ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( SString ( "MTA:SA %s required", *m_strSidegradeVersion ) );
    GetQuestionBox ().SetMessage ( SString ( "An updated version of MTA:SA %s is required to join the selected server.\n\n"
                                             "Do you want to download and install MTA:SA %s ?", *m_strSidegradeVersion, *m_strSidegradeVersion ) );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogSidegradeLaunchQuestion
//
// If can launch other version, ask for user confirmation
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogSidegradeLaunchQuestion ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( SString ( "MTA:SA %s required", *m_strSidegradeVersion ) );
    GetQuestionBox ().SetMessage ( SString ( "Do you want to launch MTA:SA %s and connect to this server ?", *m_strSidegradeVersion ) );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogSidegradeQueryError
//
// No download file for other version
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogSidegradeQueryError ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( SString ( "MTA:SA %s required", *m_strSidegradeVersion ) );
    GetQuestionBox ().SetMessage ( "It is not possible to connect at this time.\n\nPlease try later." );
    GetQuestionBox ().SetButton ( 0, "OK" );
    GetQuestionBox ().Show ();
    Push ( _PollAnyButton );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ActionReconnect
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ActionReconnect ( void )
{
    CCommandFuncs::Reconnect ( NULL );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogHide
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogHide()
{
    GetQuestionBox ().Reset ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_End
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_End ( void )
{
    ResetEverything ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ExitGame
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ExitGame ( void )
{
    // Exit game
    CCore::GetSingleton ().Quit ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetVersionCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetVersionCheckTimer ( void )
{
    OutputDebugLine ( SStringX ( "_ResetVersionCheckTimer" ) );
    m_VarConfig.version_lastCheckTime = CDateTime::Now ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetNewsCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetNewsCheckTimer ( void )
{
    OutputDebugLine ( SStringX ( "_ResetNewsCheckTimer" ) );
    m_VarConfig.news_lastCheckTime = CDateTime::Now ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetMasterCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetMasterCheckTimer ( void )
{
    m_VarConfig.master_lastCheckTime = CDateTime::Now ();
    CCore::GetSingleton ().SaveConfig ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetManualCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetManualCheckTimer ( void )
{
    // Reset delay before recheck allowed
    m_llTimeLastManualCheck = GetTickCount64_ ();
    CVARS_GET ( "update_build_type", m_strLastManualCheckBuildType );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollAnyButton
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollAnyButton ( void )
{
    // Wait for button press before continuing
    if ( GetQuestionBox ().PollButtons () == -1 )
        Push ( _PollAnyButton );
    else
        GetQuestionBox ().Reset ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollQuestionNoYes
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollQuestionNoYes ( void )
{
    switch ( GetQuestionBox ().PollButtons () )
    {
        case BUTTON_NONE:
            Push ( _PollQuestionNoYes );
            break;

        case BUTTON_0:
            m_ConditionMap.SetCondition ( "QuestionResponse", "No" );
            GetQuestionBox ().Reset ();
            break;

        case BUTTON_1:
            m_ConditionMap.SetCondition ( "QuestionResponse", "Yes" );
            GetQuestionBox ().Reset ();
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogConnectingWait
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogConnectingWait ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "Connecting" );
    GetQuestionBox ().SetMessage ( "Please wait..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogChecking
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogChecking ( void )
{
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "CHECKING" );
    GetQuestionBox ().SetMessage ( "Please wait..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogNoUpdate
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogNoUpdate ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "UPDATE CHECK" );
    GetQuestionBox ().SetMessage ( "No update needed" );
    GetQuestionBox ().SetButton ( 0, "OK" );
    GetQuestionBox ().Show ();
    Push ( _PollAnyButton );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDownloading
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDownloading ( void )
{
    // Display dialog
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "DOWNLOADING" );
    GetQuestionBox ().SetMessage ( "waiting..." );
    GetQuestionBox ().SetButton ( 0, "Cancel" );
    GetQuestionBox ().Show ();
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogServerSaysUpdateQuestion
//
// ServerSaysUpdateQuestion
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogServerSaysUpdateQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "MANDATORY UPDATE" );
    GetQuestionBox ().SetMessage ( "To join this server, you must update MTA.\n\n Do you want to update now ?" );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogServerSaysRecommendQuestion
//
// ServerSaysRecommendQuestion
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogServerSaysRecommendQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "OPTIONAL UPDATE" );
    GetQuestionBox ().SetMessage ( "Server says an update is recommended, but not essential.\n\n Do you want to update now ?" );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( m_JobInfo.strTitle );
    GetQuestionBox ().SetMessage ( m_JobInfo.strMsg );
    GetQuestionBox ().SetButton ( 0, m_JobInfo.strNo );
    GetQuestionBox ().SetButton ( 1, m_JobInfo.strYes );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateQueryError
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateQueryError ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "UPDATE CHECK" );
    GetQuestionBox ().SetMessage ( "Update not currently avalable.\n\nPlease check www.mtasa.com" );
    GetQuestionBox ().SetButton ( 0, "OK" );
    GetQuestionBox ().Show ();
    Push ( _PollAnyButton );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateResult(void)
{
    if ( m_ConditionMap.IsConditionTrue ( "Download.Fail" ) )
    {
        // Handle failure
        if ( m_ConditionMap.IsConditionTrue ( "Download.Fail.Saving" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR SAVING" );
            GetQuestionBox ().SetMessage ( "Unable to create the file." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        if ( m_ConditionMap.IsConditionTrue ( "Download.Fail.Checksum" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "The downloaded file appears to be incorrect." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "For some reason." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
    }
    else
    if ( m_ConditionMap.IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        // Exit game after button press
        GetQuestionBox ().Reset ();
        GetQuestionBox ().SetTitle ( "DOWNLOAD COMPLETE" );
        GetQuestionBox ().SetMessage ( m_JobInfo.strMsg2 );
        GetQuestionBox ().SetButton ( 0, "OK" );
        GetQuestionBox ().Show ();

        if ( m_JobInfo.strStatus == "update" )
        {
            // 'update' - Stand alone installer
            SetOnQuitCommand ( "open", m_JobInfo.strSaveLocation );
            SetOnRestartCommand ( "" );
            Push ( _ExitGame );
        }
        else
        if ( m_JobInfo.strStatus == "files" || m_JobInfo.strStatus == "silent" )
        {
            // 'files'/'silent' - Self extracting archive 
            SetOnQuitCommand ( "restart" );
            SetOnRestartCommand ( "files", m_JobInfo.strSaveLocation );
            Push ( _ExitGame );
        }
        else
        {
            GetQuestionBox ().SetMessage ( m_JobInfo.strStatus + " - Unknown problem in _DialogUpdateResult" );
        }
        Push ( _PollAnyButton );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_QUpdateResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_QUpdateResult ( void )
{
    if ( m_ConditionMap.IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        if ( m_JobInfo.strStatus == "silent" )
        {
            // 'silent' - Self extracting archive 
            SetOnRestartCommand ( "silent", m_JobInfo.strSaveLocation );
        }
#if MTA_DEBUG
        else
            MessageBox ( NULL, "_QUpdateResult m_JobInfo.strStatus != 'silent'", m_JobInfo.strStatus, MB_OK );
#endif
    }
#if MTA_DEBUG
    else
        MessageBox ( NULL, "_QUpdateResult Download.!Ok", "Error", MB_OK );
#endif
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_QUpdateNewsResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_QUpdateNewsResult ( void )
{
    if ( m_ConditionMap.IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        if ( m_JobInfo.strStatus == "silent" )
        {
            // Calc news date from filename
            SString strDate = m_JobInfo.strFilename.SplitRight ( "-" ).SplitLeft ( "." );

            if ( strDate.length () == 10 )
            {
                // Add to install news queue
                CArgMap queue;
                queue.SetFromString ( GetApplicationSetting ( "news-install" ) );
                queue.Set ( strDate, m_JobInfo.strSaveLocation );
                SetApplicationSetting ( "news-install", queue.ToString () );

                // Set last news date
                m_VarConfig.news_lastNewsDate = strDate;
                CCore::GetSingleton ().SaveConfig ();

                // See if there is more news after this one
                m_bCheckedTimeForNewsUpdate = false;
            }
           
        }
#if MTA_DEBUG
        else
            MessageBox ( NULL, "_QUpdateNewsResult m_JobInfo.strStatus != 'silent'", m_JobInfo.strStatus, MB_OK );
#endif
    }
#if MTA_DEBUG
    else
        MessageBox ( NULL, "_QUpdateNewsResult Download.!Ok", "Error", MB_OK );
#endif
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesQuestion ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "ERROR" );
    GetQuestionBox ().SetMessage ( "San Andreas data files have been modified\n\n\nDo you want to automatically fix this problem?" );
    GetQuestionBox ().SetButton ( 0, "No" );
    GetQuestionBox ().SetButton ( 1, "Yes" );
    GetQuestionBox ().Show ();
    Push ( _PollQuestionNoYes );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesQueryError
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesQueryError ( void )
{
    // Display message
    GetQuestionBox ().Reset ();
    GetQuestionBox ().SetTitle ( "DATA FILES" );
    GetQuestionBox ().SetMessage ( "Data files not currently avalable.\n\nPlease check www.mtasa.com" );
    GetQuestionBox ().SetButton ( 0, "OK" );
    GetQuestionBox ().Show ();
    Push ( _PollAnyButton );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesResult ( void )
{
    // Handle failure
    if ( m_ConditionMap.IsConditionTrue ( "Download.Fail" ) )
    {
        if ( m_ConditionMap.IsConditionTrue ( "Download.Fail.Saving" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR SAVING" );
            GetQuestionBox ().SetMessage ( "Unable to create the file." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        if ( m_ConditionMap.IsConditionTrue ( "Download.Fail.Checksum" ) )
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "The downloaded file appears to be incorrect." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
        else
        {
            GetQuestionBox ().Reset ();
            GetQuestionBox ().SetTitle ( "ERROR DOWNLOADING" );
            GetQuestionBox ().SetMessage ( "For some reason." );
            GetQuestionBox ().SetButton ( 0, "OK" );
            GetQuestionBox ().Show ();
            Push ( _PollAnyButton );
        }
    }
    else
    if ( m_ConditionMap.IsConditionTrue ( "Download.Ok" ) )
    {
        // Handle success
        SetOnQuitCommand ( "open", m_JobInfo.strSaveLocation );

        // Exit game after button press
        GetQuestionBox ().Reset ();
        GetQuestionBox ().SetTitle ( "DOWNLOAD COMPLETE" );
        GetQuestionBox ().SetMessage ( m_JobInfo.strMsg2 );
        GetQuestionBox ().SetButton ( 0, "OK" );
        GetQuestionBox ().Show ();
        Push ( _ExitGame );
        Push ( _PollAnyButton );
    }
}




//
//
// 
// Downloaders
//
//
//

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseVersionQueryURLs
//
// Called before starting the version check
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseVersionQueryURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList = MakeServerList ( m_MasterConfig.version.serverInfoMap );
    m_JobInfo.bShowDownloadPercent = false;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessPatchFileQuery
//
// Called after getting the version check info from the server
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessPatchFileQuery ( void )
{
    m_ConditionMap.SetCondition ( "ProcessResponse", "" );
    m_ConditionMap.SetCondition ( "Download", "" );

    if ( m_JobInfo.downloadBuffer.size () == 0 )
    {
        OutputDebugLine ( "Empty download buffer" );
        return;
    }

    CXMLBuffer XMLBuffer;
    CXMLNode* pRoot = XMLBuffer.SetFromData ( &m_JobInfo.downloadBuffer[0], m_JobInfo.downloadBuffer.size () );

    // Try to parse download info
    m_JobInfo = SJobInfo();
    SString                 strReportSettingsFilter;
    SString                 strReportSettingsMin;
    SString                 strReportSettingsMax;
    SString                 strNotifyMasterRevision;

    CXMLAccess XMLAccess ( XMLBuffer.m_pRoot );
    XMLAccess.GetSubNodeValue ( "status",                       m_JobInfo.strStatus );
    XMLAccess.GetSubNodeValue ( "priority",                     m_JobInfo.strPriority );
    XMLAccess.GetSubNodeValue ( "dialog.title",                 m_JobInfo.strTitle );
    XMLAccess.GetSubNodeValue ( "dialog.msg",                   m_JobInfo.strMsg );
    XMLAccess.GetSubNodeValue ( "dialog.msg2",                  m_JobInfo.strMsg2 );
    XMLAccess.GetSubNodeValue ( "dialog.yes",                   m_JobInfo.strYes );
    XMLAccess.GetSubNodeValue ( "dialog.no",                    m_JobInfo.strNo );
    XMLAccess.GetSubNodeValue ( "file.name",                    m_JobInfo.strFilename );
    XMLAccess.GetSubNodeValue ( "file.size",                    m_JobInfo.iFilesize );
    XMLAccess.GetSubNodeValue ( "file.md5",                     m_JobInfo.strMD5 );
    XMLAccess.GetSubNodeValue ( "serverlist",                   m_JobInfo.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "reportsettings.filter2",       strReportSettingsFilter );
    XMLAccess.GetSubNodeValue ( "reportsettings.minsize",       strReportSettingsMin );
    XMLAccess.GetSubNodeValue ( "reportsettings.maxsize",       strReportSettingsMax );
    XMLAccess.GetSubNodeValue ( "notifymasterrevision",         strNotifyMasterRevision );

    // Process
    if ( strNotifyMasterRevision.length () && strNotifyMasterRevision > m_MasterConfig.master.strRevision )
    {
        // Only do next bit when 'notify revision' increases (prevents superfluous downloads when notify revision is (incorrectly) higher than actual revision)
        if ( strNotifyMasterRevision < m_VarConfig.master_highestNotifyRevision )
        {
            m_VarConfig.master_highestNotifyRevision = strNotifyMasterRevision;

            // If version check file says there is a new master config file, force update next time
            m_VarConfig.master_lastCheckTime.SetFromSeconds ( 0 );
            if ( m_JobInfo.strStatus == "noupdate" )
                OnPossibleConfigProblem ();   // Do it this time if no update here
        }
    }

    // If some sort of error read the file data, re-get the master config
    if ( m_JobInfo.strStatus == "" )
        OnPossibleConfigProblem ();

    // Maybe modify report settings
    if ( strReportSettingsFilter.length () )
    {
        SString strReportSettings = SString ( "filter2@%s;min@%s;max@%s", strReportSettingsFilter.c_str (), strReportSettingsMin.c_str (), strReportSettingsMax.c_str () ); 
        GetReportWrap ()->SetSettings ( strReportSettings );
    }

    // Indicate what the server wants us to do
    m_ConditionMap.SetCondition ( "ProcessResponse", m_JobInfo.strStatus );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseMasterFetchURLs
//
// Called before starting the master xml fetch
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseMasterFetchURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList.push_back ( UPDATER_MASTER_URL1 );
    m_JobInfo.serverList.push_back ( UPDATER_MASTER_URL2 );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessMasterFetch
//
// Called after completing master xml fetch
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessMasterFetch ( void )
{
    m_ConditionMap.SetCondition ( "ProcessResponse", "" );
    m_ConditionMap.SetCondition ( "Download", "" );

    if ( m_JobInfo.downloadBuffer.size () == 0 )
        return;

    CXMLBuffer XMLBuffer;
    CXMLNode* pRoot = XMLBuffer.SetFromData ( &m_JobInfo.downloadBuffer[0], m_JobInfo.downloadBuffer.size () );

    if ( !pRoot )
        return;

    // Try to parse download info
    m_JobInfo = SJobInfo();

    CXMLAccess XMLAccess ( XMLBuffer.m_pRoot );
    SString strResetVar;
    XMLAccess.GetSubNodeValue ( "resetvar",                     strResetVar );
    SUpdaterMasterConfig newMasterConfig;
    XMLAccess.GetSubNodeValue ( "master.revision",              newMasterConfig.master.strRevision );
    XMLAccess.GetSubNodeValue ( "master.serverlist",            newMasterConfig.master.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "master.interval",              newMasterConfig.master.interval );
    XMLAccess.GetSubNodeValue ( "version.serverlist",           newMasterConfig.version.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "version.interval",             newMasterConfig.version.interval );
    XMLAccess.GetSubNodeValue ( "report.serverlist",            newMasterConfig.report.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "report.interval",              newMasterConfig.report.interval );
    XMLAccess.GetSubNodeValue ( "crashdump.serverlist",         newMasterConfig.crashdump.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "crashdump.duplicates",         newMasterConfig.crashdump.iDuplicates );
    XMLAccess.GetSubNodeValue ( "crashdump.maxhistorylength",   newMasterConfig.crashdump.iMaxHistoryLength );
    XMLAccess.GetSubNodeValue ( "gtadatafiles.serverlist",      newMasterConfig.gtadatafiles.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "trouble.serverlist",           newMasterConfig.trouble.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "ase.serverlist",               newMasterConfig.ase.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "sidegrade.serverlist",         newMasterConfig.sidegrade.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "sidegrade.nobrowselist",       newMasterConfig.sidegrade.nobrowseInfoMap );
    XMLAccess.GetSubNodeValue ( "news.serverlist",              newMasterConfig.news.serverInfoMap );
    XMLAccess.GetSubNodeValue ( "news.interval",                newMasterConfig.news.interval );
    XMLAccess.GetSubNodeValue ( "news.oldestpost",              newMasterConfig.news.strOldestPost );
    XMLAccess.GetSubNodeValue ( "news.maxhistorylength",        newMasterConfig.news.iMaxHistoryLength );

    if ( newMasterConfig.IsValid () )
    {
        if ( strResetVar == "yes" )
            m_VarConfig = SUpdaterVarConfig ();

        m_MasterConfig = newMasterConfig;
        UpdateTroubleURL ();
        m_ConditionMap.SetCondition ( "ProcessResponse", "ok" );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::UpdateTroubleURL
//
// Select a valid server incase there is trouble
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::UpdateTroubleURL ( void )
{
    std::vector < SString > serverList = MakeServerList ( m_MasterConfig.trouble.serverInfoMap );
    if ( serverList.size () && serverList[0].length () > 4 )
        SetApplicationSetting ( "trouble-url", serverList[0] );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetNewsSettings
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetNewsSettings ( SString& strOutOldestPost, uint& uiOutMaxHistoryLength )
{
    EnsureLoadedConfigFromXML ();
    strOutOldestPost = m_MasterConfig.news.strOldestPost;
    uiOutMaxHistoryLength = m_MasterConfig.news.iMaxHistoryLength;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseSidegradeURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseSidegradeURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList = MakeServerList ( m_MasterConfig.sidegrade.serverInfoMap );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseNewsUpdateURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseNewsUpdateURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList = MakeServerList ( m_MasterConfig.news.serverInfoMap );
    m_JobInfo.bShowDownloadPercent = false;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseProvidedURLs
//
// Use the urls in the previous query to download a file
// Called before starting the file download
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseProvidedURLs ( void )
{
    CDataInfoSet serverInfoMap = m_JobInfo.serverInfoMap;
    m_JobInfo.serverList = MakeServerList ( serverInfoMap );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseDataFilesURLs
//
// Called before starting the file download
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseDataFilesURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList = MakeServerList ( m_MasterConfig.gtadatafiles.serverInfoMap );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessPatchFileDownload
//
// Called after completing a file download
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessPatchFileDownload ( void )
{
    m_ConditionMap.SetCondition ( "ProcessResponse", "" );
    m_ConditionMap.SetCondition ( "Download", "" );

    if ( m_JobInfo.downloadBuffer.size () == 0 )
        return;

    unsigned int uiSize = m_JobInfo.downloadBuffer.size ();
    char* pData = &m_JobInfo.downloadBuffer[0];

    // Check MD5
    if ( m_JobInfo.strMD5 != "none" )
    {
        // Hash data
        MD5 md5Result;
        CMD5Hasher().Calculate ( pData, uiSize, md5Result );
        char szMD5[33];
        CMD5Hasher::ConvertToHex ( md5Result, szMD5 );

        if ( m_JobInfo.strMD5 != szMD5 )
        {
            // If MD5 error, remove this server from the list
            if ( m_JobInfo.serverList.size () )
                ListRemoveIndex( m_JobInfo.serverList, m_JobInfo.iCurrent-- );
            m_ConditionMap.SetCondition ( "Download", "Fail", "Checksum" );
            AddReportLog ( 5003, SString ( "DoPollDownload: Checksum wrong for %s (%s %s)", m_JobInfo.strFilename.c_str(), m_JobInfo.strMD5.c_str(), szMD5 ) );
            return;
        }
    }

    // Check signature
    if ( !CCore::GetSingleton ().GetNetwork ()->VerifySignature ( pData, uiSize ) )
    {
        AddReportLog ( 5006, SString ( "DoPollDownload: Signature wrong for %s (MD5: %s)", m_JobInfo.strFilename.c_str(), m_JobInfo.strMD5.c_str() ) );
        m_ConditionMap.SetCondition ( "Download", "Fail", "Checksum" );
        return;
    }
    
    ////////////////////////
    // Save file somewhere
    // Make a list of possible places to save the file
    SString strPathFilename;
    {
        std::list < SString > saveLocationList;
        saveLocationList.push_back ( PathJoin ( GetMTALocalAppDataPath (), "upcache", m_JobInfo.strFilename ) );
        saveLocationList.push_back ( PathJoin ( GetMTATempPath (), "upcache", m_JobInfo.strFilename ) );
        saveLocationList.push_back ( GetMTATempPath () + m_JobInfo.strFilename );
        saveLocationList.push_back ( PathJoin ( "\\temp", m_JobInfo.strFilename ) );

        // Try each place
        for ( std::list < SString > ::iterator iter = saveLocationList.begin () ; iter != saveLocationList.end () ; ++iter )
        {
            const SString strSaveLocation = MakeUniquePath ( *iter );
            MakeSureDirExists ( strSaveLocation );
            if ( FileSave ( strSaveLocation, pData, uiSize ) )
            {
                strPathFilename = strSaveLocation;
                break;
            }

            AddReportLog ( 5004, SString ( "DoPollDownload: Unable to use the path %s", strSaveLocation.c_str() ) );
        }
    }

    if ( strPathFilename.length () == 0 )
    {
        if ( m_JobInfo.serverList.size () )
            ListRemoveIndex( m_JobInfo.serverList, m_JobInfo.iCurrent-- );
        m_ConditionMap.SetCondition ( "Download", "Fail", "Saving" );
        AddReportLog ( 5005, SString ( "DoPollDownload: Unable to save the file %s (size %d)", m_JobInfo.strFilename.c_str (), uiSize ) );
        return;
    }
    ////////////////////////

    m_JobInfo.strSaveLocation = strPathFilename;
    m_ConditionMap.SetCondition ( "Download", "Ok" );
    AddReportLog ( 2007, SString ( "DoPollDownload: Downloaded %s", m_JobInfo.strSaveLocation.c_str() ) );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartDownload
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_StartDownload ( void )
{
    switch ( DoSendDownloadRequestToNextServer () )
    {
        case RES_FAIL:
            // Can't find any(more) servers to send a query to
            // Drop back to previous stack level

            // Refresh master config as well
            OnPossibleConfigProblem ();
            break;

        case RES_OK:
            // Query sent ok, now wait for response
            Push ( _PollDownload );
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollDownload
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollDownload ( void )
{
    switch ( DoPollDownload () )
    {
        case RES_FAIL:
            // Connection to current server failed, try next server
            Push ( _StartDownload );
            // Refresh master config as well
            OnPossibleConfigProblem ();
            break;

        case RES_OK:
            // Got a valid response
            // Drop back to previous stack level for processing
            break;

        case RES_POLL:
            // Waiting...

            if ( GetQuestionBox ().IsVisible () )
            {
                // Handle progress/cancel if visible
                if ( GetQuestionBox ().PollButtons () == 0 )
                {
                    m_HTTP.Get ("");
                    GetQuestionBox ().Reset ();
                    m_ConditionMap.SetCondition ( "ProcessResponse", "cancel" );
                    return;
                }
                if ( m_JobInfo.bShowDownloadPercent )
                    GetQuestionBox ().SetMessage ( SString ( "%3d %% completed", m_JobInfo.uiBytesDownloaded * 100 / Max < unsigned int > ( 1, m_JobInfo.iFilesize ) ) );
                if ( m_JobInfo.iIdleTime > 1000 && m_JobInfo.iIdleTimeLeft > 500 )
                    GetQuestionBox ().AppendMessage ( SString ( "\n\nWaiting for response  -  %-3d", m_JobInfo.iIdleTimeLeft / 1000 ) );
                else
                    GetQuestionBox ().AppendMessage ( "" );
            }

            Push ( _PollDownload );
            break;

        default:
            assert ( 0 );
    }
}





//
//
// 
// Uploaders
//
//
//

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseReportLogURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseReportLogURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList = MakeServerList ( m_MasterConfig.report.serverInfoMap );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseReportLogPostContent
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseReportLogPostContent ( void )
{
    // Remove unrequired items
#ifndef MTA_DEBUG
    GetReportWrap ()->ClearLogContents ( GetReportWrap ()->GetFilter () );
#endif
    // See if anything left to send
    SString strContents = GetReportWrap ()->GetLogContents ( GetReportWrap ()->GetFilter (), GetReportWrap ()->GetMaxSize () );
    if ( (int)strContents.length () < GetReportWrap ()->GetMinSize () )
        return;

    // Copy to post buffer
    const char* pSrc = strContents.c_str ();
    uint uiLength    = strContents.length () + 1;

    m_JobInfo.postContent.assign ( uiLength, 0 );
    char* pDst = &m_JobInfo.postContent[0];
    memcpy ( pDst, pSrc, uiLength );
    m_JobInfo.bPostContentBinary = false;

    // Wipe log here
    GetReportWrap ()->ClearLogContents ( "-all" );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseCrashDumpURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseCrashDumpURLs ( void )
{
    m_JobInfo = SJobInfo ();
    m_JobInfo.serverList = MakeServerList ( m_MasterConfig.crashdump.serverInfoMap );
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseCrashDumpPostContent
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseCrashDumpPostContent ( void )
{
    // Get filename to send
    SString strPathFilename = GetApplicationSetting ( "diagnostics", "last-dump-save" );
    if ( !strPathFilename.length () )
        return;
    SetApplicationSetting ( "diagnostics", "last-dump-save", "" );

    // Get user pref
    if ( GetApplicationSetting ( "diagnostics", "send-dumps" ) == "no" )
        return;

    // Add to history

    // Extract module and address from filename
    std::vector < SString > parts;
    strPathFilename.Split ( "_", parts );
    if ( parts.size () > 7 )
    {
        SString strModuleAndOffset = parts[2] + "_" + parts[3];
        SString strDateAndTime = parts[6] + "_" + parts[7];
        strDateAndTime.Split ( ".", &strDateAndTime, NULL );

        // Check history for duplicates
        CDataInfoSet& history = m_VarConfig.crashdump_history;
        int iDuplicates = 0;
        for ( uint i = 0 ; i < history.size () ; i++ )
            if ( strModuleAndOffset == history[i].strName )
                iDuplicates++;

        // Add to history
        if ( iDuplicates <= m_MasterConfig.crashdump.iDuplicates )
        {
            SDataInfoItem item;
            item.strName = strModuleAndOffset;
            item.strValue = strDateAndTime;
            history.push_back ( item );
        }
        else
            return;

        // Sort by date
        for ( int i = 0 ; i < (int)history.size () - 1 ; i++ )
        {
            SDataInfoItem& a = history[i];
            SDataInfoItem& b = history[i+1];
            if ( a.strValue > b.strValue )
            {
                std::swap ( a, b );
                i = Max ( i - 2, -1 );
            }
        }

        // Remove oldest if required
        int iNumToRemove = history.size () - m_MasterConfig.crashdump.iMaxHistoryLength;
        if ( iNumToRemove > 0 )
            history.erase ( history.begin (), history.begin () + iNumToRemove );

        CCore::GetSingleton ().SaveConfig ();
    }


    // Load into post buffer
    if ( FileLoad ( strPathFilename, m_JobInfo.postContent ) )
    {
        // Set post filename without path
        strPathFilename.Split ( PATH_SEPERATOR, NULL, &m_JobInfo.strPostFilename, -1 );
        m_JobInfo.bPostContentBinary = true;
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartSendPost
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_StartSendPost ( void )
{
    if ( !m_JobInfo.postContent.size () )
        return;

    switch ( DoSendPostToNextServer () )
    {
        case RES_FAIL:
            // Can't find any(more) servers to send a query to
            // Drop back to previous stack level
            break;

        case RES_OK:
            // Query sent ok, now wait for response
            Push ( _PollSendPost );
            break;

        default:
            assert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollSendPost
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollSendPost ( void )
{
    switch ( DoPollPost () )
    {
        case RES_FAIL:
            // Connection to current server failed, try next server
            Push ( _StartSendPost );
            break;

        case RES_OK:
            // Got a valid response
            // Drop back to previous stack level for processing
            break;

        case RES_POLL:

            // Waiting...
            Push ( _PollSendPost );
            break;

        default:
            assert ( 0 );
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// 'Do'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoSendDownloadRequestToNextServer
//
// RES_FAIL - Can't find a valid server to query
// RES_OK   - Query sent
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoSendDownloadRequestToNextServer ( void )
{
    if ( ++m_JobInfo.iRetryCount > m_JobInfo.iMaxServersToTry || m_JobInfo.serverList.size () == 0 )
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_JobInfo.iCurrent++;
    m_JobInfo.iCurrent = m_JobInfo.iCurrent % m_JobInfo.serverList.size ();
    SString strServerURL = m_JobInfo.serverList[ m_JobInfo.iCurrent ];

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );
    char szStatus [ 128 ];
    CCore::GetSingleton ().GetNetwork ()->GetStatus ( szStatus, sizeof ( szStatus ) );

    // Compose version string
    unsigned short usNetRev = CCore::GetSingleton ().GetNetwork ()->GetNetRev ();
    unsigned short usNetRel = CCore::GetSingleton ().GetNetwork ()->GetNetRel ();
    SString strPlayerVersion ( "%d.%d.%d-%d.%05d.%d.%03d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTASA_VERSION_TYPE
                                ,MTASA_VERSION_BUILD
                                ,usNetRev
                                ,usNetRel
                                );

    SString strUpdateBuildType;
    CVARS_GET ( "update_build_type", strUpdateBuildType );

    // Make the query URL
    SString strQueryURL = strServerURL;
    strQueryURL = strQueryURL.Replace ( "%VERSION%", strPlayerVersion );
    strQueryURL = strQueryURL.Replace ( "%ID%", szSerial );
    strQueryURL = strQueryURL.Replace ( "%STATUS%", szStatus );
    strQueryURL = strQueryURL.Replace ( "%BETA%", strUpdateBuildType );
    strQueryURL = strQueryURL.Replace ( "%TYPE%", m_strServerSaysType );
    strQueryURL = strQueryURL.Replace ( "%DATA%", m_strServerSaysData );
    strQueryURL = strQueryURL.Replace ( "%REFER%", m_strServerSaysHost );
    strQueryURL = strQueryURL.Replace ( "%WANTVER%", m_strSidegradeVersion );
    strQueryURL = strQueryURL.Replace ( "%LASTNEWS%", m_VarConfig.news_lastNewsDate );

    // Perform the HTTP request
    m_HTTP.Get ( strQueryURL );
    m_strLastQueryURL = strQueryURL;
    OutputDebugLine( SString ( "DoSendDownloadRequestToNextServer %d/%d %s", m_JobInfo.iCurrent, m_JobInfo.serverList.size (), strQueryURL.c_str () ) );
    return RES_OK;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPollDownload
//
// RES_FAIL   - Download failed
// RES_OK     - Got valid file
// RES_POLL   - Processing
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoPollDownload ( void )
{
    m_ConditionMap.SetCondition ( "Download", "" );

    // Update progress
    unsigned int uiBytesDownloaded = m_HTTP.PeekNextDataSize ();
    if ( m_JobInfo.uiBytesDownloaded != uiBytesDownloaded )
    {
        m_llTimeStart = GetTickCount64_ ();
        m_JobInfo.uiBytesDownloaded = uiBytesDownloaded;
    }

    // Are we done yet?
    CHTTPBuffer buffer;
    if ( !m_HTTP.GetData ( buffer ) )
    {
        unsigned int status = m_HTTP.GetStatus ();
        m_JobInfo.iIdleTime = GetTickCount64_ () - m_llTimeStart;
        int iUseTimeOut = m_JobInfo.uiBytesDownloaded ? m_JobInfo.iTimeoutTransfer : m_JobInfo.iTimeoutConnect;
        m_JobInfo.iIdleTimeLeft = iUseTimeOut - m_JobInfo.iIdleTime;
        // Give up if error or timeout
        if ( ( status !=0 && status != 200 ) || m_JobInfo.iIdleTimeLeft < 0 )
        {
            AddReportLog ( 4002, SString ( "DoPollDownload: Regular fail for %s (status:%u  time:%u)", m_strLastQueryURL.c_str (), status, m_JobInfo.iIdleTime ) );

            m_ConditionMap.SetCondition ( "Download", "Fail" );
            return RES_FAIL;
        }
        return RES_POLL;
    }

    // Got something
    char* pData = buffer.GetData ();
    if ( pData )
    {
        unsigned int uiSize = buffer.GetSize ();
        m_JobInfo.downloadBuffer.resize ( buffer.GetSize () );
        memcpy ( &m_JobInfo.downloadBuffer[0], pData, uiSize );

        m_ConditionMap.SetCondition ( "Download", "Ok" );
        AddReportLog ( 2005, SString ( "DoPollDownload: Downloaded %d bytes from %s", uiSize, m_strLastQueryURL.c_str() ) );
        return RES_OK;
    }

    m_ConditionMap.SetCondition ( "Download", "Fail" );
    AddReportLog ( 5007, SString ( "DoPollDownload: Fail for %s", m_strLastQueryURL.c_str() ) );
    return RES_FAIL;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoSendPostToNextServer
//
// RES_FAIL - Can't find a valid server to query
// RES_OK   - Query sent
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoSendPostToNextServer ( void )
{
    if ( ++m_JobInfo.iRetryCount > 1 || m_JobInfo.serverList.size () == 0 )
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_ ();

    // Get URL of next server in the list
    m_JobInfo.iCurrent++;
    m_JobInfo.iCurrent = m_JobInfo.iCurrent % m_JobInfo.serverList.size ();
    SString strServerURL = m_JobInfo.serverList[ m_JobInfo.iCurrent ];

    // Get our serial number
    char szSerial [ 64 ];
    CCore::GetSingleton ().GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );
    char szStatus [ 128 ];
    CCore::GetSingleton ().GetNetwork ()->GetStatus ( szStatus, sizeof ( szStatus ) );

    // Compose version string
    unsigned short usNetRev = CCore::GetSingleton ().GetNetwork ()->GetNetRev ();
    unsigned short usNetRel = CCore::GetSingleton ().GetNetwork ()->GetNetRel ();
    SString strPlayerVersion ( "%d.%d.%d-%d.%05d.%d.%03d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTASA_VERSION_TYPE
                                ,MTASA_VERSION_BUILD
                                ,usNetRev
                                ,usNetRel
                                );

    // Make the query URL
    SString strQueryURL = strServerURL;
    strQueryURL = strQueryURL.Replace ( "%VERSION%", strPlayerVersion );
    strQueryURL = strQueryURL.Replace ( "%ID%", szSerial );
    strQueryURL = strQueryURL.Replace ( "%STATUS%", szStatus );
    strQueryURL = strQueryURL.Replace ( "%FILE%", m_JobInfo.strPostFilename );

    //
    // Send data. Doesn't check if it was received.
    //
    CNetHTTPDownloadManagerInterface * downloadManager = CCore::GetSingleton ().GetNetwork ()->GetHTTPDownloadManager ();
    downloadManager->QueueFile ( strQueryURL, NULL, 0, &m_JobInfo.postContent.at ( 0 ), m_JobInfo.postContent.size (), m_JobInfo.bPostContentBinary );
    if ( !downloadManager->IsDownloading () )
        downloadManager->StartDownloadingQueuedFiles ();

    return RES_OK;
}


///////////////////////////////////////////////////////////////
//
// CVersionUpdater::DoPollPost
//
// RES_FAIL   - Post failed
// RES_OK     - Post ok
// RES_POLL   - Processing
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::DoPollPost ( void )
{
    CNetHTTPDownloadManagerInterface* pHTTP = CCore::GetSingleton ().GetNetwork ()->GetHTTPDownloadManager ();
    if ( pHTTP )
    {
        if ( !pHTTP->ProcessQueuedFiles () )
        {
            return RES_POLL;
        }
    }
    return RES_OK;
}
