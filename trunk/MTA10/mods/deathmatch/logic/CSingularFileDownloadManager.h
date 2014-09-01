/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSingularFileDownloadManager.h
*  PURPOSE:     Header for downloading single files
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


#ifndef CSINGULARFILEDOWNLOADMANAGER_H
#define CSINGULARFILEDOWNLOADMANAGER_H

#ifndef _WINDOWS_

#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce build time by 100ms

#include <windows.h>
#endif

#include <bochs_internal/crc32.h>
#include "CChecksum.h"
#include "CSingularFileDownload.h"

class CSingularFileDownloadManager
{

public:
    
                                CSingularFileDownloadManager    ( void );
                                ~CSingularFileDownloadManager   ( void );

    CSingularFileDownload*      AddFile                         ( CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL, CChecksum checksum = CChecksum () );
    void                        CancelResourceDownloads         ( CResource* pResource );

    void                        ClearList                       ( void );

    bool                        AllComplete                     ( void );


protected:

    std::list < CSingularFileDownload* >    m_Downloads;

};





#endif