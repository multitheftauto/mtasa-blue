/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CDiscordManager.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <discord.h>
#include <core/CDiscordManagerInterface.h>
#include "SharedUtil.Thread.h"

class CDiscordManager : public CDiscordManagerInterface
{
public:
    CDiscordManager();
    ~CDiscordManager();

    static void  DiscordLogCallback(discord::LogLevel level, const char* message);
    static void  OnActivityJoin(const char* joinSecret);
    static void* DiscordThread(void* arg);

    void Reconnect(bool bOnInitialization = false);
    void DoPulse();

    // ActivityManager
    void UpdateActivity(SDiscordActivity& activity, std::function<void(EDiscordRes)> callback);            // Change it all, or ...
    void UpdateActivity(std::function<void(EDiscordRes)> callback);                                        // Change it all, or ...
    void SetType(EDiscordActivityT type, std::function<void(EDiscordRes)> callback);                       // Singular modifications
    void SetName(char const* name, std::function<void(EDiscordRes)> callback);
    void SetState(char const* state, std::function<void(EDiscordRes)> callback);
    void SetDetails(char const* details, std::function<void(EDiscordRes)> callback);
    void SetStartEndTimestamp(int64 start, int64 end, std::function<void(EDiscordRes)> callback);
    void SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax, std::function<void(EDiscordRes)> callback);
    void SetSpectateSecret(const char* spectateSecret, std::function<void(EDiscordRes)> callback);
    void RegisterPlay(bool connected);
    void Disconnect();

    discord::Activity GetStoredActivity() const { return m_StoredActivity; }            // For retrieving stored information in rich presence

    bool NeedsSuicide() const { return m_Suicide; }
    void SetDead() { m_Suicide = false; }
    void DisconnectNotification();

    SString GetJoinSecret();

private:
    void Restore();

    discord::Core*    m_DiscordCore;
    discord::Activity m_StoredActivity;

    bool m_WaitingForServerName;

    volatile bool m_Suicide;            // Thread kill command

    std::mutex                 m_ThreadSafety;
    SharedUtil::CThreadHandle* m_Thread;

    CElapsedTime   m_TimeForReconnection;
    CQueryReceiver m_QueryReceiver;
};
