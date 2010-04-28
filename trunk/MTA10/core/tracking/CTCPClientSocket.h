/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/tracking/CTCPClientSocket.h
*  PURPOSE:     Interface for TCP client socket wrapper class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPCLIENTSOCKET_H
#define __CTCPCLIENTSOCKET_H

#include "CTCPSocket.h"

class CTCPClientSocket : public CTCPSocket
{
protected:
    virtual                 ~CTCPClientSocket           ( void ) {};
public:

    virtual bool            Connect                     ( const char* szHost, unsigned short usPort ) = 0;
    virtual bool            Disconnect                  ( void ) = 0;
    virtual bool            IsConnected                 ( void ) = 0;

};

#endif
