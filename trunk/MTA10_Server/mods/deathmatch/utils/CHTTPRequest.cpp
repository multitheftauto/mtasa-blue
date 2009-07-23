/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPRequest.cpp
*  PURPOSE:     HTTP request handling class
*  DEVELOPERS:  Ed Lyons <>
*               arc_ <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CHTTPValue::CHTTPValue ( const char* szName, const char* szContents, int iContentsLength, const char* szContentType, const char* szFilename )
{
    m_strName = szName;

    m_strContents.assign ( szContents, iContentsLength );

    if ( szContentType )
        m_strContentType = szContentType;

    if ( szFilename )
        m_strFileName = szFilename;
}

CHTTPValue::CHTTPValue ( const char* szName, const char* szContents, const char* szContentType, const char* szFilename )
{
    m_strName = szName;

    m_strContents = szContents;

    if ( szContentType )
        m_strContentType = szContentType;

    if ( szFilename )
        m_strFileName = szFilename;
}

CHTTPValue::CHTTPValue ( const CHTTPValue& value )
{
    m_strName = value.m_strName;
    m_strContents = value.m_strContents;
    m_strContentType = value.m_strContentType;
    m_strFileName = value.m_strFileName;
}

CHTTPValue::~CHTTPValue ()
{

}

void CHTTPValue::Write ( std::string& strResult )
{
    strResult.clear ();
    if ( !m_strFileName.empty () )
    {
        strResult += SString ( "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"", m_strName.c_str (), m_strFileName.c_str () ).c_str ();
    }
    else
    {
        strResult += SString ( "Content-Disposition: form-data; name=\"%s\"", m_strName.c_str () ).c_str ();
    }

    if ( !m_strContentType.empty () )
        strResult += SString ( "\r\nContent-Type: %s", m_strContentType.c_str () ).c_str ();

    strResult += "\r\n\r\n";
    strResult += m_strContents;
}

int CHTTPValue::GetLength ()
{
    std::string strResult;
    Write ( strResult );
    return strResult.size ();
}

//////////////////////////////////////////////////////////

CHTTPRequest::CHTTPRequest ( const char* szURL, eRequestType type, eHttpVersion version )
{
    m_version = version;
    m_type = type;
    m_port = 80;

    m_lPacketSize = 400; // 400 bytes

    m_DataSentHandler = NULL;

    std::string strProtocol;
    ParseURL ( szURL, strProtocol, m_strRemoteHost, m_port, m_strRemoteFile );
}

CHTTPRequest::~CHTTPRequest ()
{
    std::list < CHTTPValue* >::iterator iter;
    for ( iter = m_values.begin (); iter != m_values.end (); iter++ )
    {
        delete (*iter);
    }
}

void CHTTPRequest::SetType ( eRequestType type )
{
    m_type = type;
}

void CHTTPRequest::SetReferer ( const char* szReferer )
{
    m_strReferer = szReferer;
}

void CHTTPRequest::SetUserAgent ( const char* szUserAgent )
{
    m_strUserAgent = szUserAgent;
}

void CHTTPRequest::SetRemoteHost ( const char* szRemoteHost )
{
    m_strRemoteHost = szRemoteHost;
}

void CHTTPRequest::SetRemotePath ( const char* szRemotePath )
{
    m_strRemoteFile = szRemotePath;
}

void CHTTPRequest::AddValue ( CHTTPValue * pValue )
{
    CHTTPValue * clonedValue = new CHTTPValue ( *pValue );
    m_values.push_back ( clonedValue );
}

CHTTPResponse * CHTTPRequest::Send ( CTCP * pTCP )
{
    std::string strBuffer;
    if ( !m_strRemoteFile.empty () )
    {
        if ( m_type == HTTP_GET )
            strBuffer += SString ( "GET %s", m_strRemoteFile.c_str () ).c_str ();
        else if ( m_type == HTTP_POST )
            strBuffer += SString ( "POST %s", m_strRemoteFile.c_str () ).c_str ();
    }
    else
    {
        return NULL;
    }

    if ( m_version == HTTP_1_0 )
        strBuffer += " HTTP/1.0\r\n";
    else if ( m_version == HTTP_1_1 )
        strBuffer += " HTTP/1.1\r\n";

    if ( !m_strRemoteHost.empty () )
        strBuffer += SString ( "Host: %s\r\n", m_strRemoteHost.c_str () ).c_str ();
    else
        return NULL;

    if ( !m_strUserAgent.empty () )
        strBuffer += SString ( "User-Agent: %s\r\n", m_strUserAgent.c_str () ).c_str ();

    strBuffer += "Connection: close\r\n";

    if ( !m_strReferer.empty () )
        strBuffer += SString ( "Referer: %s\r\n", m_strReferer.c_str () ).c_str ();


    std::string strPOSTData;
    if ( m_type == HTTP_POST )
    {
        const char* szBoundary = "---------------------------29495281512477";

        std::list < CHTTPValue* >::iterator iter;
        for ( iter = m_values.begin (); iter != m_values.end (); iter++ )
        {
            std::string strValue;
            (*iter)->Write ( strValue, m_version );

            strPOSTData += SString ( "--%s\r\n", szBoundary ).c_str ();
            strPOSTData += strValue;
            strPOSTData += "\r\n";
        }
        strPOSTData += SString ( "--%s--\r\n", szBoundary ).c_str ();

        strBuffer += SString ( "Content-Type: multipart/form-data; boundary=%s\r\n", szBoundary ).c_str ();
        strBuffer += SString ( "Content-Length: %d\r\n", strPOSTData.size () ).c_str ();
    }
    strBuffer += "\r\n"; // terminate the header

    // Create a socket
    CTCPClientSocket* pSocket = pTCP->CreateClient ();
    if ( !pSocket )
    {
        return NULL;
    }

    // Connect to the server
    if ( !pSocket->Connect ( m_strRemoteHost.c_str (), m_port ) )
    {
        delete pSocket;
        return NULL;
    }

    // Write the header
    if ( pSocket->WriteBuffer ( strBuffer.c_str (), strBuffer.size () ) < strBuffer.size () )
    {
        delete pSocket;
        return NULL;
    }

    // send the POST data
    if ( m_type == HTTP_POST )
    {
        const char* pszContent = strPOSTData.c_str ();

        size_t lAmountSent = 0;
        size_t lAmountToSend = 0;
        while ( lAmountSent < strPOSTData.size () )
        {
            lAmountToSend = m_lPacketSize;
            if ( strPOSTData.size () - lAmountSent < m_lPacketSize )
                lAmountToSend = strPOSTData.size () - lAmountSent;

            if ( pSocket->WriteBuffer ( pszContent, lAmountToSend ) < lAmountToSend )
            {
                delete pSocket;
                return NULL;
            }
            lAmountSent += lAmountToSend;
            pszContent += lAmountToSend;

            if ( m_DataSentHandler )
                m_DataSentHandler ( lAmountSent, strPOSTData.size () );
        }
    }

    // Read out the reply header
    std::string strReplyHeader;
    if ( !ReadHeader ( pSocket, strReplyHeader ) )
    {
        delete pSocket;
        return NULL;
    }

    // read all the data
    std::string strReplyBody;
    char szTempBuffer [ 256 ];
    int iSize;
    while ( ( iSize = pSocket->ReadBuffer ( szTempBuffer, sizeof(szTempBuffer) ) ) > 0 )
    {
        strReplyBody.append ( szTempBuffer, iSize );
    }
    pSocket->Disconnect ();
    delete pSocket;
    pSocket = NULL;

    // Parse the reply
    CHTTPResponse* pResponse = new CHTTPResponse ();
    if ( !pResponse->Parse ( strReplyHeader, strReplyBody ) )
    {
        return NULL;
    }

    //m_pLastResponse = pResponse;
    return pResponse;
}

bool CHTTPRequest::ParseURL ( const char* szURL, std::string& strProtocol, std::string& strHost, unsigned short& usPort, std::string& strPath )
{
    // Does :// (protocol specifier) exist in the url?
    const char* szHostnameStart;
    const char* szProtocolSpecifier = strstr ( szURL, "://" );
    if ( szProtocolSpecifier )
    {
        // Copy the text inbetween the :// and the start to szProtocol
        unsigned int uiCount = szProtocolSpecifier - szURL;
        if ( uiCount == 0 )
        {
            // :// specified but no text infront of it
            return false;
        }

        // Copy the text to the passed buffer
        strProtocol.assign ( szURL, uiCount );

        // Set where the hostname starts
        szHostnameStart = szProtocolSpecifier + 3;
    }
    else
    {
        szHostnameStart = szURL;
    }

    // Find the first / or \ and copy the hostname to the hostname buffer
    const char* szHostnameEnd = strchr ( szHostnameStart, '/' );
    if ( !szHostnameEnd )
    {
        strHost = szHostnameStart;
        return true;
    }

    strHost.assign ( szHostnameStart, szHostnameEnd - szHostnameStart );
    strPath = szHostnameEnd;
    return true;
}

bool CHTTPRequest::ReadHeader ( CTCPSocket* pSocket, std::string& strHeader )
{
    // Read out byte for byte until we've reached a double newline
    bool bLastWasR = false;
    unsigned int uiNewLines = 0;
    char cChar;
    int iReturn;
    while ( ( iReturn = pSocket->ReadBuffer ( &cChar, 1 ) == 1 ) )
    {
        // Put it in the buffer
        strHeader += cChar;

        // Is it a new line character?
        if ( cChar == '\r' || cChar == '\n' )
        {
            if ( cChar == '\n' )
            {
                if ( bLastWasR )
                {
                    ++uiNewLines;
                }

                bLastWasR = false;
            }
            else if ( cChar == '\r' )
            {
                bLastWasR = true;
            }

            // Got 2 new lines after eachother? We're done reading
            if ( uiNewLines == 2 )
            {
                return true;
            }
        }
        else
        {
            uiNewLines = 0;
            bLastWasR = false;
        }
    }

    // Timeout or error
    return false;
}

void CHTTPRequest::SetDataSentHandler ( DataSentHandler * handler )
{
    m_DataSentHandler = handler;
}

