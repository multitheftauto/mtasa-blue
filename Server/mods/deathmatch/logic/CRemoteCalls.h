/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRemoteCalls.h
*  PURPOSE:     Remote HTTP call (callRemote) class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CREMOTECALLS_H
#define __CREMOTECALLS_H
#include "lua/CLuaArguments.h"
#define CALL_REMOTE_DEFAULT_QUEUE_NAME  "default"

/*
This represents a single live remote call. Calls are live until the call returns 
i.e. the http client has downloaded all the data returned
*/
class CRemoteCall
{
private:
    bool                m_bIsFetch;
    class CLuaMain *    m_VM;
    CLuaFunctionRef     m_iFunction;
    SString             m_strURL;
    SString             m_strQueueName;
    CLuaArguments       m_FetchArguments;
    SHttpRequestOptions m_options; 

public:
                        CRemoteCall ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
                        CRemoteCall ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
                        CRemoteCall ( const char * szURL, CLuaArguments * fetchArguments, const SString& strPostData, bool bPostBinary, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
                        CRemoteCall ( const char * szURL, CLuaArguments * fetchArguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, const SHttpRequestOptions& options );
                        ~CRemoteCall ();
    void                MakeCall();
    static void         DownloadFinishedCallback( const SHttpDownloadResult& result );
    CLuaMain *          GetVM() {return m_VM;};
    bool                IsFetch() {return m_bIsFetch;}
    bool                IsLegacy() {return m_options.bIsLegacy;}
    CLuaArguments&      GetFetchArguments() {return m_FetchArguments;}
};

/*
This class handles remote calls. Remote calls are calls between two different servers
over http. The servers can either be two MTA servers or an MTA server (this, obviously)
and a web server.
*/
class CRemoteCalls 
{
private:
    list<CRemoteCall*> m_calls;
    std::map<SString,uint> m_QueueIndexMap;
public:
                        CRemoteCalls();
                        ~CRemoteCalls();

    void                Call ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
    void                Call ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
    void                Call ( const char * szURL, CLuaArguments * fetchArguments, const SString& strPostData, bool bPostBinary, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, uint uiConnectionAttempts, uint uiConnectTimeoutMs );
    void                Call ( const char * szURL, CLuaArguments * fetchArguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, const SString& strQueueName, const SHttpRequestOptions& options );
    void                Remove ( CLuaMain * luaMain );
    void                Remove ( CRemoteCall * call );
    bool                CallExists ( CRemoteCall * call );
    void                ProcessQueuedFiles ( void );
    EDownloadModeType   GetDownloadModeForQueueName( const SString& strQueueName );
    EDownloadModeType   GetDownloadModeFromQueueIndex( uint uiIndex );
};
#endif
