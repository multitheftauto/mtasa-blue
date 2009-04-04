/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPRequest.cpp
*  PURPOSE:     HTTP request handling class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CHTTPValue::CHTTPValue ( char * szName, char * szContents, int iContentsLength, char * szContentType, char * szFilename )
{
    m_contenttype = NULL;
    m_filename = NULL;

    m_name = new char[strlen(szName) + 1];
    strcpy(m_name, szName);

    m_contents = new char[iContentsLength];
    memcpy(m_contents, szContents, iContentsLength);
    m_contentslength = iContentsLength;

    if ( szContentType )
    {
        m_contenttype = new char[strlen(szContentType) + 1];
        strcpy(m_contenttype, szContentType);
    }

    if ( szFilename )
    {
        m_filename = new char[strlen(szFilename) + 1];
        strcpy(m_filename, szFilename);
    }
}

CHTTPValue::CHTTPValue ( char * szName, char * szContents, char * szContentType, char * szFilename )
{
    m_contenttype = NULL;
    m_filename = NULL;

    m_name = new char[strlen(szName) + 1];
    strcpy(m_name, szName);

    m_contents = new char[strlen(szContents) + 1];
    strcpy(m_contents, szContents);
    m_contentslength = strlen(szContents);

    if ( szContentType )
    {
        m_contenttype = new char[strlen(szContentType) + 1];
        strcpy(m_contenttype, szContentType);
    }

    if ( szFilename )
    {
        m_filename = new char[strlen(szFilename) + 1];
        strcpy(m_filename, szFilename);
    }
}

CHTTPValue::CHTTPValue ( const CHTTPValue * value )
{
    //CHTTPValue( value->m_name, value->m_contents, value->m_contenttype, value->m_filename );
    m_contenttype = NULL;
    m_filename = NULL;

    m_name = new char[strlen(value->m_name) + 1];
    strcpy(m_name, value->m_name);

    m_contents = new char[value->m_contentslength];
    memcpy(m_contents, value->m_contents, value->m_contentslength);

    m_contentslength = value->m_contentslength;

    if ( value->m_contenttype )
    {
        m_contenttype = new char[strlen(value->m_contenttype) + 1];
        strcpy(m_contenttype, value->m_contenttype);
    }

    if ( value->m_filename )
    {
        m_filename = new char[strlen(value->m_filename) + 1];
        strcpy(m_filename, value->m_filename);
    }
}

CHTTPValue::~CHTTPValue ()
{
    if ( m_name )
        delete[] m_name;

    if ( m_contenttype )
        delete[] m_contenttype;

    if ( m_filename )
        delete[] m_filename;

    if ( m_contents )
        delete[] m_contents;
}

char * CHTTPValue::Write(char * szBuffer, eHttpVersion version)
{
    if ( m_filename )
    {
        sprintf ( szBuffer, "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"", m_name, m_filename );
    }
    else
    {
        sprintf ( szBuffer, "Content-Disposition: form-data; name=\"%s\"", m_name );
    }

    if ( m_contenttype )
        sprintf ( szBuffer, "%s\r\nContent-Type: %s", szBuffer, m_contenttype );

    sprintf ( szBuffer, "%s\r\n\r\n", szBuffer );
    int iBufferLength = strlen(szBuffer);
    memcpy ( szBuffer + iBufferLength, m_contents, m_contentslength); // because it might be binary data with nulls in
    return szBuffer;
}

int CHTTPValue::GetLength(eHttpVersion version)
{
    char szBuffer[2048];

    if ( m_filename )
    {
        _snprintf ( szBuffer, 2048, "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"", m_name, m_filename );
    }
    else
    {
        _snprintf ( szBuffer, 2048, "Content-Disposition: form-data; name=\"%s\"", m_name );
    }

    if ( m_contenttype )
        _snprintf ( szBuffer, 2048, "%s\r\nContent-Type: %s", szBuffer, m_contenttype );


   // char * szBuffer = new char[m_contentslength + strlen(m_name) + iFilenameLength + 200];
   // Write(szBuffer, version);
    //int iBufferLength = (int)strlen(szBuffer); // won't work on binary data
    //delete[] szBuffer;
    return m_contentslength + strlen(szBuffer) + 4; // 4 = \r\n\r\n
}

//////////////////////////////////////////////////////////

CHTTPRequest::CHTTPRequest ( char * szURL, eRequestType type, eHttpVersion version )
{
    m_remotefile = new char[2048];
    m_remotehost = new char[256];
    char * szProtocol = new char[10];
    m_referer = NULL;
    m_useragent = NULL;
    m_lastresponse = NULL;

    m_version = version;
    m_type = type;

    m_lPacketSize = 400; // 400 bytes

    m_DataSentHandler = NULL;

    ParseURL(szURL, szProtocol, 10, m_remotehost, 256, m_port, m_remotefile, 2048);
    if ( m_port == 0 )
        m_port = 80;
}

CHTTPRequest::~CHTTPRequest ()
{
    if ( m_useragent )
        delete[] m_useragent;

    if ( m_referer )
        delete[] m_referer;

    if ( m_remotefile )
        delete[] m_remotefile;

    if ( m_remotehost )
        delete[] m_remotehost;

    list<CHTTPValue*>::iterator iter;
    for(iter = m_values.begin(); iter != m_values.end(); iter++)
    {
        delete (*iter);
    }

    if ( m_lastresponse )
        delete m_lastresponse;
}

void CHTTPRequest::SetType ( eRequestType type )
{
    m_type = type;
}

void CHTTPRequest::SetReferer ( char * szReferer )
{
    m_referer = new char[strlen( szReferer ) + 1];
    strcpy( m_referer, szReferer );
}

void CHTTPRequest::SetUserAgent ( char * szUserAgent )
{
    m_useragent = new char[strlen( szUserAgent ) + 1];
    strcpy( m_useragent, szUserAgent );
}

void CHTTPRequest::SetRemoteHost ( char * szRemoteHost )
{
    m_remotehost = new char[strlen( szRemoteHost ) + 1];
    strcpy( m_remotehost, szRemoteHost );
}

void CHTTPRequest::SetRemotePath ( char * szRemotePath )
{
    m_remotefile = new char[strlen( szRemotePath ) + 1];
    strcpy( m_remotefile, szRemotePath );
}

void CHTTPRequest::AddValue ( CHTTPValue * pValue )
{
    CHTTPValue * clonedValue = new CHTTPValue(pValue);
    m_values.push_back(clonedValue);
}

CHTTPResponse * CHTTPRequest::Send ( CTCP * pTCP )
{
    char * szContent = new char[GetLength()];
    char * szBuffer = new char[GetLength()];
    int iContentLength = 0;
    szContent[0] = '\0';
    szBuffer[0] = '\0';
    if ( m_remotefile )
    {
        if ( m_type == HTTP_GET )
            sprintf(szBuffer, "GET %s", m_remotefile);
        else if ( m_type == HTTP_POST )
            sprintf(szBuffer, "POST %s", m_remotefile);
    }
    else
    {
        return NULL;
    }

    if ( m_version == HTTP_1_0 )
        sprintf ( szBuffer, "%s HTTP/1.0\r\n", szBuffer );
    else if ( m_version == HTTP_1_1 )
        sprintf ( szBuffer, "%s HTTP/1.1\r\n", szBuffer );

    if ( m_remotehost )
        sprintf ( szBuffer, "%sHost: %s\r\n", szBuffer, m_remotehost );
    else
        return NULL;

    if ( m_useragent )
        sprintf ( szBuffer, "%sUser-Agent: %s\r\n", szBuffer, m_useragent );

    sprintf ( szBuffer, "%sConnection: close\r\n", szBuffer );

    if ( m_referer )
        sprintf ( szBuffer, "%sReferer: %s\r\n", szBuffer, m_referer );



    if ( m_type == HTTP_POST )
    {
        char szBoundary[] = "---------------------------29495281512477";
        sprintf ( szBuffer, "%sContent-Type: multipart/form-data; boundary=%s\r\n", szBuffer, szBoundary);
           
        int iTotalLength = 0;
        int iBoundaryLength = (int)strlen(szBoundary);
        list<CHTTPValue*>::iterator iter;
        for(iter = m_values.begin(); iter != m_values.end(); iter++)
        {
            iTotalLength += (*iter)->GetLength();
            iTotalLength += iBoundaryLength;
            iTotalLength += 4; // for \r\n * 2
        }
        iTotalLength += 4 + iBoundaryLength; // for Boundary--\r\n at end
        char * valuesBuffer = new char[iTotalLength + 1];
        valuesBuffer[0] = '\0';
        char szBoundaryBegin[3] = "--";
        char szNewLine[3] = "\r\n";
        char * pActivePosition;
        pActivePosition = valuesBuffer;
        for(iter = m_values.begin(); iter != m_values.end(); iter++)
        {
            int iValueBufferLength = (*iter)->GetLength();
            char * szValueBuffer = new char[iValueBufferLength];
            (*iter)->Write ( szValueBuffer, m_version );

            memcpy ( pActivePosition, szBoundaryBegin, 2 );
            pActivePosition += 2; // add the -- to the string
            memcpy ( pActivePosition, szBoundary, iBoundaryLength );
            pActivePosition += iBoundaryLength; // add the boundary to the string
            memcpy ( pActivePosition, szNewLine, 2 );
            pActivePosition += 2; // add a new line
            memcpy ( pActivePosition, szValueBuffer, iValueBufferLength );
            pActivePosition += iValueBufferLength; // add the value's data
            memcpy ( pActivePosition, szNewLine, 2 );
            pActivePosition += 2; // add a new line

            // If it weren't binary data, this would work
            // sprintf ( valuesBuffer, "%s--%s\r\n%s\r\n", valuesBuffer, szBoundary, szValueBuffer );
        }
        memcpy ( pActivePosition, szBoundaryBegin, 2 );
        pActivePosition += 2; // add --
        memcpy ( pActivePosition, szBoundary, iBoundaryLength );
        pActivePosition += iBoundaryLength; // add the boundary
        memcpy ( pActivePosition, szBoundaryBegin, 2 );
        pActivePosition += 2; // add --
        memcpy ( pActivePosition, szNewLine, 2 );
        pActivePosition += 2; // add a new line

        //sprintf ( valuesBuffer, "%s--%s--\r\n", valuesBuffer, szBoundary ); 

        // copy it all into the content buffer (not sure quite why we do this)
        iContentLength = (int)pActivePosition - (int)valuesBuffer;
        memcpy ( szContent, valuesBuffer, iContentLength );
        
    
        // add the length of the content to the header
        sprintf ( szBuffer, "%sContent-Length: %d\r\n", szBuffer, iContentLength );
    }
    sprintf ( szBuffer, "%s\r\n", szBuffer ); // terminate the header

        // Create a socket
    CTCPClientSocket* pSocket = pTCP->CreateClient ();
    if ( !pSocket )
    {
        return NULL;
    }

    // Connect to the server
    if ( !pSocket->Connect ( m_remotehost, m_port ) )
    {
        delete pSocket;
        return NULL;
    }
    
    // Write the header
    size_t sizeRequest = strlen(szBuffer);
    if ( pSocket->WriteBuffer ( szBuffer, sizeRequest ) < (int)sizeRequest )
    {
        delete pSocket;
        return NULL;
    }

    // send the POST data
    if ( m_type == HTTP_POST )
    {
        char * pszContent = szContent;
        sizeRequest = iContentLength;
        long lAmmountSent = 0;
        long lAmmountToSend = 0;
        while ( lAmmountSent < iContentLength )
        {
            lAmmountToSend = m_lPacketSize;
            if ( iContentLength - lAmmountSent < m_lPacketSize )
                lAmmountToSend = iContentLength - lAmmountSent;

            if ( pSocket->WriteBuffer ( pszContent, lAmmountToSend ) < lAmmountToSend )
            {
                delete pSocket;
                return NULL;
            }
            lAmmountSent += lAmmountToSend;
            pszContent += lAmmountToSend;

            if ( m_DataSentHandler )
                m_DataSentHandler ( lAmmountSent, iContentLength );
        }
    }

    // Read out the reply header
    char szReplyHeaderBuffer [2048];
    unsigned int uiHeaderSize;
    if ( !( uiHeaderSize = ReadHeader ( pSocket, szReplyHeaderBuffer, 2048 ) ) )
    {
        //strcpy ( m_szLastError, "Failed reading out response" );
        delete pSocket;
        //return false;
        return NULL;
    }
    szReplyHeaderBuffer[uiHeaderSize] = '\0'; // terminate the header

    // read all the data
    char szTempBuffer[2048];
    char * szReadBuffer;
    int iBufferSize = 2048; // the original buffer size
    szReadBuffer = (char *)malloc(iBufferSize); // 2KB
    
    strcpy(szReadBuffer, "\0");
    if ( !szReadBuffer ) // couldn't allocate
        return NULL;

    int iSize = 0;
    int iSizeTotal = 0; // the total ammount read
    while ( ( iSize = pSocket->ReadBuffer ( szTempBuffer, 2048 ) ) > 0 )
    {
        iSizeTotal += iSize;
        if ( iSizeTotal > iBufferSize )
        {
            // expand the buffer by 2KB
            iBufferSize += 2048;
            szReadBuffer = (char *)realloc(szReadBuffer, iBufferSize);

            if ( !szReadBuffer ) // couldn't allocate
                return NULL;
        }
        memcpy(szReadBuffer + iSizeTotal - iSize, szTempBuffer, iSize); // using this instead of strcat or sprintf prevents a crash
    }
    szReadBuffer[iSizeTotal] = '\0';
    pSocket->Disconnect ();
    delete pSocket;

    // Parse the reply
    CHTTPResponse * Response = new CHTTPResponse();
    if ( !Response->Parse ( szReplyHeaderBuffer, uiHeaderSize, szReadBuffer, iSizeTotal ) )
    {
        free(szReadBuffer);
        return NULL;
    }

    free(szReadBuffer);
    m_lastresponse = Response;
    return Response;
}

int CHTTPRequest::GetLength ()
{
    return 1024 * 1024 * 5; // 5 MB ugly hack CBA to code a real length for now
}

bool CHTTPRequest::ParseURL ( const char* szURL, char* szProtocol, unsigned int uiProtocolSize, char* szHost, unsigned int uiSizeHost, unsigned short& usPort, char* szPath, unsigned int uiSizePath )
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

int CHTTPRequest::ReadHeader ( CTCPSocket* pSocket, char* pBuffer, unsigned int uiBufferSize )
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

CHTTPResponse * CHTTPRequest::GetLastResponse ( void )
{
    return m_lastresponse;
}

void CHTTPRequest::SetDataSentHandler ( DataSentHandler * handler )
{
    m_DataSentHandler = handler;
}

