/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRemoteDebugger.cpp
 *  PURPOSE:     Remote debugger
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CRemoteDebugger* CRemoteDebugger::_instance = NULL;

#include <v8/CV8Base.h>
#include <core/CServerInterface.h>
#include <SharedUtil.Crypto.h>

extern CServerInterface* g_pServerInterface;
CRemoteDebugger::CRemoteDebugger(CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, unsigned short usPort, const SString& strServerIPList /*, bool bLan*/)
{
    _instance = this;
    m_tStartTime = time(NULL);

    m_pMainConfig = pMainConfig;
    m_pPlayerManager = pPlayerManager;
    m_strIPList = strServerIPList;
    m_usPort = usPort;
    m_strConnectJsonVersion = "{\"Browser\": \"node.js/v8.11.4\", \"Protocol-Version\": \"1.1\"}";
    m_strTestJson = "[]";
    m_strConnectJson =
        "[{"
        "\"description\": \"MTA SERVER hello\","
        "\"devtoolsFrontendUrl\":"
        "\"chrome-devtools://devtools/bundled/inspector.html?experiments=true&v8only=true&ws=127.0.0.1:1234/0198dee4-3ed9-4715-854a-d70d333627f6\","
        "\"faviconUrl\": \"https://wiki.multitheftauto.com/images/thumb/5/58/Mtalogo.png/100px-Mtalogo.png\","
        "\"id\": \"0198dee4-3ed9-4715-854a-d70d333627f6\", "
        "\"title\": \"MTA SERVER\","
        "\"type\": \"v8\","
        "\"url\": \"file://\","
        "\"webSocketDebuggerUrl\": \"ws://127.0.0.1:1234/0198dee4-3ed9-4715-854a-d70d333627f6\""
        "}]";
    InitLan();
}

CRemoteDebugger::~CRemoteDebugger()
{
    _instance = NULL;
}

bool CRemoteDebugger::SetPortEnabled(bool bInternetEnabled, bool bLanEnabled)
{
    // Calc requirements
    bool   bPortEnableReq = bInternetEnabled || bLanEnabled;
    bool   bLanOnly = !bInternetEnabled && bLanEnabled;

    // Any change?
    if ((!m_SocketList.empty()) == bPortEnableReq)
        return true;

    // Remove current thingmy
    for (uint s = 0; s < m_SocketList.size(); s++)
    {
        closesocket(m_SocketList[s]);
    }
    m_SocketList.clear();

    if (!bPortEnableReq)
        return true;

    // Start new thingmy
    // If a local IP has been specified, ensure it is used for sending
    std::vector<SString> ipList;
    m_strIPList.Split(",", ipList);
    for (uint i = 0; i < ipList.size(); i++)
    {
        const SString& strIP = ipList[i];
        sockaddr_in    sockAddr;
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(m_usPort);
        if (!strIP.empty())
            sockAddr.sin_addr.s_addr = inet_addr(strIP);
        else
            sockAddr.sin_addr.s_addr = INADDR_ANY;

        // Initialize socket
        SOCKET newSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        // If we are in lan only mode, reuse addr to avoid possible conflicts
        //if (bLanOnly)
        //{
        //    const int Flags = 1;
        //    setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&Flags, sizeof(Flags));
        //}
        int yes = 1;
        setsockopt(newSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&yes, sizeof(int));
        int reuse = 1;
        setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int));


        // Bind the socket
        if (::bind(newSocket, (sockaddr*)&sockAddr, sizeof(sockAddr)) != 0)
        {
            sockclose(newSocket);
            newSocket = INVALID_SOCKET;
            return false;
        }

        int a = listen(newSocket, SOMAXCONN);
        // Set it to non blocking, so we dont have to wait for a packet
        #ifdef WIN32
        unsigned long ulNonBlock = 1;
        ioctlsocket(newSocket, FIONBIO, &ulNonBlock);
        #else
        fcntl(newSocket, F_SETFL, fcntl(newSocket, F_GETFL) | O_NONBLOCK);
        #endif

        m_SocketList.push_back(newSocket);
        //m_SocketList.push_back(ClientSocket);
    }

    return true;
}

void CRemoteDebugger::DoPulse()
{
    if (m_SocketList.empty())
        return;

    sockaddr_in SockAddr;
#ifndef WIN32
    socklen_t nLen = sizeof(sockaddr);
#else
    int nLen = sizeof(sockaddr);
#endif

    char szBuffer[1000];            // Extra bytes for future use

    for (uint s = 0; s < m_SocketList.size(); s++)
    {
        SOCKET socket = m_SocketList[s];
        for (uint i = 0; i < 100; i++)
        {
            SOCKET remoteDebugger = accept(m_SocketList[s], nullptr, nullptr);
            int    iBuffer = recv(remoteDebugger, szBuffer, 1, 0);
            if (iBuffer < 1)
                break;

            m_SocketsRemoteDebuggers.push_back(remoteDebugger);
        }
    }

    for (uint s = 0; s < m_SocketsRemoteDebuggers.size(); s++)
    {
        SOCKET remoteDebugger = m_SocketsRemoteDebuggers[s];
        // We set the socket to non-blocking so we can just keep reading
        int iBuffer = recv(remoteDebugger, szBuffer, sizeof(szBuffer), 0);
        if (iBuffer == 0)
        {
            m_SocketsRemoteDebuggers.erase(m_SocketsRemoteDebuggers.begin() + s);
            printf("[REMOTE DEBUGGER]: CONNECTION CLOSED\n");
            break;
        }
        if (iBuffer < 1)
            break;
        std::string request(szBuffer, iBuffer);
        printf("[REMOTE DEBUGGER]: Buffer size: %i\n", iBuffer);
        std::stringstream ss(request);
        std::string       line;

        std::string key;
        while (std::getline(ss, line, '\n'))
        {
            if (line.rfind("Sec-WebSocket-Key: ", 0) == 0)
            {
                int len = sizeof("Sec-WebSocket-Key: ");
                key = line.substr(len, line.length() - (len + 1));
                int a = 5;
            }
            printf("[REMOTE DEBUGGER]: %s\n", line.c_str());
        }
        SString           strkey = GenerateHashHexString(EHashFunctionType::SHA1, key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").SubStr(0, 20);
        std::stringstream stream;
        const char*       accept = SharedUtil::Base64encode(strkey).c_str();
        stream << "HTTP/1.1 101 WebSocket Protocol Handshake\n";
        stream << "upgrade: websocket\n";
        stream << "Connection: upgrade\n";
        stream << "Sec-WebSocket-Accept: " << accept << "\n";
        std::string handshake = stream.str();
        int sent = send(remoteDebugger, handshake.c_str(), handshake.length(), 0);
        printf("sent: %i\n", sent);
        // If our reply buffer isn't empty, send it
        //if (strReply && !strReply->empty())
        //{
        //    /*int sent =*/sendto(CRemoteDebuggerSocket, strReply->c_str(), strReply->length(), 0, (sockaddr*)&SockAddr, nLen);
        //}

    }
}

CLanBroadcast* CRemoteDebugger::InitLan()
{
    return new CLanBroadcast(m_usPort);
}
