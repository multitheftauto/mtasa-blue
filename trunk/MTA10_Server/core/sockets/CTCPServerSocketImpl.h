/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPServerSocketImpl.h
*  PURPOSE:     TCP server socket class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTCPServerSocketImpl;

#ifndef __CTCPSERVERSOCKETIMPL_H
#define __CTCPSERVERSOCKETIMPL_H

#include <core/CTCPServerSocket.h>
#include "CTCPServerSocketImplManager.h"
#include <stdio.h>
#include <string.h>

class CTCPServerSocketImpl : public CTCPServerSocket
{
public:
                                    CTCPServerSocketImpl            ( CTCPServerSocketImplManager*  pServerManager );
                                    ~CTCPServerSocketImpl           ( void );

    bool                            Listen                          ( unsigned int uiListenPort );
    CTCPServerSocket*               Accept                          ( void );
    bool                            IsListening                     ( void );

    bool                            IsConnected                     ( void );
    void                            SetConnected                    ( bool bConnnected );

    const char*                     GetLastError                    ( void );
    unsigned int                    GetListenPort                   ( void );
    sockaddr_in*                    GetSocketAddress                ( void );

    unsigned int                    GetSocket                       ( void );
    void                            SetSocket                       ( int Socket );

    int                             ReadBuffer                      ( void* pOutput, int iSize );
    int                             WriteBuffer                     ( const void* pInput, int iSize );

    bool                            Initialize                      ( void );

private:
    bool                            m_bIsListening;
    bool                            m_bIsConnected;
    char                            m_szLastError [128];
    unsigned int                    m_uiListenPort;
    struct sockaddr_in              m_SockAddr;

	unsigned int	                m_Socket;
    CTCPServerSocketImplManager*    m_pServerManager;

};

#endif
