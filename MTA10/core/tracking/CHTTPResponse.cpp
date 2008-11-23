/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/tracking/CHTTPResponse.cpp
*  PURPOSE:		HTTP response parser
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CHTTPResponse.h"

const char * CHTTPResponse::Parse ( const char * szBuffer, unsigned int nLength, unsigned int &nHeaderSize, unsigned int &ResponseCode )
{
    unsigned j;
    std::string strHeader, str;
    
    nHeaderSize = 0;
    ResponseCode = 0;

    // Search for the CRLFCRLF header separator
    if ( nLength < 4 ) return NULL;
    for ( unsigned int i = 0; i < ( nLength - 4 ); i++ )
        if ( szBuffer[i]   == '\r' && szBuffer[i+1] == '\n' &&
             szBuffer[i+2] == '\r' && szBuffer[i+3] == '\n' )
             nHeaderSize = i + 4;
    if ( !nHeaderSize ) return NULL;

    // Parse the header
    strHeader = std::string ( szBuffer, nHeaderSize );
    j = strHeader.find ( ' ' );
    if ( j == std::string::npos ) return NULL;

    // Protocol version
    /*
    str = strHeader.substr ( 0, j );
    if ( str.compare ( "HTTP/1.0" ) == 0 )
        ProtocolVersion = HTTP_10;
    else if ( str.compare ( "HTTP/1.1" ) == 0 )
        ProtocolVersion = HTTP_11;
    else return NULL;
    */

    // Response code
    ResponseCode = atoi ( strHeader.substr ( j, j + 3 ).c_str () );
    if ( ResponseCode < 100 || ResponseCode > 999 ) return NULL;

    // Return the pointer to the HTTP data
    return (szBuffer + nHeaderSize);
}
