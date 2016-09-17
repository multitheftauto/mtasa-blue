/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSingularFileDownloadManager.cpp
*  PURPOSE:     Singular file download manager interface
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CSingularFileDownloadManager::~CSingularFileDownloadManager ( void )
{
    // clear list
    ClearList ();
}

CSingularFileDownload* CSingularFileDownloadManager::AddFile ( CResource* pResource, const char *szName, const char *szNameShort, SString strHTTPURL, CChecksum checksum )
{
    CSingularFileDownload* pFile = new CSingularFileDownload ( pResource, szName, szNameShort, strHTTPURL, checksum );
    m_Downloads.push_back ( pFile );
    return NULL;
}

void CSingularFileDownloadManager::CancelResourceDownloads ( CResource* pResource )
{
    for ( auto& pDownload : m_Downloads )
    {
        if ( pDownload->GetResource () == pResource )
            pDownload->Cancel ();
    }
}

void CSingularFileDownloadManager::ClearList ( void )
{
    for (auto& pDownload : m_Downloads)
    {
        delete pDownload;
    }
    m_Downloads.clear();
}


bool CSingularFileDownloadManager::AllComplete ( void )
{
    for (auto& pDownload : m_Downloads)
    {
        if ( !pDownload->GetComplete() )
            return false;
    }
    return true;
}
