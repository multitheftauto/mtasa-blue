/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCommandLineParser.h
 *  PURPOSE:     Command line parser class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../Config.h"

class CCommandLineParser
{
public:
    CCommandLineParser();

    bool Parse(int iArgumentCount, char* szArguments[]);

    bool GetMainConfig(const char*& szMainConfig)
    {
        szMainConfig = m_strMainConfig.c_str();
        return m_bMainConfig;
    }
    bool GetIP(std::string& strIP)
    {
        strIP = m_strIP;
        return m_bIP;
    }
    bool GetPort(std::uint16_t& usPort)
    {
        usPort = m_usPort;
        return m_bPort;
    }
    bool GetHTTPPort(std::uint16_t& usHTTPPort)
    {
        usHTTPPort = m_usHTTPPort;
        return m_bHTTPPort;
    }
    bool GetMaxPlayers(std::uint32_t& uiMaxPlayers)
    {
        uiMaxPlayers = m_uiMaxPlayers;
        return m_bMaxPlayers;
    }
    bool IsVoiceDisabled(bool& bDisabled)
    {
        bDisabled = m_bDisableVoice;
        return m_bNoVoice;
    }

private:
    bool m_bMainConfig;
    bool m_bIP;
    bool m_bPort;
    bool m_bHTTPPort;
    bool m_bMaxPlayers;
    bool m_bNoVoice;

    std::string    m_strMainConfig;
    std::string    m_strIP;
    std::uint16_t m_usPort;
    std::uint16_t m_usHTTPPort;
    std::uint32_t   m_uiMaxPlayers;
    bool           m_bDisableVoice;
};
