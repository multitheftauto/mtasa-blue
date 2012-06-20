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

CLuaTimer::CLuaTimer ( const CLuaFunctionRef& iLuaFunction, const CLuaArguments& Arguments )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::TIMER );
    m_uiRepeats = 1;
    m_bBeingDeleted = false;
    m_iLuaFunction = iLuaFunction;
    m_pArguments = new CLuaArguments ( Arguments );
}


CLuaTimer::~CLuaTimer ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::TIMER, m_uiScriptID );
    delete m_pArguments;
    m_pArguments = NULL;
}


void CLuaTimer::ExecuteTimer ( CLuaMain* pLuaMain )
{
    if ( VERIFY_FUNCTION ( m_iLuaFunction ) && m_pArguments )
        m_pArguments->Call ( pLuaMain, m_iLuaFunction );
}


CTickCount CLuaTimer::GetTimeLeft ( void )
{
    CTickCount llCurrentTime = CTickCount::Now ();
    CTickCount llTimeLeft = m_llStartTime + m_llDelay - llCurrentTime;
    return llTimeLeft;
}
