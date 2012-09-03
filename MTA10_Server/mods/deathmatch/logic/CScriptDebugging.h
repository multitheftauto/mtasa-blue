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

    void                            OutputDebugInfo                 ( lua_State* luaVM, int iLevel, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255 );

    void                            LogCustom                       ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... );
    void                            LogInformation                  ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogWarning                      ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogError                        ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogError                        ( SString strFile, int iLine, SString strMsg );
    void                            LogBadPointer                   ( lua_State* luaVM, const char* szFunction, const char* szArgumentType, unsigned int uiArgument );
    void                            LogBadType                      ( lua_State* luaVM, const char* szFunction );
    void                            LogBadAccess                    ( lua_State* luaVM, const char* szFunction );
    void                            LogCustom                       ( lua_State* luaVM, const char* szMessage );

    bool                            SetLogfile                      ( const char* szFilename, unsigned int uiLevel );
    inline void                     SetHTMLLogLevel                 ( unsigned int uiLevel ) { m_uiHtmlLogLevel = uiLevel; }

    inline void                     SetErrorLineAndFile             ( SString strOutput ) { m_strLineAndFile = strOutput; }

    void                            PushLuaMain                     ( CLuaMain* pLuaMain );
    void                            PopLuaMain                      ( CLuaMain* pLuaMain );
    void                            OnLuaMainDestroy                ( CLuaMain* pLuaMain );

private:
    void                            LogString                       ( const char* szPrePend, lua_State * luaVM, const char* szMessage, unsigned int uiMinimumDebugLevelunsigned, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255 );

    void                            PrintLog                        ( const char* szText );
    void                            Broadcast                       ( const CPacket& Packet, unsigned int uiMinimumDebugLevel );

    CLuaManager*                    m_pLuaManager;
    unsigned int                    m_uiLogFileLevel;
    unsigned int                    m_uiHtmlLogLevel;
    FILE*                           m_pLogFile;
    list < class CPlayer* >         m_Players;
    bool                            m_bTriggeringOnDebugMessage;
    SString                         m_strLineAndFile;
    std::list < CLuaMain* >         m_LuaMainStack;
};

#endif
