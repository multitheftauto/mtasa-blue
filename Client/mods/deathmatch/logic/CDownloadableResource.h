/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDownloadableResource.h
 *  PURPOSE:     Header for downloadable resource class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifndef _WINDOWS_

#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce build time by 100ms

#include <windows.h>
#endif

#include <bochs_internal/bochs_crc32.h>
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
    CDownloadableResource(CResource* pResource, eResourceType resourceType, const char* szName, const char* szNameShort, uint uiDownloadSize,
                          CChecksum serverChecksum, bool bAutoDownload);
    virtual ~CDownloadableResource();

    bool DoesClientAndServerChecksumMatch();

    eResourceType GetResourceType() { return m_resourceType; };
    const char*   GetName() { return m_strName; };
    const char*   GetShortName() { return m_strNameShort; };
    CResource*    GetResource() { return m_pResource; }
    int           GetDownloadPriorityGroup();
    uint          GetDownloadSize() { return m_uiDownloadSize; }
    uint          GetHttpServerIndex() { return m_uiHttpServerIndex; }
    void          SetHttpServerIndex(uint uiHttpServerIndex) { m_uiHttpServerIndex = uiHttpServerIndex; }

    CChecksum GenerateClientChecksum();
    CChecksum GenerateClientChecksum(CBuffer& outFileData);
    CChecksum GetServerChecksum();

    bool IsAutoDownload() { return m_bAutoDownload; };
    void SetDownloaded() { m_bDownloaded = true; };
    bool IsDownloaded() { return m_bDownloaded; };
    void SetIsWaitingForDownload(bool bInDownloadQueue) { m_bInDownloadQueue = bInDownloadQueue; };
    bool IsWaitingForDownload() { return m_bInDownloadQueue; };
    void SetModifedByScript(bool bModifedByScript) { m_bModifedByScript = bModifedByScript; };
    bool IsModifedByScript() { return m_bModifedByScript; };

protected:
    CResource*    m_pResource;
    eResourceType m_resourceType;

    SString m_strName;
    SString m_strNameShort;

    CChecksum m_LastClientChecksum;
    CChecksum m_ServerChecksum;

    bool m_bAutoDownload;
    bool m_bInDownloadQueue;            // File in auto download queue
    bool m_bDownloaded;                 // File has been downloaded and is ready to use
    uint m_uiDownloadSize;
    uint m_uiHttpServerIndex;
    bool m_bModifedByScript;
};
