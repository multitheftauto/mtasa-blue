/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDownloadableResource.cpp
 *  PURPOSE:     Downloadable resource interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

static bool    s_bChecksumBatchActive = false;
static int64_t s_checksumBatchAccumMs = 0;

static bool IsEmptyFileChecksum(const CChecksum& checksum)
{
    static const CChecksum emptyFileChecksum = CChecksum::GenerateChecksumFromBuffer("", 0);
    return checksum == emptyFileChecksum;
}

// A download that is still writing the file makes it read as zero bytes, whose checksum is a valid
// but meaningless value. Retry briefly so a race is not reported to the player as a file mismatch.
// A file that does not exist yet checksums to CChecksum() instead, so it never enters this path.
static CChecksum GenerateChecksumWithEmptyFileRetry(const SString& strFilename, uint uiExpectedSize)
{
    constexpr int MAX_ATTEMPTS = 5;
    constexpr int RETRY_DELAY_MS = 20;

    CChecksum checksum = CChecksum::GenerateChecksumFromFileUnsafe(strFilename);

    if (uiExpectedSize == 0)
        return checksum;

    for (int attempt = 1; attempt < MAX_ATTEMPTS && IsEmptyFileChecksum(checksum); ++attempt)
    {
        Sleep(RETRY_DELAY_MS);
        CChecksum::InvalidateChecksumCacheEntry(strFilename);
        checksum = CChecksum::GenerateChecksumFromFileUnsafe(strFilename);
    }

    return checksum;
}

void CDownloadableResource::BeginChecksumBatch()
{
    if (!s_bChecksumBatchActive)
    {
        s_bChecksumBatchActive = true;
        s_checksumBatchAccumMs = 0;
    }
}

void CDownloadableResource::EndChecksumBatch()
{
    s_bChecksumBatchActive = false;
    s_checksumBatchAccumMs = 0;
}

CDownloadableResource::CDownloadableResource(CResource* pResource, eResourceType resourceType, const char* szName, const char* szNameShort, uint uiDownloadSize,
                                             CChecksum serverChecksum, bool bAutoDownload)
{
    m_pResource = pResource;
    m_resourceType = resourceType;
    m_strName = szName;
    m_strNameShort = szNameShort;
    m_ServerChecksum = serverChecksum;
    m_bAutoDownload = bAutoDownload;
    m_bInDownloadQueue = false;
    m_bDownloaded = false;
    m_uiDownloadSize = uiDownloadSize;
    m_uiHttpServerIndex = 0;
    m_bModifedByScript = false;
    m_bClientChecksumVerified = false;

    GenerateClientChecksum();
    g_pClientGame->GetResourceManager()->OnAddResourceFile(this);
}

CDownloadableResource::~CDownloadableResource()
{
    g_pClientGame->GetResourceFileDownloadManager()->OnRemoveResourceFile(this);
    g_pClientGame->GetResourceManager()->OnRemoveResourceFile(this);
}

bool CDownloadableResource::DoesClientAndServerChecksumMatch()
{
    return !m_bClientChecksumVerified || (m_LastClientChecksum == m_ServerChecksum);
}

CChecksum CDownloadableResource::GenerateClientChecksum()
{
    constexpr int64_t BATCH_BUDGET_MS = 5000;

    if (s_bChecksumBatchActive && s_checksumBatchAccumMs >= BATCH_BUDGET_MS)
    {
        m_LastClientChecksum = CChecksum();
        m_bClientChecksumVerified = false;
        return m_LastClientChecksum;
    }

    long long startMs = GetTickCount64_();
    m_LastClientChecksum = GenerateChecksumWithEmptyFileRetry(m_strName, m_uiDownloadSize);
    m_bClientChecksumVerified = true;

    if (s_bChecksumBatchActive)
        s_checksumBatchAccumMs += GetTickCount64_() - startMs;

    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GenerateClientChecksum(CBuffer& outFileData)
{
    if (outFileData.LoadFromFile(m_strName))
        m_LastClientChecksum = CChecksum::GenerateChecksumFromBuffer(outFileData.GetData(), outFileData.GetSize());
    else
        m_LastClientChecksum = CChecksum();

    m_bClientChecksumVerified = true;

    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GetLastClientChecksum()
{
    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GetServerChecksum()
{
    return m_ServerChecksum;
}

int CDownloadableResource::GetDownloadPriorityGroup()
{
    return m_pResource->GetDownloadPriorityGroup();
}
