/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CScriptDebugging.h
*  PURPOSE:     Script debugging class header
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CSCRIPTDEBUGGING_H
#define __CSCRIPTDEBUGGING_H

#include <cstdio>
#include <list>
#include <stdio.h>

#include "lua/LuaCommon.h"

class CLuaManager;

class CScriptDebugging
{
public:
                                    CScriptDebugging                ( CLuaManager* pLuaManager );
                                    ~CScriptDebugging               ( void );

    void                            OutputDebugInfo                 ( lua_State* luaVM, int iLevel, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255 );

    void                            LogCustom                       ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... );
    void                            LogInformation                  ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogWarning                      ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogError                        ( lua_State* luaVM, const char* szFormat, ... );
    void                            LogError                        ( SString strFile, int iLine, SString strMsg );
    void                            LogBadPointer                   ( lua_State* luaVM, const char* szFunction, const char* szArgumentType, unsigned int uiArgument );
    void                            LogBadType                      ( lua_State* luaVM, const char* szFunction );
    void                            LogBadLevel                     ( lua_State* luaVM, const char* szFunction, unsigned int uiRequiredLevel );
    void                            LogCustom                       ( lua_State* luaVM, const char* szMessage );

    bool                            SetLogfile                      ( const char* szFilename, unsigned int uiLevel );

    void                            PushLuaMain                     ( CLuaMain* pLuaMain );
    void                            PopLuaMain                      ( CLuaMain* pLuaMain );
    void                            OnLuaMainDestroy                ( CLuaMain* pLuaMain );

private:
    void                            LogString                       ( const char* szPrePend, lua_State* luaVM, const char* szMessage, unsigned int uiMinimumDebugLevelunsigned, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255 );
    void                            PrintLog                        ( const char* szText );
 
    CLuaManager*                    m_pLuaManager;
    unsigned int                    m_uiLogFileLevel;
    FILE*                           m_pLogFile;
    bool                            m_bTriggeringOnClientDebugMessage;
    std::list < CLuaMain* >         m_LuaMainStack;
};

#endif
