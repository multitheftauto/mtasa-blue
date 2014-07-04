/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/CNetHTTPDownloadManagerInterface.h
*  PURPOSE:     Net server module HTTP download manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNETHTTPDOWNLOADMANAGERINTERFACE_H
#define __CNETHTTPDOWNLOADMANAGERINTERFACE_H

#include <string.h>
#include <stdio.h>

typedef void (*PDOWNLOADPROGRESSCALLBACK) ( double, double, char *, size_t, void *, bool, int );

class CNetHTTPDownloadManagerInterface
{
public:
    enum eHTTPDownloadManagerError
    {
        UNKNOWN_ERROR,
        INVALID_FILE_DESCRIPTORS,
        INVALID_MAX_FILE_DESCRIPTOR,
        INVALID_SELECT_RETURN,
        INVALID_INITIAL_MULTI_PERFORM,
        INVALID_MULTI_PERFORM_CODE,
        INVALID_MULTI_PERFORM_CODE_NEW_DOWNLOADS,
        UNEXPECTED_CURL_MESSAGE,
        UNABLE_TO_CONNECT, // 8
        UNABLE_TO_DOWNLOAD_FILE, // 9
        FAILED_TO_INITIALIZE_DOWNLOAD
    };

    // Cleanup after downloading the queued files
    // This should only be called after you finish the ProcessQueuedFiles loop
    virtual void CleanupAfterDownloadingQueuedFiles ( void ) = 0;

    // Download a single file

    // Get some stats regarding the current download size now & total
    virtual double GetDownloadSizeNow ( void ) = 0;

    // Get an error if one has been set
    virtual const char* GetError ( void ) = 0;

    virtual bool IsDownloading ( void ) = 0;

    // Process the queued files
    // Returns true if all of the downloads have completed, false if there is additional downloads 
    virtual bool ProcessQueuedFiles ( void ) = 0;

    // Queue a file to download
    virtual bool QueueFile ( const char* szURL, const char* szOutputFile, double dSize = 0, const char * szPostData = NULL, void * objectPtr = NULL, PDOWNLOADPROGRESSCALLBACK pfnDownloadProgressCallback = NULL, bool bIsLocal = false ) = 0;

    // Start downloading the queued files
    virtual void StartDownloadingQueuedFiles ( void ) = 0;

    // Limit number of concurrent http client connections
    virtual void SetMaxConnections ( int iMaxConnections ) = 0;
};

#endif

