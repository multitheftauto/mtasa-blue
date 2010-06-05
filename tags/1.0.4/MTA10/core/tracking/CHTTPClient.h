/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/tracking/CHTTPClient.h
*  PURPOSE:     Header file for HTTP client class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHTTPCLIENT_H
#define __CHTTPCLIENT_H

#include "CHTTPResponse.h"
#include "CTCPManager.h"
#include "CTCPClientSocket.h"
#include <cstdio>

#ifndef snprintf
#define snprintf _snprintf
#endif

// HTTP buffer size (for OnRead)
#define HTTP_BUFFER_LENGTH      4096


//
// Output buffer for http queries.
//
class CHTTPBuffer : protected std::vector < char >
{
    static const unsigned int EXTRA_PADDING = 8;
    unsigned int uiLogicalSize;
public:
    CHTTPBuffer( void )
    {
        Clear ();
    }

    void Clear ( void )
    {
        resize ( EXTRA_PADDING );
        uiLogicalSize = 0;
        memset ( GetData (), 0, EXTRA_PADDING );
    }

    // Append bytes at the end of
    void Append ( const char* srcData, unsigned int uiLength )
    {
        resize ( uiLogicalSize + uiLength + EXTRA_PADDING );
        memcpy ( GetData () + uiLogicalSize, srcData, uiLength );
        uiLogicalSize += uiLength;
        memset ( GetData () + uiLogicalSize, 0, EXTRA_PADDING );
    }

    char* GetData ( void )
    {
        return &at( 0 );
    }

    unsigned int GetSize ( void ) const
    {
        return uiLogicalSize;
    }
};


//
// Thread safe output buffer for http queries.
//
class CHTTPThreadedBuffer
{
    CHTTPBuffer m_Contents;
    CCriticalSection criticalSection;
public:

    void Clear ( void )
    {
        criticalSection.Lock ();
        m_Contents.Clear ();
        criticalSection.Unlock ();
    }

    unsigned int GetSize ( void ) const
    {
        return m_Contents.GetSize ();
    }

    void Append ( const char* srcData, unsigned int uiLength )
    {
        criticalSection.Lock ();
        m_Contents.Append ( srcData, uiLength );
        criticalSection.Unlock ();
    }

    void Clone ( CHTTPBuffer& outContents )
    {
        criticalSection.Lock ();
        outContents = m_Contents;
        criticalSection.Unlock ();
    }
};


class CHTTPClient
{
public:
                        CHTTPClient             ( void );
                        ~CHTTPClient            ( void );

    void                Reset                   ( void );

    bool                Get                     ( const std::string& strURL );
    const std::string&  GetStatusMessage        ( void )        { return m_strStatus; };
    unsigned int        GetStatus               ( void )        { return m_Status; };
    
    bool                GetData                 ( CHTTPBuffer& outBuffer );
    unsigned int        PeekNextDataSize        ( void ) const;

    // Callbacks
    static void         OnRead                  ( void* pSocketPtr, void* pClassPtr );
    static void         OnConnect               ( void* pSocketPtr, void* pClassPtr );
    static void         OnClose                 ( void* pSocketPtr, void* pClassPtr );

private:
    bool                ParseURL                ( const char* szURL, char* szProtocol, unsigned int uiProtocolSize, char* szHost, unsigned int uiSizeHost, unsigned short& usPort, char* szPath, unsigned int uiSizePath );

    int                 ReadHeader              ( char* pBuffer, unsigned int uiBufferSize );

    CHTTPThreadedBuffer m_Buffer;

    unsigned int        m_Status;
    std::string         m_strStatus;
    bool                m_bCompleted;

    char                m_szProtocol[64];
    char                m_szHost[256];
    char                m_szPath[1024];
    unsigned short      m_usPort;

    CTCPClientSocket*   m_pHTTPSocket;
};

#endif
