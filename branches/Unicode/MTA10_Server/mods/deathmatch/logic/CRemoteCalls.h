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
    char *              m_szResourceName;
    char *              m_szFunctionName;
    char *              m_szServerHost;
    std::string         m_strData;
    class CLuaMain *    m_VM;
    int                 m_iFunction;
    char                m_szURL[512];

public:
                        CRemoteCall ( char * szServerHost, char * szResourceName, char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, int iFunction );
                        CRemoteCall ( char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, int iFunction );
                        ~CRemoteCall ();
    void                MakeCall();
    static void         ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error);
    CLuaMain *          GetVM() {return m_VM;};
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

    void                Call ( char * szServerHost, char * szResourceName, char * szFunctionName, CLuaArguments * arguments, CLuaMain * luaMain, int iFunction );
    void                Call ( char * szURL, CLuaArguments * arguments, CLuaMain * luaMain, int iFunction );
    void                Remove ( CLuaMain * luaMain );
    void                Remove ( CRemoteCall * call );
    bool                CallExists ( CRemoteCall * call );
};
#endif
