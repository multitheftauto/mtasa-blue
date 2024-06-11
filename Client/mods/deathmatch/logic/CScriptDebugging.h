/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CScriptDebugging.h
 *  PURPOSE:     Script debugging class header
 *
 *****************************************************************************/

#pragma once

#include <cstdio>
#include <list>
#include <stdio.h>

#include "lua/LuaCommon.h"

class CLuaManager;

struct SLogLine
{
    SString       strText;
    unsigned int  uiMinimumDebugLevel;
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;
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
    CScriptDebugging(CLuaManager* pLuaManager);
    ~CScriptDebugging();

    void LogCustom(lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ...);
    void LogDebug(lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ...);
    void LogInformationV(lua_State* luaVM, const char* format, va_list vlist);
    void LogInformation(lua_State* luaVM, const char* szFormat, ...);
    void LogWarning(lua_State* luaVM, const char* szFormat, ...);
    void LogError(lua_State* luaVM, const char* szFormat, ...);
    void LogBadPointer(lua_State* luaVM, const char* szArgumentType, unsigned int uiArgument);
    void LogBadType(lua_State* luaVM);
    void LogBadLevel(lua_State* luaVM, unsigned int uiRequiredLevel);
    void LogCustom(lua_State* luaVM, const char* szMessage);
    void LogWarning(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...);
    void LogError(const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ...);
    void LogPCallError(lua_State* luaVM, const SString& strRes, bool bInitialCall = false);

    bool SetLogfile(const char* szFilename, std::set<DebugScriptLevel> debugLevel);

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

public:
    static FILE* m_pLogFile;

private:
    CLuaManager*                   m_pLuaManager;
    std::set<DebugScriptLevel>     m_LogFileLevels;
    bool                           m_bTriggeringMessageEvent;
    SLuaDebugInfo                  m_SavedLuaDebugInfo;
    std::list<CLuaMain*>           m_LuaMainStack;
    HANDLE                         m_flushTimerHandle;
    CDuplicateLineFilter<SLogLine> m_DuplicateLineFilter;
};
