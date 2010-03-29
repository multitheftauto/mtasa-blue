/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/tracking/CHTTPClient.cpp
*  PURPOSE:     HTTP download client functionality
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CHTTPClient::CHTTPClient ( void )
{
    m_usPort = 0;
    m_strStatus = "";
    m_pHTTPSocket = NULL;

    Reset ();
}


CHTTPClient::~CHTTPClient ( void )
{
    SAFE_RELEASE ( m_pHTTPSocket )
}


void CHTTPClient::Reset ( void )
{
    m_Status = 0;
    m_Buffer.Clear ();
    m_bCompleted = false;

    memset(&m_szProtocol, NULL, sizeof(m_szProtocol));
    memset(&m_szHost, NULL, sizeof(m_szHost));
    memset(&m_szPath, NULL, sizeof(m_szPath));
}


bool CHTTPClient::Get ( const std::string& strURL )
{
    // Delete any previous socket
    if ( m_pHTTPSocket )
    {
        CTCPManager::GetSingleton ().DestroyClient ( m_pHTTPSocket );
        SAFE_RELEASE ( m_pHTTPSocket )
    }

    // Reset
    Reset ();

    // Parse the URL into protocol, host, port and path
    m_strStatus = "connecting to " + strURL;

    if ( !ParseURL ( strURL.c_str (), m_szProtocol, 64, m_szHost, 256, m_usPort, m_szPath, 1024 ) )
    {
        m_strStatus = "error: invalid URL " + strURL;
        return false;
    }

    // Default whatever optional values we got to their defaults if they weren't specified
    if ( m_usPort == 0 ) m_usPort = 80;

    // If the protocol is empty, set it to http
    if ( m_szProtocol [0] == 0 )
    {
        strcpy ( m_szProtocol, "http" );
    }
    else if ( stricmp ( m_szProtocol, "http" ) != 0 )
    {
        m_strStatus = "error: unsupported protocol " + std::string ( m_szProtocol );
        return false;
    }

    // Empty hostname?
    if ( m_szHost [0] == 0 )
    {
        m_strStatus = "error: no hostname within URL " + strURL;
        return false;
    }

    // Create a socket
    m_pHTTPSocket = CTCPManager::GetSingleton ().CreateClient ();

    if ( !m_pHTTPSocket )
    {
        m_strStatus = "error: " + std::string ( m_pHTTPSocket->GetLastError () );
        return false;
    }

    m_pHTTPSocket->AddRef ();

    // Create events
    m_pHTTPSocket->SetEventClass ( this );
    m_pHTTPSocket->SetEventRead ( CHTTPClient::OnRead );
    m_pHTTPSocket->SetEventConnect ( CHTTPClient::OnConnect );
    m_pHTTPSocket->SetEventClose ( CHTTPClient::OnClose );

    // Connect to the server
    m_pHTTPSocket->Connect ( m_szHost, m_usPort );

    return true;
}


void CHTTPClient::OnConnect ( void* pSocketPtr, void* pClassPtr )
{
    if ( pSocketPtr != NULL )
    {
        CTCPClientSocket* pSocket = reinterpret_cast < CTCPClientSocket* > ( pSocketPtr );
        CHTTPClient* pClass = reinterpret_cast < CHTTPClient * > ( pClassPtr );

        pClass->m_strStatus = "connected to server";

        // Throw a HTTP request together
        SString strBuffer;
        if ( pClass->m_usPort != 80 ) // when port isn't 80, the host: part should specify it (otherwise it MUST not)
            strBuffer.Format ( "GET %s HTTP/1.0\r\nHost: %s:%d\r\nUser-Agent: MTASA_10\r\n\r\n", pClass->m_szPath, pClass->m_szHost, pClass->m_usPort );
        else
            strBuffer.Format ( "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: MTASA_10\r\n\r\n", pClass->m_szPath, pClass->m_szHost );
        size_t sizeRequest = strBuffer.length ();

        // Write it to the TCP stream
        pSocket->WriteBuffer ( strBuffer, sizeRequest );

        OnRead ( pSocketPtr, pClassPtr );
    }
}


bool CHTTPClient::ParseURL ( const char* szURL, char* szProtocol, unsigned int uiProtocolSize, char* szHost, unsigned int uiSizeHost, unsigned short& usPort, char* szPath, unsigned int uiSizePath )
{
    // Reset the passed buffers
    szProtocol [0] = 0;
    szHost [0] = 0;
    usPort = 0;
    szPath [0] = 0;

    // Does :// (protocol specifier) exist in the url?
    const char* szHostnameStart;
    const char* szProtocolSpecifier = strstr ( szURL, "://" );
    if ( szProtocolSpecifier )
    {
        // Copy the text inbetween the :// and the start to m_szProtocol
        unsigned int uiCount = szProtocolSpecifier - szURL;
        if ( uiCount == 0 )
        {
            // :// specified but no text infront of it
            return false;
        }
        else if ( uiCount >= uiProtocolSize )
        {
            uiCount = uiProtocolSize - 1;
        }

        // Copy the text to the passed buffer
        strncpy ( szProtocol, szURL, uiCount );
        szProtocol [uiCount] = 0;

        // Set where the hostname starts
        szHostnameStart = szProtocolSpecifier + 3;
    }
    else
    {
        szHostnameStart = szURL;
    }

    // Find the first / or \ and copy the hostname to the hostname buffer
    bool bFoundDelimiter = false;
    unsigned int uiHostnameCount = 0;
    const char* szTemp = szHostnameStart;
    while ( *szTemp )
    {
        // Does it contain a / or a \ symbol?
        if ( *szTemp == '/' || *szTemp == '\\' )
        {
            // Make sure it isn't too big for the buffer passed
            if ( uiHostnameCount >= uiSizeHost )
            {
                uiHostnameCount = uiSizeHost - 1;
            }

            // Copy the hostname into the host buffer
            strncpy ( szHost, szHostnameStart, uiHostnameCount );
            m_szHost [ uiHostnameCount ] = 0;

            bFoundDelimiter = true;
            break;
        }

        ++uiHostnameCount;
        ++szTemp;
    }

    // If we didn't find a delimiter, we got all there is. Return
    if ( !bFoundDelimiter )
    {
        strncpy ( szHost, szHostnameStart, uiSizeHost - 1 );
        szHost [ uiSizeHost - 1 ] = 0;
        return true;
    }

    // If we did, copy the rest over to the path buffer
    strncpy ( szPath, szTemp, uiSizePath );
    szPath [ uiSizePath - 1 ] = 0;

    return true;
}


void CHTTPClient::OnClose ( void* pSocketPtr, void* pClassPtr )
{
    if ( pSocketPtr != NULL )
    {
        CTCPClientSocket* pSocket = reinterpret_cast < CTCPClientSocket* > ( pSocketPtr );
        CHTTPClient* pClass = reinterpret_cast < CHTTPClient * > ( pClassPtr );

        // Disconnected? This means we are done.
        pClass->m_bCompleted = true;

        // Destroy the socket
        assert ( pClass->m_pHTTPSocket == pSocket );
        CTCPManager::GetSingleton ().DestroyClient ( pSocket );
        SAFE_RELEASE ( pClass->m_pHTTPSocket )
    }
}


void CHTTPClient::OnRead ( void* pSocketPtr, void* pClassPtr )
{
    char szBuffer[HTTP_BUFFER_LENGTH] = {0};

    if ( pSocketPtr != NULL )
    {
        CTCPClientSocket* pSocket = reinterpret_cast < CTCPClientSocket* > ( pSocketPtr );
        CHTTPClient* pClass = reinterpret_cast < CHTTPClient * > ( pClassPtr );

        // Attempt to read data from the socket
        while ( true )
        {
            int iRead = pSocket->ReadBuffer ( szBuffer, HTTP_BUFFER_LENGTH );
            if ( iRead <= 0 ) break;

            // Is this our first chunk of data?
            if ( pClass->m_Buffer.GetSize () == 0 ) {
                // Parse the header
                unsigned int nHeaderSize, uiResponseCode;
                const char * szData = CHTTPResponse::Parse ( szBuffer, HTTP_BUFFER_LENGTH, nHeaderSize, uiResponseCode );
                if ( !szData ) pClass->m_strStatus = "error: failed reading HTTP header";

                // Copy data to buffer
                unsigned int nToRead = iRead - nHeaderSize;
                pClass->m_Buffer.Append ( szBuffer + nHeaderSize, nToRead );

                // Set variables
                pClass->m_Status = uiResponseCode;

                // Set status message
                stringstream ss;
                ss << "server returned response " << uiResponseCode;
                pClass->m_strStatus = ss.str ();
            } else {
                // Append the received data to the buffer
                pClass->m_Buffer.Append ( szBuffer, iRead );
            }
        }
    }
}


bool CHTTPClient::GetData ( CHTTPBuffer& outBuffer )
{
    // Only return valid data when the response was 200 (success)
    // and the HTTP request was completed
    if ( m_Status != 200 || !m_bCompleted ) return false;

    // Make a copy
    m_Buffer.Clone( outBuffer );
    return true;
}


int CHTTPClient::ReadHeader ( char* pBuffer, unsigned int uiBufferSize )
{
    // Big enough buffer?
    if ( uiBufferSize == 0 )
    {
        return 0;
    }

    // Read out byte for byte until we've reached a double newline
    bool bLastWas13 = false;
    unsigned int uiNewLines = 0;
    char cChar;
    unsigned int uiIndex = 0;
    
    while ( uiIndex < uiBufferSize )
    {
        // Get the character from the buffer
        cChar = pBuffer[uiIndex];

        // Is it a new line character?
        if ( cChar == 10 || cChar == 13 )
        {
            if ( cChar == 10 )
            {
                if ( bLastWas13 )
                {
                    ++uiNewLines;
                }

                bLastWas13 = false;
            }
            else if ( cChar == 13 )
            {
                bLastWas13 = true;
            }

            // Got 2 new lines after eachother? We're done reading
            if ( uiNewLines == 2 )
            {
                return uiIndex + 1;
            }
        }
        else
        {
            uiNewLines = 0;
            bLastWas13 = false;
        }

        // Increase the index and check that we aren't overflowing any buffer
        ++uiIndex;
    }

    // No valid response
    return 0;
}
