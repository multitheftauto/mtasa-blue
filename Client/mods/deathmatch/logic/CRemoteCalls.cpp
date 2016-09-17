/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRemoteCalls.cpp
*  PURPOSE:     Remote HTTP call (callRemote) class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CRemoteCalls::~CRemoteCalls()
{
    for ( auto& pCall : m_calls )
    {
        delete pCall;
    }

    m_calls.clear();
}


void CRemoteCalls::Call ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, uint uiConnectionAttempts, uint uiConnectTimeoutMs )
{
    m_calls.push_back ( new CRemoteCall ( szServerHost, szResourceName, szFunctionName, arguments, luaMain, iFunction, uiConnectionAttempts, uiConnectTimeoutMs ) );
    m_calls.back ()->MakeCall ();
}

void CRemoteCalls::Call ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, uint uiConnectionAttempts, uint uiConnectTimeoutMs )
{
    m_calls.push_back ( new CRemoteCall ( szURL, arguments, luaMain, iFunction, uiConnectionAttempts, uiConnectTimeoutMs ) );
    m_calls.back ()->MakeCall ();
}

void CRemoteCalls::Call ( const char * szURL, CLuaArguments * fetchArguments, const SString& strPostData, bool bPostBinary, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, uint uiConnectionAttempts, uint uiConnectTimeoutMs )
{
    m_calls.push_back ( new CRemoteCall ( szURL, fetchArguments, strPostData, bPostBinary, luaMain, iFunction, uiConnectionAttempts, uiConnectTimeoutMs ) );
    m_calls.back ()->MakeCall ();
}

void CRemoteCalls::Remove ( CLuaMain * lua )
{
    std::list<CRemoteCall *> trash;
    for ( auto& pCall : m_calls )
    {
        if ( pCall->GetVM() == lua )
        {
            trash.push_back(pCall);
        }
    }

    for ( auto& pCall : trash )
    {
        m_calls.remove (pCall);
        delete pCall;
    }
}

void CRemoteCalls::Remove ( CRemoteCall * call )
{
    m_calls.remove(call);
    delete call;
}

bool CRemoteCalls::CallExists ( CRemoteCall * call )
{
    for ( auto& pCall : m_calls )
    {
        if ( pCall == call )
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

CRemoteCall::CRemoteCall ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, uint uiConnectionAttempts, uint uiConnectTimeoutMs )
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    arguments->WriteToJSONString ( m_strData, true );
    m_bPostBinary = false;
    m_bIsFetch = false;

    m_strURL = SString ( "http://%s/%s/call/%s", szServerHost, szResourceName, szFunctionName );
    m_uiConnectionAttempts = uiConnectionAttempts;
    m_uiConnectTimeoutMs = uiConnectTimeoutMs;
}

//arbitary URL version
CRemoteCall::CRemoteCall ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, uint uiConnectionAttempts, uint uiConnectTimeoutMs )
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    arguments->WriteToJSONString ( m_strData, true );
    m_bPostBinary = false;
    m_bIsFetch = false;

    m_strURL = szURL;
    m_uiConnectionAttempts = uiConnectionAttempts;
    m_uiConnectTimeoutMs = uiConnectTimeoutMs;
}

//Fetch version
CRemoteCall::CRemoteCall ( const char * szURL, CLuaArguments * fetchArguments, const SString& strPostData, bool bPostBinary, CLuaMain * luaMain, const CLuaFunctionRef& iFunction, uint uiConnectionAttempts, uint uiConnectTimeoutMs )
    : m_FetchArguments ( *fetchArguments )
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    m_strData = strPostData;
    m_bPostBinary = bPostBinary;
    m_bIsFetch = true;

    m_strURL = szURL;
    m_uiConnectionAttempts = uiConnectionAttempts;
    m_uiConnectTimeoutMs = uiConnectTimeoutMs;
}

void CRemoteCall::MakeCall()
{
    CNetHTTPDownloadManagerInterface * downloadManager = g_pNet->GetHTTPDownloadManager ( EDownloadMode::CALL_REMOTE );
    downloadManager->QueueFile ( m_strURL, NULL, 0, m_strData.c_str (), m_strData.length (), m_bPostBinary, this, DownloadFinishedCallback, false, m_uiConnectionAttempts, m_uiConnectTimeoutMs );
}

void CRemoteCall::DownloadFinishedCallback( char * data, size_t dataLength, void * obj, bool bSuccess, int iErrorCode )
{
    //printf("Progress: %s\n", data);
    if ( bSuccess )
    {
        CRemoteCall * call = (CRemoteCall*)obj;
        if ( g_pClientGame->GetRemoteCalls()->CallExists(call) )
        {
            //printf("RECIEVED: %s\n", data);
            CLuaArguments arguments;
            if ( call->IsFetch () )
            {
                arguments.PushString ( std::string ( data, dataLength ) );
                arguments.PushNumber ( 0 );
                for ( uint i = 0 ; i < call->GetFetchArguments ().Count () ; i++ )
                    arguments.PushArgument ( *( call->GetFetchArguments ()[i] ) );
            }
            else
                arguments.ReadFromJSONString ( data );

            arguments.Call ( call->m_VM, call->m_iFunction);   

            g_pClientGame->GetRemoteCalls()->Remove(call); // delete ourselves
        }
    }
    else
    {
        CRemoteCall * call = (CRemoteCall*)obj;
        if ( g_pClientGame->GetRemoteCalls()->CallExists(call) )
        {
            CLuaArguments arguments;
            arguments.PushString("ERROR");
            arguments.PushNumber( iErrorCode );
            if ( call->IsFetch () )
                for ( uint i = 0 ; i < call->GetFetchArguments ().Count () ; i++ )
                    arguments.PushArgument ( *( call->GetFetchArguments ()[i] ) );

            arguments.Call ( call->m_VM, call->m_iFunction);   

            g_pClientGame->GetRemoteCalls()->Remove(call); // delete ourselves
        }
    }
}
