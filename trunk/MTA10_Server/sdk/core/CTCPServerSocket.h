/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CTCPServerSocket.h
*  PURPOSE:     TCP server socket interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPSERVERSOCKET_H
#define __CTCPSERVERSOCKET_H

#ifdef WIN32
    #include <windows.h>
    #include <winsock.h>

    #define CloseSocket closesocket
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <errno.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/time.h>
    #include <sys/timeb.h>
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <time.h>
    #include <fcntl.h>
    #include <dirent.h>

    #define CloseSocket close
#endif

#include "../MTAPlatform.h"
#include "CTCPSocket.h"

class CTCPServerSocket : public CTCPSocket
{
public:
    virtual                     ~CTCPServerSocket           ( void ) {};

    virtual bool                Listen                      ( unsigned int uiListenPort ) = 0;
    virtual CTCPServerSocket*   Accept                      ( void ) = 0;
    virtual bool                IsListening                 ( void ) = 0;
    virtual bool                IsConnected                 ( void ) = 0;

    virtual const char*         GetLastError                ( void ) = 0;
    virtual sockaddr_in*        GetSocketAddress            ( void ) = 0;
    virtual unsigned int        GetSocket                   ( void ) = 0;
};

#endif
