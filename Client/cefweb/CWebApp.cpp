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

[[nodiscard]] CefRefPtr<CefResourceHandler> CWebApp::HandleError(const SString& strError, unsigned int uiError)
{
    auto stream = CefStreamReader::CreateForData(
        (void*)strError.c_str(), 
        strError.length()
    );
    if (!stream)
        return nullptr;
    return CefRefPtr<CefResourceHandler>(new CefStreamResourceHandler(uiError, strError, "text/plain", CefResponse::HeaderMap(), stream));
}

void CWebApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    if (!command_line)
        return;

    const auto pWebCore = static_cast<CWebCore*>(g_pCore->GetWebCore());
    if (!pWebCore)
        return;

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
    if (!browser || !frame || !request)
        return nullptr;

    const auto pWebCore = static_cast<CWebCore*>(g_pCore->GetWebCore());
    if (!pWebCore)
        return nullptr;

    auto pWebView = pWebCore->FindWebView(browser);
    if (!pWebView || !pWebView->IsLocal())
        return nullptr;

    CefURLParts urlParts;
    if (!CefParseURL(request->GetURL(), urlParts))
        return nullptr;

    if (!urlParts.host.str)
        return nullptr;

    SString host = UTF16ToMbUTF8(urlParts.host.str);
    if (scheme_name == "http" && host == "mta")
    {
        // Scheme format: https://mta/resourceName/file.html or https://mta/local/file.html for the current resource

        // Get resource name and path
        if (!urlParts.path.str)
            return HandleError("404 - Not found", 404);

        SString path = UTF16ToMbUTF8(urlParts.path.str);
        if (std::size(path) < 2)
            return HandleError("404 - Not found", 404);

        path = path.substr(1);            // Remove slash at the front
        if (const auto slashPos = path.find('/'); slashPos == std::string::npos)
        {
            static constexpr auto ERROR_404 = "404 - Not found";
            static constexpr unsigned int CODE_404 = 404;
            return HandleError(ERROR_404, CODE_404);
        }
        else
        {
            SString resourceName = path.substr(0, slashPos);
            SString resourcePath = path.substr(slashPos + 1);

            if (resourcePath.empty())
            {
                static constexpr auto ERROR_404 = "404 - Not found";
                static constexpr unsigned int CODE_404 = 404;
                return HandleError(ERROR_404, CODE_404);
            }

            // Get mime type from extension
            CefString mimeType;
            if (const auto pos = resourcePath.find_last_of('.'); pos != std::string::npos)
                mimeType = CefGetMimeType(resourcePath.substr(pos + 1));

            // Make sure we provide a mime type, even
            // when we cannot deduct it from the file extension
            if (mimeType.empty())
            {
                mimeType = "application/octet-stream";
            }

            if (pWebView->HasAjaxHandler(resourcePath))
            {
                std::vector<SString> vecGet;
                std::vector<SString> vecPost;

                if (urlParts.query.str)
                {
                    SString              strGet = UTF16ToMbUTF8(urlParts.query.str);
                    std::vector<SString> vecTmp;
                    vecTmp.reserve(8);  // Reserve space for common query parameter count
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
                if (auto postData = request->GetPostData(); postData)
                {
                    postData->GetElements(vecPostElements);

                    SString key;
                    SString value;
                    for (auto&& post : vecPostElements)
                    {
                        // Limit to 5MiB and allow byte data only
                        constexpr size_t MAX_POST_SIZE = 5 * 1024 * 1024;
                        size_t bytesCount = post->GetBytesCount();
                        if (bytesCount > MAX_POST_SIZE || post->GetType() != CefPostDataElement::Type::PDE_TYPE_BYTES)
                            continue;

                        // Make string from buffer
                        auto buffer = std::make_unique<char[]>(bytesCount);
                        // Verify GetBytes succeeded before using buffer
                        size_t bytesRead = post->GetBytes(bytesCount, buffer.get());
                        if (bytesRead != bytesCount)
                            continue;
                        SStringX postParam(buffer.get(), bytesCount);

                        // Parse POST data into vector
                        std::vector<SString> vecTmp;
                        vecTmp.reserve(8);
                        postParam.Split("&", vecTmp);

                        for (auto&& param : vecTmp)
                        {
                            param.Split("=", &key, &value);
                            vecPost.push_back(key);
                            vecPost.push_back(value);
                        }
                    }
                }

                CefRefPtr<CAjaxResourceHandler> handler(new CAjaxResourceHandler(vecGet, vecPost, mimeType));
                pWebView->HandleAjaxRequest(resourcePath, handler.get());
                return handler;
            }
            else
            {
                // Calculate MTA resource path
                static constexpr auto LOCAL = "local";
                path = (resourceName != LOCAL) ? ":" + resourceName + "/" + resourcePath : resourcePath;

                // Calculate absolute path
                if (!pWebView->GetFullPathFromLocal(path))
                {
                    static constexpr auto ERROR_404 = "404 - Not found";
                    static constexpr unsigned int CODE_404 = 404;
                    return HandleError(ERROR_404, CODE_404);
                }

                // Verify local files
                CBuffer fileData;
                if (!pWebView->VerifyFile(path, fileData))
                {
                    static constexpr auto ERROR_403 = "403 - Access Denied";
                    static constexpr unsigned int CODE_403 = 403;
                    return HandleError(ERROR_403, CODE_403);
                }

                // Finally, load the file stream
                if (!fileData.GetData() || fileData.GetSize() == 0)
                {
                    static constexpr char emptyStr[] = "";
                    fileData = CBuffer(emptyStr, std::size(emptyStr));
                }

                auto stream = CefStreamReader::CreateForData(
                    fileData.GetData(),
                    fileData.GetSize()
                );
                if (!stream)
                {
                    static constexpr auto ERROR_404 = "404 - Not found";
                    static constexpr unsigned int CODE_404 = 404;
                    return HandleError(ERROR_404, CODE_404);
                }
                    
                return CefRefPtr<CefResourceHandler>(new CefStreamResourceHandler(mimeType, stream));
            }
        }
    }

    // Return null if there is no matching scheme
    // This falls back to letting CEF handle the request
    return nullptr;
}
