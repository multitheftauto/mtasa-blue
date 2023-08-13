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

class CDiscordRichPresence : public CDiscordInterface
{
public:
    CDiscordRichPresence();
    ~CDiscordRichPresence();

    void InitializeDiscord();
    void ShutdownDiscord();

    void UpdatePresence();
    void SetPresenceState(const char* szState);
    void SetPresenceStartTimestamp(const unsigned long ulStart);

    bool SetPresenceDetails(const char* szDetails, bool bCustom = false);
    bool SetDiscordRPCEnabled(bool bEnabled) override;
    bool IsDiscordRPCEnabled() override;
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

    std::string   m_strDiscordAppCurrentId;
    std::string   m_strDiscordAppState;
    std::string   m_strDiscordAppDetails;

    unsigned long m_uiDiscordAppStart;
    unsigned long m_uiDiscordAppEnd;

    bool m_bAllowCustomDetails;
    bool m_bDiscordRPCEnabled;
};
