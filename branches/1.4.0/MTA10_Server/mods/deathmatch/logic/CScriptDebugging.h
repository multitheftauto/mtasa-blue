/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptDebugging.h
*  PURPOSE:     Script debugging facility class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSCRIPTDEBUGGING_H
#define __CSCRIPTDEBUGGING_H

#include "lua/CLuaManager.h"
#include "packets/CPacket.h"
#include <cstdio>
#include <list>

class CScriptDebugging
{
public:
                                    CScriptDebugging                ( CLuaManager* pLuaManager );
    inline                          ~CScriptDebugging               ( void )                            { ClearPlayers (); };

    bool                            AddPlayer                       ( class CPlayer& Player, unsigned int uiLevel );
    bool                            RemovePlayer                    ( class CPlayer& Player );
    void                            ClearPlayers                    ( void );

    void                            LogCustom                       ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... );
    void                            LogInformation                  ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogWarning                      ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogError                        ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogBadPointer                   ( lua_State* luaVM, const char* szArgumentType, unsigned int uiArgument );
    void                            LogBadType                      ( lua_State* luaVM );
    void                            LogBadAccess                    ( lua_State* luaVM );
    void                            LogCustom                       ( lua_State* luaVM, const char* szMessage );
    void                            LogWarning                      ( const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ... );
    void                            LogError                        ( const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ... );
    void                            LogPCallError                   ( lua_State* luaVM, const SString& strRes, const SString& strNiceFilename = "" );

    bool                            SetLogfile                      ( const char* szFilename, unsigned int uiLevel );
    inline void                     SetHTMLLogLevel                 ( unsigned int uiLevel ) { m_uiHtmlLogLevel = uiLevel; }

    const SLuaDebugInfo&            GetLuaDebugInfo                 ( lua_State* luaVM );
    void                            SaveLuaDebugInfo                ( const SLuaDebugInfo& luaDebugInfo ) { m_SavedLuaDebugInfo = luaDebugInfo; }

    void                            PushLuaMain                     ( CLuaMain* pLuaMain );
    void                            PopLuaMain                      ( CLuaMain* pLuaMain );
    void                            OnLuaMainDestroy                ( CLuaMain* pLuaMain );
    CLuaMain*                       GetTopLuaMain                   ( void );

private:
    SString                         ComposeErrorMessage             ( const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage );
    void                            LogString                       ( const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255 );

    void                            PrintLog                        ( const char* szText );
    void                            Broadcast                       ( const CPacket& Packet, unsigned int uiMinimumDebugLevel );

    CLuaManager*                    m_pLuaManager;
    unsigned int                    m_uiLogFileLevel;
    unsigned int                    m_uiHtmlLogLevel;
    FILE*                           m_pLogFile;
    list < class CPlayer* >         m_Players;
    bool                            m_bTriggeringOnDebugMessage;
    SLuaDebugInfo                   m_SavedLuaDebugInfo;
    std::list < CLuaMain* >         m_LuaMainStack;
};

#endif
