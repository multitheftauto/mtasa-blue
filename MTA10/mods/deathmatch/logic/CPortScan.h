/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPortScan.h
*  PURPOSE:     Header for port scan class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPORTSCAN_H
#define __CPORTSCAN_H

typedef void (*CBPORTSCAN) ( unsigned short, bool, void* );

#define PROTOCOL_UDP 1
#define PROTOCOL_TCP 2

#define PORTSCAN_STR "hello world"
#define PORTSCAN_LEN 11

#ifdef WIN32
	#define sockclose closesocket
#else
	#define sockclose close
#endif

class CPortScan
{
public:
                        CPortScan           ( unsigned short usPort, unsigned char ucType = PROTOCOL_UDP, CBPORTSCAN pCallBack = NULL );
                        ~CPortScan          ( void );

    void                DoPulse             ( unsigned short& usSent );

    unsigned char       GetType             ( void )            { return m_ucType; };
    unsigned short      GetPort             ( void )            { return m_usPort; };
    CBPORTSCAN          GetCallBack         ( void )            { return m_pCallBack; };

private:

    bool                Init                ( void );
    static DWORD WINAPI Scan                ( LPVOID pObject );

private:

    unsigned char       m_ucType;

    unsigned short      m_usPort;
    CBPORTSCAN          m_pCallBack;

    int                 m_Socket;
    sockaddr_in         m_SockAddr;

    HANDLE              m_hThread;

};

#endif
