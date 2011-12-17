/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/tracking/CTCPClientSocketImpl.h
*  PURPOSE:     Header file for TCP client socket wrapper class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPCLIENTSOCKETIMPL_H
#define __CTCPCLIENTSOCKETIMPL_H

#include "CTCPClientSocket.h"

//#define NOGDI
#include <windows.h>
#include <winsock.h>

//typedef void (*PFNEVENT) ( CTCPSocket * );

#include <stdio.h>
#include <string.h>

class CTCPClientSocketImpl : public CTCPClientSocket
{
public:
                    CTCPClientSocketImpl            ( void );
protected:
                    ~CTCPClientSocketImpl           ( void );
public:
    void            AddRef                          ( void );
    void            Release                         ( void );

    bool            Connect                         ( const char* szHost, unsigned short usPort );
    bool            Disconnect                      ( void );
    bool            IsConnected                     ( void );

    const char*     GetLastError                    ( void );

    int             ReadBuffer                      ( void* pOutput, int iSize );
    int             WriteBuffer                     ( const void* pInput, int iSize );

    bool            Initialize                      ( unsigned int uiID );

    // Async related functions
    void            FireEvent                       ( bool bIsResolveEvent, uint uiResolveId, LPARAM lType );

    // Callback functions
    void            SetEventClass                   ( void* pClass );
    void            SetEventRead                    ( PFNEVENT pEvent );
    void            SetEventWrite                   ( PFNEVENT pEvent );
    void            SetEventConnect                 ( PFNEVENT pEvent );
    void            SetEventClose                   ( PFNEVENT pEvent );

private:
    void            ConnectContinue                 ( void );

    unsigned int    m_uiID;

    unsigned short  m_usPort;

    int             m_iRefCount;
    bool            m_bIsResolvingHost;
    bool            m_bIsConnected;
    char            m_szLastError [128];

    // That's not for string - WSAAsyncGetHostByName wants this type as buffer
    char            m_pHostInfo [ MAXGETHOSTSTRUCT ];

    void*           m_pAsyncHostResolving;
    unsigned int    m_Socket;

    void*           m_pClass;
    PFNEVENT        m_pEventRead;
    PFNEVENT        m_pEventWrite;
    PFNEVENT        m_pEventConnect;
    PFNEVENT        m_pEventClose;
};

#endif
