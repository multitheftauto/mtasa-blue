/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPImpl.h
*  PURPOSE:     TCP client/server manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPIMPL_H
#define __CTCPIMPL_H

#include <core/CTCP.h>
#include "CTCPClientSocketImpl.h"
#include "CTCPServerSocketImpl.h"
#include "CTCPServerSocketImplManager.h"

class CTCPImpl : public CTCP
{
public:
                                            CTCPImpl            ( void );
                                            ~CTCPImpl           ( void );

    CTCPClientSocket*                       CreateClient        ( void );
    CTCPServerSocket*                       CreateServer        ( void );

    void                                    DestroyClient       ( CTCPClientSocket* pClient );
    void                                    DestroyServer       ( CTCPServerSocket* pServer );

    inline const char*                      GetLastError        ( void )        { return m_szLastError; };
    inline CTCPServerSocketImplManager*     GetServerManager    ( void )        { return m_pServerManager; };

    bool                                    Initialize          ( void );
    void                                    Reset               ( void );

private:
    bool                                    m_bInitialized;
    char                                    m_szLastError       [128];
    CTCPServerSocketImplManager*            m_pServerManager;

};

#endif
