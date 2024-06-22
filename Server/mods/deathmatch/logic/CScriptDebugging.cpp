/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CScriptDebugging.cpp
 *  PURPOSE:     Script debugging facility class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CScriptDebugging.h"
#include "packets/CDebugEchoPacket.h"

extern CGame* g_pGame;

enum CScriptDebugging::DebugMessageLevels : std::uint32_t
{
    MESSAGE_TYPE_DEBUG = 0,
    MESSAGE_TYPE_ERROR = 1,
    MESSAGE_TYPE_WARNING = 2,
    MESSAGE_TYPE_INFO = 3,
    MESSAGE_TYPE_CUSTOM = 4,
};

enum CScriptDebugging::DebugScriptLevels : std::uint32_t
{
    ERRORS_ONLY = 1,
    ERRORS_AND_WARNINGS = 2,
    ALL = 3,
};

CScriptDebugging::CScriptDebugging()
{
    m_uiLogFileLevel = 0;
    m_uiHtmlLogLevel = 0;
    m_pLogFile = NULL;
    m_bTriggeringMessageEvent = false;
}

CScriptDebugging::~CScriptDebugging()
{
    // Flush any pending duplicate loggings
    m_DuplicateLineFilter.Flush();
    UpdateLogOutput();

    ClearPlayers();
}

bool CScriptDebugging::AddPlayer(CPlayer& Player, unsigned int uiLevel)
{
    // Want a level above 0?
    if (uiLevel > 0)
    {
        // He's not already debugging? Add him to our list.
        if (Player.m_uiScriptDebugLevel == 0)
        {
            m_Players.push_back(&Player);
        }
    }
    else
    {
        // He's already script debugging? Remove him from our list.
        if (Player.m_uiScriptDebugLevel > 0)
        {
            m_Players.remove(&Player);
        }
    }

    // Give him the level
    Player.m_uiScriptDebugLevel = uiLevel;
    return true;
}

bool CScriptDebugging::RemovePlayer(CPlayer& Player)
{
    // Is he script debugging?
    if (Player.m_uiScriptDebugLevel > 0)
    {
        // Mark it as not script debugging and remove it from our list
        Player.m_uiScriptDebugLevel = 0;
        m_Players.remove(&Player);

        return true;
    }

    return false;
}

void CScriptDebugging::ClearPlayers()
{
    // Unreference us from all players that we are script logging to
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        (*iter)->m_uiScriptDebugLevel = 0;
    }

    // Clear the list
    m_Players.clear();
}

void CScriptDebugging::LogBadAccess(lua_State* luaVM)
{
    LogWarning(luaVM, "Access denied @ '%s'", lua_tostring(luaVM, lua_upvalueindex(1)));
}

bool CScriptDebugging::SetLogfile(const char* szFilename, unsigned int uiLevel)
{
    assert(szFilename);

    // Try to load the new file
    FILE* pFile = File::Fopen(szFilename, "a+");
    if (pFile)
    {
        // Close the previously loaded file
        if (m_pLogFile)
        {
            fprintf(m_pLogFile, "INFO: Logging to this file ended\n");
            fclose(m_pLogFile);
        }

        // Set the new pointer and level and return true
        m_uiLogFileLevel = uiLevel;
        m_pLogFile = pFile;
        return true;
    }

    return false;
}

void CScriptDebugging::UpdateLogOutput()
{
    SLogLine line;
    while (m_DuplicateLineFilter.PopOutputLine(line))
    {
        // Log it to the file if enough level
        if (m_uiLogFileLevel >= line.uiMinimumDebugLevel)
        {
            PrintLog(line.strText);
        }
        // Log to console
        CLogger::LogPrintf("%s", line.strText.c_str());
        CLogger::LogPrintNoStamp("\n");
        // Tell the players
        Broadcast(CDebugEchoPacket(line.strText, line.uiMinimumDebugLevel, line.ucRed, line.ucGreen, line.ucBlue), line.uiMinimumDebugLevel);
    }
}

void CScriptDebugging::PrintLog(const char* szText)
{
    // Got a logfile?
    if (m_pLogFile)
    {
        // Log it, timestamped
        fprintf(m_pLogFile, "[%s] %s\n", *GetLocalTimeString(true), szText);
        fflush(m_pLogFile);
    }
}

bool CScriptDebugging::CheckForSufficientDebugLevel(std::uint32_t playerDebugLevel, std::uint32_t messageDebugLevel) const noexcept
{
    bool sufficientDebugLevel = false;

    switch (messageDebugLevel)
    {
        case MESSAGE_TYPE_ERROR:
            sufficientDebugLevel = (playerDebugLevel >= ERRORS_ONLY);
            break;
        case MESSAGE_TYPE_WARNING:
            sufficientDebugLevel = (playerDebugLevel >= ERRORS_AND_WARNINGS);
            break;
        case MESSAGE_TYPE_INFO:
        case MESSAGE_TYPE_CUSTOM:
        case MESSAGE_TYPE_DEBUG:
            sufficientDebugLevel = (playerDebugLevel == ALL);
            break;
    }

    return sufficientDebugLevel;
}

void CScriptDebugging::Broadcast(const CPacket& Packet, unsigned int uiMinimumDebugLevel)
{
    // Tell everyone we log to about it
    for (const auto& pPlayer : m_Players)
    {
        bool sufficientDebugLevel = CheckForSufficientDebugLevel(pPlayer->m_uiScriptDebugLevel, uiMinimumDebugLevel);

        if (sufficientDebugLevel)
        {
            pPlayer->Send(Packet);
        }
    }
}
