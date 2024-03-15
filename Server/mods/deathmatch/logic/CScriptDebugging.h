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

struct SLogLine
{
    SString       strText;
    std::uint32_t  uiMinimumDebugLevel;
    std::uint8_t ucRed;
    std::uint8_t ucGreen;
    std::uint8_t ucBlue;
    void          operator+=(const char* szAppend) { strText += szAppend; }
    bool          operator==(const SLogLine& other) const
    {
        return strText == other.strText && uiMinimumDebugLevel == other.uiMinimumDebugLevel && ucRed == other.ucRed && ucGreen == other.ucGreen &&
               ucBlue == other.ucBlue;
    }
};

class CScriptDebugging
{
public:
    CScriptDebugging();
    ~CScriptDebugging();

    bool AddPlayer(class CPlayer& Player, std::uint32_t uiLevel);
    bool RemovePlayer(class CPlayer& Player);
    void ClearPlayers();

    void LogCustom(lua_State* luaVM, std::uint8_t ucRed, std::uint8_t ucGreen, std::uint8_t ucBlue, const char* szFormat, ...);
    void LogDebug(lua_State* luaVM, std::uint8_t ucRed, std::uint8_t ucGreen, std::uint8_t ucBlue, const char* szFormat, ...);
    void LogInformationV(lua_State* luaVM, const char* format, va_list vlist);
    void LogInformation(lua_State* luaVM, const char* szFormat, ...);
    void LogWarning(lua_State* luaVM, const char* szFormat, ...);
    void LogError(lua_State* luaVM, const char* szFormat, ...);
    void LogBadPointer(lua_State* luaVM, const char* szArgumentType, std::uint32_t uiArgument);
    void LogBadType(lua_State* luaVM);
    void LogBadAccess(lua_State* luaVM);
    void LogCustom(lua_State* luaVM, const char* szMessage);
    void LogWarning(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...);
    void LogError(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...);
    void LogPCallError(lua_State* luaVM, const SString& strRes, bool bInitialCall = false);

    bool SetLogfile(const char* szFilename, std::uint32_t uiLevel);
    void SetHTMLLogLevel(std::uint32_t uiLevel) { m_uiHtmlLogLevel = uiLevel; }

    const SLuaDebugInfo& GetLuaDebugInfo(lua_State* luaVM);
    void                 SaveLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_SavedLuaDebugInfo = luaDebugInfo; }

    void      PushLuaMain(CLuaMain* pLuaMain);
    void      PopLuaMain(CLuaMain* pLuaMain);
    void      OnLuaMainDestroy(CLuaMain* pLuaMain);
    CLuaMain* GetTopLuaMain();
    void      UpdateLogOutput();

private:
    SString ComposeErrorMessage(const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage);
    void LogString(const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage, std::uint32_t uiMinimumDebugLevel, std::uint8_t ucRed = 255,
                   std::uint8_t ucGreen = 255, std::uint8_t ucBlue = 255);

    void PrintLog(const char* szText);
    void Broadcast(const CPacket& Packet, std::uint32_t uiMinimumDebugLevel);

    std::uint32_t                   m_uiLogFileLevel;
    std::uint32_t                   m_uiHtmlLogLevel;
    FILE*                          m_pLogFile;
    list<class CPlayer*>           m_Players;
    bool                           m_bTriggeringMessageEvent;
    SLuaDebugInfo                  m_SavedLuaDebugInfo;
    std::list<CLuaMain*>           m_LuaMainStack;
    CDuplicateLineFilter<SLogLine> m_DuplicateLineFilter;
};
