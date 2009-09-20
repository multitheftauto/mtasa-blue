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

#include "CTCP.h"
#include "CHTTPResponse.h"

typedef void ( DataSentHandler ) ( long lAmmountSent, long lTotalToSend );


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
    std::string         m_strContentType;
    std::string         m_strName;
    std::string         m_strFileName;
    std::string         m_strContents;

public:
                        CHTTPValue ( const char* szName, const char* szContents, int iContentsLength, const char* szContentType = NULL, const char* szFilename = NULL );
                        CHTTPValue ( const char* szName, const char* szContents, const char* szContentType = NULL, const char* szFilename = NULL );
                        CHTTPValue ( const CHTTPValue& value );
                        ~CHTTPValue ();
    void                Write ( std::string& strResult );
    int                 GetLength ();
};

class CHTTPRequest
{
private:
    eRequestType        m_type;
    std::string         m_strUserAgent;
    std::string         m_strReferer;
    std::string         m_strRemoteFile;
    std::string         m_strRemoteHost;
    std::string         m_strLocalIP;
    eHttpVersion        m_version;
    std::list<CHTTPValue*> m_values;
    unsigned short      m_port;
    long                m_lPacketSize;
    DataSentHandler *   m_DataSentHandler;

    bool                ParseURL ( const char* szURL, std::string& strProtocol, std::string& strHost, unsigned short& usPort, std::string& strPath );
    bool                ReadHeader ( CTCPSocket* pSocket, std::string& strHeader );

public:
                        CHTTPRequest ( const char * szURL, eRequestType type = HTTP_GET, eHttpVersion version = HTTP_1_0 );
                        ~CHTTPRequest ();
    void                SetType ( eRequestType type );
    void                SetReferer ( const char* szReferer );
    void                SetUserAgent ( const char* szUserAgent );
    void                SetRemoteHost ( const char* szRemoteHost );
    void                SetRemotePath ( const char* szRemotePath );
    long                SetPacketSize ( long lPacketSize ) { m_lPacketSize = lPacketSize; return lPacketSize; };
    void                SetLocalIP ( const char* szLocalIP ) { m_strLocalIP = szLocalIP ? szLocalIP : ""; }
    void                AddValue ( CHTTPValue * pValue );
    CHTTPResponse *     Send ( CTCP * pTCP );
    void                SetDataSentHandler ( DataSentHandler * handler );
};

#endif

