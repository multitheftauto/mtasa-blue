/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPResponse.cpp
*  PURPOSE:     HTTP response parsing class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CHTTPResponse::CHTTPResponse ( void )
{
    // Init
    m_uiErrorCode = 0;
    m_szErrorDescription [0] = 0;
    m_iProtocolVersion = HTTP_NONE;
    m_contentSize = 0;
    m_content = NULL;
}


CHTTPResponse::~CHTTPResponse ( void )
{
    delete[] m_content;
}


bool CHTTPResponse::Parse ( const char* pHeaderBuffer, unsigned int uiHeaderSize, const char* pContent, size_t contentSize )
{
    // Copy the buffer
    char* szTemp = new char [uiHeaderSize];
    memcpy ( szTemp, pHeaderBuffer, uiHeaderSize );

    m_contentSize = contentSize;
    // copy the contents into this
    m_content = new char[m_contentSize];
    memcpy(m_content, pContent, m_contentSize);

    // Start by reading out the first line
    char* szFirstLine = strtok ( szTemp, "\r\n" );
    if ( !szFirstLine )
    {
        delete [] szTemp;
        return false;
    }

    // Split up the parts (protocol, code and description)
    char* szProtocol = strtok ( szTemp, " " );
    char* szCode = strtok ( NULL, " " );
    char* szDescription = strtok ( NULL, "\r\n" );
    if ( !szProtocol || !szCode || !szDescription )
    {
        delete [] szTemp;
        return false;
    }

    // Store the protocol
    if ( stricmp ( szProtocol, "HTTP/1.0" ) == 0 )
    {
        m_iProtocolVersion = HTTP_10;
    }
    else if ( stricmp ( szProtocol, "HTTP/1.1" ) == 0 )
    {
        m_iProtocolVersion = HTTP_11;
    }
    else
    {
        m_iProtocolVersion = HTTP_NONE;
        delete [] szTemp;
        return false;
    }

    // Store the code
    m_uiErrorCode = atoi ( szCode );
    if ( m_uiErrorCode == 0 )
    {
        delete [] szTemp;
        return false;
    }

    // Store the description
    strncpy ( m_szErrorDescription, szDescription, sizeof ( m_szErrorDescription ) );

    // Delete the buffer
    delete [] szTemp;
    return true;
}


unsigned int CHTTPResponse::GetErrorCode ( void )
{
    return m_uiErrorCode;
}


const char* CHTTPResponse::GetErrorDescription ( void )
{
    return m_szErrorDescription;
}


int CHTTPResponse::GetProtocolVersion ( void )
{
    return m_iProtocolVersion;
}

char * CHTTPResponse::GetData ( char * szBuffer, int iBufferSize )
{
    if ( iBufferSize )
        memcpy ( szBuffer, m_content, iBufferSize );
    else
        memcpy ( szBuffer, m_content, m_contentSize );
    return szBuffer;
}

int CHTTPResponse::GetDataLength ()
{
    return (int)m_contentSize;
}
