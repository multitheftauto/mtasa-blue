/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaTimer.h
*  PURPOSE:     Lua timer class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaTimer;

#ifndef __CLUATIMER_H
#define __CLUATIMER_H

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

#define LUA_TIMER_MIN_INTERVAL      50

class CLuaTimer
{
public:

                            CLuaTimer                   ( const CLuaFunctionRef& iLuaFunction, const CLuaArguments& Arguments );
                            ~CLuaTimer                  ( void );

    void                    RemoveScriptID              ( void );

    CTickCount              GetStartTime                ( void ) const                  { return m_llStartTime; };
    inline void             SetStartTime                ( CTickCount llStartTime )      { m_llStartTime = llStartTime; };

    CTickCount              GetDelay                    ( void ) const                  { return m_llDelay; };
    inline void             SetDelay                    ( CTickCount llDelay )          { m_llDelay = llDelay; };

    inline unsigned int     GetRepeats                  ( void ) const                  { return m_uiRepeats; };
    inline void             SetRepeats                  ( unsigned int uiRepeats )      { m_uiRepeats = uiRepeats; }

    void                    ExecuteTimer                ( class CLuaMain* pLuaMain );

    CTickCount              GetTimeLeft                 ( void );

    uint                    GetScriptID                 ( void ) const                  { return m_uiScriptID; }
    const SLuaDebugInfo&    GetLuaDebugInfo             ( void )                        { return m_LuaDebugInfo; }
    void                    SetLuaDebugInfo             ( const SLuaDebugInfo& luaDebugInfo ) { m_LuaDebugInfo = luaDebugInfo; }

private:
    CLuaFunctionRef         m_iLuaFunction;
    CLuaArguments           m_Arguments;
    CTickCount              m_llStartTime;
    CTickCount              m_llDelay;
    unsigned int            m_uiRepeats;
    uint                    m_uiScriptID;
    SLuaDebugInfo           m_LuaDebugInfo;
};

#endif
