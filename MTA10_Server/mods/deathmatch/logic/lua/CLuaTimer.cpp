/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaTimer.cpp
*  PURPOSE:     Lua timer class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CLuaTimer::CLuaTimer ( void )
{
    m_ulStartTime = 0;
	m_ulDelay = 0;
	m_uiRepeats = 1;
    m_iLuaFunction = LUA_REFNIL;
    m_bBeingDeleted = false;
}


CLuaTimer::CLuaTimer ( int iLuaFunction, const CLuaArguments& Arguments )
{
	m_iLuaFunction = iLuaFunction;
    m_Arguments = Arguments;
    m_bBeingDeleted = false;
}


CLuaTimer::~CLuaTimer ( void )
{
}


void CLuaTimer::ExecuteTimer ( CLuaMain* pLuaMain )
{
    if ( m_iLuaFunction != LUA_REFNIL )
    {
        m_Arguments.Call ( pLuaMain, m_iLuaFunction );
    }
}
