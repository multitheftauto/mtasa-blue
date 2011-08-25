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

extern CGame* g_pGame;

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


void CRemoteCalls::Call ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction )
{
    m_calls.push_back ( new CRemoteCall ( szServerHost, szResourceName, szFunctionName, arguments, luaMain, iFunction ) );
}

void CRemoteCalls::Call ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction )
{
    m_calls.push_back ( new CRemoteCall ( szURL, arguments, luaMain, iFunction ) );
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

CRemoteCall::CRemoteCall ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction )
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    arguments->WriteToJSONString ( m_strData, true );
   
    m_strURL = SString ( "http://%s/%s/call/%s", szServerHost, szResourceName, szFunctionName );

    MakeCall();
}

//arbitary URL version
CRemoteCall::CRemoteCall ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction )
{
    m_VM = luaMain;
    m_iFunction = iFunction;

    arguments->WriteToJSONString ( m_strData, true );

    m_strURL = szURL;

    MakeCall();
}


CRemoteCall::~CRemoteCall () 
{
}

void CRemoteCall::MakeCall()
{
    CNetHTTPDownloadManagerInterface * downloadManager = g_pNetServer->GetHTTPDownloadManager();
    downloadManager->QueueFile ( m_strURL, NULL, 0, m_strData.c_str (), this, ProgressCallback );
    if ( !downloadManager->IsDownloading() )
        downloadManager->StartDownloadingQueuedFiles();
}

void CRemoteCall::ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error)
{
    //printf("Progress: %s\n", data);
    if ( complete )
    {
        CRemoteCall * call = (CRemoteCall*)obj;
        if ( g_pGame->GetRemoteCalls()->CallExists(call) )
        {
            //printf("RECIEVED: %s\n", data);
            CLuaArguments arguments;
            arguments.ReadFromJSONString ( data );
            arguments.Call ( call->m_VM, call->m_iFunction);   

            g_pGame->GetRemoteCalls()->Remove(call); // delete ourselves
        }
    }
    else if ( error )
    {
        CRemoteCall * call = (CRemoteCall*)obj;
        if ( g_pGame->GetRemoteCalls()->CallExists(call) )
        {
            CLuaArguments arguments;
            arguments.PushString("ERROR");
            arguments.PushNumber(error);
            arguments.Call ( call->m_VM, call->m_iFunction);   

            g_pGame->GetRemoteCalls()->Remove(call); // delete ourselves
        }
    }
}

