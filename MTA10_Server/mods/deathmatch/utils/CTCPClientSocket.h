/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CTCPClientSocket.h
*  PURPOSE:     TCP client socket interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPCLIENTSOCKET_H
#define __CTCPCLIENTSOCKET_H

#include "CTCPSocket.h"

class CTCPClientSocket : public CTCPSocket
{
public:
    virtual                 ~CTCPClientSocket           ( void ) {};

    virtual bool            Connect                     ( const char* szHost, unsigned short usPort ) = 0;
    virtual bool            Disconnect                  ( void ) = 0;
    virtual bool            IsConnected                 ( void ) = 0;

    virtual const char*     GetLastError                ( void ) = 0;
};

#endif
