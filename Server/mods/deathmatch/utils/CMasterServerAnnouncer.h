/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/utils/CMasterServerAnnouncer.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ASE.h"
#include "version.h"

struct SMasterServerDefinition
{
    bool    bAcceptsPush;
    bool    bDoReminders;
    bool    bHideProblems;
    bool    bHideSuccess;
    uint    uiReminderIntervalMins;
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

    CMasterServer(const SMasterServerDefinition& definition) : m_Definition(definition)
    {
        m_Stage = ANNOUNCE_STAGE_INITIAL;
        m_uiInitialAnnounceRetryAttempts = 5;
        m_uiInitialAnnounceRetryInterval = 1000 * 60 * 5;            // 5 mins initial announce retry interval
        m_uiPushInterval = 1000 * 60 * 10;                           // 10 mins push interval
    }

protected:
    ~CMasterServer() {}            // Must use Release()
public:
    //
    // Pulse this master server
    //
    void Pulse()
    {
        if (m_bStatusBusy)
            return;

        long long llTickCountNow = GetTickCount64_();

        // Do announce?
        if (g_pGame->GetConfig()->GetAseInternetListenEnabled())
        {
            bool bIsTimeForAnnounce = false;
            if (m_Stage == ANNOUNCE_STAGE_INITIAL)
                bIsTimeForAnnounce = true;
            if (m_Stage == ANNOUNCE_STAGE_INITIAL_RETRY && m_Definition.bDoReminders &&
                llTickCountNow - m_llLastAnnounceTime > m_uiInitialAnnounceRetryInterval)
                bIsTimeForAnnounce = true;
            if (m_Stage == ANNOUNCE_STAGE_REMINDER && m_Definition.bDoReminders &&
                llTickCountNow - m_llLastAnnounceTime > TICKS_FROM_MINUTES(m_Definition.uiReminderIntervalMins))
                bIsTimeForAnnounce = true;

            if (bIsTimeForAnnounce)
            {
                m_llLastAnnounceTime = llTickCountNow;

                // Send request
                this->AddRef();            // Keep object alive
                m_bStatusBusy = true;
                SHttpRequestOptions options;
                options.uiConnectionAttempts = 2;
                GetDownloadManager()->QueueFile(m_Definition.strURL, NULL, this, StaticDownloadFinishedCallback, options);
            }
        }
        else
        {
            m_Stage = ANNOUNCE_STAGE_INITIAL;
        }

        // Do push?
        if (g_pGame->GetConfig()->GetAseInternetPushEnabled() && m_Definition.bAcceptsPush)
        {
            if (m_llLastPushTime == 0 || llTickCountNow - m_llLastPushTime > m_uiPushInterval)
            {
                m_llLastPushTime = llTickCountNow;
                SHttpRequestOptions options;
                options.strPostData = ASE::GetInstance()->QueryLight();
                options.bPostBinary = true;
                options.uiConnectionAttempts = 1;
                GetDownloadManager()->QueueFile(m_Definition.strURL, NULL, NULL, NULL, options);
            }
        }
    }

    //
    // Process response from master server
    //
    static void StaticDownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        CMasterServer* pMasterServer = (CMasterServer*)result.pObj;
        pMasterServer->DownloadFinishedCallback(result);
        pMasterServer->Release();            // No need to keep object alive now
    }

    //
    // Process response from master server
    //
    void DownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        m_bStatusBusy = false;

        if (result.bSuccess)
        {
            if (m_Stage < ANNOUNCE_STAGE_REMINDER)
            {
                m_Stage = ANNOUNCE_STAGE_REMINDER;

                CArgMap argMap;
                argMap.SetFromString(result.pData);

                if (result.iErrorCode == 200)
                    m_remoteAddress = argMap.Get("remote_addr");

                if (!m_Definition.bHideSuccess)
                {
                    SString strOkMessage = argMap.Get("ok_message");

                    // Log successful initial announcement
                    if (result.iErrorCode == 200)
                        CLogger::LogPrintf("%s success! %s\n", *m_Definition.strDesc, *strOkMessage);
                    else
                        CLogger::LogPrintf("%s successish! (%u %s)\n", *m_Definition.strDesc, result.iErrorCode, GetDownloadManager()->GetError());
                }
            }
        }
        else
        {
            bool bCanRetry = (result.iErrorCode == 28);            // We can retry if 'Timeout was reached'

            if (m_Stage == ANNOUNCE_STAGE_INITIAL)
            {
                if (bCanRetry)
                {
                    m_Stage = ANNOUNCE_STAGE_INITIAL_RETRY;
                    if (!m_Definition.bHideProblems)
                    {
                        // Log initial failure
                        CLogger::LogPrintf("%s no response. Retrying...\n", *m_Definition.strDesc);
                    }
                }
            }

            if (m_Stage < ANNOUNCE_STAGE_REMINDER)
            {
                if (!bCanRetry || m_uiInitialAnnounceRetryAttempts-- == 0)
                {
                    // Give up initial retries
                    m_Stage = ANNOUNCE_STAGE_REMINDER;
                    if (!m_Definition.bHideProblems)
                    {
                        // Log final failure
                        CLogger::LogPrintf("%s failed! (%u %s)\n", *m_Definition.strDesc, result.iErrorCode, GetDownloadManager()->GetError());
                    }
                }
            }
        }
    }

    const SMasterServerDefinition& GetDefinition() const { return m_Definition; }

    bool               HasRemoteAddress() const noexcept { return !m_remoteAddress.empty(); }
    const std::string& GetRemoteAddress() const noexcept { return m_remoteAddress; }

    //
    // Get http downloader used for master server comms etc.
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager() { return g_pNetServer->GetHTTPDownloadManager(EDownloadMode::ASE); }

protected:
    bool                          m_bStatusBusy;
    uint                          m_Stage;
    uint                          m_uiInitialAnnounceRetryAttempts;
    uint                          m_uiInitialAnnounceRetryInterval;
    uint                          m_uiPushInterval;
    long long                     m_llLastAnnounceTime;
    long long                     m_llLastPushTime;
    const SMasterServerDefinition m_Definition;
    std::string                   m_remoteAddress;
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

    ~CMasterServerAnnouncer()
    {
        while (!m_MasterServerList.empty())
        {
            m_MasterServerList.back()->Release();
            m_MasterServerList.pop_back();
        }
    }

    //
    // Make list of master servers to contact
    //
    void InitServerList()
    {
        assert(m_MasterServerList.empty());
        AddServer(true, true, false, false, 60 * 24, "Querying MTA master server...", QUERY_URL_MTA_MASTER_SERVER);
    }

    void AddServer(bool bAcceptsPush, bool bDoReminders, bool bHideProblems, bool bHideSuccess, uint uiReminderIntervalMins, const SString& strDesc,
                   const SString& strInUrl)
    {
        // Check if server is already present
        for (auto pMasterServer : m_MasterServerList)
        {
            if (pMasterServer->GetDefinition().strURL.BeginsWithI(strInUrl.SplitLeft("%")))
                return;
        }

        CMainConfig* pMainConfig = g_pGame->GetConfig();
        SString      strServerIP = pMainConfig->GetServerIP();
        ushort       usServerPort = pMainConfig->GetServerPort();
        ushort       usHTTPPort = pMainConfig->GetHTTPPort();
        uint         uiMaxPlayerCount = pMainConfig->GetMaxPlayers();
        bool         bPassworded = pMainConfig->HasPassword();
        SString      strAseMode = pMainConfig->GetSetting("ase");
        bool         bAseLanListen = pMainConfig->GetAseLanListenEnabled();

        SString strVersion("%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD);
        SString strExtra("%d_%d_%d_%s_%d", 0, uiMaxPlayerCount, bPassworded, *strAseMode, bAseLanListen);

        SString strUrl = strInUrl;
        strUrl = strUrl.Replace("%GAME%", SString("%u", usServerPort));
        strUrl = strUrl.Replace("%ASE%", SString("%u", usServerPort + 123));
        strUrl = strUrl.Replace("%HTTP%", SString("%u", usHTTPPort));
        strUrl = strUrl.Replace("%VER%", strVersion);
        strUrl = strUrl.Replace("%EXTRA%", strExtra);
        strUrl = strUrl.Replace("%IP%", strServerIP);

        SMasterServerDefinition masterServerDefinition = {bAcceptsPush, bDoReminders, bHideProblems, bHideSuccess, uiReminderIntervalMins, strDesc, strUrl};
        m_MasterServerList.push_back(new CMasterServer(masterServerDefinition));
    }

    //
    // Pulse each master server in our list
    //
    void Pulse()
    {
        if (m_MasterServerList.empty())
            InitServerList();

        for (uint i = 0; i < m_MasterServerList.size(); i++)
        {
            m_MasterServerList[i]->Pulse();
        }
    }

    /*
     * @brief Get remote address of the first master server that has it.
     */
    const std::string& GetRemoteAddress() const noexcept
    {
        for (CMasterServer* masterServer : m_MasterServerList)
        {
            if (masterServer->HasRemoteAddress())
                return masterServer->GetRemoteAddress();
        }

        static std::string empty;
        return empty;
    }

protected:
    std::vector<CMasterServer*> m_MasterServerList;
};
