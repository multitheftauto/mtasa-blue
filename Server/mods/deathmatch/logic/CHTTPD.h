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

// This class implements the webserver, it uses EHS to do this

#pragma once

#include "CConnectHistory.h"
#include <string>
#include <list>
#include <ehs/ehs.h>

class CResource;

class CHTTPD : public EHS
{
public:
    CHTTPD();            // start the initial server
    ~CHTTPD();
    // EHS interface
    HttpResponse*  RouteRequest(HttpRequest* ipoHttpRequest);
    HttpStatusCode HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);
    void           HttpPulse();
    bool           ShouldAllowConnection(const char* szAddress);

    // CHTTPD methods
    bool            StartHTTPD(const char* szIP, unsigned int port);
    bool            StopHTTPD();
    void            SetResource(CResource* resource) { m_resource = resource; }
    CResource*      GetResource() { return m_resource; }
    class CAccount* CheckAuthentication(HttpRequest* ipoHttpRequest);
    void            SetDefaultResource(const char* szResourceName) { m_strDefaultResourceName = szResourceName ? szResourceName : ""; }
    HttpStatusCode  RequestLogin(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

private:
    CResource*  m_resource{};
    CHTTPD*     m_server{};
    std::string m_strDefaultResourceName;            // default resource name

    EHSServerParameters m_Parameters;

    bool m_bStartedServer{};

    class CAccount*             m_pGuestAccount;
    std::map<string, long long> m_LoggedInMap;
    CConnectHistory             m_BruteForceProtect;
    CConnectHistory             m_HttpDosProtect;
    std::set<SString>           m_HttpDosExcludeMap;
    std::mutex                  m_mutexHttpDosProtect;
    std::mutex                  m_mutexLoggedInMap;
    SString                     m_strWarnMessageForIp;
    CElapsedTime                m_WarnMessageTimer;
};
