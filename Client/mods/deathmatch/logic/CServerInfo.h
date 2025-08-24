/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CServerInfo.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum EServerInfoSyncFlag : uint8
{
    SERVER_INFO_FLAG_ALL = 0xFF,                  // 0b11111111
    SERVER_INFO_FLAG_MAX_PLAYERS = 1,             // 0b00000001
    SERVER_INFO_FLAG_RESERVED = 1 << 1            // 0b00000010 and so on
};

class CServerInfo
{
public:
    CServerInfo();
    ~CServerInfo() {}

    uint GetMaxPlayers() const { return m_MaxPlayersCount; }
    void SetMaxPlayers(uint set);

private:
    uint m_MaxPlayersCount;
};
