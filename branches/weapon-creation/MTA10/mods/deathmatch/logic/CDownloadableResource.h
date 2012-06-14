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
    CDownloadableResource           ( eResourceType resourceType, const char* szName, const char* szNameShort, CChecksum checksum = CChecksum (), bool bGenerateClientChecksum = false, bool bAutoDownload = true );
    virtual ~CDownloadableResource  ( void );

    bool DoesClientAndServerChecksumMatch ( void );

    eResourceType GetResourceType   ( void ) { return m_resourceType; };
    const char* GetName             ( void ) { return m_strName; };
    const char* GetShortName        ( void ) { return m_strNameShort; };

    // CRC-based methods
    CChecksum GenerateClientChecksum ( void );

    CChecksum GetLastClientChecksum  ( void );

    CChecksum GetServerChecksum      ( void );
    void SetServerChecksum           ( CChecksum serverChecksum );

    inline bool     IsAutoDownload  ( void )    { return m_bAutoDownload; };
protected:
    eResourceType       m_resourceType;

    SString             m_strName;
    SString             m_strNameShort;

    CChecksum           m_LastClientChecksum;
    CChecksum           m_ServerChecksum;

    bool                m_bAutoDownload;
};

#endif
