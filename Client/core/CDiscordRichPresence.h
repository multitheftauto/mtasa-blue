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

#include <string>

class CDiscordRichPresence
{
public:
    CDiscordRichPresence();
    ~CDiscordRichPresence();

    // void SetApplicationId();
    void UpdatePresence();
    void SetPresenceState(const char* szState);
    // void SetPresenceDetails();
    // void SetPresenceTimestamp();
    // void SetPresenceImage();
    // void SetPresenceText();

private:
    const char* m_strDiscordAppId;
    const char* m_strDiscordAppAsset;
    const char* m_strDiscordAppAssetSmall;
    const char* m_strDiscordAppAssetText;
    const char* m_strDiscordAppAssetSmallText;

    const char*   m_strDiscordAppCurrentId;
    const char*   m_strDiscordAppState;
    const char*   m_strDiscordAppDetails;
    unsigned long m_uiDiscordAppStart;
    unsigned long m_uiDiscordAppEnd;
};
