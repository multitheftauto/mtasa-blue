/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaTimer.cpp
*  PURPOSE:     Lua timer class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CLuaTimer::CLuaTimer ( void )
{
    m_ulStartTime = 0;
	m_ulDelay = 0;
	m_uiRepeats = 1;
    m_iLuaFunction = LUA_REFNIL;
    m_pArguments = new CLuaArguments;
}


CLuaTimer::CLuaTimer ( int iLuaFunction, const CLuaArguments& Arguments )
{
	m_iLuaFunction = iLuaFunction;
    m_pArguments = new CLuaArguments ( Arguments );
}


CLuaTimer::~CLuaTimer ( void )
{
    delete m_pArguments;
    m_pArguments = NULL;
}


void CLuaTimer::ExecuteTimer ( CLuaMain* pLuaMain )
{
    if ( m_iLuaFunction != LUA_REFNIL && m_pArguments )
        m_pArguments->Call ( pLuaMain, m_iLuaFunction );
}
