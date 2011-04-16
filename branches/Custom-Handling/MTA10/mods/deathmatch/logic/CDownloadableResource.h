/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDownloadableResource.h
*  PURPOSE:     Header for downloadable resource class
*  DEVELOPERS:  Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CDOWNLOADABLERESOURCE_H
#define CDOWNLOADABLERESOURCE_H

#ifndef _WINDOWS_

#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce build time by 100ms

#include <windows.h>
#endif

#include <bochs_internal/crc32.h>
#include "CChecksum.h"

class CDownloadableResource
{
public:
    enum eResourceType
    {
        RESOURCE_FILE_TYPE_MAP,
        RESOURCE_FILE_TYPE_SCRIPT,
        RESOURCE_FILE_TYPE_CLIENT_SCRIPT,
        RESOURCE_FILE_TYPE_CONFIG,
        RESOURCE_FILE_TYPE_CLIENT_CONFIG,
        RESOURCE_FILE_TYPE_HTML,
        RESOURCE_FILE_TYPE_CLIENT_FILE,
    };

public:
    CDownloadableResource           ( eResourceType resourceType, const char* szName, const char* szNameShort, CChecksum checksum = CChecksum (), bool bGenerateClientChecksum = false );
    virtual ~CDownloadableResource  ( void );

    bool DoesClientAndServerChecksumMatch ( void );

    eResourceType GetResourceType   ( void ) { return m_resourceType; };
    const char* GetName             ( void ) { return m_szName; };
    const char* GetShortName        ( void ) { return m_szNameShort; };

    // CRC-based methods
    CChecksum GenerateClientChecksum ( void );

    CChecksum GetLastClientChecksum  ( void );

    CChecksum GetServerChecksum      ( void );
    void SetServerChecksum           ( CChecksum serverChecksum );
protected:
    eResourceType       m_resourceType;

    char*               m_szName;
    char*               m_szNameShort;

    CChecksum           m_LastClientChecksum;
    CChecksum           m_ServerChecksum;
};

#endif
