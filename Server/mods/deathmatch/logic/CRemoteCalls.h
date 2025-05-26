/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRemoteCalls.h
 *  PURPOSE:     Remote HTTP call (callRemote) class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "lua/CLuaArguments.h"
#define CALL_REMOTE_DEFAULT_QUEUE_NAME "default"

/*
This represents a single live remote call. Calls are live until the call returns
i.e. the http client has downloaded all the data returned
*/
class CRemoteCall
{
private:
    bool                m_bIsFetch;
    class CLuaMain*     m_VM;
    CLuaFunctionRef     m_iFunction;
    long long           m_iStartTime;
    SString             m_strURL;
    SString             m_strQueueName;
    CLuaArguments       m_FetchArguments;
    SHttpRequestOptions m_options;
    EDownloadModeType   m_downloadMode = EDownloadModeType::NONE;
    SDownloadStatus     m_lastDownloadStatus;

public:
    CRemoteCall(const char* szServerHost, const char* szResourceName, const char* szFunctionName, CLuaArguments* arguments, CLuaMain* luaMain,
                const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs);
    CRemoteCall(const char* szURL, CLuaArguments* arguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                uint uiConnectionAttempts, uint uiConnectTimeoutMs);
    CRemoteCall(const char* szURL, CLuaArguments* fetchArguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                const SHttpRequestOptions& options);
    ~CRemoteCall();

    void                   MakeCall();
    static void            DownloadFinishedCallback(const SHttpDownloadResult& result);
    bool                   CancelDownload();
    const SDownloadStatus& GetDownloadStatus();
    void                   OnLuaMainDestroy();

    CLuaMain*                  GetVM() { return m_VM; }
    long long                  GetStartTime() { return m_iStartTime; }
    const SString&             GetURL() { return m_strURL; }
    const SString&             GetQueueName() { return m_strQueueName; }
    bool                       IsFetch() { return m_bIsFetch; }
    bool                       IsLegacy() { return m_options.bIsLegacy; }
    const CLuaArguments&       GetFetchArguments() { return m_FetchArguments; }
    const SHttpRequestOptions& GetOptions() { return m_options; }
};

/*
This class handles remote calls. Remote calls are calls between two different servers
over http. The servers can either be two MTA servers or an MTA server (this, obviously)
and a web server.
*/
class CRemoteCalls
{
private:
    std::list<CRemoteCall*> m_calls;
    std::map<SString, uint> m_QueueIndexMap;

public:
    CRemoteCalls();
    ~CRemoteCalls();

    CRemoteCall* Call(const char* szServerHost, const char* szResourceName, const char* szFunctionName, CLuaArguments* arguments, CLuaMain* luaMain,
                      const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs);
    CRemoteCall* Call(const char* szURL, CLuaArguments* arguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                      uint uiConnectionAttempts, uint uiConnectTimeoutMs);
    CRemoteCall* Call(const char* szURL, CLuaArguments* fetchArguments, CLuaMain* luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName,
                      const SHttpRequestOptions& options);

    void              OnLuaMainDestroy(CLuaMain* luaMain);
    void              Remove(CRemoteCall* call);
    bool              CallExists(CRemoteCall* call);
    void              ProcessQueuedFiles();
    EDownloadModeType GetDownloadModeForQueueName(const SString& strQueueName);
    EDownloadModeType GetDownloadModeFromQueueIndex(uint uiIndex);

    std::list<CRemoteCall*> GetCalls() { return m_calls; }
};
