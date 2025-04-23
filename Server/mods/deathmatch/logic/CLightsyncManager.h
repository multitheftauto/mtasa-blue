/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CLightsyncManager.h
 *  PURPOSE:     Lightweight synchronization manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>

#ifndef _MSC_VER
#include <stdint.h>
typedef int64_t __int64;
#endif

#define LIGHTSYNC_HEALTH_THRESHOLD          2.0f
#define LIGHTSYNC_VEHICLE_HEALTH_THRESHOLD  50.0f

class CLightsyncManager
{
public:
    CLightsyncManager();
    ~CLightsyncManager();

    void DoPulse();

    void     RegisterPlayer(CPlayer* pPlayer);
    void     UnregisterPlayer(CPlayer* pPlayer);
    CPlayer* FindPlayer(const char* szArguments);

private:
    enum EEntryType
    {
        SYNC_PLAYER,
        DELTA_MARKER_HEALTH,
        DELTA_MARKER_VEHICLE_HEALTH
    };

    struct SEntry
    {
        __int64      ullTime;
        CPlayer*     pPlayer;
        EEntryType   eType;
        unsigned int uiContext;
    };

    std::list<SEntry> m_Queue;
};
