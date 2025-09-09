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
using std::list;

CSingularFileDownloadManager::CSingularFileDownloadManager() : m_nextHandlerId(1)
{
}

CSingularFileDownloadManager::~CSingularFileDownloadManager()
{
    // clear list
    ClearList();
}

CSingularFileDownload* CSingularFileDownloadManager::AddFile(CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL,
                                                             CResource* pRequestResource, CChecksum checksum)
{
    const std::uint32_t handlerId = m_nextHandlerId++;
    auto* pFile = new CSingularFileDownload(pResource, szName, szNameShort, strHTTPURL, pRequestResource, checksum, handlerId);
    m_Downloads.push_back(pFile);
    m_HandlerMap[handlerId] = pFile;
    return pFile;
}

void CSingularFileDownloadManager::CancelResourceDownloads(CResource* pResource)
{
    for (const auto& pDownload : m_Downloads)
    {
        if (pDownload->GetResource() == pResource)
            pDownload->Cancel();
    }
}

void CSingularFileDownloadManager::ClearList()
{
    for (const auto& pDownload : m_Downloads)
    {
        delete pDownload;
    }
    m_Downloads.clear();
    m_HandlerMap.clear();
}

bool CSingularFileDownloadManager::AllComplete()
{
    for (const auto& pDownload : m_Downloads)
    {
        if (!pDownload->GetComplete())
            return false;
    }
    return true;
}

CSingularFileDownload* CSingularFileDownloadManager::FindDownloadByHandler(std::uint32_t handlerId) const
{
    const auto it = m_HandlerMap.find(handlerId);
    return (it != m_HandlerMap.end()) ? it->second : nullptr;
}

bool CSingularFileDownloadManager::AbortDownload(std::uint32_t handlerId)
{
    auto* pDownload = FindDownloadByHandler(handlerId);
    if (!pDownload)
        return false;

    if (pDownload->GetComplete() || pDownload->IsCancelled())
        return false;

    const bool success = pDownload->Cancel();
    RemoveDownload(pDownload);
    return success;
}

void CSingularFileDownloadManager::RemoveDownload(CSingularFileDownload* pDownload)
{
    if (!pDownload)
        return;

    m_HandlerMap.erase(pDownload->GetHandlerId());
    m_Downloads.remove(pDownload);
    pDownload->MarkForDeletion();
}