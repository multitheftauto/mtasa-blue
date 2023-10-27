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
    void SetPresenceStartTimestamp(const unsigned long ulStart);
    void SetPresenceEndTimestamp(const unsigned long ulEnd);
    void SetAsset(const char* szAsset, const char* szAssetText, bool bIsLarge = false);
    void SetAssetLargeData(const char* szAsset, const char* szAssetText);
    void SetAssetSmallData(const char* szAsset, const char* szAssetText);

    bool ResetDiscordData();
    bool SetPresenceState(const char* szState, bool bCustom = false);
    bool SetPresenceDetails(const char* szDetails, bool bCustom = false);
    bool SetPresenceButtons(unsigned short int iIndex, const char* szName, const char* szUrl);
    void SetPresencePartySize(int iSize, int iMax, bool bCustom = false);
    bool SetDiscordRPCEnabled(bool bEnabled);
    bool IsDiscordCustomDetailsDisallowed() const;
    bool IsDiscordRPCEnabled() const;
    bool SetApplicationID(const char* szAppID);

#ifdef DISCORD_DISABLE_IO_THREAD
    void UpdatePresenceConnection();
#endif

private:
    std::string m_strDiscordAppId;
    std::string m_strDiscordAppAsset;
    std::string m_strDiscordAppAssetText;
    std::string m_strDiscordAppAssetSmall;
    std::string m_strDiscordAppAssetSmallText;

    std::string m_strDiscordAppCurrentId;
    std::string m_strDiscordAppState;
    std::string m_strDiscordAppDetails;
    std::string m_strDiscordAppCustomState;
    std::string m_strDiscordAppCustomDetails;

    std::optional<std::tuple<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> m_aButtons;

    unsigned long m_uiDiscordAppStart;
    unsigned long m_uiDiscordAppEnd;

    bool m_bDisallowCustomDetails;
    bool m_bDiscordRPCEnabled;
    bool m_bUpdateRichPresence;

    int m_iPartySize;
    int m_iPartyMax;

    int m_iPlayersCount;
    int m_iMaxPlayers;
};
