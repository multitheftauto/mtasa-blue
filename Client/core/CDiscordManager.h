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

#include <core/CDiscordManagerInterface.h>
#include "SharedUtil.Thread.h"

namespace discord
{
    enum class LogLevel;
    class Core;
    class Activity;
}            // namespace discord

class CDiscordManager : public CDiscordManagerInterface
{
public:
    ~CDiscordManager();

    void Initialize();

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

    SString GetJoinSecret();

private:
    void Reconnect(bool bOnInitialization = false);
    void DoPulse();
    void Restore();

    static void  DiscordLogCallback(discord::LogLevel level, const char* message);
    static void  OnActivityJoin(const char* joinSecret);
    static void* DiscordThread(void* arg);

    bool NeedsSuicide() const { return m_Suicide; }
    void SetDead() { m_Suicide = false; }

    discord::Core*     m_DiscordCore = nullptr;
    discord::Activity* m_StoredActivity;

    bool m_WaitingForServerName = false;
    bool m_Initialized = false;

    volatile bool m_Suicide = false;            // Thread kill command

    std::mutex                 m_ThreadSafety;
    SharedUtil::CThreadHandle* m_Thread = nullptr;

    CElapsedTime   m_TimeForReconnection;
    CQueryReceiver m_QueryReceiver;
};
