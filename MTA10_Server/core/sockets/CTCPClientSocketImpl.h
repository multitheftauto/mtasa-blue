/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPClientSocketImpl.h
*  PURPOSE:     TCP client socket class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPCLIENTSOCKETIMPL_H
#define __CTCPCLIENTSOCKETIMPL_H

#include <core/CTCPClientSocket.h>

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

#include <stdio.h>
#include <string.h>

class CTCPClientSocketImpl : public CTCPClientSocket
{
public:
                    CTCPClientSocketImpl            ( void );
                    ~CTCPClientSocketImpl           ( void );

    bool            Connect                         ( const char* szHost, unsigned short usPort, const char* szLocalIP = NULL );
    bool            Disconnect                      ( void );
    bool            IsConnected                     ( void );

    const char*     GetLastError                    ( void );

    int             ReadBuffer                      ( void* pOutput, int iSize );
    int             WriteBuffer                     ( const void* pInput, int iSize );

    bool            Initialize                      ( void );

private:
    bool            m_bIsConnected;
    char            m_szLastError [128];

    unsigned int    m_Socket;
};

#endif
