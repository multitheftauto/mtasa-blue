/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDownloadableResource.cpp
*  PURPOSE:     Downloadable resource interface
*  DEVELOPERS:  Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CDownloadableResource::CDownloadableResource ( CResource* pResource, eResourceType resourceType, const char* szName, const char* szNameShort, uint uiDownloadSize, CChecksum serverChecksum, bool bAutoDownload )
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

    GenerateClientChecksum ();
    g_pClientGame->GetResourceManager()->OnAddResourceFile( this );
}

CDownloadableResource::~CDownloadableResource ( void )
{
    g_pClientGame->GetResourceFileDownloadManager()->OnRemoveResourceFile( this );
    g_pClientGame->GetResourceManager()->OnRemoveResourceFile( this );
}

bool CDownloadableResource::DoesClientAndServerChecksumMatch ( void )
{
    return ( m_LastClientChecksum == m_ServerChecksum );
}

CChecksum CDownloadableResource::GenerateClientChecksum ( void )
{
    m_LastClientChecksum = CChecksum::GenerateChecksumFromFile ( m_strName );
    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GetServerChecksum ( void )
{
    return m_ServerChecksum;
}

int CDownloadableResource::GetDownloadPriorityGroup ( void )
{
    return m_pResource->GetDownloadPriorityGroup();
}
