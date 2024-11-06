/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CQueryReceiver.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct SQueryInfo
{
    SQueryInfo()
    {
        containingInfo = false;
        port = 0;
        isStatusVerified = true;
        isPassworded = false;
        serials = false;
        players = 0;
        playerSlot = 0;
        buildType = 1;
        buildNum = 0;
        httpPort = 0;
        pingTime = 0;
    }

    bool    containingInfo;
    SString gameName;
    ushort  port;
    SString serverName;
    SString gameType;
    SString mapName;
    SString versionText;
    bool    isStatusVerified;
    bool    isPassworded;
    bool    serials;
    ushort  players;
    ushort  playerSlot;
    int     buildType;
    int     buildNum;
    SString netRoute;
    SString upTime;
    ushort  httpPort;
    ushort  pingTime;

    std::vector<SString> playersPool;
};

class CQueryReceiver
{
public:
    CQueryReceiver();
    ~CQueryReceiver();

    void RequestQuery(in_addr address, ushort port);
    void RequestQuery(const SString& address, ushort port);
    void InvalidateSocket();

    SQueryInfo GetServerResponse();

    uint GetElapsedTimeSinceLastQuery() { return static_cast<uint>(m_ElapsedTime.Get()); };

    bool IsSocketValid() const { return (m_Socket != INVALID_SOCKET); }

private:
    bool ReadString(std::string& strRead, const char* szBuffer, int& i, int nLength);

    SOCKET       m_Socket;
    CElapsedTime m_ElapsedTime;
};
