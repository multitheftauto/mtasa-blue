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
using std::list;

CSingularFileDownloadManager::CSingularFileDownloadManager ( void )
{
}


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

void CSingularFileDownloadManager::ClearList ( void )
{
    list < CSingularFileDownload* > ::const_iterator iter = m_Downloads.begin();
    for ( ; iter != m_Downloads.end(); ++iter )
    {
        delete *iter;
    }
    m_Downloads.clear();
}


bool CSingularFileDownloadManager::AllComplete ( void )
{
    list < CSingularFileDownload* > ::const_iterator iter = m_Downloads.begin();
    for ( ; iter != m_Downloads.end(); ++iter )
    {
        if ( !(*iter)->GetComplete() )
            return false;
    }
    return true;
}