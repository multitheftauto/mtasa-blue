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

CDownloadableResource::CDownloadableResource ( eResourceType resourceType, const char* szName, const char* szNameShort, CChecksum serverChecksum, bool bGenerateClientChecksum )
{
    // Store the resource type
    m_resourceType = resourceType;

    // Store the name
    size_t sizeName = strlen ( szName );
    m_szName = new char [ sizeName + 1 ];
    strcpy ( m_szName, szName );
    m_szName[sizeName] = '\0';

    // Store the  name (short)
    size_t sizeNameShort = strlen ( szNameShort );
    m_szNameShort = new char [ sizeNameShort + 1 ];
    strcpy ( m_szNameShort, szNameShort );
    m_szNameShort[sizeNameShort] = '\0';

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
}

CDownloadableResource::~CDownloadableResource ( void )
{
    if ( m_szName )
    {
        delete [] m_szName;
        m_szName = 0;
    }

    if ( m_szNameShort )
    {
        delete [] m_szNameShort;
        m_szNameShort = 0;
    }
}

bool CDownloadableResource::DoesClientAndServerChecksumMatch ( void )
{
    return ( m_LastClientChecksum.CompareWithLegacy ( m_ServerChecksum ) );
}

CChecksum CDownloadableResource::GenerateClientChecksum ( void )
{
    WIN32_FIND_DATA fdInfo;
    if ( INVALID_HANDLE_VALUE != FindFirstFile( m_szName, &fdInfo ) )
    {
        m_LastClientChecksum = CChecksum::GenerateChecksumFromFile ( m_szName );
    }
    else
    {
        // Reset the last client checksum, as the  does not exist
        m_LastClientChecksum = CChecksum ();
    }

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
