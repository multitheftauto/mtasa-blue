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

typedef bool (*PDOWNLOADPROGRESSCALLBACK) ( double dDownloadNow, double dDownloadTotal, char* pCompletedData, size_t completedLength, void *pObj, bool bComplete, int iError );

class CNetHTTPDownloadManagerInterface
{
public:
    // Get some stats regarding the current download size now & total
    virtual double          GetDownloadSizeNow  ( void ) = 0;

    // Get an error if one has been set
    virtual const char*     GetError            ( void ) = 0;

    // Process the queued files
    // Returns true if all of the downloads have completed, false if there are additional downloads 
    virtual bool            ProcessQueuedFiles  ( void ) = 0;

    // Queue a file to download
    virtual bool            QueueFile           ( const char* szURL, const char* szOutputFile, double dSize = 0, const char* szPostData = NULL, unsigned int uiPostSize = 0, bool bPostBinary = false, void * objectPtr = NULL, PDOWNLOADPROGRESSCALLBACK pfnDownloadProgressCallback = NULL, bool bIsLocal = false, uint uiConnectionAttempts = 10, uint uiConnectTimeoutMs = 10000, bool bCheckContents = false ) = 0;

    // Limit number of concurrent http client connections
    virtual void            SetMaxConnections   ( int iMaxConnections ) = 0;
};

#endif

