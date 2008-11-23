/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/tracking/CHTTPClient.h
*  PURPOSE:		Header file for HTTP client class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHTTPCLIENT_H
#define __CHTTPCLIENT_H

#include "CHTTPResponse.h"
#include "CTCPManager.h"
#include "CTCPClientSocket.h"
#include <cstdio>

#ifndef snprintf
#define snprintf _snprintf
#endif

using namespace std;

class CHTTPClient
{
public:
                        CHTTPClient             ( void );
                        ~CHTTPClient            ( void );

    bool                Get                     ( std::string strURL, char * szBuffer, unsigned int nBufferLength );
    const std::string&  GetStatusMessage        ( void )        { return m_strStatus; };
    unsigned int        GetStatus               ( void )        { return m_Status; };
    
    char *              GetBuffer               ( void )        { return m_szBuffer; };
    bool                GetData                 ( char ** szBuffer, unsigned int &nDataLength );

	// Callbacks
	static void			OnRead					( void* pSocketPtr, void* pClassPtr );
	static void			OnConnect				( void* pSocketPtr, void* pClassPtr );
	static void			OnClose					( void* pSocketPtr, void* pClassPtr );

private:
    bool				ParseURL                ( const char* szURL, char* szProtocol, unsigned int uiProtocolSize, char* szHost, unsigned int uiSizeHost, unsigned short& usPort, char* szPath, unsigned int uiSizePath );

    int                 ReadHeader              ( char* pBuffer, unsigned int uiBufferSize );

    char *              m_szBuffer;
    unsigned int        m_nBufferLength;
    unsigned int        m_nHeaderSize;

    unsigned int        m_Status;
    std::string         m_strStatus;

    char				m_szProtocol[64];
    char				m_szHost[256];
    char				m_szPath[1024];
    unsigned short		m_usPort;

	CTCPClientSocket*	m_pHTTPSocket;
};

#endif
