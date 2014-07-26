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

CDownloadableResource::CDownloadableResource ( eResourceType resourceType, const char* szName, const char* szNameShort, CChecksum serverChecksum, bool bGenerateClientChecksum, bool bAutoDownload )
{
    // Store the resource type
    m_resourceType = resourceType;

    // Store the name
    m_strName = szName;

    // Store the  name (short)
    m_strNameShort = szNameShort;

    // Store the server checksum
    m_ServerChecksum = serverChecksum;

    // Check to see if the client checksum should be generated now
    if ( bGenerateClientChecksum )
    {
        GenerateClientChecksum ();
    }
    else
    {
        // Default the last client checksum
        m_LastClientChecksum = CChecksum ();
    }

    m_bAutoDownload = bAutoDownload;
    g_pClientGame->GetResourceManager()->OnAddResourceFile( this );
}

CDownloadableResource::~CDownloadableResource ( void )
{
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

CChecksum CDownloadableResource::GetLastClientChecksum ( void )
{
    return m_LastClientChecksum;
}

CChecksum CDownloadableResource::GetServerChecksum ( void )
{
    return m_ServerChecksum;
}

void CDownloadableResource::SetServerChecksum ( CChecksum serverChecksum )
{
    m_ServerChecksum = serverChecksum;
}
