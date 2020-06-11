/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDownloadableResource.cpp
 *  PURPOSE:     Downloadable resource interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include <future>     // return value from SharedUtil::async

#define INVALIDATE_OLD_FUTURE if (m_GenerateChecksumFuture.valid()) m_GenerateChecksumFuture = std::future<CChecksum>();

CDownloadableResource::CDownloadableResource(CResource* pResource, eResourceType resourceType, const char* szName, const char* szNameShort, uint uiDownloadSize,
                                             CChecksum serverChecksum, bool bAutoDownload) :
    m_pResource(pResource),
    m_resourceType(resourceType),
    m_strName(std::string(szName)),
    m_strNameShort(std::string(szNameShort)),
    m_ServerChecksum(serverChecksum),
    m_bAutoDownload(bAutoDownload),
    m_uiDownloadSize(uiDownloadSize)
{
    g_pClientGame->GetResourceManager()->OnAddResourceFile(this);
}

CDownloadableResource::~CDownloadableResource()
{
    g_pClientGame->GetResourceFileDownloadManager()->OnRemoveResourceFile(this);
    g_pClientGame->GetResourceManager()->OnRemoveResourceFile(this);
}

bool CDownloadableResource::DoesClientAndServerChecksumMatch()
{
    MakeSureChecksumIsGenerated();
    return (m_LastClientChecksum == m_ServerChecksum);
}

void CDownloadableResource::GenerateClientChecksumAsync()
{
    // Make sure future is invalidated, so MakeSureChecksumIsGenerated doesnt apply it
    INVALIDATE_OLD_FUTURE

    g_pCore->GetConsole()->Printf("Generating async checksum for %s", GetShortName());
    m_GenerateChecksumFuture = SharedUtil::async([this]() {
        // Use a buffer here instead of `GenerateChecksumFromFile`, because HDD overhead will be
        // Higher than CPU, so not loading the file twice will be faster(especially if it's a big file)
        CBuffer buffer;
        if (buffer.LoadFromFile(m_strName))
            return CChecksum::GenerateChecksumFromBuffer(buffer.GetData(), buffer.GetSize());
    });
}

CChecksum CDownloadableResource::GenerateClientChecksum()
{
    // Make sure future is invalidated, so MakeSureChecksumIsGenerated doesnt apply it
    INVALIDATE_OLD_FUTURE

    m_LastClientChecksum = CChecksum::GenerateChecksumFromFile(m_strName);

    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GenerateClientChecksum(CBuffer& outFileData)
{
    // Make sure future is invalidated, so MakeSureChecksumIsGenerated doesnt apply it
    INVALIDATE_OLD_FUTURE

    // If LoadFromFile fails, a default initialized checksum is returned (just like GenerateClientChecksum() behaves)
    if (outFileData.LoadFromFile(m_strName))
        m_LastClientChecksum = CChecksum::GenerateChecksumFromBuffer(outFileData.GetData(), outFileData.GetSize());
    
    return m_LastClientChecksum;
}

void CDownloadableResource::MakeSureChecksumIsGenerated()
{
    // Await if theres a valid future
    if (m_GenerateChecksumFuture.valid())
        m_LastClientChecksum = m_GenerateChecksumFuture.get();

    // If we still don't have a checksum by now, generate one on this thread
    if (!HasClientChecksumGenerated())
        GenerateClientChecksum(); // This is slow, so try to avoid it
}

bool CDownloadableResource::HasClientChecksumGenerated()
{
    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GetServerChecksum()
{
    return m_ServerChecksum;
}

CChecksum CDownloadableResource::GetLastClientChecksum()
{
    MakeSureChecksumIsGenerated();
    return m_LastClientChecksum; 
}

int CDownloadableResource::GetDownloadPriorityGroup()
{
    return m_pResource->GetDownloadPriorityGroup();
}
