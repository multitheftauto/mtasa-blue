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
#include <string>
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
    void SetAsset(const char* szAsset, const char* szAssetText, bool bIsLarge = false);
    void SetAssetLargeData(const char* szAsset, const char* szAssetText);
    void SetAssetSmallData(const char* szAsset, const char* szAssetText);

    bool ResetDiscordData();
    bool SetPresenceState(const char* szState, bool bCustom = false);
    bool SetPresenceDetails(const char* szDetails, bool bCustom = false);
    bool SetPresenceButtons(const int iIndex, const char* szName, const char* szUrl);
    bool SetDiscordRPCEnabled(bool bEnabled);
    bool IsDiscordRPCEnabled();
    bool SetApplicationID(const char* szAppID);

    // void SetPresenceTimestamp();
    // void SetPresenceImage();
    // void SetPresenceText();

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

    std::optional<std::tuple<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> m_aButtons;

    unsigned long m_uiDiscordAppStart;
    unsigned long m_uiDiscordAppEnd;

    bool m_bAllowCustomDetails;
    bool m_bDiscordRPCEnabled;
    bool m_bUpdateRichPresence;
};
