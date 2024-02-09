/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDiscordRichPresence.h
 *  PURPOSE:     Header file for discord rich presence
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <core/CDiscordInterface.h>
#include <optional>

class CDiscordRichPresence : public CDiscordInterface
{
public:
    CDiscordRichPresence();
    ~CDiscordRichPresence();

    void InitializeDiscord();
    void ShutdownDiscord();
    void RestartDiscord();
    void SetDefaultData();
    void UpdatePresence();
#ifdef DISCORD_DISABLE_IO_THREAD
    void UpdatePresenceConnection();
#endif

    void SetPresenceStartTimestamp(const unsigned long ulStart);
    void SetPresenceEndTimestamp(const unsigned long ulEnd);
    void SetAsset(const char* szAsset, const char* szAssetText, bool bIsLarge);
    void SetAssetLargeData(const char* szAsset, const char* szAssetText);
    void SetAssetSmallData(const char* szAsset, const char* szAssetText);
    void SetDiscordClientConnected(bool bConnected) { m_bConnected = bConnected; };
    void SetPresencePartySize(int iSize, int iMax, bool bCustom);
    void SetDiscordUserID(const std::string& strUserID);

    bool ResetDiscordData();
    bool SetPresenceState(const char* szState, bool bCustom);
    bool SetPresenceDetails(const char* szDetails, bool bCustom);
    bool SetPresenceButtons(unsigned short int iIndex, const char* szName, const char* szUrl);
    bool SetDiscordRPCEnabled(bool bEnabled);
    bool SetApplicationID(const char* resourceName, const char* szAppID);
    bool IsDiscordCustomDetailsDisallowed() const { return m_bDisallowCustomDetails; };
    bool IsDiscordRPCEnabled() const { return m_bDiscordRPCEnabled; };
    bool IsDiscordClientConnected() const { return m_bConnected; };

    std::string GetDiscordResourceName() const { return m_strDiscordCustomResourceName; };
    std::string GetDiscordUserID() const;

    // static handlers
    static void HandleDiscordReady(const struct DiscordUser* pDiscordUser);
    static void HandleDiscordDisconnected(int iErrorCode, const char* szMessage);
    static void HandleDiscordError(int iErrorCode, const char* szMessage);

private:
    std::string m_strDiscordUserID;
    std::string m_strDiscordAppId;
    std::string m_strDiscordAppAsset;
    std::string m_strDiscordAppAssetText;
    std::string m_strDiscordAppAssetSmall;
    std::string m_strDiscordAppAssetSmallText;

    std::string m_strDiscordCustomResourceName;
    std::string m_strDiscordAppCurrentId;
    std::string m_strDiscordAppState;
    std::string m_strDiscordAppDetails;
    std::string m_strDiscordAppCustomState;
    std::string m_strDiscordAppCustomDetails;

    std::optional<std::tuple<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> m_aButtons;

    std::mutex m_threadSafetyMutex;

    unsigned long m_uiDiscordAppStart;
    unsigned long m_uiDiscordAppEnd;

    bool m_bDisallowCustomDetails;
    bool m_bDiscordRPCEnabled;
    bool m_bUpdateRichPresence;
    bool m_bConnected;

    int m_iPartySize;
    int m_iPartyMax;

    int m_iPlayersCount;
    int m_iMaxPlayers;
};
