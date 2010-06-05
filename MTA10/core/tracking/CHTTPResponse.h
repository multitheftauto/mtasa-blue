/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/tracking/CHTTPResponse.h
*  PURPOSE:		Header file for HTTP reponse parser class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHTTPRESPONSE_H
#define __CHTTPRESPONSE_H

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
    static const char * Parse   ( const char * szBuffer, unsigned int nLength, unsigned int &nHeaderSize, unsigned int &ResponseCode );
};

#endif
