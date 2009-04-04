/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPClient.cpp
*  PURPOSE:     HTTP client class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CHTTPClient.h"
#include "CHTTPResponse.h"
#include <cstdio>
#include <cstring>

using namespace std;

CHTTPClient::CHTTPClient ( CTCP* tcp )
{
    // Init
    m_szLastError [0] = 0;
    m_TCP = tcp;
}


CHTTPClient::~CHTTPClient ( void )
{

}

bool CHTTPClient::Get ( const char* szURL, const char* szDestinationFilename )
{
    // Parse the URL into protocol, host, port and path
    char szProtocol [64];
    char szHost [256];
    unsigned short usPort = 0;
    char szPath [1024];

    if ( !ParseURL ( szURL, szProtocol, 64, szHost, 256, usPort, szPath, 1024 ) )
    {
        strcpy ( m_szLastError, "Invalid URL" );
        return false;
    }

    // Default whatever optional values we got to their defaults if they weren't specified
    if ( usPort == 0 )
    {
        usPort = 80;
    }

    // If the protocol is empty, set it to http
    if ( szProtocol [0] == 0 )
    {
        strcpy ( szProtocol, "http" );
    }
    else if ( stricmp ( szProtocol, "http" ) != 0 )
    {
        strcpy ( m_szLastError, "Unsupported protocol" );
        return false;
    }

    // Empty hostname?
    if ( szHost [0] == 0 )
    {
        strcpy ( m_szLastError, "No hostname within the URL" );
        return false;
    }

    // Create a socket
    CTCPClientSocket* pSocket = m_TCP->CreateClient ();
    if ( !pSocket )
    {
        strcpy ( m_szLastError, m_TCP->GetLastError () );
        return false;
    }

    // Connect to the server
    if ( !pSocket->Connect ( szHost, usPort ) )
    {
        strcpy ( m_szLastError, pSocket->GetLastError () );
        delete pSocket;
        return false;
    }

    // Throw a HTTP request together
    char szBuffer [2048];
    if ( usPort != 80 ) // when port isn't 80, the host: part should specify it (otherwise it MUST not)
        snprintf ( szBuffer, 2048, "GET %s HTTP/1.0\r\nHost: %s:%d\r\nUser-Agent: MTASA_ERRORTOOL\r\n\r\n", szPath, szHost, usPort );
    else
        snprintf ( szBuffer, 2048, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: MTASA_ERRORTOOL\r\n\r\n", szPath, szHost );
    szBuffer [2047] = 0;
    size_t sizeRequest = strlen ( szBuffer );

    // Write it to the TCP stream
    if ( pSocket->WriteBuffer ( szBuffer, sizeRequest ) < (int) sizeRequest )
    {
        strcpy ( m_szLastError, "Error writing to socket" );
        delete pSocket;
        return false;
    }

    // Read out the reply header
    char szReplyHeaderBuffer [2048];
    unsigned int uiHeaderSize;
    if ( !( uiHeaderSize = ReadHeader ( pSocket, szReplyHeaderBuffer, 2048 ) ) )
    {
        //strcpy ( m_szLastError, "Failed reading out response" );
        delete pSocket;
        //return false;
        return true;
    }

    // Parse the reply
    CHTTPResponse Response;
   /* if ( !Response.Parse ( szReplyHeaderBuffer, uiHeaderSize ) )
    {
        //strcpy ( m_szLastError, "Parsing response failed" );

        pSocket->Disconnect ();
        delete pSocket;
        //return false;
        return true;
    }*/

    // Errocode 200? (success)
    if ( Response.GetErrorCode () != 200 )
    {
        snprintf ( m_szLastError, sizeof ( m_szLastError ), "%u - %s", Response.GetErrorCode (), Response.GetErrorDescription () );

        pSocket->Disconnect ();
        delete pSocket;
        return false;
    }

    bool bWriteSuccess = false;
    if ( WriteSocketToFile(pSocket, szDestinationFilename) )
        bWriteSuccess = true;

    // Disconnect and delete the socket
    pSocket->Disconnect ();
    delete pSocket;

    return bWriteSuccess;
}


bool CHTTPClient::WriteSocketToFile ( CTCPClientSocket * pSocket, const char * szDestinationFilename )
{
    char szBuffer[2048];
    // Open the destination file as w+
    FILE* pFile = fopen ( szDestinationFilename, "wb+" );
    if ( !pFile )
    {
        strcpy ( m_szLastError, "Unable to write destination file" );
        return false;
    }

    // Read from the socket to the file until there's nothing more to read
    int iSize = 0;
    while ( ( iSize = pSocket->ReadBuffer ( szBuffer, 2048 ) ) > 0 )
    {
        fwrite ( &szBuffer, iSize, 1, pFile );
    }

    // Close the file
    fclose ( pFile );
    return true;
}

const char* CHTTPClient::GetLastError ( void )
{
    return m_szLastError;
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
        // Copy the text inbetween the :// and the start to szProtocol
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
            szHost [ uiHostnameCount ] = 0;

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


int CHTTPClient::ReadHeader ( CTCPSocket* pSocket, char* pBuffer, unsigned int uiBufferSize )
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
    int iReturn;
    while ( ( iReturn = pSocket->ReadBuffer ( &cChar, 1 ) == 1 ) )
    {
        // Put it in the buffer
        pBuffer [uiIndex] = cChar;

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
        if ( uiIndex >= uiBufferSize )
        {
            return 0;
        }
    }

    // Timeout or error
    return 0;
}

