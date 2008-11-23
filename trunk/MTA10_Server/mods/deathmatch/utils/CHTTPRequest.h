/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CHTTPRequest.h
*  PURPOSE:     HTTP request handling class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CHTTPRequest;
class CHTTPValue;

#ifndef CHTTPREQUEST_H
#define CHTTPREQUEST_H

#include <list>
#include "CTCP.h"
#include "CHTTPResponse.h"

typedef void ( DataSentHandler ) ( long lAmmountSent, long lTotalToSend );


using namespace std;
enum eHttpVersion
{
    HTTP_1_0 = 0,
    HTTP_1_1
};

enum eRequestType
{
    HTTP_GET = 0,
    HTTP_POST
};

class CHTTPCookie
{
private:
    char *              m_name;
    char *              m_value;
public:
                        CHTTPCookie ( char * szName, char * szValue );
                        ~CHTTPCookie ();
    char *              Write(char * szBuffer, eHttpVersion = HTTP_1_0);
    int                 GetLength(eHttpVersion = HTTP_1_0);
};

class CHTTPValue
{
private:
    char *              m_contenttype;
    char *              m_name;
    char *              m_filename;
    char *              m_contents;
    int                 m_contentslength;
public:
                        CHTTPValue ( char * szName, char * szContents, int iContentsLength, char * szContentType = NULL, char * szFilename = NULL );
                        CHTTPValue ( char * szName, char * szContents, char * szContentType = NULL, char * szFilename = NULL );
                        CHTTPValue ( const CHTTPValue * value );
                        ~CHTTPValue ();
    char *              Write(char * szBuffer, eHttpVersion version = HTTP_1_0);
    int                 GetLength(eHttpVersion version = HTTP_1_0);
};

class CHTTPRequest
{
private:
    eRequestType        m_type;
    char *              m_useragent;
    char *              m_referer;
    char *              m_remotefile;
    char *              m_remotehost;
    eHttpVersion        m_version;
    list<CHTTPValue*>   m_values;
    unsigned short      m_port;
    CHTTPResponse *     m_lastresponse;
    long                m_lPacketSize;
    DataSentHandler *   m_DataSentHandler;

    int                 GetLength ();
    bool                ParseURL ( const char* szURL, char* szProtocol, unsigned int uiProtocolSize, char* szHost, unsigned int uiSizeHost, unsigned short& usPort, char* szPath, unsigned int uiSizePath );
    int                 ReadHeader ( CTCPSocket* pSocket, char* pBuffer, unsigned int uiBufferSize );
public:
                        CHTTPRequest ( char * szURL, eRequestType type = HTTP_GET, eHttpVersion version = HTTP_1_0 );
                        ~CHTTPRequest ();
    void                SetType ( eRequestType type );
    void                SetReferer ( char * szReferer );
    void                SetUserAgent ( char * szUserAgent );
    void                SetRemoteHost ( char * szRemoteHost );
    void                SetRemotePath ( char * szRemotePath );
    long                SetPacketSize ( long lPacketSize ) { m_lPacketSize = lPacketSize; return lPacketSize; };
    void                AddValue ( CHTTPValue * pValue );
    CHTTPResponse *     Send ( CTCP * pTCP );
    CHTTPResponse *     GetLastResponse ( void );
    void                SetDataSentHandler ( DataSentHandler * handler );
    

};

#endif

