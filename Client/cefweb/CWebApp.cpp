/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CWebApp.cpp
 *  PURPOSE:     Web app class
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CWebApp.h"

#include <cef3/cef/include/wrapper/cef_stream_resource_handler.h>
#include <cef3/cef/include/cef_parser.h>
#include "CAjaxResourceHandler.h"

CefRefPtr<CefResourceHandler> CWebApp::HandleError(const SString& strError, unsigned int uiError)
{
    auto stream = CefStreamReader::CreateForData((void*)strError.c_str(), strError.length());
    return new CefStreamResourceHandler(uiError, strError, "text/plain", CefResponse::HeaderMap(), stream);
}

void CWebApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    CWebCore* pWebCore = static_cast<CWebCore*>(g_pCore->GetWebCore());

    if (!pWebCore->GetGPUEnabled())
        command_line->AppendSwitch("disable-gpu");

    // Disable the AutoDeElevate feature to make launching CEF with Admin privileges work.
    // https://github.com/chromiumembedded/cef/issues/3960
    // https://chromium-review.googlesource.com/c/chromium/src/+/6515318
    command_line->AppendSwitch("do-not-de-elevate");

    command_line->AppendSwitch("disable-gpu-compositing"); // always disable this, causes issues with official builds

    // command_line->AppendSwitch("disable-d3d11");
    command_line->AppendSwitch("enable-begin-frame-scheduling");

    // browser-signin switch(or lack thereof) produces crashes when GOOGLE API keys are present in the OS registry
    command_line->AppendSwitchWithValue("allow-browser-signin", "false");

    if (process_type.empty())
    {
        command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
        command_line->AppendSwitchWithValue("enable-blink-features", "ShadowDOMV0,CustomElementsV0,HTMLImports");
    }
}

CefRefPtr<CefResourceHandler> CWebApp::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name,
                                              CefRefPtr<CefRequest> request)
{
    // browser or frame are NULL if the request does not orginate from a browser window
    // This is for exmaple true for the application cache or CEFURLRequests
    // (https://www.html5rocks.com/en/tutorials/appcache/beginner/)
    if (!browser || !frame)
        return nullptr;

    CWebCore* pWebCore = static_cast<CWebCore*>(g_pCore->GetWebCore());
    auto      pWebView = pWebCore->FindWebView(browser);
    if (!pWebView || !pWebView->IsLocal())
        return nullptr;

    CefURLParts urlParts;
    if (!CefParseURL(request->GetURL(), urlParts))
        return nullptr;

    SString host = UTF16ToMbUTF8(urlParts.host.str);
    if (scheme_name == "http" && host == "mta")
    {
        // Scheme format: https://mta/resourceName/file.html or https://mta/local/file.html for the current resource

        // Get resource name and path
        SString path = UTF16ToMbUTF8(urlParts.path.str).substr(1);            // Remove slash at the front
        size_t  slashPos = path.find('/');
        if (slashPos == std::string::npos)
            return HandleError("404 - Not found", 404);

        SString resourceName = path.substr(0, slashPos);
        SString resourcePath = path.substr(slashPos + 1);

        if (resourcePath.empty())
            return HandleError("404 - Not found", 404);

        // Get mime type from extension
        CefString mimeType;
        size_t    pos = resourcePath.find_last_of('.');
        if (pos != std::string::npos)
            mimeType = CefGetMimeType(resourcePath.substr(pos + 1));

        // Make sure we provide a mime type, even
        // when we cannot deduct it from the file extension
        if (mimeType.empty())
            mimeType = "application/octet-stream";

        if (pWebView->HasAjaxHandler(resourcePath))
        {
            std::vector<SString> vecGet;
            std::vector<SString> vecPost;

            if (urlParts.query.str != nullptr)
            {
                SString              strGet = UTF16ToMbUTF8(urlParts.query.str);
                std::vector<SString> vecTmp;
                strGet.Split("&", vecTmp);

                SString key;
                SString value;
                for (auto&& param : vecTmp)
                {
                    param.Split("=", &key, &value);
                    vecGet.push_back(key);
                    vecGet.push_back(value);
                }
            }

            CefPostData::ElementVector vecPostElements;
            auto                       postData = request->GetPostData();
            if (postData.get())
            {
                request->GetPostData()->GetElements(vecPostElements);

                SString key;
                SString value;
                for (auto&& post : vecPostElements)
                {
                    // Limit to 5MiB and allow byte data only
                    size_t bytesCount = post->GetBytesCount();
                    if (bytesCount > 5 * 1024 * 1024 || post->GetType() != CefPostDataElement::Type::PDE_TYPE_BYTES)
                        continue;

                    // Make string from buffer
                    std::unique_ptr<char[]> buffer{new char[bytesCount]};
                    post->GetBytes(bytesCount, buffer.get());
                    SStringX param(buffer.get(), bytesCount);

                    // Parse POST data into vector
                    std::vector<SString> vecTmp;
                    param.Split("&", vecTmp);

                    for (auto&& param : vecTmp)
                    {
                        param.Split("=", &key, &value);
                        vecPost.push_back(key);
                        vecPost.push_back(value);
                    }
                }
            }

            auto handler = new CAjaxResourceHandler(vecGet, vecPost, mimeType);
            pWebView->HandleAjaxRequest(resourcePath, handler);
            return handler;
        }
        else
        {
            // Calculate MTA resource path
            if (resourceName != "local")
                path = ":" + resourceName + "/" + resourcePath;
            else
                path = resourcePath;

            // Calculate absolute path
            if (!pWebView->GetFullPathFromLocal(path))
                return HandleError("404 - Not found", 404);

            // Verify local files
            CBuffer fileData;
            if (!pWebView->VerifyFile(path, fileData))
                return HandleError("403 - Access Denied", 403);

            // Finally, load the file stream
            if (fileData.GetData() == nullptr || fileData.GetSize() == 0)
                fileData = CBuffer("", sizeof(""));

            auto stream = CefStreamReader::CreateForData(fileData.GetData(), fileData.GetSize());
            if (stream.get())
                return new CefStreamResourceHandler(mimeType, stream);
            return HandleError("404 - Not found", 404);
        }
    }

    // Return null if there is no matching scheme
    // This falls back to letting CEF handle the request
    return nullptr;
}
