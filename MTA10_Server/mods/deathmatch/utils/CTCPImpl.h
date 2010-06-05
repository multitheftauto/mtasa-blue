/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CTCPImpl.h
*  PURPOSE:     TCP manager class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPIMPL_H
#define __CTCPIMPL_H

#include "CTCP.h"
#include "CTCPClientSocketImpl.h"
#include "CTCPServerSocketImpl.h"

class CTCPImpl : public CTCP
{
public:
                        CTCPImpl            ( void );
                        ~CTCPImpl           ( void );

    CTCPClientSocket*   CreateClient        ( void );
    CTCPServerSocket*   CreateServer        ( void );

    void                DestroyClient       ( CTCPClientSocket* pClient );
    void                DestroyServer       ( CTCPServerSocket* pServer );

    const char*         GetLastError        ( void );

    bool                Initialize          ( void );
    void                Reset               ( void );

private:
    bool                m_bInitialized;
    char                m_szLastError       [128];
};

#endif
