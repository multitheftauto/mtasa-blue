/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CTCP.h
*  PURPOSE:     TCP socket manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCP_H
#define __CTCP_H

#include "CTCPClientSocket.h"
#include "CTCPServerSocket.h"

class CTCP
{
public:
    virtual CTCPClientSocket*       CreateClient        ( void ) = 0;
    virtual CTCPServerSocket*       CreateServer        ( void ) = 0;

    virtual void                    DestroyClient       ( CTCPClientSocket* pClient ) = 0;
    virtual void                    DestroyServer       ( CTCPServerSocket* pServer ) = 0;

    virtual const char*             GetLastError        ( void ) = 0;
};

#endif
