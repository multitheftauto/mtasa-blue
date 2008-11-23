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

CDownloadableResource::CDownloadableResource ( eResourceType resourceType, const char* szName, const char* szNameShort, unsigned long ulServerCRC, bool bGenerateClientCRC, CRCGenerator* pCRCGen )
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

    // Store the server CRC
    m_ulServerCRC = ulServerCRC;

    // Check to see if the client CRC should be generated now
    if ( bGenerateClientCRC )
    {
        // Was a CRC Generator passed in?
        if ( pCRCGen )
        {
            this->GenerateClientCRC ( pCRCGen );
        }
        else
        {
            this->GenerateClientCRC ();
        }
    }
    else
    {
        // Default the last client CRC
        m_ulLastClientCRC = 0;
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

bool CDownloadableResource::DoesClientAndServerCRCMatch ( void )
{
    return ( m_ulLastClientCRC == m_ulServerCRC );
}

unsigned long CDownloadableResource::GenerateClientCRC ( void )
{
    CRCGenerator* pCRCGen = CRCGenerator::GetInstance();

    return GenerateClientCRC ( pCRCGen );
}

unsigned long CDownloadableResource::GenerateClientCRC ( CRCGenerator* pCRCGen )
{
    WIN32_FIND_DATA fdInfo;
    if ( INVALID_HANDLE_VALUE != FindFirstFile( m_szName, &fdInfo ) )
    {
        m_ulLastClientCRC = pCRCGen->GetCRC ( m_szName );
    }
    else
    {
        // Reset the last client CRC, as the  does not exist
        m_ulLastClientCRC = 0;
    }

    return m_ulLastClientCRC;
}

unsigned long CDownloadableResource::GetLastClientCRC ( void )
{
    return m_ulLastClientCRC;
}

unsigned long CDownloadableResource::GetServerCRC ( void )
{
    return m_ulServerCRC;
}

void CDownloadableResource::SetServerCRC ( unsigned long ulServerCRC )
{
    m_ulServerCRC = ulServerCRC;
}
