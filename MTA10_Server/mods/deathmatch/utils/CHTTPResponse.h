/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPResponse.h
*  PURPOSE:     HTTP response parsing class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHTTPRESPONSE_H
#define __CHTTPRESPONSE_H

#include "../Config.h"
#include "CTCPClientSocket.h"

class CHTTPResponse
{
public:
    enum
    {
        HTTP_NONE,
        HTTP_10,
        HTTP_11,
    };

public:
                    CHTTPResponse               ( void );
                    ~CHTTPResponse              ( void );

    bool            Parse                       ( const char* pBuffer, unsigned int uiSize, const char* pContent, size_t contentSize );

    unsigned int    GetErrorCode                ( void );
    const char*     GetErrorDescription         ( void );
    int             GetProtocolVersion          ( void );
    char *          GetData                     ( char * szBuffer, int iBufferSize = 0 );
    int             GetDataLength               ( void );

private:
    unsigned int    m_uiErrorCode;
    char            m_szErrorDescription [256];
    int             m_iProtocolVersion;
    char*           m_content;
    size_t          m_contentSize;
};

#endif
