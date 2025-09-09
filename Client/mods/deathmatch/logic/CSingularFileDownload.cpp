/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSingularFileDownloadManager.cpp
 *  PURPOSE:     Singular file download manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CSingularFileDownload::CSingularFileDownload(CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL, CResource* pRequestResource,
                                             CChecksum serverChecksum, std::uint32_t handlerId)
{
    // Store the name
    m_strName = szName;

    // Store the name (short)
    m_strNameShort = szNameShort;

    // store resources
    m_pResource = pResource;
    m_pRequestResource = pRequestResource;

    // Store the server checksum
    m_ServerChecksum = serverChecksum;

    m_bBeingDeleted = false;
    m_bCancelled = false;
    m_handlerId = handlerId;
    m_pHTTPManager = nullptr;
    m_downloadMode = EDownloadModeType::NONE;

    GenerateClientChecksum();

    if (!DoesClientAndServerChecksumMatch())
    {
        SHttpRequestOptions options;
        options.bCheckContents = true;
        m_pHTTPManager = g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::RESOURCE_SINGULAR_FILES);
        m_downloadMode = EDownloadMode::RESOURCE_SINGULAR_FILES;
        m_pHTTPManager->QueueFile(strHTTPURL.c_str(), szName, this, DownloadFinishedCallBack, options);
        m_bComplete = false;
        g_pClientGame->SetTransferringSingularFiles(true);
    }
    else
    {
        CallFinished(true);
    }
}

CSingularFileDownload::~CSingularFileDownload()
{
}

void CSingularFileDownload::DownloadFinishedCallBack(const SHttpDownloadResult& result)
{
    CSingularFileDownload* pFile = (CSingularFileDownload*)result.pObj;
    pFile->CallFinished(result.bSuccess);
}

void CSingularFileDownload::CallFinished(bool bSuccess)
{
    // Flag file as loadable
    g_pClientGame->GetResourceManager()->OnDownloadedResourceFile(GetName());

    if (!m_bBeingDeleted && m_pResource)
    {
        // Call the onClientFileDownloadComplete event
        CLuaArguments Arguments;
        Arguments.PushString(GetShortName());            // file name
        Arguments.PushBoolean(bSuccess);                 // Completed successfully?
        if (m_pRequestResource)
        {
            Arguments.PushResource(m_pRequestResource);            // Resource that called downloadFile
        }
        else
        {
            Arguments.PushBoolean(false);            // or false
        }

        m_pResource->GetResourceEntity()->CallEvent("onClientFileDownloadComplete", Arguments, false);
    }
    SetComplete();
}

bool CSingularFileDownload::Cancel()
{
    if (m_bCancelled || m_bComplete)
        return false;

    m_bCancelled = true;
    m_bBeingDeleted = true;
    m_pResource = nullptr;
    m_pRequestResource = nullptr;

    // Cancel the actual HTTP download
    if (!m_pHTTPManager || m_downloadMode == EDownloadModeType::NONE)
        return true;

    return m_pHTTPManager->CancelDownload(this, DownloadFinishedCallBack);
}

bool CSingularFileDownload::DoesClientAndServerChecksumMatch()
{
    return (m_LastClientChecksum == m_ServerChecksum);
}

CChecksum CSingularFileDownload::GenerateClientChecksum()
{
    m_LastClientChecksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strName);
    return m_LastClientChecksum;
}
