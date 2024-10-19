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

CDownloadableResource::CDownloadableResource(CResource* pResource, eResourceType resourceType, const char* szName, const char* szNameShort, uint uiDownloadSize,
                                             CChecksum serverChecksum, bool bAutoDownload)
{
    switch (resourceType)
    {
        case eResourceType::RESOURCE_FILE_TYPE_SCRIPT:
        case eResourceType::RESOURCE_FILE_TYPE_CLIENT_SCRIPT:
            m_resourceCategoryType = eResourceCategory::SCRIPTS;
        case eResourceType::RESOURCE_FILE_TYPE_MAP:
            m_resourceCategoryType = eResourceCategory::MAPS;
        case eResourceType::RESOURCE_FILE_TYPE_CONFIG:
        case eResourceType::RESOURCE_FILE_TYPE_CLIENT_CONFIG:
            m_resourceCategoryType = eResourceCategory::CONFIGS;
        case eResourceType::RESOURCE_FILE_TYPE_CLIENT_FILE:
        case eResourceType::RESOURCE_FILE_TYPE_HTML:
            m_resourceCategoryType = eResourceCategory::FILES;
    }

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
    return (m_LastClientChecksum == m_ServerChecksum);
}

CChecksum CDownloadableResource::GenerateClientChecksum()
{
    m_LastClientChecksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strName);
    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GenerateClientChecksum(CBuffer& outFileData)
{
    // If LoadFromFile fails, a default initialized checksum is returned (just like GenerateClientChecksum() behaves)
    if (outFileData.LoadFromFile(m_strName))
        m_LastClientChecksum = CChecksum::GenerateChecksumFromBuffer(outFileData.GetData(), outFileData.GetSize());

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
