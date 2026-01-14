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
#include "CWebAppAuth.h"            // IPC code generation
#include <cstdlib>

namespace
{
    // Helper to detect Wine/Proton environment
    bool IsRunningOnWine()
    {
        HMODULE hNtdll = GetModuleHandle("ntdll.dll");
        return hNtdll && GetProcAddress(hNtdll, "wine_get_version");
    }

    // Centralises command-line switch setup so both pre-launch callbacks stay in sync
    void ConfigureCommandLineSwitches(const CefRefPtr<CefCommandLine>& commandLine, const CefString& processType)
    {
        if (!commandLine)
            return;

        // CEF occasionally forwards dangling pointers when destroying
        if (!IsReadablePointer(commandLine.get(), sizeof(void*)))
            return;

        // Always provide base installation paths so loader-proxy can validate subprocess origin
        SString gtaPath = GetCommonRegistryValue("", "GTA:SA Path");
        if (gtaPath.empty())
        {
            // Fallback for Wine/compatibility layer: check environment variable
            if (const char* envGtaPath = std::getenv("MTA_GTA_PATH"))
            {
                gtaPath = envGtaPath;
            }
        }

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

        // Enable external begin frame scheduling for MTA-controlled rendering
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
        bool enableVideoAccel = true;
        if (g_pCore && IsReadablePointer(g_pCore, sizeof(void*)))
        {
            auto* cvars = g_pCore->GetCVars();
            if (cvars && IsReadablePointer(cvars, sizeof(void*)))
            {
                bool gpuEnabled = true;
                cvars->Get("browser_enable_gpu", gpuEnabled);
                disableGpu = !gpuEnabled;

                cvars->Get("browser_enable_video_acceleration", enableVideoAccel);
            }
        }

        // Wine/Proton compatibility: Allow GPU unless explicitly disabled or forced software
        if (IsRunningOnWine())
        {
            if (std::getenv("MTA_FORCE_SOFTWARE_RENDERING"))
            {
                disableGpu = true;
            }
            else
            {
                // In Wine, we generally want to try GPU (DXVK handles it well)
                // But disable-gpu-compositing is already set above which is key
                // If user hasn't explicitly disabled GPU in cvars, let it run
            }
        }

        if (disableGpu)
        {
            commandLine->AppendSwitch("disable-gpu");
            // Also disable GPU compositing when GPU is disabled
            commandLine->AppendSwitch("disable-gpu-compositing");
        }

        // Hardware video decoding - enable when GPU is enabled and video acceleration is requested
        if (!disableGpu && enableVideoAccel)
        {
            commandLine->AppendSwitch("enable-accelerated-video-decode");
        }
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

    const CefString processType = command_line->GetSwitchValue("type");
    ConfigureCommandLineSwitches(command_line, processType);

    // Attach IPC validation code for render processes
    // This runs in browser process context where g_pCore and webCore are valid
    // The auth code is generated in CWebCore constructor and passed to subprocesses
    WebAppAuth::AttachAuthCodeToCommandLine(command_line);
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
                std::vector<std::string> vecGet;
                std::vector<std::string> vecPost;

                if (urlParts.query.str)
                {
                    const SString        strGet = UTF16ToMbUTF8(urlParts.query.str);
                    std::vector<SString> vecTmp;
                    vecTmp.reserve(8);  // Reserve space for common query parameter count
                    strGet.Split("&", vecTmp);

                    const size_t paramCount = vecTmp.size();
                    if (paramCount > 0)
                        vecGet.reserve(vecGet.size() + paramCount * 2);

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

                        const size_t postParamCount = vecTmp.size();
                        if (postParamCount > 0)
                            vecPost.reserve(vecPost.size() + postParamCount * 2);

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
