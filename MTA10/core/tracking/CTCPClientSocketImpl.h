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

#define CloseSocket closesocket

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
    void            FireEvent                       ( LPARAM lType );

    // Callback functions
    void            SetEventClass                   ( void* pClass );
    void            SetEventRead                    ( PFNEVENT pEvent );
    void            SetEventWrite                   ( PFNEVENT pEvent );
    void            SetEventConnect                 ( PFNEVENT pEvent );
    void            SetEventClose                   ( PFNEVENT pEvent );

private:
    int             m_iRefCount;
    bool            m_bIsConnected;
    char            m_szLastError [128];

    unsigned int    m_Socket;

    void*           m_pClass;
    PFNEVENT        m_pEventRead;
    PFNEVENT        m_pEventWrite;
    PFNEVENT        m_pEventConnect;
    PFNEVENT        m_pEventClose;
};

#endif
