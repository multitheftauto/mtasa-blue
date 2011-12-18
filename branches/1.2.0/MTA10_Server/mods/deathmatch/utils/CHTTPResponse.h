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

    bool            Parse                       ( const std::string& strHeader, const std::string& strContent );

    unsigned int    GetErrorCode                ( void );
    const char*     GetErrorDescription         ( void );
    int             GetProtocolVersion          ( void );
    const char*     GetData                     ( void );
    size_t          GetDataLength               ( void );

private:
    unsigned int    m_uiErrorCode;
    std::string     m_strErrorDescription;
    int             m_iProtocolVersion;
    std::string     m_strContent;
};

#endif
