/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/CServer.h
*  PURPOSE:     Server interface handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSERVER_H
#define __CSERVER_H

#include <core/CServerBase.h>

class CServer : public CServerBase
{
public:
                        CServer             ( void );
    virtual             ~CServer            ( void );

    void                ServerInitialize    ( CServerInterface* pServerInterface );
    bool                ServerStartup       ( int iArgumentCount, char* szArguments [] );
    void                ServerShutdown      ( void );

    void                DoPulse             ( void );

    void                GetTag              ( char* szInfoTag, int iInfoTag );
    void                HandleInput         ( char* szCommand );

    bool                IsFinished          ( void );
    bool                PendingWorkToDo     ( int& iSleepMs );

private:
    CServerInterface*   m_pServerInterface;
    class CGame*        m_pGame;
};

#endif

