/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSingularFileDownloadManager.cpp
 *  PURPOSE:     Singular file download manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
using std::list;

CSingularFileDownloadManager::CSingularFileDownloadManager()
{
}

CSingularFileDownloadManager::~CSingularFileDownloadManager()
{
    // clear list
    ClearList();
}

CSingularFileDownload* CSingularFileDownloadManager::AddFile(CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL,
                                                             CResource* pRequestResource, CChecksum checksum)
{
    CSingularFileDownload* pFile = new CSingularFileDownload(pResource, szName, szNameShort, strHTTPURL, pRequestResource, checksum);
    m_Downloads.push_back(pFile);
    return NULL;
}

void CSingularFileDownloadManager::CancelResourceDownloads(CResource* pResource)
{
    list<CSingularFileDownload*>::const_iterator iter = m_Downloads.begin();
    for (; iter != m_Downloads.end(); ++iter)
    {
        if ((*iter)->GetResource() == pResource)
            (*iter)->Cancel();
    }
}

void CSingularFileDownloadManager::ClearList()
{
    list<CSingularFileDownload*>::const_iterator iter = m_Downloads.begin();
    for (; iter != m_Downloads.end(); ++iter)
    {
        delete *iter;
    }
    m_Downloads.clear();
}

bool CSingularFileDownloadManager::AllComplete()
{
    list<CSingularFileDownload*>::const_iterator iter = m_Downloads.begin();
    for (; iter != m_Downloads.end(); ++iter)
    {
        if (!(*iter)->GetComplete())
            return false;
    }
    return true;
}