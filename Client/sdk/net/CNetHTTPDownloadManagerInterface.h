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

struct SHttpRequestOptions
{
    bool bIsLegacy = false;         // true = old error behaviour
    SString strPostData;
    bool bPostBinary = false;       // false = truncate data to first null character and send as text/plain (true = send as application/octet-stream)
    uint uiConnectionAttempts = 10;
    uint uiConnectTimeoutMs = 10000;
    SString strRequestMethod;
    std::map<SString,SString> requestHeaders;
    uint uiMaxRedirects = 8;
    SString strUsername;
    SString strPassword;
};

struct SHttpDownloadResult
{
    const char* pData;
    size_t dataSize;
    void* pObj;
    bool bSuccess;
    int iErrorCode;
    std::map<SString,SString> headers;
};

// PFN_DOWNLOAD_FINISHED_CALLBACK is called once at the end of the download.
// If bSuccess is true, then pData+dataSize will be set or the output file will be ready.
// If bSuccess is false, then iErrorCode and CNetHTTPDownloadManagerInterface->GetError() will reveal the problem.
// When iErrorCode is 400-599:
//      If bIsLegacy is true, then bSuccess is false and pData+dataSize contain nothing
//      If bIsLegacy is false, then bSuccess is true and pData+dataSize contain the server response
typedef void (*PFN_DOWNLOAD_FINISHED_CALLBACK) ( const SHttpDownloadResult& result );

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
    virtual bool            QueueFile           ( const char* szURL, const char* szOutputFile, const char* szPostData = NULL, unsigned int uiPostSize = 0, bool bPostBinary = false, void * objectPtr = NULL, PFN_DOWNLOAD_FINISHED_CALLBACK pfnDownloadFinishedCallback = NULL, bool bIsLocal = false, uint uiConnectionAttempts = 10, uint uiConnectTimeoutMs = 10000, bool bCheckContents = false, bool bResumeFile = false ) = 0;
    virtual bool            QueueFile           ( const char* szURL, const char* szOutputFile, void * objectPtr = NULL, PFN_DOWNLOAD_FINISHED_CALLBACK pfnDownloadFinishedCallback = NULL, bool bIsLocal = false, const SHttpRequestOptions& options = SHttpRequestOptions(), bool bCheckContents = false, bool bResumeFile = false ) = 0;

    // Limit number of concurrent http client connections
    virtual void            SetMaxConnections   ( int iMaxConnections ) = 0;

    virtual void            Reset               ( void ) = 0;
};

#endif

