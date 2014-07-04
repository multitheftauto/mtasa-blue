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

using std::list;

CRemoteCalls::CRemoteCalls()
{
    
}

CRemoteCalls::~CRemoteCalls()
{
    list< CRemoteCall* >::iterator iter = m_calls.begin ();
    for ( ; iter != m_calls.end (); iter++ )
    {
        delete (*iter);
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
    list<CRemoteCall *> trash;
    list< CRemoteCall* >::iterator iter = m_calls.begin ();
    for ( ; iter != m_calls.end (); iter++ )
    {
        if ( (*iter)->GetVM() == lua )
        {
            trash.push_back((*iter));
        }
    }

    iter = trash.begin ();
    for ( ; iter != trash.end (); iter++ )
    {
        m_calls.remove ( (*iter));
        delete (*iter);
    }
}

void CRemoteCalls::Remove ( CRemoteCall * call )
{
    m_calls.remove(call);
    delete call;
}

bool CRemoteCalls::CallExists ( CRemoteCall * call )
{
    list< CRemoteCall* >::iterator iter = m_calls.begin ();
    for ( ; iter != m_calls.end (); iter++ )
    {
        if ( (*iter) == call )
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


CRemoteCall::~CRemoteCall () 
{
}

void CRemoteCall::MakeCall()
{
    CNetHTTPDownloadManagerInterface * downloadManager = g_pNet->GetHTTPDownloadManager ( EDownloadMode::CALL_REMOTE );
    downloadManager->QueueFile ( m_strURL, NULL, 0, m_strData.c_str (), m_strData.length (), m_bPostBinary, this, ProgressCallback, false, m_uiConnectionAttempts, m_uiConnectTimeoutMs );
}

bool CRemoteCall::ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error)
{
    //printf("Progress: %s\n", data);
    if ( complete )
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
            return true;
        }
    }
    else if ( error )
    {
        CRemoteCall * call = (CRemoteCall*)obj;
        if ( g_pClientGame->GetRemoteCalls()->CallExists(call) )
        {
            CLuaArguments arguments;
            arguments.PushString("ERROR");
            arguments.PushNumber(error);
            if ( call->IsFetch () )
                for ( uint i = 0 ; i < call->GetFetchArguments ().Count () ; i++ )
                    arguments.PushArgument ( *( call->GetFetchArguments ()[i] ) );

            arguments.Call ( call->m_VM, call->m_iFunction);   

            g_pClientGame->GetRemoteCalls()->Remove(call); // delete ourselves
            return true;
        }
    }

    return false;   // Possible problem
}
