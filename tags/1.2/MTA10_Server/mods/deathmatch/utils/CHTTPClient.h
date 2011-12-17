/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPClient.h
*  PURPOSE:     HTTP client class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHTTPCLIENT_H
#define __CHTTPCLIENT_H

#include "../Config.h"
#include "CTCP.h"
#include "CTCPClientSocket.h"
#include <cstdio>

using namespace std;

class CHTTPClient
{
public:
                        CHTTPClient             ( CTCP * tcp );
                        ~CHTTPClient            ( void );

    bool                Get                     ( const char* szURL, const char* szDestinationFilename );

    const char*         GetLastError            ( void );

private:
    bool                ParseURL                ( const char* szURL, char* szProtocol, unsigned int uiProtocolSize, char* szHost, unsigned int uiSizeHost, unsigned short& usPort, char* szPath, unsigned int uiSizePath );
    bool                WriteSocketToFile       ( CTCPClientSocket * pSocket, const char * szDestinationFilename );

    int                 ReadHeader              ( CTCPSocket* pSocket, char* pBuffer, unsigned int uiBufferSize );

    char                m_szLastError [128];

    CTCP*               m_TCP;
};

#endif
