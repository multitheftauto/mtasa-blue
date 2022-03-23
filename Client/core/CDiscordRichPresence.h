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

private:
    std::string m_strDiscordAppId;
    std::string m_strDiscordAppAsset;
    std::string m_strDiscordAppAssetSmall;
    std::string m_strDiscordAppAssetText;
    std::string m_strDiscordAppAssetSmallText;
};
