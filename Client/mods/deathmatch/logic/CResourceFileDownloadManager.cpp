/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceFileDownloadManager.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CResourceFileDownloadManager::CResourceFileDownloadManager() :
    m_asyncFileChecksumTaskSched(CAsyncTaskScheduler(Clamp<int>(2, std::thread::hardware_concurrency(), 16)))
{
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::AddServer
//
// Add http server to use.
// Assumes last one added is the internal server
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::AddServer(const SString& strUrl, int iMaxConnectionsPerClient, EDownloadModeType downloadChannel, uint uiConnectionAttempts,
                                             uint uiConnectTimeoutMs)
{
    SHttpServerInfo serverInfo;
    serverInfo.bEnabled = true;
    serverInfo.downloadChannel = downloadChannel;
    serverInfo.strUrl = strUrl;
    serverInfo.uiConnectionAttempts = uiConnectionAttempts;
    serverInfo.uiConnectTimeoutMs = uiConnectTimeoutMs;
    m_HttpServerList.push_back(serverInfo);
    g_pCore->GetNetwork()->GetHTTPDownloadManager(downloadChannel)->SetMaxConnections(iMaxConnectionsPerClient);
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::AddPendingFileDownload
//
// Add resource file to the list of files to be downloaded
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::AddPendingFileDownload(CDownloadableResource* pResourceFile)
{
    assert(!ListContains(m_PendingFileDownloadList, pResourceFile));
    assert(!ListContains(m_ActiveFileDownloadList, pResourceFile));
    m_PendingFileDownloadList.push_back(pResourceFile);
    pResourceFile->SetIsWaitingForDownload(true);
    AddDownloadSize(pResourceFile->GetDownloadSize());
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::UpdatePendingDownloads
//
// Figure which download group should be running
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::UpdatePendingDownloads()
{
    // Get download group to use
    int iGroup = INVALID_DOWNLOAD_PRIORITY_GROUP;
    if (!m_ActiveFileDownloadList.empty())
    {
        // Use active download group
        iGroup = std::max(iGroup, m_ActiveFileDownloadList[0]->GetDownloadPriorityGroup());
    }
    else
    {
        // If no resource files being downloaded, find highest pending download group
        for (auto pResourceFile : m_PendingFileDownloadList)
        {
            iGroup = std::max(iGroup, pResourceFile->GetDownloadPriorityGroup());
        }
    }

    // Start (or add to) matching group
    std::vector<CDownloadableResource*> pendingListCopy = m_PendingFileDownloadList;
    for (auto pResourceFile : pendingListCopy)
    {
        if (pResourceFile->GetDownloadPriorityGroup() == iGroup)
        {
            // Move to active list and begin download
            ListRemove(m_PendingFileDownloadList, pResourceFile);
            m_ActiveFileDownloadList.push_back(pResourceFile);
            BeginResourceFileDownload(pResourceFile, 0);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::OnRemoveResourceFile
//
// Tidy up lists when a resource file is delected
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::OnRemoveResourceFile(CDownloadableResource* pResourceFile)
{
    ListRemove(m_PendingFileDownloadList, pResourceFile);
    ListRemove(m_ActiveFileDownloadList, pResourceFile);
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::DoPulse
//
// Downloading initial resource files
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::DoPulse()
{
    if (!g_pNet->IsConnected())
        return;

    if (!IsTransferringInitialFiles())
        return;

    m_asyncFileChecksumTaskSched.CollectResults();

    // Pulse the http downloads
    uint uiDownloadSizeTotal = 0;
    for (auto& serverInfo : m_HttpServerList)
    {
        CNetHTTPDownloadManagerInterface* pHTTP = g_pNet->GetHTTPDownloadManager(serverInfo.downloadChannel);
        pHTTP->ProcessQueuedFiles();
        uiDownloadSizeTotal += pHTTP->GetDownloadSizeNow();
    }

    // Handle fatal error
    if (!m_strLastHTTPError.empty())
    {
        // Throw the error and disconnect
        g_pCore->GetConsole()->Printf(_("Download error: %s"), *m_strLastHTTPError);
        AddReportLog(7106, SString("Game - HTTPError (%s)", *m_strLastHTTPError));

        g_pCore->GetModManager()->RequestUnload();
        g_pCore->ShowMessageBox(_("Error") + _E("CD20"), *m_strLastHTTPError, MB_BUTTON_OK | MB_ICON_ERROR);            // HTTP Error
        m_bIsTransferingFiles = false;
        return;
    }

    // Update progress box
    GetTransferBox()->SetInfo(uiDownloadSizeTotal);
    GetTransferBox()->DoPulse();

    // Check if completed downloading current group
    if (m_ActiveFileDownloadList.empty())
    {
        // Hide progress box if all groups done
        if (m_PendingFileDownloadList.empty())
        {
            m_bIsTransferingFiles = false;
            GetTransferBox()->Hide();
        }

        // Load our newly ready resources
        g_pClientGame->GetResourceManager()->OnDownloadGroupFinished();
        UpdatePendingDownloads();
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::AddDownloadSize
//
// Add byte count to transfer box
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::AddDownloadSize(int iSize)
{
    if (!m_bIsTransferingFiles)
    {
        GetTransferBox()->Show();
        m_bIsTransferingFiles = true;
        for (auto serverInfo : m_HttpServerList)
        {
            CNetHTTPDownloadManagerInterface* pHTTP = g_pNet->GetHTTPDownloadManager(serverInfo.downloadChannel);
            pHTTP->ResetDownloadSize();
        }
    }
    GetTransferBox()->AddToTotalSize(iSize);
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::DisableHttpServer
//
// Mark http server as unusable.
// Returns false if server cannot be disabled.
//
///////////////////////////////////////////////////////////////
bool CResourceFileDownloadManager::DisableHttpServer(uint uiHttpServerIndex)
{
    // Can only disable if not the last one
    if (uiHttpServerIndex + 1 >= m_HttpServerList.size())
        return false;
    m_HttpServerList[uiHttpServerIndex].bEnabled = false;
    return true;
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::BeginResourceFileDownload
//
// Return false if file could not be queued
//
///////////////////////////////////////////////////////////////
bool CResourceFileDownloadManager::BeginResourceFileDownload(CDownloadableResource* pResourceFile, uint uiHttpServerIndex)
{
    if (uiHttpServerIndex >= m_HttpServerList.size())
        return false;

    // Find enabled server
    while (m_HttpServerList[uiHttpServerIndex].bEnabled == false)
    {
        uiHttpServerIndex++;
        if (uiHttpServerIndex >= m_HttpServerList.size())
            return false;
    }

    pResourceFile->SetHttpServerIndex(uiHttpServerIndex);

    const SHttpServerInfo&            serverInfo = m_HttpServerList[uiHttpServerIndex];
    CNetHTTPDownloadManagerInterface* pHTTP = g_pCore->GetNetwork()->GetHTTPDownloadManager(serverInfo.downloadChannel);
    SString strHTTPDownloadURLFull("%s/%s/%s", *serverInfo.strUrl, pResourceFile->GetResource()->GetName(), pResourceFile->GetShortName());

    SHttpRequestOptions options;
    options.uiConnectionAttempts = serverInfo.uiConnectionAttempts;
    options.uiConnectTimeoutMs = serverInfo.uiConnectTimeoutMs;
    options.bCheckContents = true;
    options.bIsLocal = g_pClientGame->IsLocalGame();
    SString* pstrContext = MakeDownloadContextString(pResourceFile);
    SString  strFilename = pResourceFile->GetName();
    bool bUniqueDownload = pHTTP->QueueFile(strHTTPDownloadURLFull, strFilename, pstrContext, StaticDownloadFinished, options);
    if (!bUniqueDownload)
    {
        // TODO - If piggybacking on another matching download, then adjust progress bar
        OutputDebugLine(SString("[ResourceFileDownload] Using existing download for %s", *strHTTPDownloadURLFull));
    }
    return true;
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::StaticDownloadFinished
//
// Callback when file download has finished
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::StaticDownloadFinished(const SHttpDownloadResult& result)
{
    g_pClientGame->GetResourceFileDownloadManager()->DownloadFinished(result);
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::DownloadFinished
//
// Callback when file download has finished
//
///////////////////////////////////////////////////////////////
void CResourceFileDownloadManager::DownloadFinished(const SHttpDownloadResult& result)
{
    CDownloadableResource* pResourceFile = ResolveDownloadContextString((SString*)result.pObj);
    if (!pResourceFile)
        return;

    assert(ListContains(m_ActiveFileDownloadList, pResourceFile));

    if (result.bSuccess)
    {
        using namespace std::chrono;

        using steady_time_point_t = time_point<steady_clock>;
        struct AsyncReturnValue
        {
            steady_time_point_t GenerationStart;
            steady_time_point_t GenerationEnd;
            bool                DidMatch;
            bool                DidActaullyDoSomething;
        };

        m_asyncFileChecksumTaskSched.PushTask<AsyncReturnValue>(
        [=]() {
            AsyncReturnValue ReturnValue;
            ReturnValue.GenerationStart = steady_clock::now();

            CBuffer buffer; // Force the function to use a buffer, instead of reading the file twice(its less disk intensive)
            pResourceFile->GenerateClientChecksum(buffer); // Update checksum internally

            ReturnValue.DidActaullyDoSomething = buffer.GetSize() > 0;
            ReturnValue.GenerationEnd = steady_clock::now();
            ReturnValue.DidMatch = pResourceFile->DoesClientAndServerChecksumMatch();

            return ReturnValue;
        },
        [=](const AsyncReturnValue& ReturnVal) {
            // Write out some debug info: TODO - remove
            {
                const auto GenerationIntervalMS = (uint)duration_cast<milliseconds>(ReturnVal.GenerationEnd - ReturnVal.GenerationStart).count();
                const auto CallbackIntervalMS = (uint)duration_cast<milliseconds>(steady_clock::now() - ReturnVal.GenerationEnd).count();

                g_pCore->GetConsole()->Printf("[ResFileDLManager - Checksum gen]: %s in %u ms (gen: %u ms,   callback: %u ms) [%s,  %sgot: %s]",
                    pResourceFile->GetShortName(),
                    GenerationIntervalMS + CallbackIntervalMS,
                    GenerationIntervalMS,
                    CallbackIntervalMS,
                    ReturnVal.DidMatch ? "true" : "false",
                    ReturnVal.DidActaullyDoSomething ? "true" : "falsee",
                    ConvertDataToHexString(pResourceFile->GetLastClientChecksum().md5.data, sizeof(MD5)).c_str()
                );
            }

            // If checksum incorrect - Try download on next server
            if (!ReturnVal.DidMatch && BeginResourceFileDownload(pResourceFile, pResourceFile->GetHttpServerIndex() + 1))
            {
                // Was re-added - Add size again to total.
                AddDownloadSize(pResourceFile->GetDownloadSize());
                SString strMessage("External HTTP file mismatch (Retrying this file with internal HTTP) [%s]", *ConformResourcePath(pResourceFile->GetName()));
                g_pClientGame->TellServerSomethingImportant(1011, strMessage, 3);
            }
            else
            {
                // File now done(or if failed, it'll be handled in CResource::CheckAllFilesAndReDownloadIfNeeded())
                ListRemove(m_ActiveFileDownloadList, pResourceFile);
                pResourceFile->SetIsWaitingForDownload(false);
            }
        });
    }
    else
    {
        if (result.iErrorCode == 1007)
        {
            // Download failed due to being unable to create file
            // Ignore here so it will be processed at CResource::HandleDownloadedFileTrouble
        }
        else
        {
            // Download failed due to connection type problem
            CNetHTTPDownloadManagerInterface* pHTTP = g_pNet->GetHTTPDownloadManager(m_HttpServerList[pResourceFile->GetHttpServerIndex()].downloadChannel);
            SString                           strHTTPError = pHTTP->GetError();
            // Disable server from being used (if possible)
            if (DisableHttpServer(pResourceFile->GetHttpServerIndex()))
            {
                //  Try download on next server
                if (BeginResourceFileDownload(pResourceFile, pResourceFile->GetHttpServerIndex() + 1))
                {
                    // Was re-added - Add size again to total.
                    AddDownloadSize(pResourceFile->GetDownloadSize());

                    SString strMessage("External HTTP file download error:[%d] %s (Disabling External HTTP) [%s]", result.iErrorCode, *strHTTPError,
                        *ConformResourcePath(pResourceFile->GetName()));
                    g_pClientGame->TellServerSomethingImportant(1012, strMessage, 3);

                    // Don't remove file from active dl list
                    return;
                }
            }

            // File now done(or if failed, it'll be handled in CResource::CheckAllFilesAndReDownloadIfNeeded())
            m_strLastHTTPError = strHTTPError;
            ListRemove(m_ActiveFileDownloadList, pResourceFile);
            pResourceFile->SetIsWaitingForDownload(false);
        }   
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::MakeDownloadContextString
//
// For passing to HTTP->QueueFile
//
///////////////////////////////////////////////////////////////
SString* CResourceFileDownloadManager::MakeDownloadContextString(CDownloadableResource* pDownloadableResource)
{
    return new SString("%d:%s", pDownloadableResource->GetResource()->GetScriptID(), pDownloadableResource->GetShortName());
}

///////////////////////////////////////////////////////////////
//
// CResourceFileDownloadManager::ResolveDownloadContextString
//
// Decode previously generated context string.
// Automatically deletes the string
//
///////////////////////////////////////////////////////////////
CDownloadableResource* CResourceFileDownloadManager::ResolveDownloadContextString(SString* pString)
{
    SString strFilename;
    uint    uiScriptId = atoi(pString->SplitLeft(":", &strFilename));
    delete pString;

    CResource* pResource = g_pClientGame->GetResourceManager()->GetResourceFromScriptID(uiScriptId);
    if (!pResource)
        return NULL;

    // Find match in active downloads
    for (auto pDownloadableResource : m_ActiveFileDownloadList)
    {
        if (pDownloadableResource->GetResource() == pResource && pDownloadableResource->GetShortName() == strFilename)
        {
            return pDownloadableResource;
        }
    }
    return NULL;
}
