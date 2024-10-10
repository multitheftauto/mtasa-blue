/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CQueryReceiver.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CQueryReceiver::CQueryReceiver()
{
    m_Socket = INVALID_SOCKET;
}

CQueryReceiver::~CQueryReceiver()
{
    InvalidateSocket();
}

void CQueryReceiver::RequestQuery(in_addr address, ushort port)
{
    if (m_Socket == INVALID_SOCKET)            // Create the socket
    {
        m_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        u_long flag = 1;
        ioctlsocket(m_Socket, FIONBIO, &flag);            // Nonblocking I/O
    }

    sockaddr_in addr;
    memset(&addr, NULL, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = address;
    addr.sin_port = htons(port);

    // Trailing data to work around 1 byte UDP packet filtering
    int iSendByte = g_pCore->GetNetwork()->SendTo(m_Socket, "r mtasa", 1, 0, (sockaddr*)&addr, sizeof(addr));
    m_ElapsedTime.Reset();
}

void CQueryReceiver::RequestQuery(const SString& address, ushort port)
{
    in_addr addr;
    addr.S_un.S_addr = inet_addr(*address);
    if (addr.S_un.S_addr == INADDR_NONE)
    {
        hostent* pHostent = gethostbyname(*address);
        if (!pHostent)
            return;
        DWORD* pIP = (DWORD*)pHostent->h_addr_list[0];
        if (!pIP)
            return;
        addr.S_un.S_addr = *pIP;
    }
    RequestQuery(addr, port);
}

void CQueryReceiver::InvalidateSocket()
{
    if (m_Socket != INVALID_SOCKET)
    {
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
}

bool CQueryReceiver::ReadString(std::string& strRead, const char* szBuffer, int& i, int nLength)
{
    if (i <= nLength)
    {
        unsigned char len = szBuffer[i];
        if (i + len <= nLength && len > 0)
        {
            const char* ptr = &szBuffer[i + 1];
            i += len;
            strRead = std::string(ptr, len - 1);
            return true;
        }
        i++;
    }
    return false;
}

SQueryInfo CQueryReceiver::GetServerResponse()
{
    SQueryInfo info;

    if (m_Socket == INVALID_SOCKET)
        return info;            // Query not sent

    char szBuffer[SERVER_LIST_QUERY_BUFFER] = {0};

    // Poll the socket
    sockaddr_in clntAddr;
    int         addrLen = sizeof(clntAddr);
    int         len = recvfrom(m_Socket, szBuffer, SERVER_LIST_QUERY_BUFFER, MSG_PARTIAL, (sockaddr*)&clntAddr, &addrLen);

    if (len >= 0)
    {
        // Parse data

        // Check length
        if (len < 15)
            return info;

        // Check header
        if (strncmp(szBuffer, "EYE2", 4) != 0)
            return info;

        // Calculate the ping/latency
        info.pingTime = m_ElapsedTime.Get();

        // Parse relevant data
        SString strTemp;
        SString strMapTemp;
        int     i = 4;

        // Game
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.gameName = strTemp;

        // Port (Ignore result as we must already have the correct value)
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;

        // Server name
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.serverName = strTemp;

        // Game type
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.gameType = strTemp;

        // Map name
        if (!ReadString(strMapTemp, szBuffer, i, len))
            return info;
        info.mapName = strMapTemp;

        // Version
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.versionText = strTemp;

        // Got space for password, serial verification, player count, players max?
        if (i + 4 > len)
            return info;

        info.isPassworded = (szBuffer[i++] == 1);
        info.serials = (szBuffer[i++] == 1);
        info.players = (unsigned char)szBuffer[i++];
        info.playerSlot = (unsigned char)szBuffer[i++];

        // Recover large player count if present
        const SString strPlayerCount = strMapTemp.Right(strMapTemp.length() - strlen(strMapTemp) - 1);
        if (!strPlayerCount.empty())
        {
            SString strJoinedPlayers, strMaxPlayers;
            if (strPlayerCount.Split("/", &strJoinedPlayers, &strMaxPlayers))
            {
                info.players = atoi(strJoinedPlayers);
                info.playerSlot = atoi(strMaxPlayers);
            }
        }

        // Recover server build type if present
        const SString strBuildType = strPlayerCount.Right(strPlayerCount.length() - strlen(strPlayerCount) - 1);
        if (!strBuildType.empty())
            info.buildType = atoi(strBuildType);
        else
            info.buildType = 1;

        // Recover server build number if present
        const SString strBuildNumber = strBuildType.Right(strBuildType.length() - strlen(strBuildType) - 1);
        if (!strBuildNumber.empty())
            info.buildNum = atoi(strBuildNumber);
        else
            info.buildNum = 0;

        // Recover server ping status if present
        const SString strPingStatus = strBuildNumber.Right(strBuildNumber.length() - strlen(strBuildNumber) - 1);
        CCore::GetSingleton().GetNetwork()->UpdatePingStatus(*strPingStatus, info.players, info.isStatusVerified);

        // Recover server http port if present
        const SString strNetRoute = strPingStatus.Right(strPingStatus.length() - strlen(strPingStatus) - 1);
        const SString strUpTime = strNetRoute.Right(strNetRoute.length() - strlen(strNetRoute) - 1);
        const SString strHttpPort = strUpTime.Right(strUpTime.length() - strlen(strUpTime) - 1);
        if (!strHttpPort.empty())
            info.httpPort = atoi(strHttpPort);

        // Get player nicks
        while (i < len)
        {
            std::string strPlayer;
            try
            {
                if (ReadString(strPlayer, szBuffer, i, len))
                {
                    // Remove color code, unless that results in an empty string
                    SString strResult = RemoveColorCodes(strPlayer.c_str());
                    if (strResult.length() == 0)
                        strResult = strPlayer;
                    info.playersPool.push_back(strResult);
                }
            }
            catch (...)
            {
                // yeah that's what I thought.
                return info;
            }
        }

        InvalidateSocket();

        if (info.players > info.playerSlot)
        {
            info.players = info.playerSlot;
            info.isStatusVerified = false;
        }

        info.containingInfo = true;
    }

    return info;
}
