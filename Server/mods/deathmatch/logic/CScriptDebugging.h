/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CScriptDebugging.h
 *  PURPOSE:     Script debugging facility class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaManager.h"
#include "packets/CPacket.h"
#include <cstdio>
#include <list>

#include <SharedUtil.DebugScript.h>

struct SLogLine
{
    DebugScriptLevel debugLevel;

    SString      strText;
    std::uint8_t ucRed;
    std::uint8_t ucGreen;
    std::uint8_t ucBlue;

    void operator+=(const char* szAppend) noexcept { strText += szAppend; }
    bool operator==(const SLogLine& other) const noexcept
    {
        if (strText != other.strText)
            return false;
        if (ucRed != other.ucRed)
            return false;
        if (ucGreen != other.ucGreen)
            return false;
        if (ucBlue != other.ucBlue)
            return false;
        if (debugLevel != other.debugLevel)
            return false;

        return true;
    }
    bool operator!=(const SLogLine& other) const noexcept { return !operator==(other); }
};

class CScriptDebugging
{
public:
    CScriptDebugging();
    ~CScriptDebugging();

    bool AddPlayer(class CPlayer& Player, unsigned int uiLevel);
    bool RemovePlayer(class CPlayer& Player);
    void ClearPlayers();

    void LogCustom(lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ...);
    void LogDebug(lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ...);
    void LogInformationV(lua_State* luaVM, const char* format, va_list vlist);
    void LogInformation(lua_State* luaVM, const char* szFormat, ...);
    void LogWarning(lua_State* luaVM, const char* szFormat, ...);
    void LogError(lua_State* luaVM, const char* szFormat, ...);
    void LogBadPointer(lua_State* luaVM, const char* szArgumentType, unsigned int uiArgument);
    void LogBadType(lua_State* luaVM);
    void LogBadAccess(lua_State* luaVM);
    void LogCustom(lua_State* luaVM, const char* szMessage);
    void LogWarning(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...);
    void LogError(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...);
    void LogPCallError(lua_State* luaVM, const SString& strRes, bool bInitialCall = false);

    bool SetLogfile(const char* szFilename, std::set<DebugScriptLevel> uiLevel) noexcept;
    void SetHTMLLogLevel(unsigned int uiLevel) { m_uiHtmlLogLevel = uiLevel; }

    const SLuaDebugInfo& GetLuaDebugInfo(lua_State* luaVM);
    void                 SaveLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_SavedLuaDebugInfo = luaDebugInfo; }

    void      PushLuaMain(CLuaMain* pLuaMain);
    void      PopLuaMain(CLuaMain* pLuaMain);
    void      OnLuaMainDestroy(CLuaMain* pLuaMain);
    CLuaMain* GetTopLuaMain();
    void      UpdateLogOutput();

private:
    SString ComposeErrorMessage(const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage);
    void LogString(const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed = 255,
                   unsigned char ucGreen = 255, unsigned char ucBlue = 255);

    void PrintLog(const char* szText);
    void Broadcast(const CPacket& Packet, DebugScriptLevel uiMinimumDebugLevel);

    std::set<DebugScriptLevel>     m_LogFileLevels;
    unsigned int                   m_uiHtmlLogLevel;
    FILE*                          m_pLogFile;
    std::list<class CPlayer*>      m_Players;
    bool                           m_bTriggeringMessageEvent;
    SLuaDebugInfo                  m_SavedLuaDebugInfo;
    std::list<CLuaMain*>           m_LuaMainStack;
    CDuplicateLineFilter<SLogLine> m_DuplicateLineFilter;
};
