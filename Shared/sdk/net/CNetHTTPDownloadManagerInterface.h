/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/net/CNetHTTPDownloadManagerInterface.h
 *  PURPOSE:     Network HTTP download interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <map>
#include <cassert>
#include "SString.h"
#include "SharedUtil.IntTypes.h"

struct SHttpRequestOptions
{
    SHttpRequestOptions() {}
    SHttpRequestOptions(const struct SHttpRequestOptionsTx& in);
    bool    bIsLegacy = false;                 // true = sets CURLOPT_FAILONERROR
    bool    bIsLocal = false;                  // false = download aborts if < 10 bytes/s for uiConnectTimeoutMs
    bool    bCheckContents = false;            // true = check for naughty things before saving file
    bool    bResumeFile = false;               // true = attempt to resume previously interrupted download
    SString strPostData;
    bool    bPostBinary = false;                      // false = truncate strPostData to first null character and send as text/plain
                                                      // (true = send as application/octet-stream)
    std::map<SString, SString> formFields;            // If set, send items as multipart/formdata (and ignore strPostData)
    uint                       uiConnectionAttempts = 10;
    uint                       uiConnectTimeoutMs = 10000;
    SString                    strRequestMethod;
    std::map<SString, SString> requestHeaders;
    uint                       uiMaxRedirects = 8;
    SString                    strUsername;
    SString                    strPassword;
};

struct SHttpDownloadResult
{
    const char* pData;
    size_t      dataSize;
    void*       pObj;
    bool        bSuccess;
    int         iErrorCode;
    const char* szHeaders;
    uint        uiAttemptNumber;
    uint        uiContentLength;
};

// For transferring SString to net module
struct SStringContent
{
    void operator=(const SString& other)
    {
        length = other.length();
        pData = *other;
    }
                operator SString() const { return SStringX(pData, length); }
    size_t      length = 0;
    const char* pData = nullptr;
};

// For transferring std::map<SString,SString> to net module
struct SStringMapContent
{
    ~SStringMapContent() { delete[] pItems; }
    void operator=(const std::map<SString, SString>& other)
    {
        numItems = other.size() * 2;
        pItems = new SStringContent[numItems];
        size_t pos = 0;
        for (const auto& iter : other)
        {
            pItems[pos++] = iter.first;
            pItems[pos++] = iter.second;
        }
        assert(pos == numItems);
    }
    operator std::map<SString, SString>() const
    {
        std::map<SString, SString> result;
        for (size_t i = 0; i < numItems; i += 2)
        {
            result[pItems[i]] = pItems[i + 1];
        }
        return result;
    }
    size_t          numItems = 0;
    SStringContent* pItems = nullptr;
};

// For transferring SHttpRequestOptions to net module
struct SHttpRequestOptionsTx
{
    SHttpRequestOptionsTx() {}
    SHttpRequestOptionsTx(const SHttpRequestOptions& in);
    bool              bIsLegacy = false;
    bool              bIsLocal = false;
    bool              bCheckContents = false;
    bool              bResumeFile = false;
    SStringContent    strPostData;
    bool              bPostBinary = false;
    SStringMapContent formFields;
    uint              uiConnectionAttempts = 10;
    uint              uiConnectTimeoutMs = 10000;
    SStringContent    strRequestMethod;
    SStringMapContent requestHeaders;
    uint              uiMaxRedirects = 8;
    SStringContent    strUsername;
    SStringContent    strPassword;
};

// Convert SHttpRequestOptionsTx to SHttpRequestOptions
inline SHttpRequestOptions::SHttpRequestOptions(const SHttpRequestOptionsTx& in)
{
    bIsLegacy = in.bIsLegacy;
    bIsLocal = in.bIsLocal;
    bCheckContents = in.bCheckContents;
    bResumeFile = in.bResumeFile;
    strPostData = in.strPostData;
    bPostBinary = in.bPostBinary;
    formFields = in.formFields;
    uiConnectionAttempts = in.uiConnectionAttempts;
    uiConnectTimeoutMs = in.uiConnectTimeoutMs;
    strRequestMethod = in.strRequestMethod;
    requestHeaders = in.requestHeaders;
    uiMaxRedirects = in.uiMaxRedirects;
    strUsername = in.strUsername;
    strPassword = in.strPassword;
}

// Convert SHttpRequestOptions to SHttpRequestOptionsTx
inline SHttpRequestOptionsTx::SHttpRequestOptionsTx(const SHttpRequestOptions& in)
{
    bIsLegacy = in.bIsLegacy;
    bIsLocal = in.bIsLocal;
    bCheckContents = in.bCheckContents;
    bResumeFile = in.bResumeFile;
    strPostData = in.strPostData;
    bPostBinary = in.bPostBinary;
    formFields = in.formFields;
    uiConnectionAttempts = in.uiConnectionAttempts;
    uiConnectTimeoutMs = in.uiConnectTimeoutMs;
    strRequestMethod = in.strRequestMethod;
    requestHeaders = in.requestHeaders;
    uiMaxRedirects = in.uiMaxRedirects;
    strUsername = in.strUsername;
    strPassword = in.strPassword;
}

struct SDownloadStatus
{
    uint uiAttemptNumber = 0;            // 0=Queued 1+=Downloading
    uint uiContentLength = 0;            // Item total size. Will be 0 if http header 'Content-Length' is missing
    uint uiBytesReceived = 0;            // Download progress
};

// PFN_DOWNLOAD_FINISHED_CALLBACK is called once at the end of the download.
// If bSuccess is true, then pData+dataSize will be set or the output file will be ready.
// If bSuccess is false, then iErrorCode and CNetHTTPDownloadManagerInterface->GetError() will reveal the problem.
// When iErrorCode is 400-599:
//      If bIsLegacy is true, then bSuccess is false and pData+dataSize contain nothing
//      If bIsLegacy is false, then bSuccess is true and pData+dataSize contain the server response
typedef void (*PFN_DOWNLOAD_FINISHED_CALLBACK)(const SHttpDownloadResult& result);

class CNetHTTPDownloadManagerInterface
{
public:
    // Get some stats regarding the current download size now & total
    virtual uint GetDownloadSizeNow() = 0;
    virtual void ResetDownloadSize() = 0;

    // Get an error if one has been set
    virtual const char* GetError() = 0;

    // Process the queued files
    // Returns true if all of the downloads have completed, false if there are additional downloads
    virtual bool ProcessQueuedFiles() = 0;

    // Queue a file to download
    virtual bool QueueFile(const char* szURL, const char* szOutputFile, void* objectPtr = NULL,
                           PFN_DOWNLOAD_FINISHED_CALLBACK pfnDownloadFinishedCallback = NULL,
                           const SHttpRequestOptionsTx&   options = SHttpRequestOptionsTx()) = 0;

    // Limit number of concurrent http client connections
    virtual void SetMaxConnections(int iMaxConnections) = 0;

    virtual void Reset() = 0;

    // objectPtr and pfnDownloadFinishedCallback are used to identify the download and should be the same as when QueueFile was originally called
    virtual bool CancelDownload(void* objectPtr, PFN_DOWNLOAD_FINISHED_CALLBACK pfnDownloadFinishedCallback) = 0;
    virtual bool GetDownloadStatus(void* objectPtr, PFN_DOWNLOAD_FINISHED_CALLBACK pfnDownloadFinishedCallback, SDownloadStatus& outDownloadStatus) = 0;
};
