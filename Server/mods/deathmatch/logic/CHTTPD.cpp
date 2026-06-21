/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHTTPD.cpp
 *  PURPOSE:     Built-in HTTP webserver class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// cpp-httplib defines its own ssize_t typedef (as __int64) on Windows, guarded
// by _SSIZE_T_DEFINED. json-c (pulled in later through the resource/database
// headers in this translation unit) unconditionally does `typedef SSIZE_T
// ssize_t`. On 32-bit builds SSIZE_T is 32-bit while httplib's __int64 is
// 64-bit, so the two typedefs clash with "redefinition; different basic types".
// That hard error desyncs the parser and cascades into every MTA header that
// follows. Define ssize_t as SSIZE_T up-front and mark it defined so httplib
// reuses it and stays consistent with json-c.
#ifdef _WIN32
    #include <BaseTsd.h>
    #ifndef _SSIZE_T_DEFINED
typedef SSIZE_T ssize_t;
        #define _SSIZE_T_DEFINED
    #endif
#endif

#include <httplib.h>
#include "CHTTPD.h"
#include "CGame.h"
#include "CAccountManager.h"
#include "CMainConfig.h"
#include "CResource.h"
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <SharedUtil.Crypto.h>

#ifndef WIN32
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#ifdef WIN32
    #include <ws2tcpip.h>
#endif

extern CGame* g_pGame;

CHTTPD::CHTTPD()
    : m_httpServer(std::make_unique<httplib::Server>()),
      m_BruteForceProtect(4, 30000, 60000 * 5)  // Max of 4 attempts per 30 seconds, then 5 minute ignore
      ,
      m_HttpDosProtect(0, 0, 0)
{
    m_pGuestAccount = g_pGame->GetAccountManager()->AddGuestAccount(HTTP_GUEST_ACCOUNT_NAME);

    m_HttpDosProtect = CConnectHistory(g_pGame->GetConfig()->GetHTTPDosThreshold(), 10000,
                                       60000 * 1);  // Max of 'n' connections per 10 seconds, then 1 minute ignore

    std::vector<SString> excludeList;
    g_pGame->GetConfig()->GetHTTPDosExclude().Split(",", excludeList);
    m_HttpDosExcludeMap = std::set<SString>(excludeList.begin(), excludeList.end());
}

CHTTPD::~CHTTPD()
{
    StopHTTPD();
}

bool CHTTPD::StopHTTPD()
{
    if (m_bStartedServer)
    {
        m_httpServer->stop();
        if (m_serverThread.joinable())
            m_serverThread.join();
        m_bStartedServer = false;
        return true;
    }
    return false;
}

// Populate our HttpRequest from a cpp-httplib request
static void PopulateHttpRequest(HttpRequest& httpReq, const httplib::Request& req)
{
    httpReq.sUri = req.path;
    httpReq.sOriginalUri = req.path;
    httpReq.sBody = req.body;
    httpReq.m_strAddress = req.remote_addr;
    httpReq.m_nPort = req.remote_port;
    httpReq.nSecure = 0;

    // Copy headers, normalizing names to lowercase.
    // cpp-httplib preserves the original header case (e.g. "Authorization"),
    // but MTA code (CheckAuthentication, etc.) expects lowercase keys.
    for (const auto& h : req.headers)
    {
        std::string key = h.first;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        httpReq.oRequestHeaders[key] = h.second;
    }

    // Determine request method
    if (req.method == "GET")
        httpReq.nRequestMethod = REQUESTMETHOD_GET;
    else if (req.method == "POST")
        httpReq.nRequestMethod = REQUESTMETHOD_POST;
    else if (req.method == "HEAD")
        httpReq.nRequestMethod = REQUESTMETHOD_HEAD;
    else if (req.method == "OPTIONS")
        httpReq.nRequestMethod = REQUESTMETHOD_OPTIONS;
    else if (req.method == "PUT")
        httpReq.nRequestMethod = REQUESTMETHOD_PUT;
    else if (req.method == "DELETE")
        httpReq.nRequestMethod = REQUESTMETHOD_DELETE;
    else if (req.method == "PATCH")
        httpReq.nRequestMethod = REQUESTMETHOD_PATCH;
    else
        httpReq.nRequestMethod = REQUESTMETHOD_UNKNOWN;

    // Parse query string parameters and populate both maps.
    // oFormValueMap is what CResourceHTMLItem passes to Lua callbacks;
    // oQueryValueMap is used by some C++ code for direct lookups.
    for (const auto& p : req.params)
    {
        FormValue fv;
        fv.sBody = p.second;
        httpReq.oFormValueMap[p.first] = fv;
        httpReq.oQueryValueMap[p.first] = fv;
    }

    // Parse cookies
    if (req.has_header("Cookie"))
    {
        std::string cookieStr = req.get_header_value("Cookie");
        size_t      pos = 0;
        while (pos < cookieStr.size())
        {
            // Skip whitespace
            while (pos < cookieStr.size() && cookieStr[pos] == ' ')
                pos++;
            size_t eq = cookieStr.find('=', pos);
            size_t semi = cookieStr.find(';', pos);
            if (eq == std::string::npos || eq >= semi)
            {
                pos = (semi != std::string::npos) ? semi + 1 : std::string::npos;
                continue;
            }
            std::string name = cookieStr.substr(pos, eq - pos);
            std::string value = cookieStr.substr(eq + 1, (semi != std::string::npos) ? semi - eq - 1 : std::string::npos);
            httpReq.oCookieMap[name] = value;
            pos = (semi != std::string::npos) ? semi + 1 : std::string::npos;
        }
    }
}

// Case-insensitive comparison of a header name against a known literal.
static bool EqualsIgnoreCase(const std::string& strHeader, const char* szName)
{
    size_t i = 0;
    for (; i < strHeader.size() && szName[i]; ++i)
        if (::tolower(static_cast<unsigned char>(strHeader[i])) != ::tolower(static_cast<unsigned char>(szName[i])))
            return false;
    return i == strHeader.size() && szName[i] == '\0';
}

// Apply our HttpResponse to a cpp-httplib response
static void ApplyHttpResponse(const HttpResponse& httpRes, httplib::Response& res)
{
    res.status = static_cast<int>(httpRes.m_nResponseCode);

    // cpp-httplib's set_content owns the Content-Type header, so we must source
    // the resource-provided value (stored in oResponseHeaders, e.g.
    // "content-type") and pass it there. Emitting it again in the loop below
    // would produce a duplicate/conflicting Content-Type header. Default to
    // text/html when the resource didn't set one.
    std::string strContentType = "text/html";
    for (const auto& h : httpRes.oResponseHeaders)
    {
        if (EqualsIgnoreCase(h.first, "content-type"))
        {
            strContentType = h.second;
            break;
        }
    }

    const std::string& body = httpRes.GetBody();
    res.set_content(body.data(), body.size(), strContentType);

    for (const auto& h : httpRes.oResponseHeaders)
    {
        // Already applied via set_content; skip to avoid a duplicate header.
        if (EqualsIgnoreCase(h.first, "content-type"))
            continue;
        res.set_header(h.first, h.second);
    }
}

// Extract the first path segment (resource name) and shorten the URI.
// Returns true if a resource was extracted, false if at root.
static bool ExtractResourceName(std::string& sUri, std::string& strResourceName)
{
    if (sUri.empty() || sUri[0] != '/')
        return false;

    size_t start = 1;  // Skip leading '/'
    size_t end = sUri.find('/', start);

    if (end == std::string::npos)
    {
        strResourceName = sUri.substr(start);
        sUri = "/";
    }
    else
    {
        strResourceName = sUri.substr(start, end - start);
        sUri = sUri.substr(end);  // Keep the trailing slash, e.g. "/rest/of/path"
    }

    return !strResourceName.empty();
}

void CHTTPD::SetupHandlers()
{
    // Catch-all handler for all HTTP methods and paths.
    // cpp-httplib handles the routing; we just do resource dispatch.
    auto handler = [this](const httplib::Request& req, httplib::Response& res)
    {
        if (!PassesDosCheck(req.remote_addr.c_str(), req.remote_port))
        {
            res.status = 503;
            res.set_content("Service Unavailable", "text/plain");
            return;
        }

        // Server not ready check
        if (!g_pGame->IsServerFullyUp())
        {
            res.status = 200;
            res.set_content("The server is not ready. Please try again in a minute.", 57, "text/plain");
            return;
        }

        // Populate our request object
        HttpRequest httpReq;
        PopulateHttpRequest(httpReq, req);

        HttpResponse httpRes;

        // Lock game state before routing to resources
        g_pGame->Lock();

        // Extract resource name from path
        std::string sUri = httpReq.sUri;
        std::string strResourceName;
        bool        bFoundResource = ExtractResourceName(sUri, strResourceName);

        if (bFoundResource)
        {
            // Update the URI to reflect what EHS routing would have produced.
            // EHS stripped the resource name prefix from the path; we do the same.
            // Also strip the /call/ prefix so HandleRequestCall receives just the
            // function name (e.g. "setQuery" not "/call/setQuery").
            if (sUri.compare(0, 6, "/call/") == 0)
                sUri.erase(0, 6);  // remove "/call/" prefix, leaving just "functionName"

            httpReq.sUri = sUri;

            // Look up the resource
            auto it = m_ResourceMap.find(strResourceName);
            if (it != m_ResourceMap.end())
            {
                // Capture the status code. HandleRequest and its callees
                // (RequestLogin, HandleRequestActive) return the code rather
                // than setting it on the response object directly.
                httpRes.m_nResponseCode = it->second->HandleRequest(&httpReq, &httpRes);
            }
            else
            {
                httpRes.SetBody("404 - Not Found", 15);
                httpRes.m_nResponseCode = HTTP_STATUS_CODE_404_NOT_FOUND;
            }
        }
        else
        {
            // No resource in path, use the default handler
            httpRes.m_nResponseCode = HandleRequest(&httpReq, &httpRes);
        }

        g_pGame->Unlock();

        // Translate response back to httplib
        ApplyHttpResponse(httpRes, res);

        // Track bytes sent for stats
        m_llTotalBytesSent += res.body.size();
    };

    m_httpServer->Get(".*", handler);
    m_httpServer->Post(".*", handler);
    m_httpServer->Put(".*", handler);
    m_httpServer->Delete(".*", handler);
    m_httpServer->Patch(".*", handler);
    m_httpServer->Options(".*", handler);
}

bool CHTTPD::StartHTTPD(const char* szIP, unsigned int port)
{
    if (m_bStartedServer)
        return false;

    // Size the worker thread pool from the server config so the httpthreadcount
    // setting keeps working; cpp-httplib would otherwise use its own default.
    int iThreadCount = g_pGame->GetConfig()->GetHTTPThreadCount();
    if (iThreadCount < 1)
        iThreadCount = 1;
    m_httpServer->new_task_queue = [iThreadCount] { return new httplib::ThreadPool(static_cast<size_t>(iThreadCount)); };

    // Bind first and verify it succeeded. A failed bind (port in use,
    // permission denied, ...) must propagate so the caller reports the error,
    // and so we never spin a background thread on an invalid socket.
    bool bBound;
    if (szIP && szIP[0])
    {
        in_addr addr{};
#ifdef WIN32
        if (InetPtonA(AF_INET, szIP, &addr) != 1)
            return false;
#else
        if (inet_pton(AF_INET, szIP, &addr) != 1)
            return false;
#endif
        bBound = m_httpServer->bind_to_port(szIP, static_cast<int>(port));
    }
    else
    {
        bBound = m_httpServer->bind_to_port("0.0.0.0", static_cast<int>(port));
    }

    if (!bBound)
        return false;

    SetupHandlers();

    // Run the accept loop on a background thread. listen_after_bind() blocks
    // until stop() closes the socket, then returns, so a single call is correct.
    // Looping it would busy-spin the moment the socket became invalid.
    m_serverThread = std::thread([this]() { m_httpServer->listen_after_bind(); });

    m_bStartedServer = true;
    return true;
}

void CHTTPD::RegisterResource(CResource* resource, const char* szName)
{
    if (szName && szName[0])
        m_ResourceMap[szName] = resource;
}

void CHTTPD::UnregisterResource(const char* szName)
{
    if (szName && szName[0])
        m_ResourceMap.erase(szName);
}

void CHTTPD::AddBytesSent(long long llBytes)
{
    m_llTotalBytesSent += llBytes;
}

void CHTTPD::GetAllocationStats(SAllocationStats& outStats)
{
    // The old EHS server exposed live request/response/KB allocation counters.
    // cpp-httplib does not surface equivalent internals, so these stats are no
    // longer available and are reported as zero rather than as a misleading
    // proxy value.
    outStats = {};
}

// Called from worker thread. g_pGame->Lock() has already been called.
// creates a page based on user input -- either displays data from
//   form or presents a form for users to submit data.
HttpStatusCode CHTTPD::HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
{
    // Check if server verification was requested
    auto challenge = ipoHttpRequest->oRequestHeaders["crypto_challenge"];
    if (ipoHttpRequest->sUri == "/get_verification_key_code" && challenge != "")
    {
        auto    path = g_pServerInterface->GetModManager()->GetAbsolutePath("verify.key");
        SString encodedPublicKey;
        SharedUtil::FileLoad(path, encodedPublicKey, 392);

        using namespace CryptoPP;

        try
        {
            // Load public RSA key from disk
            RSA::PublicKey publicKey;
            std::string    base64Data = SharedUtil::Base64decode(encodedPublicKey);
            StringSource   stringSource(base64Data, true);
            publicKey.Load(stringSource);

            // Launch encryptor and encrypt
            RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
            SecByteBlock             cipherText(encryptor.CiphertextLength(challenge.size()));
            AutoSeededRandomPool     rng;
            encryptor.Encrypt(rng, (const CryptoPP::byte*)challenge.data(), challenge.size(), cipherText.begin());

            if (!cipherText.empty())
            {
                ipoHttpResponse->SetBody((const char*)cipherText.BytePtr(), cipherText.SizeInBytes());
                return HTTP_STATUS_CODE_200_OK;
            }
            else
                CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Empty crypto challenge was passed during verification\n");
        }
        catch (const std::exception&)
        {
            CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Invalid verify.key keyfile\n");
        }

        ipoHttpResponse->SetBody("", 0);
        return HTTP_STATUS_CODE_401_UNAUTHORIZED;
    }

    CAccount* account = CheckAuthentication(ipoHttpRequest);

    if (account)
    {
        if (!m_strDefaultResourceName.empty())
        {
            SString strWelcome("<a href='/%s/'>This is the page you want</a>", m_strDefaultResourceName.c_str());
            ipoHttpResponse->SetBody(strWelcome.c_str(), strWelcome.size());
            SString strNewURL("https://%s/%s/", ipoHttpRequest->oRequestHeaders["host"].c_str(), m_strDefaultResourceName.c_str());
            ipoHttpResponse->oResponseHeaders["location"] = strNewURL.c_str();
            return HTTP_STATUS_CODE_302_FOUND;
        }
    }

    SString strWelcome(
        "You haven't set a default resource in your configuration file. You can either do this or visit https://%s/<i>resourcename</i>/ to see a specific "
        "resource.<br/><br/>Alternatively, the server may be still starting up, if so, please try again in a minute.",
        ipoHttpRequest->oRequestHeaders["host"].c_str());
    ipoHttpResponse->SetBody(strWelcome.c_str(), strWelcome.size());
    return HTTP_STATUS_CODE_200_OK;
}

HttpStatusCode CHTTPD::RequestLogin(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
{
    if (m_WarnMessageTimer.Get() < 4000 && m_strWarnMessageForIp == ipoHttpRequest->GetAddress())
    {
        SString strMessage;
        strMessage += SString("Your IP address ('%s') is not associated with an authorized serial.", ipoHttpRequest->GetAddress().c_str());
        strMessage += SString("<br/><a href='%s'>See here for more information</a>",
                              "https:"
                              "//multitheftauto.com/authserialhttp");
        ipoHttpResponse->SetBody(strMessage, strMessage.length());
        return HTTP_STATUS_CODE_401_UNAUTHORIZED;
    }

    const char* szAuthenticateMessage = "Access denied, please login";
    ipoHttpResponse->SetBody(szAuthenticateMessage, strlen(szAuthenticateMessage));
    SString strName("Basic realm=\"%s\"", g_pGame->GetConfig()->GetServerName().c_str());
    ipoHttpResponse->oResponseHeaders["WWW-Authenticate"] = strName.c_str();
    return HTTP_STATUS_CODE_401_UNAUTHORIZED;
}

CAccount* CHTTPD::CheckAuthentication(HttpRequest* ipoHttpRequest)
{
    const std::string strAuthorization = ipoHttpRequest->oRequestHeaders["authorization"];

    if (strAuthorization.length() < 7 || strAuthorization.substr(0, 6) != "Basic ")
    {
        return m_pGuestAccount;
    }

    const std::string strAddress = ipoHttpRequest->GetAddress();
    const char*       szAddress = strAddress.c_str();
    const bool        bIsFlooding = m_BruteForceProtect.IsFlooding(szAddress);

    // Basic auth
    SString strAuthName, strAuthPassword;

    // If we're not flooding, or authorization header value isn't crazy long, let's parse it to get the username for console logs
    if (!bIsFlooding || strAuthorization.length() < 768)
    {
        SString strAuthDecoded = SharedUtil::Base64decode(strAuthorization.substr(6));
        strAuthDecoded.Split(":", &strAuthName, &strAuthPassword);
    }

    if (bIsFlooding)
    {
        if (strAuthName.length() > 0)
        {
            CLogger::AuthPrintf("HTTP: Ignoring login attempt for user '%s' from %s\n", strAuthName.substr(0, CAccountManager::MAX_USERNAME_LENGTH).c_str(),
                                szAddress);
        }
        else
        {
            CLogger::AuthPrintf("HTTP: Ignoring login attempt from %s\n", szAddress);
        }

        return m_pGuestAccount;
    }

    if (strAuthName.length() < CAccountManager::MIN_USERNAME_LENGTH || strAuthName.length() > CAccountManager::MAX_USERNAME_LENGTH ||
        strAuthPassword.length() < MIN_PASSWORD_LENGTH)
    {
        m_BruteForceProtect.AddConnect(szAddress);

        CLogger::AuthPrintf("HTTP: Failed login attempt from %s (bad login)\n", szAddress);

        return m_pGuestAccount;
    }

    if (CAccount* pAccount = g_pGame->GetAccountManager()->Get(strAuthName.c_str()); pAccount)
    {
        bool bSkipIpCheck;

        // Check that the password is right
        if (pAccount->IsPassword(strAuthPassword, &bSkipIpCheck))
        {
            // Check that it isn't the Console account
            if (pAccount->GetName() != CONSOLE_ACCOUNT_NAME)
            {
                // Do IP check if required
                if (!bSkipIpCheck && !g_pGame->GetAccountManager()->IsHttpLoginAllowed(pAccount, strAddress))
                {
                    if (m_WarnMessageTimer.Get() > 8000 || m_strWarnMessageForIp != strAddress)
                    {
                        m_strWarnMessageForIp = strAddress;
                        m_WarnMessageTimer.Reset();
                    }

                    CLogger::AuthPrintf("HTTP: Failed login for user '%s' because %s not associated with authorized serial\n", strAuthName.c_str(), szAddress);

                    return m_pGuestAccount;
                }

                // Handle initial login logging
                std::lock_guard guard(m_mutexLoggedInMap);

                if (m_LoggedInMap.find(strAuthName) == m_LoggedInMap.end())
                {
                    CLogger::AuthPrintf("HTTP: '%s' entered correct password from %s\n", strAuthName.c_str(), szAddress);
                }

                m_LoggedInMap[strAuthName] = GetTickCount64_();

                pAccount->OnLoginHttpSuccess(strAddress);

                return pAccount;
            }
        }
    }

    m_BruteForceProtect.AddConnect(szAddress);

    CLogger::AuthPrintf("HTTP: Failed login attempt for user '%s' from %s\n", strAuthName.c_str(), szAddress);

    return m_pGuestAccount;
}

// Called from worker thread. Careful now.
void CHTTPD::HttpPulse()
{
    // Prune connection-cache entries older than 60s so the map stays bounded
    // over time while each active connection is still evaluated only once.
    {
        const long long             llExpire = GetTickCount64_() - 60000;
        std::lock_guard<std::mutex> guard(m_mutexDosCache);
        for (auto it = m_DosCheckedConnections.begin(); it != m_DosCheckedConnections.end();)
        {
            if (it->second < llExpire)
                it = m_DosCheckedConnections.erase(it);
            else
                ++it;
        }
    }

    std::lock_guard<std::mutex> guard(m_mutexLoggedInMap);

    long long llExpireTime = GetTickCount64_() - 1000 * 60 * 5;  // 5 minute timeout

    map<string, long long>::iterator iter = m_LoggedInMap.begin();
    while (iter != m_LoggedInMap.end())
    {
        // Remove if too long since last request
        if (iter->second < llExpireTime)
        {
            CLogger::AuthPrintf("HTTP: '%s' no longer connected\n", iter->first.c_str());
            m_LoggedInMap.erase(iter++);
        }
        else
            iter++;
    }
}

//
// Do DoS check here
// Called from worker thread. Careful now.
bool CHTTPD::ShouldAllowConnection(const char* szAddress)
{
    std::lock_guard<std::mutex> guard(m_mutexHttpDosProtect);

    if (MapContains(m_HttpDosExcludeMap, szAddress))
        return true;

    if (m_HttpDosProtect.IsFlooding(szAddress))
        return false;

    m_HttpDosProtect.AddConnect(szAddress);

    if (m_HttpDosProtect.IsFlooding(szAddress))
    {
        CLogger::AuthPrintf("HTTP: Connection flood from '%s'. Ignoring for 1 min.\n", szAddress);
        return false;
    }

    return true;
}

// Connection gate invoked from the request handler. cpp-httplib calls the
// handler once per HTTP request, whereas ShouldAllowConnection (CConnectHistory)
// operates once per TCP connection. The cache is keyed by "address:port" so the
// requests sharing a single connection reuse one decision, while each new
// connection is still evaluated. Entries are pruned by age in HttpPulse.
bool CHTTPD::PassesDosCheck(const char* szAddress, int port)
{
    const std::string strKey = SString("%s:%d", szAddress, port);

    {
        std::lock_guard<std::mutex> guard(m_mutexDosCache);
        if (m_DosCheckedConnections.find(strKey) != m_DosCheckedConnections.end())
            return true;
    }

    if (!ShouldAllowConnection(szAddress))
        return false;

    std::lock_guard<std::mutex> guard(m_mutexDosCache);
    m_DosCheckedConnections[strKey] = GetTickCount64_();
    return true;
}
