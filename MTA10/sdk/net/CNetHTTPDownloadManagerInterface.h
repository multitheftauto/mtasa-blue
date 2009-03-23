/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/net/CNetHTTPDownloadManagerInterface.h
*  PURPOSE:		Network HTTP download interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNETHTTPDOWNLOADMANAGERINTERFACE_H
#define __CNETHTTPDOWNLOADMANAGERINTERFACE_H

#include <windows.h>

typedef void (*PDOWNLOADPROGRESSCALLBACK) ( double, double, char*, size_t, void*, bool, int );

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
    virtual void DownloadFile ( const char* szURL, const char* szOutputFile, double dSize = 0, PDOWNLOADPROGRESSCALLBACK pfnDownloadProgressCallback = NULL, bool bIsLocal = false ) = 0;

    // Get some stats regarding the current download size now & total
    virtual double GetDownloadSizeNow ( void ) = 0;
    virtual double GetDownloadSizeTotal ( void ) = 0;

    // Get an error if one has been set
    virtual eHTTPDownloadManagerError GetErrorNum ( void ) = 0;
    virtual const char* GetError ( void ) = 0;
    virtual char* GetErrorInfo ( void ) = 0;

    // Get the number of queued files
    virtual int GetNumberOfQueuedFiles ( void ) = 0;
    virtual int GetNumberOfQueuedFilesRemaining ( void ) = 0;

    virtual bool GetSingleDownloadOption ( void ) = 0;

    virtual bool IsDownloading ( void ) = 0;

    // Process the queued files
    // Returns true if all of the downloads have completed, false if there is additional downloads 
    virtual bool ProcessQueuedFiles ( void ) = 0;

    // Queue a file to download
    virtual bool QueueFile ( const char* szURL, const char* szOutputFile, double dSize = 0, PDOWNLOADPROGRESSCALLBACK pfnDownloadProgressCallback = NULL, bool bIsLocal = false ) = 0;
	virtual bool QueueFile ( const char* szURL, const char* szOutputFile, double dSize = 0, char* szPostData = NULL, void * objectPtr = NULL, PDOWNLOADPROGRESSCALLBACK pfnDownloadProgressCallback = NULL, bool bIsLocal = false ) = 0;

    // Sets the single download option
    // If true, only one file will be downloaded at any given time, regardless of how many files have been queued.  When one file finishes, the next file will be started
    // If false, all queued files will be downloaded at the any time
    virtual void SetSingleDownloadOption ( bool bSingleDownload ) = 0;

    // Start downloading the queued files
    // Returns true if the downloads were started, false if an error occured
    virtual bool StartDownloadingQueuedFiles ( void ) = 0;




    // Increment the download counters
    // NOTE: Do not call this, it is only exposed on a public level to provide access via a weird circular reference in the implementation
    virtual void IncrementDownloadSizeNow ( double dDownloadSizeToAdd ) = 0;
    virtual void IncrementDownloadSizeTotal ( double dDownloadSizeToAdd ) = 0;
};

#endif

