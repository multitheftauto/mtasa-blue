/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/AjaxResourceHandler.cpp
*  PURPOSE:     CEF Handler for Ajax Requests with delayed results
*
*****************************************************************************/

#include "StdInc.h"
#include "CWebCore.h"
#include "CWebView.h"
#include "CAjaxResourceHandler.h"
#undef min

CAjaxResourceHandler::CAjaxResourceHandler ( std::vector<SString>& vecGet, std::vector<SString>& vecPost, const CefString& strMime) 
    : m_vecGetData(vecGet), m_vecPostData(vecPost), m_strMime(strMime)
{
}

std::vector<SString>& CAjaxResourceHandler::GetGetData ()
{
    return m_vecGetData;
}

std::vector<SString>& CAjaxResourceHandler::GetPostData ()
{
    return m_vecPostData;
}

void CAjaxResourceHandler::SetResponse ( const SString& data )
{
    m_strResponse = data;
    m_bHasData = true;

    if ( m_callback )
        m_callback->Continue( );
}

// CefResourceHandler implementation
void CAjaxResourceHandler::Cancel ()
{
}

bool CAjaxResourceHandler::CanGetCookie ( const CefCookie& cookie )
{
    return false;
}

bool CAjaxResourceHandler::CanSetCookie ( const CefCookie& cookie )
{
    return false;
}

void CAjaxResourceHandler::GetResponseHeaders ( CefRefPtr< CefResponse > response, int64& response_length, CefString& redirectUrl )
{
    response->SetStatus ( 200 );
    response->SetStatusText ( "OK" );
    response->SetMimeType ( m_strMime );
    response_length = -1;
}

bool CAjaxResourceHandler::ProcessRequest ( CefRefPtr< CefRequest > request, CefRefPtr< CefCallback > callback )
{
    // Since we have nothing to process yet, continue
    callback->Continue ();
    return true;
}

bool CAjaxResourceHandler::ReadResponse ( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr< CefCallback > callback )
{
    // If we have no data yet, wait
    if ( !m_bHasData )
    {   
        bytes_read = 0;
        m_callback = callback;
        callback->Continue ();
        return true;
    }

    // Are we done?
    if ( m_strResponse.length( ) - m_DataOffset <= 0 )
        return false;

    int copyBytes = std::min ( (uint)bytes_to_read, m_strResponse.length () - m_DataOffset );

    memcpy ( data_out, m_strResponse.c_str () + m_DataOffset, copyBytes );
    bytes_read = copyBytes;

    m_DataOffset += copyBytes;

    return true;
}
