/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CVersionUpdater.cpp
 *  PURPOSE:     Version update check and message dialog class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CSettings.h>
#include "CVersionUpdater.Util.hpp"
#include "CNewsBrowser.h"
#include "CFilePathTranslator.h"
#include "SharedUtil.Thread.h"
#include <charconv>

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
    CVersionUpdater();
    virtual ~CVersionUpdater();
    virtual bool SaveConfigToXML();
    virtual void EnableChecking(bool bOn);
    virtual void DoPulse();
    virtual void InitiateUpdate(const SString& strType, const SString& strData, const SString& strHost);
    virtual bool IsOptionalUpdateInfoRequired(const SString& strHost);
    virtual void InitiateDataFilesFix();
    virtual void InitiateManualCheck();
    virtual void GetAseServerList(std::vector<SString>& outResult);
    virtual void InitiateSidegradeLaunch(const SString& strVersion, const SString& strHost, ushort usPort, const SString& strName, const SString& strPassword);
    virtual void GetBrowseVersionMaps(std::map<SString, int>& outBlockedVersionMap, std::map<SString, int>& outAllowedVersionMap);
    virtual void GetNewsSettings(SString& strOutOldestPost, uint& uiOutMaxHistoryLength);
    virtual const SString& GetDebugFilterString();

    // CVersionUpdater functions
    bool EnsureLoadedConfigFromXML();

    // Command lists
    void RunProgram(EUpdaterProgramType strProgramName);

    void Program_VersionCheck();
    void Program_ManualCheck();
    void Program_ManualCheckSim();
    void Program_ServerSaysUpdate();
    void Program_ServerSaysRecommend();
    void Program_ServerSaysDataFilesWrong();
    void Program_MasterFetch();
    void Program_SendCrashDump();
    void Program_SendReportLog();
    void Program_SidegradeLaunch();
    void Program_NewsUpdate();

    // Util
    CXMLNode*                         GetXMLConfigNode(bool bCreateIfRequired);
    bool                              IsBusy();
    void                              ResetEverything();
    CReportWrap*                      GetReportWrap();
    static std::vector<SString>       MakeServerList(const CDataInfoSet& infoMap);
    void                              PostChangeMasterConfig();
    void                              OnPossibleConfigProblem();
    CNetHTTPDownloadManagerInterface* GetHTTP();
    void                              GetSaveLocationList(std::list<SString>& outSaveLocationList, const SString& strFilename);
    SString                           GetResumableSaveLocation(const SString& strFilename, const SString& strMD5, uint iFilesize);
    static void                       StaticDownloadFinished(const SHttpDownloadResult& result);
    void                              DownloadFinished(const SHttpDownloadResult& result);

    // Commands
    void _UseMasterFetchURLs();
    void _UseVersionQueryURLs();
    void _UseProvidedURLs();
    void _UseReportLogURLs();
    void _UseCrashDumpQueryURLs();
    void _UseCrashDumpURLs();
    void _UseSidegradeURLs();
    void _UseNewsUpdateURLs();
    void _UseReportLogPostContent();
    void _UseCrashDumpPostContent();
    void _ShouldSendCrashDump();
    void _ResetLastCrashDump();
    int  ValidateCrashDumpPath(const SString& strPathFilename, SString* pOutErrorMsg, bool bCheckFileExists = false);
    void _ActionReconnect();
    void _DialogHide();
    void _ExitGame();
    void _ResetVersionCheckTimer();
    void _ResetNewsCheckTimer();
    void _ResetMasterCheckTimer();
    void _ResetManualCheckTimer();
    void _PollQuestionNoYes();
    void _PollAnyButton();
    void _DialogConnectingWait();
    void _DialogChecking();
    void _DialogNoUpdate();
    void _DialogDownloading();
    void _DialogServerSaysUpdateQuestion();
    void _DialogServerSaysRecommendQuestion();
    void _DialogUpdateQuestion();
    void _DialogUpdateQueryError();
    void _DialogUpdateResult();
    void _QUpdateResult();
    void _QUpdateNewsResult();
    void _DialogDataFilesQuestion();
    void _DialogExeFilesResult();
    void _StartDownload();
    int  _PollDownload();
    bool _StartSendPost();
    int  _PollSendPost();
    void _CheckSidegradeRequirements();
    void _DoSidegradeLaunch();
    void _DialogSidegradeDownloadQuestion();
    void _DialogSidegradeLaunchQuestion();
    void _DialogSidegradeQueryError();

    void _ProcessCrashDumpQuery();
    void _ProcessMasterFetch();
    void _ProcessPatchFileQuery();
    void _ProcessPatchFileDownload();
    void _QuitCurrentProgram();

    // Doers
    int DoSendDownloadRequestToNextServer();
    int DoPollDownload();
    int DoSendPostToNextServer();
    int DoPollPost();

    static void* StaticThreadProc(void* pContext);
    void*        ThreadProc();
    void         StopThread();
    void         ProcessCommand(EUpdaterProgramType eProgramType);
    void         MainStep();
    void         UpdaterYield();

    SJobInfo      m_JobInfo;
    long long     m_llTimeStart;
    CConditionMap m_ConditionMap;
    SString       m_strServerSaysType;
    SString       m_strServerSaysData;
    SString       m_strServerSaysHost;

    SString m_strSidegradeVersion;
    SString m_strSidegradeHost;
    SString m_strSidegradeName;
    SString m_strSidegradePassword;
    SString m_strSidegradePath;

    bool                   m_bCheckedTimeForVersionCheck;
    bool                   m_bCheckedTimeForNewsUpdate;
    bool                   m_bCheckedTimeForMasterFetch;
    bool                   m_bLoadedConfig;
    bool                   m_bSentCrashDump;
    bool                   m_bSentReportLog;
    std::map<SString, int> m_DoneOptionalMap;

    long long m_llTimeLastManualCheck;
    SString   m_strLastManualCheckBuildType;

    CReportWrap* m_pReportWrap;
    SString      m_strLastQueryURL;
    bool         m_bEnabled;
    uint         m_uiFrameCounter;
    int          m_iForceMasterConfigRefreshCount;

    SUpdaterMasterConfig m_MasterConfig;
    SUpdaterVarConfig    m_VarConfig;

    CThreadHandle* m_pProgramThreadHandle;

    // Shared variables
    struct
    {
        bool                m_bTerminateThread;
        bool                m_bThreadTerminated;
        EUpdaterProgramType m_CurrentProgram;
        CComboMutex         m_Mutex;
        bool                m_bQuitCurrentProgram;
        bool                m_bExitGame;
    } shared;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CVersionUpdater* g_pVersionUpdater = NULL;

CVersionUpdaterInterface* GetVersionUpdater()
{
    if (!g_pVersionUpdater)
        g_pVersionUpdater = new CVersionUpdater();
    return g_pVersionUpdater;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::CVersionUpdater
//
//
//
///////////////////////////////////////////////////////////////
CVersionUpdater::CVersionUpdater()
{
    shared.m_Mutex.Lock();
    m_pProgramThreadHandle = new CThreadHandle(CVersionUpdater::StaticThreadProc, this);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::~CVersionUpdater
//
//
//
///////////////////////////////////////////////////////////////
CVersionUpdater::~CVersionUpdater()
{
    StopThread();
    SAFE_DELETE(m_pProgramThreadHandle);
    SAFE_DELETE(m_pReportWrap);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::StopThread
//
// Stop the updater processing thread
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::StopThread()
{
    shared.m_bTerminateThread = true;
    shared.m_Mutex.Signal();
    shared.m_Mutex.Unlock();

    for (uint i = 0; i < 5000; i += 15)
    {
        if (shared.m_bThreadTerminated)
            return;

        Sleep(15);
    }

    // If thread not stopped, (async) cancel it
    m_pProgramThreadHandle->Cancel();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetXMLConfigNode
//
// Get the main node for load/saving settings
//
///////////////////////////////////////////////////////////////
CXMLNode* CVersionUpdater::GetXMLConfigNode(bool bCreateIfRequired)
{
    // Get the root node
    CXMLNode* pRoot = CCore::GetSingleton().GetConfig();
    if (!pRoot)
        return NULL;

    // Get the top config node
    CXMLNode* pSectionNode = pRoot->FindSubNode(CONFIG_NODE_UPDATER);
    if (!pSectionNode)
    {
        if (!bCreateIfRequired)
            return NULL;

        // Non-existent, create a new node
        pSectionNode = pRoot->CreateSubNode(CONFIG_NODE_UPDATER);
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
bool CVersionUpdater::EnsureLoadedConfigFromXML()
{
    if (m_bLoadedConfig)
        return false;

    m_bLoadedConfig = true;

    // Reset
    m_MasterConfig = SUpdaterMasterConfig();

    // Try load
    CXMLNode* pMainNode = GetXMLConfigNode(false);

    if (!pMainNode)
        return false;

    if (CXMLNode* pVar = pMainNode->FindSubNode("var"))
    {
        CXMLAccess XMLAccess(pVar);
        // Stuff for here
        XMLAccess.GetSubNodeValue("version_lastchecktime", m_VarConfig.version_lastCheckTime);
        XMLAccess.GetSubNodeValue("master_lastchecktime", m_VarConfig.master_lastCheckTime);
        XMLAccess.GetSubNodeValue("master_highestnotifyrevision", m_VarConfig.master_highestNotifyRevision);
        XMLAccess.GetSubNodeValue("news_lastchecktime", m_VarConfig.news_lastCheckTime);
        XMLAccess.GetSubNodeValue("news_lastnewsdate", m_VarConfig.news_lastNewsDate);
        XMLAccess.GetSubNodeValue("crashdump_historylist", m_VarConfig.crashdump_history);
    }

    if (CXMLNode* pMaster = pMainNode->FindSubNode("mastercache"))
    {
        CXMLAccess XMLAccess(pMaster);
        // Stuff from master
        XMLAccess.GetSubNodeValue("master.revision", m_MasterConfig.master.strRevision);
        XMLAccess.GetSubNodeValue("master.serverlist", m_MasterConfig.master.serverInfoMap);
        XMLAccess.GetSubNodeValue("master.interval", m_MasterConfig.master.interval);
        XMLAccess.GetSubNodeValue("version.serverlist", m_MasterConfig.version.serverInfoMap);
        XMLAccess.GetSubNodeValue("version.interval", m_MasterConfig.version.interval);
        XMLAccess.GetSubNodeValue("report.serverlist", m_MasterConfig.report.serverInfoMap);
        XMLAccess.GetSubNodeValue("report.interval", m_MasterConfig.report.interval);
        XMLAccess.GetSubNodeValue("report.filter2", m_MasterConfig.report.strFilter);
        XMLAccess.GetSubNodeValue("report.minsize", m_MasterConfig.report.iMinSize);
        XMLAccess.GetSubNodeValue("report.maxsize", m_MasterConfig.report.iMaxSize);
        XMLAccess.GetSubNodeValue("crashdump.serverlist", m_MasterConfig.crashdump.serverInfoMap);
        XMLAccess.GetSubNodeValue("crashdump.duplicates", m_MasterConfig.crashdump.iDuplicates);
        XMLAccess.GetSubNodeValue("crashdump.maxhistorylength", m_MasterConfig.crashdump.iMaxHistoryLength);
        XMLAccess.GetSubNodeValue("trouble.serverlist", m_MasterConfig.trouble.serverInfoMap);
        XMLAccess.GetSubNodeValue("ase.serverlist", m_MasterConfig.ase.serverInfoMap);
        XMLAccess.GetSubNodeValue("sidegrade.serverlist", m_MasterConfig.sidegrade.serverInfoMap);
        XMLAccess.GetSubNodeValue("sidegrade.nobrowselist", m_MasterConfig.sidegrade.nobrowseInfoMap);
        XMLAccess.GetSubNodeValue("sidegrade.onlybrowselist", m_MasterConfig.sidegrade.onlybrowseInfoMap);
        XMLAccess.GetSubNodeValue("news.serverlist", m_MasterConfig.news.serverInfoMap);
        XMLAccess.GetSubNodeValue("news.interval", m_MasterConfig.news.interval);
        XMLAccess.GetSubNodeValue("news.oldestpost", m_MasterConfig.news.strOldestPost);
        XMLAccess.GetSubNodeValue("news.maxhistorylength", m_MasterConfig.news.iMaxHistoryLength);
        XMLAccess.GetSubNodeValue("misc.debug.filter2", m_MasterConfig.misc.debug.strFilter);

        PostChangeMasterConfig();
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
bool CVersionUpdater::SaveConfigToXML()
{
    if (!m_bLoadedConfig)
        return false;

    CXMLNode* pMainNode = GetXMLConfigNode(true);

    if (!pMainNode)
        return false;

    pMainNode->DeleteAllSubNodes();

    if (CXMLNode* pVar = pMainNode->CreateSubNode("var"))
    {
        CXMLAccess XMLAccess(pVar);
        // Stuff for here
        XMLAccess.SetSubNodeValue("version_lastchecktime", m_VarConfig.version_lastCheckTime);
        XMLAccess.SetSubNodeValue("master_lastchecktime", m_VarConfig.master_lastCheckTime);
        XMLAccess.SetSubNodeValue("master_highestnotifyrevision", m_VarConfig.master_highestNotifyRevision);
        XMLAccess.SetSubNodeValue("news_lastchecktime", m_VarConfig.news_lastCheckTime);
        XMLAccess.SetSubNodeValue("news_lastnewsdate", m_VarConfig.news_lastNewsDate);
        XMLAccess.SetSubNodeValue("crashdump_historylist", m_VarConfig.crashdump_history);
    }

    if (CXMLNode* pMaster = pMainNode->CreateSubNode("mastercache"))
    {
        CXMLAccess XMLAccess(pMaster);
        // Stuff from master
        XMLAccess.SetSubNodeValue("master.revision", m_MasterConfig.master.strRevision);
        XMLAccess.SetSubNodeValue("master.serverlist", m_MasterConfig.master.serverInfoMap);
        XMLAccess.SetSubNodeValue("master.interval", m_MasterConfig.master.interval);
        XMLAccess.SetSubNodeValue("version.serverlist", m_MasterConfig.version.serverInfoMap);
        XMLAccess.SetSubNodeValue("version.interval", m_MasterConfig.version.interval);
        XMLAccess.SetSubNodeValue("report.serverlist", m_MasterConfig.report.serverInfoMap);
        XMLAccess.SetSubNodeValue("report.interval", m_MasterConfig.report.interval);
        XMLAccess.SetSubNodeValue("report.filter2", m_MasterConfig.report.strFilter);
        XMLAccess.SetSubNodeValue("report.minsize", m_MasterConfig.report.iMinSize);
        XMLAccess.SetSubNodeValue("report.maxsize", m_MasterConfig.report.iMaxSize);
        XMLAccess.SetSubNodeValue("crashdump.serverlist", m_MasterConfig.crashdump.serverInfoMap);
        XMLAccess.SetSubNodeValue("crashdump.duplicates", m_MasterConfig.crashdump.iDuplicates);
        XMLAccess.SetSubNodeValue("crashdump.maxhistorylength", m_MasterConfig.crashdump.iMaxHistoryLength);
        XMLAccess.SetSubNodeValue("trouble.serverlist", m_MasterConfig.trouble.serverInfoMap);
        XMLAccess.SetSubNodeValue("ase.serverlist", m_MasterConfig.ase.serverInfoMap);
        XMLAccess.SetSubNodeValue("sidegrade.serverlist", m_MasterConfig.sidegrade.serverInfoMap);
        XMLAccess.SetSubNodeValue("sidegrade.nobrowselist", m_MasterConfig.sidegrade.nobrowseInfoMap);
        XMLAccess.SetSubNodeValue("sidegrade.onlybrowselist", m_MasterConfig.sidegrade.onlybrowseInfoMap);
        XMLAccess.SetSubNodeValue("news.serverlist", m_MasterConfig.news.serverInfoMap);
        XMLAccess.SetSubNodeValue("news.interval", m_MasterConfig.news.interval);
        XMLAccess.SetSubNodeValue("news.oldestpost", m_MasterConfig.news.strOldestPost);
        XMLAccess.SetSubNodeValue("news.maxhistorylength", m_MasterConfig.news.iMaxHistoryLength);
        XMLAccess.SetSubNodeValue("misc.debug.filter2", m_MasterConfig.misc.debug.strFilter);
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
void CVersionUpdater::EnableChecking(bool bOn)
{
    m_bEnabled = bOn;
    EnsureLoadedConfigFromXML();
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
    if (!m_pReportWrap)
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
void CVersionUpdater::DoPulse()
{
    if (!m_bEnabled)
        return;

    TIMING_CHECKPOINT("+VersionUpdaterPulse");

    EnsureLoadedConfigFromXML();

    //
    // Time for master check?
    //
    if (!m_bCheckedTimeForMasterFetch && !IsBusy())
    {
        m_bCheckedTimeForMasterFetch = true;

        // Do update from master now if our config is wrong
        if (!m_MasterConfig.IsValid())
            m_VarConfig.master_lastCheckTime.SetFromSeconds(0);

        // Master update interval maximum is 7 days
        if (m_MasterConfig.master.interval.ToSeconds() > CTimeSpan("7d").ToSeconds())
            m_MasterConfig.master.interval.SetFromString("7d");

        time_t secondsSinceCheck = CDateTime::Now().ToSeconds() - m_VarConfig.master_lastCheckTime.ToSeconds();
        OutputDebugLine(SString("[Updater] master timeSinceCheck: %d  time till next check: %d", (int)(secondsSinceCheck),
                                (int)(m_MasterConfig.master.interval.ToSeconds() - secondsSinceCheck)));

        // Only check once a week
        if (secondsSinceCheck > m_MasterConfig.master.interval.ToSeconds() || secondsSinceCheck < 0)
        {
            RunProgram(EUpdaterProgramType::MasterFetch);
        }
    }

    //
    // Time for periodic check?
    //
    if (!m_bCheckedTimeForVersionCheck && !IsBusy() && !CCore::GetSingleton().WasLaunchedWithConnectURI())
    {
        m_bCheckedTimeForVersionCheck = true;

        // Do update check now if game did not stop cleanly last time
        if (WatchDogWasUncleanStop())
            m_VarConfig.version_lastCheckTime.SetFromSeconds(0);

        time_t secondsSinceCheck = CDateTime::Now().ToSeconds() - m_VarConfig.version_lastCheckTime.ToSeconds();
        OutputDebugLine(SString("[Updater] version timeSinceCheck: %d  time till next check: %d", (int)(secondsSinceCheck),
                                (int)(m_MasterConfig.version.interval.ToSeconds() - secondsSinceCheck)));

        // Only check once a day
        if (secondsSinceCheck > m_MasterConfig.version.interval.ToSeconds() || secondsSinceCheck < 0)
        {
            RunProgram(EUpdaterProgramType::VersionCheck);
        }
    }

    //
    // Time for news update?
    //
    if (!m_bCheckedTimeForNewsUpdate && !IsBusy())
    {
        m_bCheckedTimeForNewsUpdate = true;

        time_t secondsSinceCheck = CDateTime::Now().ToSeconds() - m_VarConfig.news_lastCheckTime.ToSeconds();
        OutputDebugLine(SString("[Updater] news timeSinceCheck: %d  time till next check: %d", (int)(secondsSinceCheck),
                                (int)(m_MasterConfig.news.interval.ToSeconds() - secondsSinceCheck)));

        // Only check once an interval
        if (secondsSinceCheck > m_MasterConfig.news.interval.ToSeconds() || secondsSinceCheck < 0)
        {
            RunProgram(EUpdaterProgramType::NewsUpdate);
        }
    }

    //
    // Should send previous crash dump?
    //
    if (!m_bSentCrashDump && !IsBusy())
    {
        m_bSentCrashDump = true;
        RunProgram(EUpdaterProgramType::SendCrashDump);
    }

    //
    // Should send report on how new features are functioning?
    //
    if (!m_bSentReportLog && !IsBusy())
    {
        m_bSentReportLog = true;
        RunProgram(EUpdaterProgramType::SendReportLog);
    }

    //
    // Give updater thread some time if it is doing something
    //
    if (IsBusy())
    {
        MainStep();
    }

    TIMING_CHECKPOINT("-VersionUpdaterPulse");
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateUpdate
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateUpdate(const SString& strType, const SString& strData, const SString& strHost)
{
    // Disable server-initiated updates for secondary client
    if (CCore::GetSingleton().IsSecondaryClient())
        return;

    // Don't allow update if CL2 is running
    HANDLE hCL2Mutex = OpenMutexA(SYNCHRONIZE, FALSE, "Global\\{4962AF5F-5D82-412D-9CCA-AB8BB9DBD354}");
    if (hCL2Mutex)
    {
        CloseHandle(hCL2Mutex);
        return;
    }

    if (strType == "Mandatory")
    {
        CCore::GetSingleton().RemoveMessageBox();
        RunProgram(EUpdaterProgramType::ServerSaysUpdate);
    }
    else if (strType == "Optional" || strType == "Recommended")
    {
        CCore::GetSingleton().RemoveMessageBox();
        MapSet(m_DoneOptionalMap, strHost, 1);
        RunProgram(EUpdaterProgramType::ServerSaysRecommend);
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
bool CVersionUpdater::IsOptionalUpdateInfoRequired(const SString& strHost)
{
    // Should this server tell us about recommended updates?
    return !MapContains(m_DoneOptionalMap, strHost);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateDataFilesFix
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateDataFilesFix()
{
    RunProgram(EUpdaterProgramType::ServerSaysDataFilesWrong);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateManualCheck
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateManualCheck()
{
    // Disable update checking for secondary client
    if (CCore::GetSingleton().IsSecondaryClient())
    {
        CCore::GetSingleton().ShowMessageBox(_("Information"), _("Update checking is disabled in secondary client"), MB_BUTTON_OK | MB_ICON_INFO);
        return;
    }

    // Don't allow update if CL2 is running
    HANDLE hCL2Mutex = OpenMutexA(SYNCHRONIZE, FALSE, "Global\\{4962AF5F-5D82-412D-9CCA-AB8BB9DBD354}");
    if (hCL2Mutex)
    {
        CloseHandle(hCL2Mutex);
        CCore::GetSingleton().ShowMessageBox(_("Information"), _("Can't check for updates while secondary client is running"), MB_BUTTON_OK | MB_ICON_INFO);
        return;
    }

    if (GetQuestionBox().IsVisible())
    {
        // Bring to the front
        GetQuestionBox().Show();
        return;
    }

    if (IsBusy())
    {
        CCore::GetSingleton().ShowMessageBox(_("Busy"), _("Can't check for updates right now"), MB_BUTTON_OK | MB_ICON_ERROR);
        return;
    }

    SString strUpdateBuildType;
    CVARS_GET("update_build_type", strUpdateBuildType);

    if (GetTickCount64_() - m_llTimeLastManualCheck > 10000 || strUpdateBuildType != m_strLastManualCheckBuildType)
    {
        RunProgram(EUpdaterProgramType::ManualCheck);
    }
    else
    {
        // Pretend to do a check if less than a minute since last check
        RunProgram(EUpdaterProgramType::ManualCheckSim);
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::InitiateSidegradeLaunch
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::InitiateSidegradeLaunch(const SString& strVersion, const SString& strIp, ushort usPort, const SString& strName,
                                              const SString& strPassword)
{
    RunProgram(EUpdaterProgramType::SidegradeLaunch);

    m_strSidegradeVersion = strVersion;
    m_strSidegradeHost = SString("%s:%d", *strIp, usPort);
    m_strSidegradeName = strName;
    m_strSidegradePassword = strPassword;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetAseServerList
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetAseServerList(std::vector<SString>& outResult)
{
    EnsureLoadedConfigFromXML();

    outResult = MakeServerList(m_MasterConfig.ase.serverInfoMap);

    // Backup plan if list is empty
    if (outResult.empty())
        outResult.push_back(SERVER_LIST_MASTER_URL);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetBrowseVersionMaps
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetBrowseVersionMaps(std::map<SString, int>& outBlockedVersionMap, std::map<SString, int>& outAllowedVersionMap)
{
    EnsureLoadedConfigFromXML();

    outBlockedVersionMap.clear();
    outAllowedVersionMap.clear();

    //
    // Blocked - Find list for our version
    //
    {
        const CDataInfoSet& dataInfoSet = m_MasterConfig.sidegrade.nobrowseInfoMap;
        for (uint i = 0; i < dataInfoSet.size(); i++)
        {
            const SDataInfoItem& line = dataInfoSet[i];
            const SString*       pstrVersion = MapFind(line.attributeMap, "version");
            if (pstrVersion && *pstrVersion == MTA_DM_ASE_VERSION)
            {
                // Found list for our version
                std::vector<SString> blockedVersionList;
                line.strValue.Split(";", blockedVersionList);

                for (uint i = 0; i < blockedVersionList.size(); i++)
                {
                    outBlockedVersionMap[blockedVersionList[i]] = 1;
                }

                break;
            }
        }
    }

    //
    // Allowed - Find list for our version
    //
    {
        const CDataInfoSet& dataInfoSet = m_MasterConfig.sidegrade.onlybrowseInfoMap;
        for (uint i = 0; i < dataInfoSet.size(); i++)
        {
            const SDataInfoItem& line = dataInfoSet[i];
            const SString*       pstrVersion = MapFind(line.attributeMap, "version");
            if (pstrVersion && *pstrVersion == MTA_DM_ASE_VERSION)
            {
                // Found list for our version
                std::vector<SString> allowedVersionList;
                line.strValue.Split(";", allowedVersionList);

                for (uint i = 0; i < allowedVersionList.size(); i++)
                {
                    outAllowedVersionMap[allowedVersionList[i]] = 1;
                }

                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetDebugFilterString
//
//
//
///////////////////////////////////////////////////////////////
const SString& CVersionUpdater::GetDebugFilterString()
{
    EnsureLoadedConfigFromXML();
    return m_MasterConfig.misc.debug.strFilter;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetHTTP
//
// Get the HTTP download manager used for all version updater stuff
//
///////////////////////////////////////////////////////////////
CNetHTTPDownloadManagerInterface* CVersionUpdater::GetHTTP()
{
    return g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::CORE_UPDATER);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetSaveLocationList
//
// Get list of places to try to save a file to
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetSaveLocationList(std::list<SString>& outSaveLocationList, const SString& strFilename)
{
    outSaveLocationList.push_back(PathJoin(GetMTADataPath(), "upcache", strFilename));
    outSaveLocationList.push_back(PathJoin(GetMTATempPath(), "upcache", strFilename));
    outSaveLocationList.push_back(GetMTATempPath() + strFilename);
    outSaveLocationList.push_back(PathJoin("\\temp", strFilename));
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetResumableSaveLocation
//
// Get path and filename to use for a resumable file download
//
///////////////////////////////////////////////////////////////
SString CVersionUpdater::GetResumableSaveLocation(const SString& strFilename, const SString& strMD5, uint iFilesize)
{
    if (strMD5.empty() || strFilename.empty())
        return "";

    // Get a list of possible save places
    std::list<SString> saveLocationList;
    GetSaveLocationList(saveLocationList, SString("%s-%s.tmp", *strFilename, *strMD5));

    // First, find to find existing file to resume
    for (std::list<SString>::iterator iter = saveLocationList.begin(); iter != saveLocationList.end(); ++iter)
    {
        // Check exists and can be appended to, and is smaller than what we want
        const SString strSaveLocation = *iter;
        if (FileExists(strSaveLocation) && FileAppend(strSaveLocation, "") && FileSize(strSaveLocation) < iFilesize)
        {
            AddReportLog(
                5008, SString("GetResumableSaveLocation: Will resume download at %d/%d for %s", (int)FileSize(strSaveLocation), iFilesize, *strSaveLocation));
            return strSaveLocation;
        }
    }

    // Otherwise, find writable place to save new file
    for (std::list<SString>::iterator iter = saveLocationList.begin(); iter != saveLocationList.end(); ++iter)
    {
        const SString strSaveLocation = *iter;
        MakeSureDirExists(strSaveLocation);
        // Check this is writable
        if (FileSave(strSaveLocation, ""))
        {
            FileDelete(strSaveLocation);
            AddReportLog(5009, SString("GetResumableSaveLocation: New download of %d for %s", iFilesize, *strSaveLocation));
            return strSaveLocation;
        }
    }
    return "";
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::ResetEverything
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::ResetEverything()
{
    m_JobInfo = SJobInfo();
    GetHTTP()->Reset();
    m_llTimeStart = 0;
    m_ConditionMap.clear();
    m_strServerSaysType = "";
    m_strServerSaysData = "";
    m_strServerSaysHost = "";
    m_strSidegradeVersion = "";
    m_strSidegradeHost = "";
    m_strSidegradeName = "";
    m_strSidegradePassword = "";

    GetQuestionBox().Reset();
    shared.m_bQuitCurrentProgram = false;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::IsBusy
//
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::IsBusy()
{
    if (shared.m_CurrentProgram != EUpdaterProgramType::None)
        return true;
    if (GetQuestionBox().IsVisible())
        return true;
    if (CCore::GetSingleton().WillRequestNewNickOnStart())
        return true;
    return false;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::MakeServerList
//
// Make ordered server list from config map
//
///////////////////////////////////////////////////////////////
std::vector<SString> CVersionUpdater::MakeServerList(const CDataInfoSet& dataInfoSetIn)
{
    std::vector<SString> serverList;

    RandomizeRandomSeed();

    //
    // Sort out server order
    //
    CDataInfoSet dataInfoSet = dataInfoSetIn;
    if (int iSize = dataInfoSet.size())
    {
        // randomize list
        for (int i = 0; i < iSize * 10; i++)
        {
            int iThis = i % iSize;
            int iOther = (rand() + 1) % iSize;
            std::swap(dataInfoSet[iThis], dataInfoSet[iOther]);
        }

        // bubble sort based on the priority attribute
        for (int i = 0; i < iSize - 1; i++)
        {
            SDataInfoItem& a = dataInfoSet[i];
            SDataInfoItem& b = dataInfoSet[i + 1];
            int            iPriorityA = MapFind(a.attributeMap, "priority") ? atoi(*MapFind(a.attributeMap, "priority")) : 5;
            int            iPriorityB = MapFind(b.attributeMap, "priority") ? atoi(*MapFind(b.attributeMap, "priority")) : 5;
            if (iPriorityA > iPriorityB)
            {
                std::swap(a, b);
                i = std::max(i - 2, -1);
            }
        }

        // If servers have the same priority, keep one and put the other(s) at the back of the list
        CDataInfoSet keepList;
        CDataInfoSet tailList;
        for (int iFirst = 0; iFirst < iSize; iFirst++)
        {
            // First item in range
            const SDataInfoItem& a = dataInfoSet[iFirst];
            int                  iPriorityA = MapFind(a.attributeMap, "priority") ? atoi(*MapFind(a.attributeMap, "priority")) : 5;

            // Find last item in range
            int iLast;
            for (iLast = iFirst; iLast < iSize - 1; iLast++)
            {
                const SDataInfoItem& b = dataInfoSet[iLast + 1];
                int                  iPriorityB = MapFind(b.attributeMap, "priority") ? atoi(*MapFind(b.attributeMap, "priority")) : 5;
                if (iPriorityA != iPriorityB)
                    break;
            }

            // Choose random server to keep in range, put the others at the back
            int iRangeSize = iLast - iFirst + 1;
            int iKeepIdx = rand() % (iRangeSize);

            for (int i = 0; i < iRangeSize; i++)
            {
                if (i == iKeepIdx)
                    keepList.push_back(dataInfoSet[iFirst + i]);
                else
                    tailList.push_back(dataInfoSet[iFirst + i]);
            }

            // continue with first item in the next priority range
            iFirst = iLast;
        }

        // Output values into the result list
        for (uint i = 0; i < keepList.size(); i++)
            serverList.push_back(keepList[i].strValue);

        for (uint i = 0; i < tailList.size(); i++)
            serverList.push_back(tailList[i].strValue);
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
void CVersionUpdater::OnPossibleConfigProblem()
{
    // Limit number of times this can be done
    if (m_iForceMasterConfigRefreshCount < 1)
    {
        m_iForceMasterConfigRefreshCount++;

        // Queue master config refresh
        m_VarConfig.master_lastCheckTime.SetFromSeconds(0);
        m_bCheckedTimeForMasterFetch = false;
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::RunProgram
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::RunProgram(EUpdaterProgramType strProgramName)
{
    assert(IsMainThread());
    OutputDebugLine(SString("[Updater] RunProgram %d", strProgramName));

    // Stop any running program
    while (shared.m_CurrentProgram != EUpdaterProgramType::None)
    {
        shared.m_bQuitCurrentProgram = true;
        MainStep();
    }

    ResetEverything();
    shared.m_CurrentProgram = strProgramName;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::MainStep
//
// Let the updater thread have some time
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::MainStep()
{
    assert(IsMainThread());
    shared.m_Mutex.Signal();
    shared.m_Mutex.Wait(-1);

    if (shared.m_bExitGame)
        CCore::GetSingleton().Quit();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::UpdaterYield
//
// Let the main thread continue
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::UpdaterYield()
{
    assert(!IsMainThread());
    shared.m_Mutex.Signal();
    shared.m_Mutex.Wait(-1);

    if (shared.m_bQuitCurrentProgram)
        _QuitCurrentProgram();
}

//
//
//
// Job servicing thread
//
//
//

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::StaticThreadProc
//
// static function
// Thread entry point
//
///////////////////////////////////////////////////////////////
void* CVersionUpdater::StaticThreadProc(void* pContext)
{
    CThreadHandle::AllowASyncCancel();
    return ((CVersionUpdater*)pContext)->ThreadProc();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::ThreadProc
//
// Job service loop
//
///////////////////////////////////////////////////////////////
void* CVersionUpdater::ThreadProc()
{
    shared.m_Mutex.Lock();
    while (!shared.m_bTerminateThread)
    {
        if (shared.m_CurrentProgram != EUpdaterProgramType::None)
        {
            try
            {
                ProcessCommand(shared.m_CurrentProgram);
            }
            catch (ExceptionQuitProgram)
            {
                OutputDebugLine(SString("[Updater] Program terminated %d", shared.m_CurrentProgram));
            }
            ResetEverything();

            shared.m_CurrentProgram = EUpdaterProgramType::None;
        }
        shared.m_Mutex.Signal();
        shared.m_Mutex.Wait(-1);
    }

    shared.m_bThreadTerminated = true;
    shared.m_Mutex.Unlock();

    return NULL;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::ProcessCommand
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::ProcessCommand(EUpdaterProgramType eProgramType)
{
    struct
    {
        EUpdaterProgramType eProgramType;
        PFNVOIDVOID         pFunction;
    } programList[] = {
        {EUpdaterProgramType::VersionCheck, &CVersionUpdater::Program_VersionCheck},
        {EUpdaterProgramType::ManualCheck, &CVersionUpdater::Program_ManualCheck},
        {EUpdaterProgramType::ManualCheckSim, &CVersionUpdater::Program_ManualCheckSim},
        {EUpdaterProgramType::ServerSaysUpdate, &CVersionUpdater::Program_ServerSaysUpdate},
        {EUpdaterProgramType::ServerSaysRecommend, &CVersionUpdater::Program_ServerSaysRecommend},
        {EUpdaterProgramType::ServerSaysDataFilesWrong, &CVersionUpdater::Program_ServerSaysDataFilesWrong},
        {EUpdaterProgramType::MasterFetch, &CVersionUpdater::Program_MasterFetch},
        {EUpdaterProgramType::SendCrashDump, &CVersionUpdater::Program_SendCrashDump},
        {EUpdaterProgramType::SendReportLog, &CVersionUpdater::Program_SendReportLog},
        {EUpdaterProgramType::SidegradeLaunch, &CVersionUpdater::Program_SidegradeLaunch},
        {EUpdaterProgramType::NewsUpdate, &CVersionUpdater::Program_NewsUpdate},
    };
    for (uint i = 0; i < NUMELMS(programList); i++)
    {
        if (programList[i].eProgramType == eProgramType)
        {
            OutputDebugLine(SString("[Updater] ProcessCommand %d", eProgramType));
            (this->*(programList[i].pFunction))();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
// 'Program_'
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_VersionCheck
//
// VersionCheck
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_VersionCheck()
{
    _UseVersionQueryURLs();  // Use VERSION_CHECKER_URL*
    _StartDownload();        // Fetch update info from update server
    _ProcessPatchFileQuery();

    if (m_ConditionMap.IsConditionTrue("ProcessResponse.update"))
        goto dload;  // If update server says 'update' then goto dload
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.files"))
        goto dload;  // If update server says 'files' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.silent"))
        goto silentdload;  // If update server says 'silent' then goto silentdload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.noupdate"))
        goto noupdate;  // If update server says 'noupdate' then goto noupdate:
    return;

dload:
    _DialogUpdateQuestion();  // Show "Update available" dialog
    if (m_ConditionMap.IsConditionTrue("QuestionResponse.!Yes"))
        goto end;          // If user says 'No', then goto end:
    _DialogDownloading();  // Show "Downloading..." message
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _DialogUpdateResult();  // Show "Update ok/failed" message
    return;

silentdload:
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _QUpdateResult();  // Maybe set OnRestartCommand
    return;

noupdate:
    _ResetVersionCheckTimer();  // Wait 24hrs before checking again
    return;

end:
    _ResetVersionCheckTimer();
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_ManualCheck
//
// Manual update check
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_ManualCheck()
{
    _UseVersionQueryURLs();  // Use VERSION_CHECKER_URL*
    _DialogChecking();       // Show "Checking..." message
    _StartDownload();        // Fetch update info from update server
    _ProcessPatchFileQuery();
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.update"))
        goto dload;  // If update server says 'update' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.files"))
        goto dload;  // If update server says 'files' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.silent"))
        goto dload;  // If update server says 'silent' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.noupdate"))
        goto noupdate;  // If update server says 'noupdate' then goto noupdate:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.cancel"))
        goto end;
    _DialogUpdateQueryError();
    return;

dload:
    _DialogUpdateQuestion();  // Show "Update available" dialog
    if (m_ConditionMap.IsConditionTrue("QuestionResponse.!Yes"))
        goto end;          // If user says 'No', then goto end:
    _DialogDownloading();  // Show "Downloading..." message
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _DialogUpdateResult();  // Show "Update ok/failed" message
    return;

noupdate:
    _ResetManualCheckTimer();  // Wait 1min before checking again
    _DialogNoUpdate();         // Show "No update available" dialog
    return;

end:
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_ManualCheckSim
//
// Manual update simulation
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_ManualCheckSim()
{
    _DialogNoUpdate();  // Show "No update available" dialog
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_ServerSaysUpdate
//
// ServerSaysUpdate
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_ServerSaysUpdate()
{
    _UseVersionQueryURLs();             // Use VERSION_CHECKER_URL*
    _DialogServerSaysUpdateQuestion();  // Show "Server says update" dialog
    if (m_ConditionMap.IsConditionTrue("QuestionResponse.!Yes"))
        goto end;       // If user says 'No', then goto end:
    _DialogChecking();  // Show "Checking..." message
    _StartDownload();   // Fetch update info from update server
    _ProcessPatchFileQuery();
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.update"))
        goto dload;  // If update server says 'update' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.files"))
        goto dload;  // If update server says 'files' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.silent"))
        goto dload;  // If update server says 'silent' then goto dload:
    _DialogUpdateQueryError();
    return;

dload:
    _DialogDownloading();  // Show "Downloading..." message
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _DialogUpdateResult();  // Show "Update ok/failed" message
    return;

end:
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_ServerSaysRecommend
//
// ServerSaysRecommend
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_ServerSaysRecommend()
{
    _UseVersionQueryURLs();   // Use VERSION_CHECKER_URL*
    _DialogConnectingWait();  // Show "Please wait..." message
    _StartDownload();         // Fetch update info from update server
    _ProcessPatchFileQuery();
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.update"))
        goto dload;  // If update server says 'update' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.files"))
        goto dload;  // If update server says 'files' then goto dload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.silent"))
        goto silentdload;  // If update server says 'silent' then goto silentdload:
    _ActionReconnect();
    return;

dload:
    _DialogServerSaysRecommendQuestion();  // Show "Server says update" dialog
    if (m_ConditionMap.IsConditionTrue("QuestionResponse.!Yes"))
        goto reconnect;    // If user says 'No', then goto reconnect:
    _DialogDownloading();  // Show "Downloading..." message
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _DialogUpdateResult();  // Show "Update ok/failed" message
    return;

silentdload:
    _DialogHide();       // Don't show downloading progress
    _ActionReconnect();  // Reconnect to game
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _QUpdateResult();  // Maybe set OnRestartCommand
    return;

reconnect:
    _ActionReconnect();
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_ServerSaysDataFilesWrong
//
// ServerSaysDataFilesWrong
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_ServerSaysDataFilesWrong()
{
    _DialogDataFilesQuestion();
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_MasterFetch
//
// MasterFetch
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_MasterFetch()
{
    _UseMasterFetchURLs();  // Use VERSION_CHECKER_URL*
    _StartDownload();       // Fetch file info from server
    _ProcessMasterFetch();
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.!ok"))
        goto error1;
    _ResetMasterCheckTimer();
    return;

error1:
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_SendCrashDump
//
// SendCrashDump
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_SendCrashDump()
{
    bool bUploadSuccess = false;  // Declare before any goto targets

    _ShouldSendCrashDump();  // Have we already sent a matching dump?
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.!ok"))
        goto end;
    _UseCrashDumpQueryURLs();
    _StartDownload();
    _ProcessCrashDumpQuery();  // Does the server want this dump?
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.!ok"))
        goto end;
    _UseCrashDumpURLs();         // Use CRASH_DUMP_URL*
    _UseCrashDumpPostContent();  // Use crash dump source

    // Send data and check if upload succeeded
    bUploadSuccess = _StartSendPost();

    if (bUploadSuccess)
    {
        // Upload succeeded - clear all settings to prevent duplicate sends
        SetApplicationSetting("diagnostics", "last-dump-save", "");
        SetApplicationSetting("diagnostics", "last-dump-load-retry", "");
        SetApplicationSetting("diagnostics", "last-dump-retry-time", "");
        SetApplicationSetting("diagnostics", "last-dump-complete", "");
    }
    else
    {
        // Upload failed - retry with backoff
        const int MAX_UPLOAD_RETRIES = 3;
        int       iCurrentRetry = GetApplicationSettingInt("diagnostics", "last-dump-load-retry");

        if (iCurrentRetry < MAX_UPLOAD_RETRIES)
        {
            iCurrentRetry++;
            SetApplicationSettingInt("diagnostics", "last-dump-load-retry", iCurrentRetry);

            // Store timestamp as string for backoff enforcement (prevents int overflow)
            const long long llCurrentTimeSec = GetTickCount64_() / 1000;
            SetApplicationSetting("diagnostics", "last-dump-retry-time", SString("%lld", llCurrentTimeSec));

            // Calculate backoff delay
            int iBackoffSeconds = 5;
            for (int i = 1; i < iCurrentRetry; i++)
                iBackoffSeconds *= 3;

            AddReportLog(9320, SString("Program_SendCrashDump: Upload failed (attempt %d/%d) - Will retry in %d seconds", iCurrentRetry, MAX_UPLOAD_RETRIES,
                                       iBackoffSeconds));

            // Reset flag to allow retry on next pulse
            m_bSentCrashDump = false;
        }
        else
        {
            // Max retries exceeded - give up
            AddReportLog(9321, SString("Program_SendCrashDump: Upload failed after %d attempts - Giving up", MAX_UPLOAD_RETRIES));
            _ResetLastCrashDump();
        }
    }
    return;

end:
    // Only reset if we're not retrying
    int iRetryCount = GetApplicationSettingInt("diagnostics", "last-dump-load-retry");
    if (iRetryCount == 0)
    {
        const SString strDumpSave = GetApplicationSetting("diagnostics", "last-dump-save");
        const SString strDumpComplete = GetApplicationSetting("diagnostics", "last-dump-complete");

        if (strDumpSave.length() > 0 && strDumpComplete != "1")
        {
        }
        else
        {
            _ResetLastCrashDump();
        }
    }
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_SendReportLog
//
// SendReportLog
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_SendReportLog()
{
    _UseReportLogURLs();         // Use REPORT_LOG_URL*
    _UseReportLogPostContent();  // Use report log source
    _StartSendPost();            // Send data
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_SidegradeLaunch
//
// SidegradeLaunch
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_SidegradeLaunch()
{
    _CheckSidegradeRequirements();  // Check if other version already installed
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.!installed"))
        goto notinstalled;             // Other version present and valid?
    _DialogSidegradeLaunchQuestion();  // Does user want to launch and connect using the other version?
    if (m_ConditionMap.IsConditionTrue("QuestionResponse.!Yes"))
        goto nolaunch;
    _DoSidegradeLaunch();  // If user says 'Yes', then launch
nolaunch:
    return;

notinstalled:
    _UseSidegradeURLs();  // Use sidegrade URLs
    _StartDownload();     // Fetch file info from server
    _ProcessPatchFileQuery();
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.update"))
        goto hasfile;              // Does server have the required file?
    _DialogSidegradeQueryError();  // If no download available, show message
    return;

hasfile:
    _DialogSidegradeDownloadQuestion();  // If it is downloadable, ask user what to do
    if (m_ConditionMap.IsConditionTrue("QuestionResponse.Yes"))
        goto yesdownload;
    return;  // If user says 'No', then finish

yesdownload:
    _DialogDownloading();  // Show "Downloading..." message
    _UseProvidedURLs();
    _StartDownload();  // Fetch file binary from mirror
    _ProcessPatchFileDownload();
    _DialogExeFilesResult();  // Show "ok/failed" message
    return;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::Program_NewsUpdate
//
// NewsUpdate
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::Program_NewsUpdate()
{
    _UseNewsUpdateURLs();  // Use news serverlist
    _StartDownload();      // Fetch update info from update server
    _ProcessPatchFileQuery();
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.silent"))
        goto silentdload;  // If update server says 'silent' then goto silentdload:
    if (m_ConditionMap.IsConditionTrue("ProcessResponse.noupdate"))
        goto noupdate;  // If update server says 'noupdate' then goto noupdate:
    return;

silentdload:
    _UseProvidedURLs();
    _StartDownload();  // Fetch update binary from update mirror
    _ProcessPatchFileDownload();
    _QUpdateNewsResult();  // Maybe update news install queue
    return;

noupdate:
    _ResetNewsCheckTimer();  // Wait interval before checking again
    return;
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
void CVersionUpdater::_CheckSidegradeRequirements()
{
    // Is version allowed to be launched?

    // Find reg settings for other version
    m_strSidegradePath = GetVersionRegistryValue(m_strSidegradeVersion, "", "Last Run Path");
    SString strLaunchHash = GetVersionRegistryValue(m_strSidegradeVersion, "", "Last Run Path Hash");
    SString strLaunchVersion = GetVersionRegistryValue(m_strSidegradeVersion, "", "Last Run Path Version");

    // Validate
    bool bVersionMatch = (strLaunchVersion == m_strSidegradeVersion);
    bool bLaunchPathValid = (strLaunchHash == CMD5Hasher::CalculateHexString(m_strSidegradePath));

    if (bVersionMatch && bLaunchPathValid)
    {
        // Check core.dll version in case user has installed over different major version
        SString         strCoreDll = PathJoin(ExtractPath(m_strSidegradePath), "mta", "core.dll");
        SLibVersionInfo versionInfo;
        if (GetLibVersionInfo(strCoreDll, &versionInfo))
        {
            SString strVersion("%d.%d", versionInfo.dwProductVersionMS >> 16, versionInfo.dwProductVersionMS & 0xffff);
            if (strVersion != m_strSidegradeVersion)
                bVersionMatch = false;
        }
        else
            bLaunchPathValid = false;
    }

    if (bVersionMatch && bLaunchPathValid)
    {
        m_ConditionMap.SetCondition("ProcessResponse", "installed");
    }
    else
    {
        m_ConditionMap.SetCondition("ProcessResponse", "notinstalled");
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DoSidegradeLaunch
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DoSidegradeLaunch()
{
    SString strURL("mtasa://%s:%s@%s", *m_strSidegradeName, *m_strSidegradePassword, *m_strSidegradeHost);
    SetOnQuitCommand("open", m_strSidegradePath, strURL);
    _ExitGame();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogSidegradeDownloadQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogSidegradeDownloadQuestion()
{
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(SString(_("MTA:SA %s required"), *m_strSidegradeVersion));
    GetQuestionBox().SetMessage(SString(_("An updated version of MTA:SA %s is required to join the selected server.\n\n"
                                          "Do you want to download and install MTA:SA %s ?"),
                                        *m_strSidegradeVersion, *m_strSidegradeVersion));
    GetQuestionBox().SetButton(0, _("No"));
    GetQuestionBox().SetButton(1, _("Yes"));
    GetQuestionBox().Show();
    _PollQuestionNoYes();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogSidegradeLaunchQuestion
//
// If can launch other version, ask for user confirmation
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogSidegradeLaunchQuestion()
{
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(SString(_("MTA:SA %s required"), *m_strSidegradeVersion));
    GetQuestionBox().SetMessage(SString(_("Do you want to launch MTA:SA %s and connect to this server ?"), *m_strSidegradeVersion));
    GetQuestionBox().SetButton(0, _("No"));
    GetQuestionBox().SetButton(1, _("Yes"));
    GetQuestionBox().Show();
    _PollQuestionNoYes();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogSidegradeQueryError
//
// No download file for other version
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogSidegradeQueryError()
{
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(SString(_("MTA:SA %s required"), *m_strSidegradeVersion));
    GetQuestionBox().SetMessage(_("It is not possible to connect at this time.\n\nPlease try later."));
    GetQuestionBox().SetButton(0, _("OK"));
    GetQuestionBox().Show();
    _PollAnyButton();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ActionReconnect
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ActionReconnect()
{
    CCommandFuncs::Reconnect(NULL);
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
    GetQuestionBox().Reset();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_QuitCurrentProgram
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_QuitCurrentProgram()
{
    throw ExceptionQuitProgram();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ExitGame
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ExitGame()
{
    shared.m_bExitGame = true;
    _QuitCurrentProgram();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetVersionCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetVersionCheckTimer()
{
    OutputDebugLine(SStringX("[Updater] _ResetVersionCheckTimer"));
    m_VarConfig.version_lastCheckTime = CDateTime::Now();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetNewsCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetNewsCheckTimer()
{
    OutputDebugLine(SStringX("[Updater] _ResetNewsCheckTimer"));
    m_VarConfig.news_lastCheckTime = CDateTime::Now();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetMasterCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetMasterCheckTimer()
{
    m_VarConfig.master_lastCheckTime = CDateTime::Now();
    CCore::GetSingleton().SaveConfig();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetManualCheckTimer
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetManualCheckTimer()
{
    // Reset delay before recheck allowed
    m_llTimeLastManualCheck = GetTickCount64_();
    CVARS_GET("update_build_type", m_strLastManualCheckBuildType);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollAnyButton
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollAnyButton()
{
    while (true)
    {
        UpdaterYield();

        // Abort if external force has closed the question box
        if (!GetQuestionBox().IsVisible())
            _QuitCurrentProgram();

        // Wait for button press before continuing
        if (GetQuestionBox().PollButtons() != BUTTON_NONE)
        {
            GetQuestionBox().Reset();
            return;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollQuestionNoYes
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_PollQuestionNoYes()
{
    while (true)
    {
        UpdaterYield();

        // Abort if external force has closed the question box
        if (!GetQuestionBox().IsVisible())
            _QuitCurrentProgram();

        switch (GetQuestionBox().PollButtons())
        {
            case BUTTON_NONE:
                break;

            case BUTTON_0:
                m_ConditionMap.SetCondition("QuestionResponse", "No");
                GetQuestionBox().Reset();
                return;

            case BUTTON_1:
                m_ConditionMap.SetCondition("QuestionResponse", "Yes");
                GetQuestionBox().Reset();
                return;

            default:
                assert(0);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogConnectingWait
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogConnectingWait()
{
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("Connecting"));
    GetQuestionBox().SetMessage(_("Please wait..."));
    GetQuestionBox().SetButton(0, _("Cancel"));
    GetQuestionBox().Show();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogChecking
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogChecking()
{
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("CHECKING"));
    GetQuestionBox().SetMessage(_("Please wait..."));
    GetQuestionBox().SetButton(0, _("Cancel"));
    GetQuestionBox().Show();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogNoUpdate
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogNoUpdate()
{
    // Display message
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("UPDATE CHECK"));
    GetQuestionBox().SetMessage(_("No update needed"));
    GetQuestionBox().SetButton(0, _("OK"));
    GetQuestionBox().Show();
    _PollAnyButton();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDownloading
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDownloading()
{
    // Display dialog
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("DOWNLOADING"));
    GetQuestionBox().SetMessage(_("waiting..."));
    GetQuestionBox().SetButton(0, _("Cancel"));
    GetQuestionBox().Show();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogServerSaysUpdateQuestion
//
// ServerSaysUpdateQuestion
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogServerSaysUpdateQuestion()
{
    // Display message
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("MANDATORY UPDATE"));
    GetQuestionBox().SetMessage(_("To join this server, you must update MTA.\n\n Do you want to update now ?"));
    GetQuestionBox().SetButton(0, _("No"));
    GetQuestionBox().SetButton(1, _("Yes"));
    GetQuestionBox().Show();
    _PollQuestionNoYes();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogServerSaysRecommendQuestion
//
// ServerSaysRecommendQuestion
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogServerSaysRecommendQuestion()
{
    // Display message
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("OPTIONAL UPDATE"));
    GetQuestionBox().SetMessage(_("Server says an update is recommended, but not essential.\n\n Do you want to update now ?"));
    GetQuestionBox().SetButton(0, _("No"));
    GetQuestionBox().SetButton(1, _("Yes"));
    GetQuestionBox().Show();
    _PollQuestionNoYes();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateQuestion
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateQuestion()
{
    // Display message
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(m_JobInfo.strTitle);
    GetQuestionBox().SetMessage(m_JobInfo.strMsg);
    GetQuestionBox().SetButton(0, m_JobInfo.strNo);
    GetQuestionBox().SetButton(1, m_JobInfo.strYes);
    GetQuestionBox().Show();
    GetQuestionBox().SetAutoCloseOnConnect(true);
    _PollQuestionNoYes();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateQueryError
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateQueryError()
{
    // Display message
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("UPDATE CHECK"));
    GetQuestionBox().SetMessage(_("An update is currently not available.\n\nPlease check www.multitheftauto.com"));
    GetQuestionBox().SetButton(0, _("OK"));
    GetQuestionBox().Show();
    _PollAnyButton();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogUpdateResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogUpdateResult()
{
    if (m_ConditionMap.IsConditionTrue("Download.Fail"))
    {
        // Handle failure
        if (m_ConditionMap.IsConditionTrue("Download.Fail.Saving"))
        {
            GetQuestionBox().Reset();
            GetQuestionBox().SetTitle(_("ERROR SAVING"));
            GetQuestionBox().SetMessage(_("Unable to create the file."));
            GetQuestionBox().SetButton(0, _("OK"));
            GetQuestionBox().Show();
            _PollAnyButton();
        }
        else if (m_ConditionMap.IsConditionTrue("Download.Fail.Checksum"))
        {
            GetQuestionBox().Reset();
            GetQuestionBox().SetTitle(_("ERROR DOWNLOADING"));
            GetQuestionBox().SetMessage(_("The downloaded file appears to be incorrect."));
            GetQuestionBox().SetOnLineHelpOption(SString("dl-incorrect&fname=%s", *m_JobInfo.strFilename));
            GetQuestionBox().Show();
            _PollAnyButton();
        }
        else
        {
            GetQuestionBox().Reset();
            GetQuestionBox().SetTitle(_("ERROR DOWNLOADING"));
            GetQuestionBox().SetMessage(_("For some reason."));
            GetQuestionBox().SetButton(0, _("OK"));
            GetQuestionBox().Show();
            _PollAnyButton();
        }
    }
    else if (m_ConditionMap.IsConditionTrue("Download.Ok"))
    {
        // Handle success
        // Exit game after button press
        GetQuestionBox().Reset();
        GetQuestionBox().SetTitle(_("DOWNLOAD COMPLETE"));
        GetQuestionBox().SetMessage(m_JobInfo.strMsg2);
        GetQuestionBox().SetButton(0, _("OK"));
        GetQuestionBox().Show();

        if (m_JobInfo.strStatus == "update")
        {
            // 'update' - Stand alone installer
            SetOnQuitCommand("open", m_JobInfo.strSaveLocation);
            SetOnRestartCommand("");
            _PollAnyButton();
            _ExitGame();
        }
        else if (m_JobInfo.strStatus == "files" || m_JobInfo.strStatus == "silent")
        {
            // 'files'/'silent' - Self extracting archive
            SetOnQuitCommand("restart");
            SetOnRestartCommand("files", m_JobInfo.strSaveLocation, m_JobInfo.strParameters);
            _PollAnyButton();
            SetPostUpdateConnect(m_strServerSaysHost);
            _ExitGame();
        }
        else
        {
            GetQuestionBox().SetMessage(m_JobInfo.strStatus + _(" - Unknown problem in _DialogUpdateResult"));
            _PollAnyButton();
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_QUpdateResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_QUpdateResult()
{
    if (m_ConditionMap.IsConditionTrue("Download.Ok"))
    {
        // Handle success
        if (m_JobInfo.strStatus == "silent")
        {
            // 'silent' - Self extracting archive
            SetOnRestartCommand("silent", m_JobInfo.strSaveLocation, m_JobInfo.strParameters);
        }
#if MTA_DEBUG
        else
            MessageBoxUTF8(NULL, "_QUpdateResult m_JobInfo.strStatus != 'silent'", m_JobInfo.strStatus, MB_OK);
#endif
    }
#if MTA_DEBUG
    else
        MessageBoxUTF8(NULL, "_QUpdateResult Download.!Ok", "Error", MB_OK);
#endif
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_QUpdateNewsResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_QUpdateNewsResult()
{
    if (m_ConditionMap.IsConditionTrue("Download.Ok"))
    {
        // Handle success
        if (m_JobInfo.strStatus == "silent")
        {
            // Calc news date from filename
            SString strDate = m_JobInfo.strFilename.SplitRight("-").SplitLeft(".");

            if (strDate.length() == 10)
            {
                // Add to install news queue
                CArgMap queue;
                queue.SetFromString(GetApplicationSetting("news-install"));
                queue.Set(strDate, m_JobInfo.strSaveLocation);
                SetApplicationSetting("news-install", queue.ToString());

                // Set last news date
                m_VarConfig.news_lastNewsDate = strDate;
                CCore::GetSingleton().SaveConfig();

                // See if there is more news after this one
                m_bCheckedTimeForNewsUpdate = false;
            }
        }
#if MTA_DEBUG
        else
            MessageBoxUTF8(NULL, "_QUpdateNewsResult m_JobInfo.strStatus != 'silent'", m_JobInfo.strStatus, MB_OK);
#endif
    }
#if MTA_DEBUG
    else
        MessageBoxUTF8(NULL, "_QUpdateNewsResult Download.!Ok", "Error", MB_OK);
#endif
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogDataFilesQuestion
//
// Just displays information these days
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogDataFilesQuestion()
{
    // If using customized SA files, advise user to stop using customized SA files
    if (GetApplicationSettingInt("customized-sa-files-using"))
    {
        GetQuestionBox().Reset();
        GetQuestionBox().SetTitle(_("CUSTOMIZED GTA:SA FILES"));
        SString strMessage;
        strMessage += "This server is blocking your customized GTA:SA files.";
        strMessage += "\n\nTo join this server please uncheck:";
        strMessage += "\nSettings->Multiplayer->Use customized GTA:SA files";
        strMessage += "\n\n";
        GetQuestionBox().SetMessage(_(strMessage));
        GetQuestionBox().SetButton(0, _("Ok"));
        GetQuestionBox().Show();
        _PollAnyButton();
        return;
    }

    // Display message
    GetQuestionBox().Reset();
    GetQuestionBox().SetTitle(_("ERROR"));
    GetQuestionBox().SetMessage(_("Some MTA:SA data files are missing.\n\n\nPlease reinstall MTA:SA"));
    GetQuestionBox().SetButton(0, _("Ok"));
    GetQuestionBox().Show();
    _PollAnyButton();
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_DialogExeFilesResult
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_DialogExeFilesResult()
{
    // Handle failure
    if (m_ConditionMap.IsConditionTrue("Download.Fail"))
    {
        if (m_ConditionMap.IsConditionTrue("Download.Fail.Saving"))
        {
            GetQuestionBox().Reset();
            GetQuestionBox().SetTitle(_("ERROR SAVING"));
            GetQuestionBox().SetMessage(_("Unable to create the file."));
            GetQuestionBox().SetButton(0, _("OK"));
            GetQuestionBox().Show();
            _PollAnyButton();
        }
        else if (m_ConditionMap.IsConditionTrue("Download.Fail.Checksum"))
        {
            GetQuestionBox().Reset();
            GetQuestionBox().SetTitle(_("ERROR DOWNLOADING"));
            GetQuestionBox().SetMessage(_("The downloaded file appears to be incorrect."));
            GetQuestionBox().SetButton(0, _("OK"));
            GetQuestionBox().Show();
            _PollAnyButton();
        }
        else
        {
            GetQuestionBox().Reset();
            GetQuestionBox().SetTitle(_("ERROR DOWNLOADING"));
            GetQuestionBox().SetMessage(_("For some reason."));
            GetQuestionBox().SetButton(0, _("OK"));
            GetQuestionBox().Show();
            _PollAnyButton();
        }
    }
    else if (m_ConditionMap.IsConditionTrue("Download.Ok"))
    {
        // Handle success
        SetOnQuitCommand("open", m_JobInfo.strSaveLocation);

        // Exit game after button press
        GetQuestionBox().Reset();
        GetQuestionBox().SetTitle(_("DOWNLOAD COMPLETE"));
        GetQuestionBox().SetMessage(m_JobInfo.strMsg2);
        GetQuestionBox().SetButton(0, _("OK"));
        GetQuestionBox().Show();
        _PollAnyButton();
        _ExitGame();
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
void CVersionUpdater::_UseVersionQueryURLs()
{
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList = MakeServerList(m_MasterConfig.version.serverInfoMap);
    m_JobInfo.bShowDownloadPercent = false;
}

/**
 * @brief Extracts the revision from an update file name.
 * @param fileName Name of the file
 * @param revision Revision of the update
 */
static bool GetRevisionFromFileName(std::string_view fileName, std::uint32_t& revision)
{
    revision = {};

    // Example:                          mtasa-1.5.9-rc-21519-0-000-files-all-cksummed.rar
    // Search for the delimiter before the revision ^^^^.
    if (size_t revisionStart = fileName.find("-rc-"); revisionStart != std::string_view::npos)
    {
        revisionStart += 4;

        // Example:                mtasa-1.5.9-rc-21519-0-000-files-all-cksummed.rar
        // Search for the delimiter after the revision ^.
        if (size_t revisionStop = fileName.find('-', revisionStart); revisionStop != std::string_view::npos)
        {
            std::string_view raw = fileName.substr(revisionStart, revisionStop - revisionStart);

            if (auto [ptr, ec] = std::from_chars(raw.data(), raw.data() + raw.size(), revision); ec == std::errc{})
                return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessPatchFileQuery
//
// Called after getting the version check info from the server
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessPatchFileQuery()
{
    m_ConditionMap.SetCondition("ProcessResponse", "");
    m_ConditionMap.SetCondition("Download", "");

    if (m_JobInfo.downloadBuffer.size() == 0)
    {
        OutputDebugLine("[Error] Empty download buffer");
        return;
    }

    CXMLBuffer XMLBuffer;
    CXMLNode*  pRoot = XMLBuffer.SetFromData(&m_JobInfo.downloadBuffer[0], m_JobInfo.downloadBuffer.size());

    // Try to parse download info
    m_JobInfo = SJobInfo();
    SString strReportSettingsFilter;
    SString strReportSettingsMin;
    SString strReportSettingsMax;
    SString strNotifyMasterRevision;

    CXMLAccess XMLAccess(XMLBuffer.m_pRoot);
    XMLAccess.GetSubNodeValue("status", m_JobInfo.strStatus);
    XMLAccess.GetSubNodeValue("parameters", m_JobInfo.strParameters);
    XMLAccess.GetSubNodeValue("priority", m_JobInfo.strPriority);
    XMLAccess.GetSubNodeValue("dialog.title", m_JobInfo.strTitle);
    XMLAccess.GetSubNodeValue("dialog.msg", m_JobInfo.strMsg);
    XMLAccess.GetSubNodeValue("dialog.msg2", m_JobInfo.strMsg2);
    XMLAccess.GetSubNodeValue("dialog.yes", m_JobInfo.strYes);
    XMLAccess.GetSubNodeValue("dialog.no", m_JobInfo.strNo);
    XMLAccess.GetSubNodeValue("file.name", m_JobInfo.exe.strFilename);
    XMLAccess.GetSubNodeValue("file.size", m_JobInfo.exe.iFilesize);
    XMLAccess.GetSubNodeValue("file.md5", m_JobInfo.exe.strMD5);
    XMLAccess.GetSubNodeValue("serverlist", m_JobInfo.exe.serverInfoMap);
    XMLAccess.GetSubNodeValue("file_sig.name", m_JobInfo.sig.strFilename);
    XMLAccess.GetSubNodeValue("file_sig.size", m_JobInfo.sig.iFilesize);
    XMLAccess.GetSubNodeValue("file_sig.md5", m_JobInfo.sig.strMD5);
    XMLAccess.GetSubNodeValue("serverlist_sig", m_JobInfo.sig.serverInfoMap);
    XMLAccess.GetSubNodeValue("file_rar.name", m_JobInfo.rar.strFilename);
    XMLAccess.GetSubNodeValue("file_rar.size", m_JobInfo.rar.iFilesize);
    XMLAccess.GetSubNodeValue("file_rar.md5", m_JobInfo.rar.strMD5);
    XMLAccess.GetSubNodeValue("serverlist_rar", m_JobInfo.rar.serverInfoMap);
    XMLAccess.GetSubNodeValue("file_slim.name", m_JobInfo.slim.strFilename);
    XMLAccess.GetSubNodeValue("file_slim.size", m_JobInfo.slim.iFilesize);
    XMLAccess.GetSubNodeValue("file_slim.md5", m_JobInfo.slim.strMD5);
    XMLAccess.GetSubNodeValue("serverlist_slim", m_JobInfo.slim.serverInfoMap);
    XMLAccess.GetSubNodeValue("dependencies_slim", m_JobInfo.slim.dependencyInfoMap);
    XMLAccess.GetSubNodeValue("reportsettings.filter2", strReportSettingsFilter);
    XMLAccess.GetSubNodeValue("reportsettings.minsize", strReportSettingsMin);
    XMLAccess.GetSubNodeValue("reportsettings.maxsize", strReportSettingsMax);
    XMLAccess.GetSubNodeValue("notifymasterrevision", strNotifyMasterRevision);

    // Use rar info if present
    if (!m_JobInfo.rar.strFilename.empty())
    {
        m_JobInfo.strFilename = m_JobInfo.rar.strFilename;
        m_JobInfo.iFilesize = m_JobInfo.rar.iFilesize;
        m_JobInfo.strMD5 = m_JobInfo.rar.strMD5;
        m_JobInfo.serverInfoMap = m_JobInfo.rar.serverInfoMap;
    }
    else if (!m_JobInfo.sig.strFilename.empty())
    {
        // Otherwise Authenticode signed exe
        m_JobInfo.strFilename = m_JobInfo.sig.strFilename;
        m_JobInfo.iFilesize = m_JobInfo.sig.iFilesize;
        m_JobInfo.strMD5 = m_JobInfo.sig.strMD5;
        m_JobInfo.serverInfoMap = m_JobInfo.sig.serverInfoMap;
    }
    else
    {
        // Finally, unsigned exe
        m_JobInfo.strFilename = m_JobInfo.exe.strFilename;
        m_JobInfo.iFilesize = m_JobInfo.exe.iFilesize;
        m_JobInfo.strMD5 = m_JobInfo.exe.strMD5;
        m_JobInfo.serverInfoMap = m_JobInfo.exe.serverInfoMap;
    }

    // Change from 'silent' if user has disabled automatic install
    if (m_JobInfo.strStatus == "silent" && m_JobInfo.strParameters.Contains("allowuseroverride"))
    {
        if (CVARS_GET_VALUE<bool>("update_auto_install") == false)
        {
            m_JobInfo.strStatus = "files";
            m_JobInfo.strParameters = m_JobInfo.strParameters.Replace("hideprogress", "");
        }
    }

    // Process
    if (strNotifyMasterRevision.length() && strNotifyMasterRevision > m_MasterConfig.master.strRevision)
    {
        // Only do next bit when 'notify revision' increases (prevents superfluous downloads when notify revision is (incorrectly) higher than actual revision)
        if (m_VarConfig.master_highestNotifyRevision.empty() || strNotifyMasterRevision > m_VarConfig.master_highestNotifyRevision)
        {
            m_VarConfig.master_highestNotifyRevision = strNotifyMasterRevision;

            // If version check file says there is a new master config file, force update next time
            m_VarConfig.master_lastCheckTime.SetFromSeconds(0);
            if (m_JobInfo.strStatus == "noupdate")
                OnPossibleConfigProblem();  // Do it this time if no update here
        }
    }

    // If some sort of error read the file data, re-get the master config
    if (m_JobInfo.strStatus == "")
        OnPossibleConfigProblem();

    // Modify report settings if data here, and none in master config
    if (!strReportSettingsFilter.empty() && m_MasterConfig.report.strFilter.empty())
    {
        SString strReportSettings =
            SString("filter2@%s;min@%s;max@%s", strReportSettingsFilter.c_str(), strReportSettingsMin.c_str(), strReportSettingsMax.c_str());
        GetReportWrap()->SetSettings(strReportSettings);
    }

    // Indicate what the server wants us to do
    m_ConditionMap.SetCondition("ProcessResponse", m_JobInfo.strStatus);

    // Report update response
    if (!m_JobInfo.strStatus.empty() && m_JobInfo.strStatus != "noupdate" && !_strnicmp("mtasa-", m_JobInfo.strFilename.c_str(), 6))
    {
        uint32_t revision{};

        if (GetRevisionFromFileName(m_JobInfo.strFilename, revision) && revision < MTASA_VERSION_BUILD)
        {
            unsigned short netRev = CCore::GetSingleton().GetNetwork()->GetNetRev();
            unsigned short netRel = CCore::GetSingleton().GetNetwork()->GetNetRel();

            SString playerVersion("%d.%d.%d-%d.%05d.%d.%03d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE,
                                  MTASA_VERSION_BUILD, netRev, netRel);

            AddReportLog(5061, SString("Processing patch file '%s' [%s, %s] with '%s' (version: %s) from '%s'", m_JobInfo.strFilename.c_str(),
                                       m_JobInfo.iFilesize.ToString().c_str(), m_JobInfo.strMD5.c_str(), m_JobInfo.strStatus.c_str(), playerVersion.c_str(),
                                       m_strLastQueryURL.c_str()));
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseMasterFetchURLs
//
// Called before starting the master xml fetch
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseMasterFetchURLs()
{
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList.push_back(UPDATER_MASTER_URL1);
    m_JobInfo.serverList.push_back(UPDATER_MASTER_URL2);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessMasterFetch
//
// Called after completing master xml fetch
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessMasterFetch()
{
    m_ConditionMap.SetCondition("ProcessResponse", "");
    m_ConditionMap.SetCondition("Download", "");

    if (m_JobInfo.downloadBuffer.size() == 0)
        return;

    CXMLBuffer XMLBuffer;
    CXMLNode*  pRoot = XMLBuffer.SetFromData(&m_JobInfo.downloadBuffer[0], m_JobInfo.downloadBuffer.size());

    if (!pRoot)
        return;

    // Try to parse download info
    m_JobInfo = SJobInfo();

    CXMLAccess XMLAccess(XMLBuffer.m_pRoot);
    SString    strResetVar;
    XMLAccess.GetSubNodeValue("resetvar", strResetVar);
    SUpdaterMasterConfig newMasterConfig;
    XMLAccess.GetSubNodeValue("master.revision", newMasterConfig.master.strRevision);
    XMLAccess.GetSubNodeValue("master.serverlist", newMasterConfig.master.serverInfoMap);
    XMLAccess.GetSubNodeValue("master.interval", newMasterConfig.master.interval);
    XMLAccess.GetSubNodeValue("version.serverlist", newMasterConfig.version.serverInfoMap);
    XMLAccess.GetSubNodeValue("version.interval", newMasterConfig.version.interval);
    XMLAccess.GetSubNodeValue("report.serverlist", newMasterConfig.report.serverInfoMap);
    XMLAccess.GetSubNodeValue("report.interval", newMasterConfig.report.interval);
    XMLAccess.GetSubNodeValue("report.filter2", newMasterConfig.report.strFilter);
    XMLAccess.GetSubNodeValue("report.minsize", newMasterConfig.report.iMinSize);
    XMLAccess.GetSubNodeValue("report.maxsize", newMasterConfig.report.iMaxSize);
    XMLAccess.GetSubNodeValue("crashdump.serverlist", newMasterConfig.crashdump.serverInfoMap);
    XMLAccess.GetSubNodeValue("crashdump.duplicates", newMasterConfig.crashdump.iDuplicates);
    XMLAccess.GetSubNodeValue("crashdump.maxhistorylength", newMasterConfig.crashdump.iMaxHistoryLength);
    XMLAccess.GetSubNodeValue("trouble.serverlist", newMasterConfig.trouble.serverInfoMap);
    XMLAccess.GetSubNodeValue("ase.serverlist", newMasterConfig.ase.serverInfoMap);
    XMLAccess.GetSubNodeValue("sidegrade.serverlist", newMasterConfig.sidegrade.serverInfoMap);
    XMLAccess.GetSubNodeValue("sidegrade.nobrowselist", newMasterConfig.sidegrade.nobrowseInfoMap);
    XMLAccess.GetSubNodeValue("sidegrade.onlybrowselist", newMasterConfig.sidegrade.onlybrowseInfoMap);
    XMLAccess.GetSubNodeValue("news.serverlist", newMasterConfig.news.serverInfoMap);
    XMLAccess.GetSubNodeValue("news.interval", newMasterConfig.news.interval);
    XMLAccess.GetSubNodeValue("news.oldestpost", newMasterConfig.news.strOldestPost);
    XMLAccess.GetSubNodeValue("news.maxhistorylength", newMasterConfig.news.iMaxHistoryLength);
    XMLAccess.GetSubNodeValue("misc.debug.filter2", newMasterConfig.misc.debug.strFilter);

    if (newMasterConfig.IsValid())
    {
        if (strResetVar == "yes")
            m_VarConfig = SUpdaterVarConfig();

        m_MasterConfig = newMasterConfig;
        PostChangeMasterConfig();
        m_ConditionMap.SetCondition("ProcessResponse", "ok");
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::PostChangeMasterConfig
//
// Transfer certain settings from the master config
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::PostChangeMasterConfig()
{
    // Update trouble url for far away modules
    std::vector<SString> serverList = MakeServerList(m_MasterConfig.trouble.serverInfoMap);
    if (serverList.size() && serverList[0].length() > 4)
        SetApplicationSetting("trouble-url", serverList[0]);

    // Update debug filter for far away modules
    SetApplicationSetting("debugfilter", m_MasterConfig.misc.debug.strFilter);

    // Maybe modify report settings
    if (!m_MasterConfig.report.strFilter.empty())
    {
        SString strReportSettings =
            SString("filter2@%s;min@%d;max@%d", *m_MasterConfig.report.strFilter, (int)m_MasterConfig.report.iMinSize, (int)m_MasterConfig.report.iMaxSize);
        GetReportWrap()->SetSettings(strReportSettings);
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::GetNewsSettings
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::GetNewsSettings(SString& strOutOldestPost, uint& uiOutMaxHistoryLength)
{
    EnsureLoadedConfigFromXML();
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
void CVersionUpdater::_UseSidegradeURLs()
{
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList = MakeServerList(m_MasterConfig.sidegrade.serverInfoMap);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseNewsUpdateURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseNewsUpdateURLs()
{
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList = MakeServerList(m_MasterConfig.news.serverInfoMap);
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
void CVersionUpdater::_UseProvidedURLs()
{
    if (m_JobInfo.slim.iFilesize > 0)
    {
        // Check if files not included in the smaller archive are already installed
        bool bUseSlim = true;
        for (uint i = 0; i < m_JobInfo.slim.dependencyInfoMap.size(); i++)
        {
            const SDataInfoItem& dependencyItem = m_JobInfo.slim.dependencyInfoMap[i];
            if (dependencyItem.strName == "dependency")
            {
                SString strRequiredMd5 = dependencyItem.GetAttribute("md5");
                SString strFilename = dependencyItem.strValue;
                if (strRequiredMd5.length() == 32)
                {
                    SString strPathFilename = CalcMTASAPath(strFilename);
                    SString strActualMd5 = GenerateHashHexStringFromFile(EHashFunctionType::MD5, strPathFilename);
                    if (strRequiredMd5.CompareI(strActualMd5) == false)
                    {
                        bUseSlim = false;
                        break;
                    }
                }
            }
        }

        if (bUseSlim)
        {
            // We can use the smaller archive
            m_JobInfo.strFilename = m_JobInfo.slim.strFilename;
            m_JobInfo.iFilesize = m_JobInfo.slim.iFilesize;
            m_JobInfo.strMD5 = m_JobInfo.slim.strMD5;
            m_JobInfo.serverInfoMap = m_JobInfo.slim.serverInfoMap;
        }
        m_JobInfo.slim.iFilesize = 0;
    }

    CDataInfoSet serverInfoMap = m_JobInfo.serverInfoMap;
    m_JobInfo.serverList = MakeServerList(serverInfoMap);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessPatchFileDownload
//
// Called after completing a file download
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessPatchFileDownload()
{
    // Check if the saved filename has already been set
    if (!m_JobInfo.strSaveLocation.empty())
        return;

    m_ConditionMap.SetCondition("ProcessResponse", "");
    m_ConditionMap.SetCondition("Download", "");

    if (m_JobInfo.downloadBuffer.size() == 0)
        return;

    unsigned int uiSize = m_JobInfo.downloadBuffer.size();
    char*        pData = &m_JobInfo.downloadBuffer[0];

    // Check MD5
    if (m_JobInfo.strMD5 != "none")
    {
        // Hash data
        MD5 md5Result;
        CMD5Hasher().Calculate(pData, uiSize, md5Result);
        char szMD5[33];
        CMD5Hasher::ConvertToHex(md5Result, szMD5);

        if (m_JobInfo.strMD5 != szMD5)
        {
            // If MD5 error, remove this server from the list
            if (m_JobInfo.serverList.size())
                ListRemoveIndex(m_JobInfo.serverList, m_JobInfo.iCurrent--);
            m_ConditionMap.SetCondition("Download", "Fail", "Checksum");
            AddReportLog(5003, SString("DoPollDownload: Checksum wrong for %s (Want:%d-%s Got:%d-%s)", m_JobInfo.strFilename.c_str(), (int)m_JobInfo.iFilesize,
                                       m_JobInfo.strMD5.c_str(), uiSize, szMD5));
            return;
        }
    }

    // Check signature
    if (!CCore::GetSingleton().GetNetwork()->VerifySignature(pData, uiSize))
    {
        AddReportLog(5006, SString("DoPollDownload: Signature wrong for %s (MD5: %s)", m_JobInfo.strFilename.c_str(), m_JobInfo.strMD5.c_str()));
        m_ConditionMap.SetCondition("Download", "Fail", "Checksum");
        return;
    }

    ////////////////////////
    // Save file somewhere
    SString strPathFilename;
    {
        // Get a list of possible places to save the file
        std::list<SString> saveLocationList;
        GetSaveLocationList(saveLocationList, m_JobInfo.strFilename);

        // Try each place
        for (std::list<SString>::iterator iter = saveLocationList.begin(); iter != saveLocationList.end(); ++iter)
        {
            const SString strSaveLocation = MakeUniquePath(*iter);
            MakeSureDirExists(strSaveLocation);
            if (FileSave(strSaveLocation, pData, uiSize))
            {
                strPathFilename = strSaveLocation;
                break;
            }

            AddReportLog(5004, SString("DoPollDownload: Unable to use the path %s", strSaveLocation.c_str()));
        }
    }

    if (strPathFilename.length() == 0)
    {
        if (m_JobInfo.serverList.size())
            ListRemoveIndex(m_JobInfo.serverList, m_JobInfo.iCurrent--);
        m_ConditionMap.SetCondition("Download", "Fail", "Saving");
        AddReportLog(5005, SString("DoPollDownload: Unable to save the file %s (size %d)", m_JobInfo.strFilename.c_str(), uiSize));
        return;
    }
    ////////////////////////

    m_JobInfo.strSaveLocation = strPathFilename;
    m_ConditionMap.SetCondition("Download", "Ok");
    AddReportLog(2007, SString("DoPollDownload: Downloaded %s", m_JobInfo.strSaveLocation.c_str()));
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartDownload
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_StartDownload()
{
    if (!m_JobInfo.strFilename.empty())
    {
        // See if file already exists in upcache
        std::list<SString> saveLocationList;
        GetSaveLocationList(saveLocationList, m_JobInfo.strFilename);

        // Try each place
        for (std::list<SString>::iterator iter = saveLocationList.begin(); iter != saveLocationList.end(); ++iter)
        {
            SString strPath, strFilename;
            ExtractFilename(*iter, &strPath, &strFilename);
            SString strMain;
            if (ExtractExtension(*iter, &strMain, NULL))
            {
                std::vector<SString> fileList = FindFiles(strMain + "*", true, false);

                for (std::vector<SString>::iterator iter = fileList.begin(); iter != fileList.end(); ++iter)
                {
                    // Check filesize and then md5 and then verify signature and then use that file
                    SString strPathFilename = PathJoin(strPath, *iter);

                    // Check filesize
                    if (FileSize(strPathFilename) != m_JobInfo.iFilesize)
                        continue;

                    CBuffer buffer;
                    buffer.LoadFromFile(strPathFilename);

                    // Check MD5
                    // Hash data
                    MD5 md5Result;
                    CMD5Hasher().Calculate(buffer.GetData(), buffer.GetSize(), md5Result);
                    char szMD5[33];
                    CMD5Hasher::ConvertToHex(md5Result, szMD5);
                    if (m_JobInfo.strMD5 != szMD5)
                    {
                        AddReportLog(5807, SString("StartDownload: Cached file reuse - Size correct, but md5 did not match (%s)", *strPathFilename));
                        continue;
                    }

                    // Check signature
                    if (!CCore::GetSingleton().GetNetwork()->VerifySignature(buffer.GetData(), buffer.GetSize()))
                    {
                        AddReportLog(5808, SString("StartDownload: Cached file reuse - Size and md5 correct, but signature incorrect (%s)", *strPathFilename));
                        continue;
                    }

                    // Reuse file now
                    m_JobInfo.strSaveLocation = strPathFilename;
                    m_ConditionMap.SetCondition("ProcessResponse", "");
                    m_ConditionMap.SetCondition("Download", "Ok");
                    AddReportLog(5809, SString("StartDownload: Cached file reuse - Size, md5 and signature correct (%s)", *strPathFilename));
                    return;
                }
            }
        }
    }

    while (true)
    {
        UpdaterYield();
        switch (DoSendDownloadRequestToNextServer())
        {
            case RES_FAIL:
                // Can't find any(more) servers to send a query to
                // Drop back to previous stack level

                // Refresh master config as well
                OnPossibleConfigProblem();
                return;

            case RES_OK:
            {
                // Query sent ok, now wait for response
                if (_PollDownload() == RES_OK)
                {
                    // Got a valid response or cancelled by user
                    return;
                }
                // Connection to current server failed, try next server
                break;
            }

            default:
                assert(0);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollDownload
//
//
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::_PollDownload()
{
    while (true)
    {
        UpdaterYield();
        switch (DoPollDownload())
        {
            case RES_FAIL:
                // Connection to current server failed, try next server
                OnPossibleConfigProblem();
                return RES_FAIL;

            case RES_OK:
                // Got a valid response
                return RES_OK;

            case RES_POLL:
                // Waiting...

                if (GetQuestionBox().IsVisible())
                {
                    // Handle progress/cancel if visible
                    if (GetQuestionBox().PollButtons() == BUTTON_0)
                    {
                        GetHTTP()->Reset();
                        GetQuestionBox().Reset();
                        m_ConditionMap.SetCondition("ProcessResponse", "cancel");
                        return RES_OK;
                    }
                    if (m_JobInfo.bShowDownloadPercent)
                    {
                        const bool bIsDownloadedSizeRight =
                            m_JobInfo.uiBytesDownloaded > 0 && static_cast<unsigned int>(m_JobInfo.iFilesize) >= m_JobInfo.uiBytesDownloaded;
                        const int fDownloadedPercent = bIsDownloadedSizeRight ? Round((float)m_JobInfo.uiBytesDownloaded / m_JobInfo.iFilesize * 100.f) : 0;
                        GetQuestionBox().SetMessage(SString(_("%3d %% completed"), fDownloadedPercent));
                    }
                    if (m_JobInfo.iIdleTime > 1000 && m_JobInfo.iIdleTimeLeft > 500)
                        GetQuestionBox().AppendMessage(SString(_("\n\nWaiting for response  -  %-3d"), m_JobInfo.iIdleTimeLeft / 1000));
                    else
                        GetQuestionBox().AppendMessage("");
                }

                break;

            default:
                assert(0);
        }
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
void CVersionUpdater::_UseReportLogURLs()
{
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList = MakeServerList(m_MasterConfig.report.serverInfoMap);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseReportLogPostContent
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseReportLogPostContent()
{
    // Remove unrequired items
#ifndef MTA_DEBUG
    GetReportWrap()->ClearLogContents(GetReportWrap()->GetFilter());
#endif
    // See if anything left to send
    SString strContents = GetReportWrap()->GetLogContents(GetReportWrap()->GetFilter(), GetReportWrap()->GetMaxSize());
    if ((int)strContents.length() < GetReportWrap()->GetMinSize())
        return;

    // Copy to post buffer
    const char* pSrc = strContents.c_str();
    uint        uiLength = strContents.length() + 1;

    m_JobInfo.postContent.assign(uiLength, 0);
    char* pDst = &m_JobInfo.postContent[0];
    memcpy(pDst, pSrc, uiLength);
    m_JobInfo.bPostContentBinary = false;

    // Wipe log here
    GetReportWrap()->ClearLogContents("-all");
}

int CVersionUpdater::ValidateCrashDumpPath(const SString& strPathFilename, SString* pOutErrorMsg, bool bCheckFileExists)
{
    // Constants for validation with constexpr for compile-time evaluation
    constexpr size_t MAX_PATH_LENGTH = 32767;
    constexpr size_t MAX_DUMP_FILE_SIZE = 2 * 1024 * 1024;
    constexpr size_t MAX_PATH_COMPONENTS = 256;
    constexpr size_t PATH_PARTS_RESERVE = 32;
    constexpr size_t MAX_DUMP_FILE_SIZE_MB = MAX_DUMP_FILE_SIZE / (1024 * 1024);

    if (const size_t pathLength = strPathFilename.length(); pathLength > MAX_PATH_LENGTH)
    {
        if (pOutErrorMsg)
            *pOutErrorMsg = SString("Path too long (%d > %d)", pathLength, MAX_PATH_LENGTH);
        return 9311;
    }

    if (!strPathFilename.EndsWithI(".dmp"))
    {
        if (pOutErrorMsg)
            *pOutErrorMsg = "Invalid file extension (must be .dmp)";
        return 9315;
    }

    auto validatePathParts = [&](const SString& separator) noexcept -> int
    {
        std::vector<SString> pathParts;
        pathParts.reserve(PATH_PARTS_RESERVE);
        strPathFilename.Split(separator, pathParts);

        if (const size_t numParts = pathParts.size(); numParts > MAX_PATH_COMPONENTS)
        {
            if (pOutErrorMsg)
                *pOutErrorMsg = SString("Too many path components (%d > %d)", numParts, MAX_PATH_COMPONENTS);
            return 9315;
        }

        for (const auto& part : pathParts)
        {
            if (part == "..")
            {
                if (pOutErrorMsg)
                    *pOutErrorMsg = "Path traversal detected";
                return 9312;
            }
        }
        return 0;
    };

    if (const int result = validatePathParts(PATH_SEPERATOR); result != 0)
        return result;

    if (const bool hasForwardSlash = strPathFilename.Contains("/"); hasForwardSlash)
    {
        if (const int result = validatePathParts("/"); result != 0)
            return result;
    }

    if (bCheckFileExists)
    {
        if (FILE* pFile = File::Fopen(strPathFilename, "rb"); !pFile)
        {
            if (pOutErrorMsg)
                *pOutErrorMsg = "File does not exist or cannot be opened";
            return 9317;
        }
        else
        {
            if (const int seekResult = fseek(pFile, 0, SEEK_END); seekResult != 0)
            {
                fclose(pFile);
                if (pOutErrorMsg)
                    *pOutErrorMsg = "Failed to seek in file";
                return 9317;
            }

            const long fileSize = ftell(pFile);
            fclose(pFile);

            if (const bool isInvalidSize = fileSize < 0 || static_cast<size_t>(fileSize) > MAX_DUMP_FILE_SIZE; isInvalidSize)
            {
                if (pOutErrorMsg)
                    *pOutErrorMsg = SString("File too large (%d bytes, max %d MB)", fileSize, MAX_DUMP_FILE_SIZE_MB);
                return 9316;
            }
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ShouldSendCrashDump
//
// Check if upload is required by client
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ShouldSendCrashDump()
{
    // constexpr string literals for diagnostics settings keys
    constexpr const char* DIAGNOSTICS_SECTION = "diagnostics";
    constexpr const char* LAST_DUMP_SAVE = "last-dump-save";
    constexpr const char* LAST_DUMP_TAG = "last-dump-tag";
    constexpr const char* LAST_DUMP_DATETIME = "last-dump-datetime";
    constexpr const char* PROCESS_RESPONSE = "ProcessResponse";
    constexpr const char* RESPONSE_OK = "ok";
    constexpr const char* EMPTY_VALUE = "";

    m_ConditionMap.SetCondition(PROCESS_RESPONSE, EMPTY_VALUE);

    if (const SString strPathFilename = GetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE); strPathFilename.empty())
    {
        return;
    }
    else
    {
        if (SString strErrorMsg; const int iValidationResult = ValidateCrashDumpPath(strPathFilename, &strErrorMsg, true))
        {
            AddReportLog(iValidationResult, SString("_ShouldSendCrashDump: %s", *strErrorMsg));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);  // Clear invalid path
            return;
        }

        SString strFilename;
        strPathFilename.Split(PATH_SEPERATOR, nullptr, &strFilename, -1);

        // constexpr validation constants for compile-time evaluation
        constexpr size_t MAX_FILENAME_LENGTH = 2048;
        constexpr size_t MAX_FILENAME_PARTS = 64;
        constexpr size_t PARTS_RESERVE = 8;
        constexpr size_t MAX_TAG_LENGTH = 1024;
        constexpr size_t MAX_DATETIME_LENGTH = 256;
        constexpr size_t MAX_HISTORY_SIZE = 100000;
        constexpr int    MAX_DUPLICATE_COUNT = 10000;

        if (const size_t filenameLength = strFilename.length(); filenameLength > MAX_FILENAME_LENGTH)
        {
            AddReportLog(9325, SString("_ShouldSendCrashDump: Filename too long (%d > %d)", filenameLength, MAX_FILENAME_LENGTH));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);
            return;
        }

        std::vector<SString> parts;
        parts.reserve(PARTS_RESERVE);
        strFilename.Split("_", parts);

        if (const size_t numParts = parts.size(); numParts > MAX_FILENAME_PARTS)
        {
            AddReportLog(9315, SString("_ShouldSendCrashDump: Too many filename parts (%d > %d)", numParts, MAX_FILENAME_PARTS));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);
            return;
        }

        if (const size_t maxRequiredIndex = std::max({DUMP_PART_VERSION, DUMP_PART_MODULE, DUMP_PART_OFFSET, DUMP_PART_DATE, DUMP_PART_TIME});
            parts.size() <= maxRequiredIndex)
        {
            AddReportLog(9313, SString("_ShouldSendCrashDump: Insufficient filename parts (parts=%d, max index=%d)", parts.size(), maxRequiredIndex));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);
            return;
        }

        const SString strVersionAndModuleAndOffset = parts[DUMP_PART_VERSION] + "_" + parts[DUMP_PART_MODULE] + "_" + parts[DUMP_PART_OFFSET];

        if (const size_t tagLength = strVersionAndModuleAndOffset.length(); tagLength > MAX_TAG_LENGTH)
        {
            AddReportLog(9326, SString("_ShouldSendCrashDump: Tag too long (%d > %d)", tagLength, MAX_TAG_LENGTH));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);
            return;
        }

        SString strDateAndTime = parts[DUMP_PART_DATE] + "_" + parts[DUMP_PART_TIME];
        strDateAndTime.Split(".", &strDateAndTime, nullptr);  // Remove .dmp extension from time part

        if (const size_t dateTimeLength = strDateAndTime.length(); dateTimeLength > MAX_DATETIME_LENGTH)
        {
            AddReportLog(9327, SString("_ShouldSendCrashDump: Date/time string too long (%d > %d)", dateTimeLength, MAX_DATETIME_LENGTH));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);
            return;
        }

        CDataInfoSet& history = m_VarConfig.crashdump_history;

        if (const size_t historySize = history.size(); historySize > MAX_HISTORY_SIZE)
        {
            AddReportLog(9318, SString("_ShouldSendCrashDump: History size limit exceeded (%d > %d)", historySize, MAX_HISTORY_SIZE));
            // Aggressively trim to half of max size with explicit calculation
            constexpr size_t TRIM_TO_RATIO = 2;  // Keep half when trimming
            history.erase(history.begin(), history.begin() + (historySize - MAX_HISTORY_SIZE / TRIM_TO_RATIO));
            CCore::GetSingleton().SaveConfig();
        }

        int iDuplicates = 0;
        for (const auto& item : history)
        {
            // Use if-init to cache GetAttribute result and compare
            if (const SString& itemTag = item.GetAttribute("tag"); strVersionAndModuleAndOffset == itemTag)
            {
                if (++iDuplicates > MAX_DUPLICATE_COUNT)
                {
                    AddReportLog(9314, SString("_ShouldSendCrashDump: Excessive duplicates detected (%d+)", MAX_DUPLICATE_COUNT));
                    return;
                }
            }
        }

        if (const int duplicateLimit = static_cast<int>(m_MasterConfig.crashdump.iDuplicates); iDuplicates > duplicateLimit)
        {
            AddReportLog(9314, SString("_ShouldSendCrashDump: Duplicate limit reached (%d/%d)", iDuplicates, duplicateLimit));
            return;
        }

        SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_TAG, strVersionAndModuleAndOffset);
        SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_DATETIME, strDateAndTime);

        m_ConditionMap.SetCondition(PROCESS_RESPONSE, RESPONSE_OK);
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ResetLastCrashDump
//
// Make sure last-dump-save is ignored next time
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ResetLastCrashDump()
{
    SetApplicationSetting("diagnostics", "last-dump-save", "");
    SetApplicationSetting("diagnostics", "last-dump-load-retry", "");
    SetApplicationSetting("diagnostics", "last-dump-retry-time", "");
    SetApplicationSetting("diagnostics", "last-dump-complete", "");
    SetApplicationSetting("diagnostics", "last-dump-tag", "");
    SetApplicationSetting("diagnostics", "last-dump-datetime", "");
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseCrashDumpQueryURLs
//
// Check if upload is needed by server
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseCrashDumpQueryURLs()
{
    constexpr const char* DIAGNOSTICS_SECTION = "diagnostics";
    constexpr const char* LAST_DUMP_SAVE = "last-dump-save";
    constexpr const char* EMPTY_VALUE = "";

    constexpr size_t        MAX_POST_FILENAME_LENGTH = 1024;
    constexpr unsigned char CONTROL_CHAR_THRESHOLD = 32;
    constexpr unsigned char DELETE_CHAR = 127;
    constexpr char          REPLACEMENT_CHAR = '_';

    // Initialize job info with server list
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList = MakeServerList(m_MasterConfig.crashdump.serverInfoMap);

    if (const SString strPathFilename = GetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE); strPathFilename.empty())
    {
        return;
    }
    else
    {
        if (SString strErrorMsg; const int iValidationResult = ValidateCrashDumpPath(strPathFilename, &strErrorMsg))
        {
            AddReportLog(iValidationResult, SString("_UseCrashDumpQueryURLs: %s", *strErrorMsg));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);  // Clear invalid path
            return;
        }

        // Extract filename from path
        SString strFilename;
        strPathFilename.Split(PATH_SEPERATOR, nullptr, &strFilename, -1);

        if (const size_t queryFilenameLength = strFilename.length(); queryFilenameLength > MAX_POST_FILENAME_LENGTH)
        {
            AddReportLog(9328, SString("_UseCrashDumpQueryURLs: Filename too long (%d > %d)", queryFilenameLength, MAX_POST_FILENAME_LENGTH));
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE, EMPTY_VALUE);
            return;
        }

        const auto is_control_char = [CONTROL_CHAR_THRESHOLD, DELETE_CHAR](unsigned char c) noexcept -> bool
        { return c < CONTROL_CHAR_THRESHOLD || c == DELETE_CHAR; };

        SString strSanitizedFilename = strFilename;

        for (auto& ch : strSanitizedFilename)
        {
            if (is_control_char(static_cast<unsigned char>(ch)))
                ch = REPLACEMENT_CHAR;
        }

        m_JobInfo.strPostFilename = strSanitizedFilename;
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_ProcessCrashDumpQuery
//
// Called after completing a CrashDumpQuery
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_ProcessCrashDumpQuery()
{
    constexpr const char* PROCESS_RESPONSE = "ProcessResponse";
    constexpr const char* DIAGNOSTICS_SECTION = "diagnostics";
    constexpr const char* LAST_DUMP_TAG = "last-dump-tag";
    constexpr const char* LAST_DUMP_DATETIME = "last-dump-datetime";
    constexpr const char* EMPTY_VALUE = "";
    constexpr const char* RESPONSE_OK = "ok";
    constexpr const char* RESPONSE_YES = "yes";
    constexpr const char* DUMP_ITEM_NAME = "dump";
    constexpr const char* TAG_ATTRIBUTE = "tag";
    constexpr const char* DATE_ATTRIBUTE = "date";

    constexpr size_t MAX_RESPONSE_SIZE = 10 * 1024 * 1024;

    m_ConditionMap.SetCondition(PROCESS_RESPONSE, EMPTY_VALUE);

    // Early return if no download buffer
    if (const bool hasData = !m_JobInfo.downloadBuffer.empty(); !hasData)
        return;

    if (const size_t responseSize = m_JobInfo.downloadBuffer.size(); responseSize > MAX_RESPONSE_SIZE)
    {
        AddReportLog(9319, SString("_ProcessCrashDumpQuery: Response too large (%d > %d)", responseSize, MAX_RESPONSE_SIZE));
        return;
    }

    const SStringX strResponse(m_JobInfo.downloadBuffer.data(), m_JobInfo.downloadBuffer.size());

    // Check if dump is wanted
    if (const bool isDumpWanted = strResponse.BeginsWithI(RESPONSE_YES); isDumpWanted)
    {
        // Server confirmed it wants the dump - retrieve metadata
        if (const SString strTag = GetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_TAG),
            strDateTime = GetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_DATETIME);
            !strTag.empty() && !strDateTime.empty())
        {
            CDataInfoSet& history = m_VarConfig.crashdump_history;

            // Add to history
            SDataInfoItem item;
            item.strName = DUMP_ITEM_NAME;
            item.SetAttribute(TAG_ATTRIBUTE, strTag);
            item.SetAttribute(DATE_ATTRIBUTE, strDateTime);
            history.push_back(item);

            // Trim history if needed
            if (const size_t historySize = history.size(); historySize > static_cast<size_t>(m_MasterConfig.crashdump.iMaxHistoryLength))
            {
                // Lambda for date comparison (GetAttribute is evaluated at runtime)
                auto date_comparator = [](const SDataInfoItem& a, const SDataInfoItem& b) noexcept -> bool
                { return a.GetAttribute("date") < b.GetAttribute("date"); };
                std::sort(history.begin(), history.end(), date_comparator);

                // Remove oldest entries
                // Note: If numToRemove exceeds MAX_HISTORY_EXCESS, trim to safe limit to prevent oversized history
                if (const size_t numToRemove = historySize - m_MasterConfig.crashdump.iMaxHistoryLength; numToRemove > 0)
                {
                    constexpr size_t MAX_HISTORY_EXCESS = 1000;
                    const size_t     actualRemove = (numToRemove > MAX_HISTORY_EXCESS) ? (historySize - MAX_HISTORY_EXCESS) : numToRemove;
                    history.erase(history.begin(), history.begin() + actualRemove);
                }
            }

            // Save config
            CCore::GetSingleton().SaveConfig();

            // Clear metadata settings using constexpr string literals
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_TAG, EMPTY_VALUE);
            SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_DATETIME, EMPTY_VALUE);
        }

        m_ConditionMap.SetCondition(PROCESS_RESPONSE, RESPONSE_OK);
    }
    else
    {
        // Server rejected dump or sent invalid response - clear temporary metadata
        SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_TAG, EMPTY_VALUE);
        SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_DATETIME, EMPTY_VALUE);
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseCrashDumpURLs
//
//
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseCrashDumpURLs()
{
    m_JobInfo = SJobInfo();
    m_JobInfo.serverList = MakeServerList(m_MasterConfig.crashdump.serverInfoMap);
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_UseCrashDumpPostContent
//
// Load and prepare crash dump file for upload
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::_UseCrashDumpPostContent()
{
    constexpr const char* DIAGNOSTICS_SECTION = "diagnostics";
    constexpr const char* LAST_DUMP_SAVE = "last-dump-save";
    constexpr const char* SEND_DUMPS = "send-dumps";
    constexpr const char* NO_SEND = "no";
    constexpr const char* LAST_DUMP_COMPLETE = "last-dump-complete";
    constexpr const char* COMPLETE_FLAG = "1";
    constexpr const char* LAST_DUMP_LOAD_RETRY = "last-dump-load-retry";
    constexpr const char* LAST_DUMP_RETRY_TIME = "last-dump-retry-time";
    constexpr const char* LOG_PREFIX = "_UseCrashDumpPostContent: ";

    constexpr size_t   MAX_POST_FILENAME_LENGTH = 1024;
    constexpr size_t   MAX_POST_CONTENT_SIZE = 2 * 1024 * 1024;  // 2 MB for minidumps
    constexpr int      MAX_LOAD_RETRIES = 3;
    constexpr int      INITIAL_BACKOFF_SECONDS = 5;
    constexpr int      BACKOFF_MULTIPLIER = 3;
    constexpr uint32_t MDMP_SIGNATURE = 0x504D444D;  // "MDMP" in little-endian
    constexpr size_t   MIN_SIGNATURE_SIZE = sizeof(uint32_t);
    constexpr int      MILLISECONDS_PER_SECOND = 1000;

    if (const SString strPathFilename = GetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_SAVE); strPathFilename.empty())
    {
        return;
    }
    else
    {
        // Early return if user disabled dump sending
        if (const SString strSendDumps = GetApplicationSetting(DIAGNOSTICS_SECTION, SEND_DUMPS); strSendDumps == NO_SEND)
        {
            _ResetLastCrashDump();
            return;
        }

        // Validate path and file
        SString strErrorMsg;
        if (const int iValidationResult = ValidateCrashDumpPath(strPathFilename, &strErrorMsg, true); iValidationResult != 0)
        {
            AddReportLog(iValidationResult, SString("%s%s", LOG_PREFIX, *strErrorMsg));
            _ResetLastCrashDump();
            return;
        }

        // Extract filename from path
        SString strFilename;
        strPathFilename.Split(PATH_SEPERATOR, nullptr, &strFilename, -1);

        // Validate filename length
        if (const size_t postFilenameLength = strFilename.length(); postFilenameLength > MAX_POST_FILENAME_LENGTH)
        {
            AddReportLog(9329, SString("%sFilename too long (%d > %d)", LOG_PREFIX, postFilenameLength, MAX_POST_FILENAME_LENGTH));
            _ResetLastCrashDump();
            return;
        }

        // Check if dump file writing is complete
        if (const SString strDumpComplete = GetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_COMPLETE); strDumpComplete != COMPLETE_FLAG)
        {
            AddReportLog(9320, SString("%sDump file not yet complete - waiting for write to finish", LOG_PREFIX));

            // Don't start retry counter yet - just wait for next pulse
            // This prevents premature file access while crash handler is still writing
            m_bSentCrashDump = false;
            return;
        }

        // Get current retry count
        const int iCurrentRetry = GetApplicationSettingInt(DIAGNOSTICS_SECTION, LAST_DUMP_LOAD_RETRY);

        // Lambda for backoff calculation with explicit return type
        const auto calculate_backoff = [INITIAL_BACKOFF_SECONDS, BACKOFF_MULTIPLIER](int retry_count) noexcept -> int
        {
            if (retry_count <= 0)
                return 0;
            int backoff = INITIAL_BACKOFF_SECONDS;
            for (int i = 1; i < retry_count; ++i)
                backoff *= BACKOFF_MULTIPLIER;
            return backoff;
        };

        // Calculate backoff for current retry
        const int iBackoffSeconds = calculate_backoff(iCurrentRetry);

        // Check if we need to wait for retry backoff delay
        if (iCurrentRetry > 0)
        {
            // Get retry timestamp
            const long long llRetryTimeSec = GetApplicationSettingInt(DIAGNOSTICS_SECTION, LAST_DUMP_RETRY_TIME);
            const long long llCurrentTimeSec = GetTickCount64_() / MILLISECONDS_PER_SECOND;
            const long long llElapsedSec = llCurrentTimeSec - llRetryTimeSec;

            // Check if backoff delay has elapsed
            if (llElapsedSec >= 0 && llElapsedSec < iBackoffSeconds)
            {
                // Still waiting for backoff delay
                m_bSentCrashDump = false;
                return;
            }
        }

        // Attempt to load file
        if (const bool bLoadSuccess = FileLoad(strPathFilename, m_JobInfo.postContent); bLoadSuccess)
        {
            if (m_JobInfo.postContent.empty())
            {
                AddReportLog(9317, SString("%sLoaded content is empty", LOG_PREFIX));
                _ResetLastCrashDump();
                return;
            }

            // Validate content size
            if (const size_t contentSize = m_JobInfo.postContent.size(); contentSize > MAX_POST_CONTENT_SIZE)
            {
                AddReportLog(9316, SString("%sLoaded content too large (%d > %d)", LOG_PREFIX, contentSize, MAX_POST_CONTENT_SIZE));
                m_JobInfo.postContent.clear();
                _ResetLastCrashDump();
                return;
            }

            // Validate minidump signature
            if (const size_t contentSize = m_JobInfo.postContent.size(); contentSize >= MIN_SIGNATURE_SIZE)
            {
                uint32_t signature;
                memcpy(&signature, m_JobInfo.postContent.data(), sizeof(signature));

                if (signature != MDMP_SIGNATURE)
                {
                    AddReportLog(9322, SString("%sInvalid minidump signature (0x%08X, expected 0x%08X)", LOG_PREFIX, signature, MDMP_SIGNATURE));
                    m_JobInfo.postContent.clear();
                    _ResetLastCrashDump();
                    return;
                }
            }

            // Set metadata - upload ready
            m_JobInfo.strPostFilename = strFilename;
            m_JobInfo.bPostContentBinary = true;

            // Don't clear settings yet - wait until upload succeeds
            // Settings will be cleared in Program_SendCrashDump after _StartSendPost returns true

            AddReportLog(5200, SString("%sSuccessfully loaded crash dump file: %s (upload pending)", LOG_PREFIX, *strFilename));
        }
        else
        {
            // File load failed - retry
            if (const int iNextRetry = iCurrentRetry + 1; iNextRetry <= MAX_LOAD_RETRIES)
            {
                // Increment retry counter
                SetApplicationSettingInt(DIAGNOSTICS_SECTION, LAST_DUMP_LOAD_RETRY, iNextRetry);

                // Store current timestamp for backoff delay
                const long long llCurrentTimeSec = GetTickCount64_() / MILLISECONDS_PER_SECOND;
                SetApplicationSetting(DIAGNOSTICS_SECTION, LAST_DUMP_RETRY_TIME, SString("%lld", llCurrentTimeSec));

                // Calculate backoff for next retry
                const int iNextBackoffSeconds = calculate_backoff(iNextRetry);
                AddReportLog(9310, SString("%sFailed to load crash dump file (attempt %d/%d): %s - Will retry in %d seconds", LOG_PREFIX, iNextRetry,
                                           MAX_LOAD_RETRIES, *strFilename, iNextBackoffSeconds));

                // Reset flag to allow retry on next pulse (after backoff delay)
                m_bSentCrashDump = false;
            }
            else
            {
                // Max retries exceeded - give up
                AddReportLog(9324, SString("%sFailed to load crash dump file after %d attempts: %s - Giving up", LOG_PREFIX, MAX_LOAD_RETRIES, *strFilename));
                _ResetLastCrashDump();
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_StartSendPost
//
// Returns true if upload succeeded, false on failure
//
//
///////////////////////////////////////////////////////////////
bool CVersionUpdater::_StartSendPost()
{
    if (!m_JobInfo.postContent.size())
        return false;

    while (true)
    {
        UpdaterYield();
        switch (DoSendPostToNextServer())
        {
            case RES_FAIL:
                // Can't find any(more) servers to send a query to
                // Drop back to previous stack level
                return false;

            case RES_OK:
            {
                // Query sent ok, now wait for response
                if (_PollSendPost() == RES_OK)
                {
                    // Got a valid response
                    return true;
                }
                // Connection to current server failed, try next server
                break;
            }

            default:
                assert(0);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::_PollSendPost
//
//
//
///////////////////////////////////////////////////////////////
int CVersionUpdater::_PollSendPost()
{
    while (true)
    {
        UpdaterYield();
        switch (DoPollPost())
        {
            case RES_FAIL:
                // Connection to current server failed, try next server
                return RES_FAIL;

            case RES_OK:
                // Got a valid response
                return RES_OK;

            case RES_POLL:

                // Waiting...
                break;

            default:
                assert(0);
        }
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
int CVersionUpdater::DoSendDownloadRequestToNextServer()
{
    if (++m_JobInfo.iRetryCount > m_JobInfo.iMaxServersToTry || m_JobInfo.serverList.size() == 0)
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_();

    // Get URL of next server in the list
    m_JobInfo.iCurrent++;
    m_JobInfo.iCurrent = m_JobInfo.iCurrent % m_JobInfo.serverList.size();
    SString strServerURL = m_JobInfo.serverList[m_JobInfo.iCurrent];

    // Get our serial number
    char szSerial[64];
    CCore::GetSingleton().GetNetwork()->GetSerial(szSerial, sizeof(szSerial));
    char szStatus[128];
    CCore::GetSingleton().GetNetwork()->GetStatus(szStatus, sizeof(szStatus));

    // Compose version string
    unsigned short usNetRev = CCore::GetSingleton().GetNetwork()->GetNetRev();
    unsigned short usNetRel = CCore::GetSingleton().GetNetwork()->GetNetRel();
    SString        strPlayerVersion("%d.%d.%d-%d.%05d.%d.%03d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE,
                                    MTASA_VERSION_BUILD, usNetRev, usNetRel);

    SString strUpdateBuildType;
    CVARS_GET("update_build_type", strUpdateBuildType);

    // Supercode to determine if sound files have been cut
    static SString strSoundCut = "u";
    if (strSoundCut == "u")
    {
        SString strGTARootDir;
        CFilePathTranslator().GetGTARootDirectory(strGTARootDir);
        if (FILE* fh = File::Fopen(PathJoin(strGTARootDir, "audio", "SFX", "SPC_EA"), "rb"))
        {
            strSoundCut = "y";
            fseek(fh, 0x38BDC80, SEEK_SET);
            for (uint i = 0; i < 20; i++)
                if (fgetc(fh))
                    strSoundCut = "n";  // Non-zero found
            if (ferror(fh))
                strSoundCut = "e";  // File error
            fclose(fh);
        }
    }

    // Mem stats
    MEMORYSTATUSEX memoryStatus = {sizeof(memoryStatus)};
    GlobalMemoryStatusEx(&memoryStatus);

    // Info required for hotfix update
    int             iReqKB3033929 = -1;
    int             iReqKB3035131 = -1;
    SLibVersionInfo verInfoKernel32 = {}, verInfoNcrypt = {};
    GetLibVersionInfo("kernel32.dll", &verInfoKernel32);
    GetLibVersionInfo("ncrypt.dll", &verInfoNcrypt);
    if (Is64BitOS() && GetApplicationSetting("real-os-version") == "6.1" && GetApplicationSetting("real-os-build") == "7601" &&
        verInfoNcrypt.GetFileVersionString() < "6.1.7601.18741")
    {
        iReqKB3033929 = IsHotFixInstalled("KB3033929") ? 0 : 1;
        iReqKB3035131 = IsHotFixInstalled("KB3035131") ? 0 : 1;
    }

    int           secureBootStatus = 0;
    const SString secureBootValue =
        GetSystemRegistryValue((uint)HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SecureBoot\\State", "UEFISecureBootEnabled", &secureBootStatus);
    bool bSecureBootEnabled = false;
    if (secureBootStatus > 0)
    {
        // Parse the registry value into a numeric flag
        char*      secureBootEnd = nullptr;
        const long secureBootNumeric = strtol(secureBootValue.c_str(), &secureBootEnd, 10);
        if (secureBootEnd != secureBootValue.c_str() && *secureBootEnd == '\0')
        {
            bSecureBootEnabled = secureBootNumeric != 0;
        }
    }
    // Compile some system stats
    SDxStatus dxStatus;
    g_pGraphics->GetRenderItemManager()->GetDxStatus(dxStatus);
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    SString        strVideoCard = SStringX(g_pDeviceState->AdapterState.Name).Left(30);
    std::replace_if(strVideoCard.begin(), strVideoCard.end(), [](int c) { return !isalnum(c); }, '_');
    SString strSystemStats(
        "1_%d_%d_%d_%d_%d"
        "_%d%d%d%d"
        "_%s",
        static_cast<int>(GetWMITotalPhysicalMemory() / 1024LL / 1024LL),  //
        g_pDeviceState->AdapterState.InstalledMemoryKB / 1024,            //
        gameSettings->GetCurrentVideoMode(),                              //
        gameSettings->GetFXQuality(),                                     //
        dxStatus.settings.iDrawDistance,                                  //

        GetVideoModeManager()->IsWindowed(),      //
        GetVideoModeManager()->IsMultiMonitor(),  //
        dxStatus.settings.bVolumetricShadows,     //
        dxStatus.settings.bAllowScreenUpload,     //

        *GetApplicationSetting("real-os-version")  //
    );

    SString strSystemStats2(
        "2_%d_%d_%d"
        "_%d_%d_%d"
        "_%d_%d_%d_%d_%d_%x",
        g_pGraphics->GetViewportWidth(),   //
        g_pGraphics->GetViewportHeight(),  //
        dxStatus.settings.b32BitColor,     //

        GetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_LOWEST_UNSAFE),  //
        GetApplicationSettingInt(DIAG_MINIDUMP_DETECTED_COUNT),         //
        GetApplicationSettingInt(DIAG_MINIDUMP_CONFIRMED_COUNT),        //

        atoi(dxStatus.videoCard.strPSVersion),    //
        dxStatus.videoCard.iNumSimultaneousRTs,   //
        dxStatus.settings.iAntiAliasing,          //
        dxStatus.settings.iAnisotropicFiltering,  //
        (int)dxStatus.settings.fFieldOfView,      //
        dxStatus.videoCard.depthBufferFormat      //
    );

    SString strSystemStats3(
        "3_0"  // Was VS2013 runtime installed
        "_%s_%s_%d"
        "_0"  // Was VS2015 runtime version
        "_%d_%d_%d_%d_%d_%s_%s",
        *GetApplicationSetting("real-os-build"),                          //
        *GetApplicationSetting("locale").Replace("_", "-"),               //
        (uint)FileSize(PathJoin(GetSystemSystemPath(), "normaliz.dll")),  //

        bSecureBootEnabled,                                     //
        static_cast<int>(memoryStatus.ullTotalVirtual / 1024),  //
        Is64BitOS(),                                            //
        iReqKB3033929,                                          //
        iReqKB3035131,                                          //
        *verInfoKernel32.GetFileVersionString(),                //
        *verInfoNcrypt.GetFileVersionString()                   //
    );

    SString strConnectUsage = SString("%i_%i", GetApplicationSettingInt("times-connected-editor"), GetApplicationSettingInt("times-connected"));
    SString strOptimusInfo = SString("%i_%i_%i", GetApplicationSettingInt("nvhacks", "optimus"), GetApplicationSettingInt("nvhacks", "optimus-startup-option"),
                                     GetApplicationSettingInt("nvhacks", "optimus-force-windowed"));

    // Make the query URL
    SString strQueryURL = strServerURL;
    strQueryURL = strQueryURL.Replace("%", "_");
    strQueryURL = strQueryURL.Replace("_VERSION_", strPlayerVersion);
    strQueryURL = strQueryURL.Replace("_ID_", szSerial);
    strQueryURL = strQueryURL.Replace("_STATUS_", szStatus);
    strQueryURL = strQueryURL.Replace("_BETA_", strUpdateBuildType);
    strQueryURL = strQueryURL.Replace("_TYPE_", m_strServerSaysType);
    strQueryURL = strQueryURL.Replace("_DATA_", m_strServerSaysData);
    strQueryURL = strQueryURL.Replace("_REFER_", m_strServerSaysHost);
    strQueryURL = strQueryURL.Replace("_REFERSG_", m_strSidegradeHost);
    strQueryURL = strQueryURL.Replace("_WANTVER_", m_strSidegradeVersion);
    strQueryURL = strQueryURL.Replace("_LASTNEWS_", m_VarConfig.news_lastNewsDate);
    strQueryURL = strQueryURL.Replace("_FILE_", m_JobInfo.strPostFilename);
    strQueryURL = strQueryURL.Replace("_SYS_", strSystemStats);
    strQueryURL = strQueryURL.Replace("_SYS2_", strSystemStats2);
    strQueryURL = strQueryURL.Replace("_SYS3_", strSystemStats3);
    strQueryURL = strQueryURL.Replace("_VID_", strVideoCard);
    strQueryURL = strQueryURL.Replace("_USAGE_", strConnectUsage);
    strQueryURL = strQueryURL.Replace("_SCUT_", strSoundCut);
    strQueryURL = strQueryURL.Replace("_OPTIMUS_", strOptimusInfo);
    strQueryURL = strQueryURL.Replace("_GTAMD5_", GetApplicationSetting("gta-exe-md5"));

    // See if this download qualifies for using a resumable file
    if (!m_JobInfo.strFilename.empty() && strQueryURL.EndsWith(m_JobInfo.strFilename))
    {
        m_JobInfo.strResumableSaveLocation = GetResumableSaveLocation(m_JobInfo.strFilename, m_JobInfo.strMD5, m_JobInfo.iFilesize);
    }
    else
    {
        m_JobInfo.strResumableSaveLocation = "";
    }

    // Perform the HTTP request
    m_JobInfo.downloadStatus = EDownloadStatus::Running;
    m_JobInfo.iDownloadResultCode = 0;
    GetHTTP()->Reset();
    SHttpRequestOptions options;
    options.bResumeFile = true;
    GetHTTP()->QueueFile(strQueryURL, m_JobInfo.strResumableSaveLocation, this, StaticDownloadFinished, options);
    m_strLastQueryURL = strQueryURL;
    OutputDebugLine(SString("[Updater] DoSendDownloadRequestToNextServer %d/%d %s", m_JobInfo.iCurrent, m_JobInfo.serverList.size(), strQueryURL.c_str()));
    return RES_OK;
}

///////////////////////////////////////////////////////////////
//
// CVersionUpdater::StaticDownloadFinished
//
// Handle when download finishes
//
///////////////////////////////////////////////////////////////
void CVersionUpdater::StaticDownloadFinished(const SHttpDownloadResult& result)
{
    ((CVersionUpdater*)result.pObj)->DownloadFinished(result);
}

void CVersionUpdater::DownloadFinished(const SHttpDownloadResult& result)
{
    if (result.bSuccess)
    {
        m_JobInfo.downloadStatus = EDownloadStatus::Success;
        m_JobInfo.iDownloadResultCode = result.iErrorCode;
        // Save data if a file wasn't used
        m_JobInfo.downloadBuffer.resize(result.dataSize);
        if (result.dataSize > 0)
            memcpy(&m_JobInfo.downloadBuffer[0], result.pData, result.dataSize);
    }
    else
    {
        m_JobInfo.downloadStatus = EDownloadStatus::Failure;
        m_JobInfo.iDownloadResultCode = result.iErrorCode;
    }
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
int CVersionUpdater::DoPollDownload()
{
    m_ConditionMap.SetCondition("Download", "");
    GetHTTP()->ProcessQueuedFiles();

    // Update progress
    unsigned int uiBytesDownloaded = GetHTTP()->GetDownloadSizeNow();
    if (m_JobInfo.uiBytesDownloaded != uiBytesDownloaded)
    {
        m_llTimeStart = GetTickCount64_();
        m_JobInfo.uiBytesDownloaded = uiBytesDownloaded;
        OutputDebugLine(SString("[Updater] uiBytesDownloaded:%d", uiBytesDownloaded));
    }

    // Are we done yet?
    if (m_JobInfo.downloadStatus != EDownloadStatus::Success)
    {
        m_JobInfo.iIdleTime = GetTickCount64_() - m_llTimeStart;
        int iUseTimeOut = m_JobInfo.uiBytesDownloaded ? m_JobInfo.iTimeoutTransfer : m_JobInfo.iTimeoutConnect;
        m_JobInfo.iIdleTimeLeft = iUseTimeOut - m_JobInfo.iIdleTime;
        // Give up if error or timeout
        if ((m_JobInfo.downloadStatus == EDownloadStatus::Failure) || m_JobInfo.iIdleTimeLeft < 0)
        {
            GetHTTP()->Reset();
            AddReportLog(4002, SString("DoPollDownload: Regular fail for %s (status:%u  time:%u)", m_strLastQueryURL.c_str(), m_JobInfo.iDownloadResultCode,
                                       m_JobInfo.iIdleTime));

            m_ConditionMap.SetCondition("Download", "Fail");
            return RES_FAIL;
        }
        return RES_POLL;
    }

    // If was using file to store resumable data, load it up
    if (m_JobInfo.downloadBuffer.empty() && !m_JobInfo.strResumableSaveLocation.empty())
    {
        FileLoad(m_JobInfo.strResumableSaveLocation, m_JobInfo.downloadBuffer);
        FileDelete(m_JobInfo.strResumableSaveLocation);
    }

    // Got something
    if (!m_JobInfo.downloadBuffer.empty())
    {
        m_ConditionMap.SetCondition("Download", "Ok");
        AddReportLog(2005, SString("DoPollDownload: Downloaded %d bytes from %s", m_JobInfo.downloadBuffer.size(), m_strLastQueryURL.c_str()));
        return RES_OK;
    }

    m_ConditionMap.SetCondition("Download", "Fail");
    AddReportLog(5007, SString("DoPollDownload: Fail for %s", m_strLastQueryURL.c_str()));
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
int CVersionUpdater::DoSendPostToNextServer()
{
    if (++m_JobInfo.iRetryCount > 1 || m_JobInfo.serverList.size() == 0)
    {
        return RES_FAIL;
    }

    m_llTimeStart = GetTickCount64_();

    // Get URL of next server in the list
    m_JobInfo.iCurrent++;
    m_JobInfo.iCurrent = m_JobInfo.iCurrent % m_JobInfo.serverList.size();
    SString strServerURL = m_JobInfo.serverList[m_JobInfo.iCurrent];

    // Get our serial number
    char szSerial[64];
    CCore::GetSingleton().GetNetwork()->GetSerial(szSerial, sizeof(szSerial));
    szSerial[sizeof(szSerial) - 1] = '\0';  // Defensive null-termination
    char szStatus[128];
    CCore::GetSingleton().GetNetwork()->GetStatus(szStatus, sizeof(szStatus));
    szStatus[sizeof(szStatus) - 1] = '\0';  // Defensive null-termination

    // Compose version string
    unsigned short usNetRev = CCore::GetSingleton().GetNetwork()->GetNetRev();
    unsigned short usNetRel = CCore::GetSingleton().GetNetwork()->GetNetRel();
    SString        strPlayerVersion("%d.%d.%d-%d.%05d.%d.%03d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE,
                                    MTASA_VERSION_BUILD, usNetRev, usNetRel);

    // Make the query URL
    SString strQueryURL = strServerURL;
    strQueryURL = strQueryURL.Replace("%", "_");
    strQueryURL = strQueryURL.Replace("_VERSION_", strPlayerVersion);
    strQueryURL = strQueryURL.Replace("_ID_", szSerial);
    strQueryURL = strQueryURL.Replace("_STATUS_", szStatus);
    strQueryURL = strQueryURL.Replace("_FILE_", m_JobInfo.strPostFilename);

    //
    // Send data. Doesn't check if it was received.
    //
    GetHTTP()->Reset();
    SHttpRequestOptions options;
    options.strPostData = SStringX(&m_JobInfo.postContent.at(0), m_JobInfo.postContent.size());
    options.bPostBinary = m_JobInfo.bPostContentBinary;
    GetHTTP()->QueueFile(strQueryURL, nullptr, nullptr, nullptr, options);

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
int CVersionUpdater::DoPollPost()
{
    if (!GetHTTP()->ProcessQueuedFiles())
    {
        return RES_POLL;
    }
    return RES_OK;
}
