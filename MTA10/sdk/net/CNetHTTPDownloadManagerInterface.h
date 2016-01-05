/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/CNetHTTPDownloadManagerInterface.h
*  PURPOSE:     Network HTTP download interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNETHTTPDOWNLOADMANAGERINTERFACE_H
#define __CNETHTTPDOWNLOADMANAGERINTERFACE_H

#include <windows.h>

// PFN_DOWNLOAD_FINISHED_CALLBACK is called once at the end of the download.
// If bSuccess is true, then pCompletedData/Length will be set or the output file will be ready.
// If bSuccess is false, then iErrorCode and CNetHTTPDownloadManagerInterface->GetError() will reveal the problem.
typedef void (*PFN_DOWNLOAD_FINISHED_CALLBACK) ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode );

class CNetHTTPDownloadManagerInterface
{
public:
    // Get some stats regarding the current download size now & total
    virtual uint            GetDownloadSizeNow  ( void ) = 0;
    virtual void            ResetDownloadSize   ( void ) = 0;

    // Get an error if one has been set
    virtual const char*     GetError            ( void ) = 0;

    // Process the queued files
    // Returns true if all of the downloads have completed, false if there are additional downloads 
    virtual bool            ProcessQueuedFiles  ( void ) = 0;

    // Queue a file to download
    // szPostHeaders is a new line separated list of HTTP headers. Examples at https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    virtual bool            QueueFile           ( const char* szURL, const char* szOutputFile, double dSize = 0, const char* szPostData = NULL, unsigned int uiPostSize = 0, bool bPostBinary = false, void * objectPtr = NULL, PFN_DOWNLOAD_FINISHED_CALLBACK pfnDownloadFinishedCallback = NULL, bool bIsLocal = false, uint uiConnectionAttempts = 10, uint uiConnectTimeoutMs = 10000, bool bCheckContents = false, bool bResumeFile = false, const char* szPostHeaders = NULL ) = 0;

    // Limit number of concurrent http client connections
    virtual void            SetMaxConnections   ( int iMaxConnections ) = 0;

    virtual void            Reset               ( void ) = 0;
};

#endif

