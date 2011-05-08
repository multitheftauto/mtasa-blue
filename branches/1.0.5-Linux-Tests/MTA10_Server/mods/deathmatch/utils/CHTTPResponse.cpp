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
    m_iProtocolVersion = HTTP_NONE;
}


CHTTPResponse::~CHTTPResponse ( void )
{

}


bool CHTTPResponse::Parse ( const std::string& strHeader, const std::string& strContent )
{
    // Copy the buffer
    char* szTemp = new char [ strHeader.size () + 1 ];
    memcpy ( szTemp, strHeader.c_str (), strHeader.size () );
    szTemp [ strHeader.size () ] = '\0';

    // copy the contents into this
    m_strContent = strContent;

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
    m_strErrorDescription = szDescription;

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
    return m_strErrorDescription.c_str ();
}


int CHTTPResponse::GetProtocolVersion ( void )
{
    return m_iProtocolVersion;
}

const char* CHTTPResponse::GetData ()
{
    return m_strContent.c_str ();
}

size_t CHTTPResponse::GetDataLength ()
{
    return m_strContent.size ();
}
