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
/*
This represents a single live remote call. Calls are live until the call returns 
i.e. the http client has downloaded all the data returned
*/
class CRemoteCall
{
private:
    class CResource *   m_resource;
    std::string         m_strData;
    bool                m_bPostBinary;
    bool                m_bIsFetch;
    class CLuaMain *    m_VM;
    CLuaFunctionRef     m_iFunction;
    SString             m_strURL;

public:
                        CRemoteCall ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction );
                        CRemoteCall ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction );
                        CRemoteCall ( const char * szURL, const SString& strPostData, bool bPostBinary, CLuaMain * luaMain, const CLuaFunctionRef& iFunction );
                        ~CRemoteCall ();
    void                MakeCall();
    static void         ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error);
    CLuaMain *          GetVM() {return m_VM;};
    bool                IsFetch() {return m_bIsFetch;}
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
public:
                        CRemoteCalls();
                        ~CRemoteCalls();

    void                Call ( const char * szServerHost, const char * szResourceName, const char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction );
    void                Call ( const char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, const CLuaFunctionRef& iFunction );
    void                Call ( const char * szURL, const SString& strPostData, bool bPostBinary, CLuaMain * luaMain, const CLuaFunctionRef& iFunction );
    void                Remove ( CLuaMain * luaMain );
    void                Remove ( CRemoteCall * call );
    bool                CallExists ( CRemoteCall * call );
};
#endif
