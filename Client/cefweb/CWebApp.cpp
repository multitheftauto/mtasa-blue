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

#include <cef3/cef/include/cef_command_line.h>
#include <cef3/cef/include/cef_parser.h>
#include <cef3/cef/include/cef_resource_handler.h>
#include <cef3/cef/include/cef_response.h>
#include <cef3/cef/include/cef_stream.h>
#include <cef3/cef/include/wrapper/cef_stream_resource_handler.h>
#include "CAjaxResourceHandler.h"

namespace
{
    // Centralises command-line switch setup so both pre-launch callbacks stay in sync
    void ConfigureCommandLineSwitches(const CefRefPtr<CefCommandLine>& commandLine, const CefString& processType)
    {
        if (!commandLine)
            return;

        // CEF occasionally forwards dangling pointers when destroying
        if (!IsReadablePointer(commandLine.get(), sizeof(void*)))
            return;

        // Always provide base installation paths so loader-proxy can validate subprocess origin
        const SString gtaPath = GetCommonRegistryValue("", "GTA:SA Path");
        if (!gtaPath.empty())
        {
            commandLine->AppendSwitchWithValue("mta-gta-path", gtaPath);
        }

        const SString mtaPath = GetMTAProcessBaseDir();
        if (!mtaPath.empty())
        {
            commandLine->AppendSwitchWithValue("mta-base-path", mtaPath);
        }

        // Prevent Chromium from dropping privileges; required for elevated launches (see chromium/3960)
        commandLine->AppendSwitch("do-not-de-elevate");

        // Must apply essential CEF switches regardless of WebCore availability
        commandLine->AppendSwitch("disable-gpu-compositing");
        commandLine->AppendSwitch("enable-begin-frame-scheduling");
        // Explicitly block account sign-in to avoid crashes when Google API keys are registered on the system
        commandLine->AppendSwitchWithValue("allow-browser-signin", "false");

        if (processType.empty())
        {
            // Browser process only: unlock autoplay and legacy Blink features for resource compatibility
            commandLine->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
            commandLine->AppendSwitchWithValue("enable-blink-features", "ShadowDOMV0,CustomElementsV0,HTMLImports");
        }

        bool disableGpu = false;
        if (g_pCore && IsReadablePointer(g_pCore, sizeof(void*)))
        {
            auto* cvars = g_pCore->GetCVars();
            if (cvars && IsReadablePointer(cvars, sizeof(void*)))
            {
                bool gpuEnabled = true;
                cvars->Get("browser_enable_gpu", gpuEnabled);
                disableGpu = !gpuEnabled;
            }
        }

        if (disableGpu)
            commandLine->AppendSwitch("disable-gpu");
    }
}            // namespace

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
    ConfigureCommandLineSwitches(command_line, process_type);
}

void CWebApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    if (!command_line)
        return;

    // Add GTA path and MTA base path switches before g_pCore check
    // This callback runs in both browser process and subprocess
    // In subprocess, g_pCore is NULL, so switches must be added before that check
    // Read GTA path from registry
    const SString strGTAPath = GetCommonRegistryValue("", "GTA:SA Path");
    if (!strGTAPath.empty())
    {
        // Pass GTA directory path to CEFLauncher subprocess via command-line switch
        // CEF's AppendSwitchWithValue handles quoting automatically
        command_line->AppendSwitchWithValue("mta-gta-path", strGTAPath);
        // AddReportLog only available in browser process where g_pCore exists
    }
    const CefString processType = command_line->GetSwitchValue("type");
    ConfigureCommandLineSwitches(command_line, processType);
}

CefRefPtr<CefBrowserProcessHandler> CWebApp::GetBrowserProcessHandler()
{
    return this;
}

CefRefPtr<CefResourceHandler> CWebApp::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name,
                                              CefRefPtr<CefRequest> request)
{
    // browser or frame are NULL if the request does not orginate from a browser window
    // This is for exmaple true for the application cache or CEFURLRequests
    // (https://www.html5rocks.com/en/tutorials/appcache/beginner/)
    if (!browser || !frame || !request)
        return nullptr;

    if (!g_pCore) [[unlikely]]
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
            const SString resourceName = path.substr(0, slashPos);
            const SString resourcePath = path.substr(slashPos + 1);

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
                    const SString        strGet = UTF16ToMbUTF8(urlParts.query.str);
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
                if (const auto postData = request->GetPostData(); postData)
                {
                    postData->GetElements(vecPostElements);

                    SString key;
                    SString value;
                    for (const auto& post : vecPostElements)
                    {
                        // Limit to 5MiB and allow byte data only
                        constexpr size_t MAX_POST_SIZE = 5 * 1024 * 1024;
                        size_t bytesCount = post->GetBytesCount();
                        if (bytesCount > MAX_POST_SIZE || post->GetType() != CefPostDataElement::Type::PDE_TYPE_BYTES)
                            continue;

                        // Make string from buffer
                        const auto buffer = std::make_unique<char[]>(bytesCount);
                        // Verify GetBytes succeeded before using buffer
                        const size_t bytesRead = post->GetBytes(bytesCount, buffer.get());
                        if (bytesRead != bytesCount)
                            continue;
                        const SStringX postParam(buffer.get(), bytesCount);

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

                CefRefPtr<CAjaxResourceHandler> handler(new CAjaxResourceHandler(std::move(vecGet), std::move(vecPost), mimeType));
                pWebView->HandleAjaxRequest(resourcePath, handler.get());
                return handler;
            }
            else
            {
                // Calculate MTA resource path
                static constexpr auto LOCAL = "local";
                path = (resourceName != LOCAL) ? SString(":" + resourceName + "/" + resourcePath) : resourcePath;

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
