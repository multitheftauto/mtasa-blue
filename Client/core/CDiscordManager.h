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

class CDiscordManager : public CDiscordManagerInterface
{
public:
    ~CDiscordManager();

    void Initialize();

    // ActivityManager
    void UpdateActivity(SDiscordActivity& activity);            // Change it all, or ...
    void UpdateActivity();                                        // Change it all, or ...
    void SetType(EDiscordActivityT type);                       // Singular modifications
    void SetName(char const* name);
    void SetState(char const* state);
    void SetDetails(char const* details);
    void SetStartEndTimestamp(int64 start, int64 end);
    void SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax);
    void SetSpectateSecret(const char* spectateSecret);
    void RegisterPlay(bool connected);
    void Disconnect();

    SString GetJoinSecret();

protected:
    void SetConnected(bool state);

private:
    void Reconnect(bool bOnInitialization = false);
    void DoPulse();
    void Restore();

    // Callbacks
    static void HandleDiscordReady(const struct DiscordUser* connectedUser);
    static void HandleDiscordDisconnected(int errcode, const char* message);
    static void HandleDiscordError(int errcode, const char* message);
    static void HandleDiscordJoin(const char* secret);
    static void HandleDiscordSpectate(const char* secret);
    static void HandleDiscordJoinRequest(const struct DiscordUser* request);

    static void* DiscordThread(void* arg);

    bool NeedsSuicide() const { return m_Suicide; }
    void SetDead() { m_Suicide = false; }

    SDiscordActivity m_StoredActivity;

    bool m_Connected = false;
    bool m_WaitingForServerName = false;
    bool m_Initialized = false;

    volatile bool m_Suicide = false;            // Thread kill command

    std::mutex                 m_ThreadSafety;
    SharedUtil::CThreadHandle* m_Thread = nullptr;

    CElapsedTime   m_TimeForReconnection;
    CQueryReceiver m_QueryReceiver;
};
