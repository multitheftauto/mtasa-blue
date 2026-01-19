/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        cefweb/CAjaxResourceHandler.cpp
 *  PURPOSE:     CEF Handler for Ajax Requests with delayed results
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAjaxResourceHandler.h"
#undef min

CAjaxResourceHandler::CAjaxResourceHandler(std::vector<SString> vecGet, std::vector<SString> vecPost, const CefString& strMime)
    : m_vecGetData(std::move(vecGet)), m_vecPostData(std::move(vecPost)), m_strMime(strMime)
{
}

CAjaxResourceHandler::~CAjaxResourceHandler()
{
    // Ensure callback is released if handler is destroyed before completion
    if (!m_callback)
        return;

    m_callback = nullptr;
}

void CAjaxResourceHandler::SetResponse(SString data)
{
    // Prevent response corruption: ignore subsequent calls after data is set
    if (m_bHasData) [[unlikely]]
        return;

    m_strResponse = std::move(data);
    m_bHasData = true;

    // Clear request data to free memory as it's no longer needed
    m_vecGetData.clear();
    m_vecGetData.shrink_to_fit();
    m_vecPostData.clear();
    m_vecPostData.shrink_to_fit();

    if (!m_callback)
        return;

    // Release callback to prevent memory leak
    auto callback = std::exchange(m_callback, nullptr);
    if (callback)
        callback->Continue();
}

// CefResourceHandler implementation
void CAjaxResourceHandler::Cancel()
{
    // Release callback reference on cancellation to prevent memory leak
    m_callback = nullptr;
}

void CAjaxResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
    if (!response) [[unlikely]]
        return;

    constexpr int HTTP_OK = 200;
    response->SetStatus(HTTP_OK);
    response->SetStatusText("OK");

    // Use default MIME type if none provided
    if (!m_strMime.empty())
        response->SetMimeType(m_strMime);
    else
        response->SetMimeType("application/octet-stream");

    response_length = -1;
}

bool CAjaxResourceHandler::ProcessRequest([[maybe_unused]] CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
    if (!callback)
        return false;

    // Store callback so SetResponse can resume once data is ready
    m_callback = callback;

    // Do not call Continue() yet; SetResponse triggers it after data is prepared
    return true;
}

bool CAjaxResourceHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, [[maybe_unused]] CefRefPtr<CefCallback> callback)
{
    // Validate input parameters first
    if (!data_out || bytes_to_read <= 0) [[unlikely]]
    {
        bytes_read = 0;
        return false;
    }

    // If we have no data yet, wait until SetResponse provides it
    if (!m_bHasData)
    {
        bytes_read = 0;
        return true;
    }

    // Are we done or response is empty?
    const auto responseLength = m_strResponse.length();
    if (m_DataOffset >= responseLength) [[unlikely]]
    {
        bytes_read = 0;
        return false;
    }

    const auto remainingBytes = responseLength - m_DataOffset;
    const auto copyBytes = std::min(static_cast<size_t>(bytes_to_read), remainingBytes);

    memcpy(data_out, m_strResponse.c_str() + m_DataOffset, copyBytes);

    // copyBytes is bounded by bytes_to_read (an int), so cast is always safe
    bytes_read = static_cast<int>(copyBytes);

    m_DataOffset += copyBytes;

    return true;
}
