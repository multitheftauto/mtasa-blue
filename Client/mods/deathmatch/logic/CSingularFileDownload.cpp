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
    resource = pResource;
    requestResource = pRequestResource;

    // Store the server checksum
    m_ServerChecksum = serverChecksum;

    beingDeleted = false;
    cancelled = false;
    handlerId = handlerId;
    httpManager = nullptr;
    downloadMode = EDownloadMode::NONE;

    GenerateClientChecksum();

    if (!DoesClientAndServerChecksumMatch())
    {
        SHttpRequestOptions options;
        options.bCheckContents = true;
        httpManager = g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::RESOURCE_SINGULAR_FILES);
        downloadMode = EDownloadMode::RESOURCE_SINGULAR_FILES;
        httpManager->QueueFile(strHTTPURL.c_str(), szName, this, DownloadFinishedCallBack, options);
        complete = false;
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

    if (!beingDeleted && resource)
    {
        // Call the onClientFileDownloadComplete event
        CLuaArguments Arguments;
        Arguments.PushString(GetShortName());            // file name
        Arguments.PushBoolean(bSuccess);                 // Completed successfully?
        if (requestResource)
        {
            Arguments.PushResource(requestResource);            // Resource that called downloadFile
        }
        else
        {
            Arguments.PushBoolean(false);            // or false
        }

        resource->GetResourceEntity()->CallEvent("onClientFileDownloadComplete", Arguments, false);
    }
    SetComplete();
}

bool CSingularFileDownload::Cancel()
{
    if (cancelled || complete)
        return false;

    cancelled = true;
    beingDeleted = true;
    resource = nullptr;
    requestResource = nullptr;

    // Cancel the actual HTTP download
    if (!httpManager || downloadMode == EDownloadMode::NONE)
        return false;

    return httpManager->CancelDownload(this, DownloadFinishedCallBack);
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
