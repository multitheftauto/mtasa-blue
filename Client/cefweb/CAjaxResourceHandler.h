/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/AjaxResourceHandler.h
 *  PURPOSE:     CEF Handler for Ajax Requests with delayed results
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <core/CAjaxResourceHandlerInterface.h>
#include <cef3/cef/include/cef_resource_handler.h>
#include <SString.h>

class CWebView;

class CAjaxResourceHandler : public CefResourceHandler, public CAjaxResourceHandlerInterface
{
public:
    CAjaxResourceHandler(std::vector<SString>& vecGet, std::vector<SString>& vecPost, const CefString& mimeType);

    virtual std::vector<SString>& GetGetData() override;
    virtual std::vector<SString>& GetPostData() override;
    ;
    virtual void SetResponse(const SString& data) override;
    ;

    // CefResourceHandler
    virtual void Cancel() override;
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
    virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;
    virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

    IMPLEMENT_REFCOUNTING(CAjaxResourceHandler);
    DISALLOW_COPY_AND_ASSIGN(CAjaxResourceHandler);

private:
    CefRefPtr<CefCallback> m_callback;
    std::vector<SString>   m_vecGetData;
    std::vector<SString>   m_vecPostData;
    SString                m_strResponse;
    CefString              m_strMime;
    bool                   m_bHasData = false;
    int                    m_DataOffset = 0;
};
