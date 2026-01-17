/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSingularFileDownloadManager.h
 *  PURPOSE:     Header for downloading single files
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
#include "CSingularFileDownload.h"

class CSingularFileDownloadManager
{
public:
    CSingularFileDownloadManager();
    ~CSingularFileDownloadManager();

    CSingularFileDownload* AddFile(CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL, CResource* pRequestResource,
                                   CChecksum checksum = CChecksum());
    void                   CancelResourceDownloads(CResource* pResource);

    void ClearList();

    bool AllComplete();

protected:
    std::list<CSingularFileDownload*> m_Downloads;
};
