/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSingularFileDownload.h
 *  PURPOSE:     Header for singular file download
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
#include "net/CNetHTTPDownloadManagerInterface.h"
#include <cstdint>

class CSingularFileDownload
{
public:
    CSingularFileDownload(CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL, CResource* pRequestResource,
                          CChecksum serverChecksum, std::uint32_t handlerId);
    ~CSingularFileDownload();

    static void DownloadFinishedCallBack(const SHttpDownloadResult& result);

    bool DoesClientAndServerChecksumMatch();

    const char* GetName() const noexcept { return m_strName; }
    const char* GetShortName() const noexcept { return m_strNameShort; }

    CResource* GetResource() const noexcept { return resource; }
    std::uint32_t GetHandlerId() const noexcept { return handlerId; }

    void SetComplete() noexcept { complete = true; }
    bool GetComplete() const noexcept { return complete; }
    bool IsCancelled() const noexcept { return cancelled; }

    void CallFinished(bool bSuccess);
    bool Cancel();
    void MarkForDeletion();

    CChecksum GenerateClientChecksum();

protected:
    SString m_strName;
    SString m_strNameShort;

    CResource* resource;
    CResource* requestResource;

    bool complete;
    bool beingDeleted;
    bool cancelled;

    std::uint32_t handlerId;
    CNetHTTPDownloadManagerInterface* httpManager;
    EDownloadMode::EDownloadModeType downloadMode;

    CChecksum m_LastClientChecksum;
    CChecksum m_ServerChecksum;
};
