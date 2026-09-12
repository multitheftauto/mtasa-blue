/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHTTPD.h
 *  PURPOSE:     Built-in HTTP webserver class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class implements the built-in HTTP web server.
// Uses cpp-httplib under the hood, replacing the old EHS dependency.

#pragma once

#include "CConnectHistory.h"
#include "httpd/Types.h"
#include <string>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <set>

// Forward-declare httplib types to avoid pulling in httplib.h
// (which requires _WIN32_WINNT >= 0x0A00) into every translation unit.
namespace httplib
{
    class Server;
    struct Request;
    struct Response;
}

class CResource;

class CHTTPD
{
public:
    CHTTPD();
    ~CHTTPD();

    // HTTP server lifecycle
    bool StartHTTPD(const char* szIP, unsigned int port);
    bool StopHTTPD();

    // Resource registration (maps a resource name to its HTTP request handler)
    void RegisterResource(CResource* resource, const char* szName);
    void UnregisterResource(const char* szName);

    // Request handling
    HttpStatusCode HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

    // Periodic maintenance (called from main thread)
    void HttpPulse();

    // Connection filtering
    bool ShouldAllowConnection(const char* szAddress);

    // Accessors
    void       SetResource(CResource* resource) { m_resource = resource; }
    CResource* GetResource() { return m_resource; }
    void       SetDefaultResource(const char* szResourceName) { m_strDefaultResourceName = szResourceName ? szResourceName : ""; }

    // Authentication
    class CAccount* CheckAuthentication(HttpRequest* ipoHttpRequest);
    HttpStatusCode  RequestLogin(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

    // Stats
    long long GetTotalBytesSent() const { return m_llTotalBytesSent; }
    void      GetAllocationStats(SAllocationStats& outStats);
    void      AddBytesSent(long long llBytes);

private:
    // Setup the httplib server handlers
    void SetupHandlers();

    // Connection gate invoked per request. See CHTTPD.cpp.
    bool PassesDosCheck(const char* szAddress, int port);

    CResource*  m_resource{};
    std::string m_strDefaultResourceName;
    bool        m_bStartedServer{};

    // Resource routing map (replaces EHS tree)
    std::map<std::string, CResource*> m_ResourceMap;

    // cpp-httplib server and thread (pimpl to avoid httplib.h in header)
    std::unique_ptr<httplib::Server> m_httpServer;
    std::thread                      m_serverThread;

    // Stats
    std::atomic<long long> m_llTotalBytesSent{0};

    // Authentication & security
    class CAccount*                  m_pGuestAccount;
    std::map<std::string, long long> m_LoggedInMap;
    CConnectHistory                  m_BruteForceProtect;
    CConnectHistory                  m_HttpDosProtect;
    std::set<SString>                m_HttpDosExcludeMap;
    std::mutex                       m_mutexHttpDosProtect;
    std::mutex                       m_mutexLoggedInMap;
    SString                          m_strWarnMessageForIp;
    CElapsedTime                     m_WarnMessageTimer;

    // Connection cache for the request-handler DoS gate. cpp-httplib fires the
    // handler for every HTTP request, but ShouldAllowConnection operates per TCP
    // connection. Caching one decision per connection keeps normal browsing
    // responsive. Entries are pruned by age in HttpPulse.
    std::map<std::string, long long> m_DosCheckedConnections;
    std::mutex                       m_mutexDosCache;
};
