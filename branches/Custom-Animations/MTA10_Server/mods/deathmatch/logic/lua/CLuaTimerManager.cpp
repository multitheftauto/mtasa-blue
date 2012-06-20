/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaTimerManager.cpp
*  PURPOSE:     Lua timer manager class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaTimerManager::DoPulse ( CLuaMain* pLuaMain )
{
    CTickCount llCurrentTime = CTickCount::Now ();
    m_bIteratingList = true;
    list < CLuaTimer* > ::iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end (); )
    {
        CLuaTimer* pLuaTimer = *iter;
        CTickCount llStartTime = pLuaTimer->GetStartTime ();
        CTickCount llDelay = pLuaTimer->GetDelay ();
        unsigned int uiRepeats = pLuaTimer->GetRepeats ();

        // Is the time up and is not being deleted
        if ( !pLuaTimer->IsBeingDeleted() && llCurrentTime >= ( llStartTime + llDelay ) )
        {
            // Set our debug info
            g_pGame->GetScriptDebugging()->SetErrorLineAndFile ( pLuaTimer->GetDebugInfo ( ) );
            
            pLuaTimer->ExecuteTimer ( pLuaMain );
            // Reset
            g_pGame->GetScriptDebugging()->SetErrorLineAndFile ( "" );

            // If this is the last repeat, remove
            if ( uiRepeats == 1 )
            {
                delete pLuaTimer;
                iter = m_TimerList.erase ( iter );
            }
            else
            {
                // Decrease repeats if not infinite
                if ( uiRepeats != 0 )
                    (*iter)->SetRepeats ( uiRepeats - 1 );

                pLuaTimer->SetStartTime ( llCurrentTime );

                iter++;
            }
        }
        else
        {
            iter ++;
        }
    }
    m_bIteratingList = false;
    TakeOutTheTrash ();
}


void CLuaTimerManager::RemoveTimer ( CLuaTimer* pLuaTimer )
{
    assert ( pLuaTimer );

    if ( m_bIteratingList )
    {
        pLuaTimer->SetBeingDeleted( true );
        m_TrashCan.push_back ( pLuaTimer );
    }
    else
    {
        if ( !m_TimerList.empty() ) m_TimerList.remove ( pLuaTimer );
        delete pLuaTimer;
    }
}


void CLuaTimerManager::RemoveAllTimers ( void )
{
    // Delete all the timers
    list < CLuaTimer* > ::const_iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the timer list
    m_TimerList.clear ();
}


void CLuaTimerManager::ResetTimer ( CLuaTimer* pLuaTimer )
{
    assert ( pLuaTimer );

    CTickCount llCurrentTime = CTickCount::Now ();
    pLuaTimer->SetStartTime ( llCurrentTime );
}


bool CLuaTimerManager::Exists ( CLuaTimer* pLuaTimer )
{
    return m_TimerList.Contains ( pLuaTimer );
}


CLuaTimer* CLuaTimerManager::GetTimerFromScriptID ( uint uiScriptID )
{
    CLuaTimer* pLuaTimer = (CLuaTimer*) CIdArray::FindEntry ( uiScriptID, EIdClass::TIMER );
    dassert ( !pLuaTimer || m_TimerList.Contains ( pLuaTimer ) );
    return pLuaTimer;
}


CLuaTimer* CLuaTimerManager::AddTimer ( const CLuaFunctionRef& iLuaFunction, CTickCount llTimeDelay, unsigned int uiRepeats, const CLuaArguments& Arguments )
{
    // Check for the minimum interval
    if ( llTimeDelay.ToLongLong () < LUA_TIMER_MIN_INTERVAL )
        return NULL;

    if ( VERIFY_FUNCTION ( iLuaFunction ) )
    {
        // Add the timer
        CLuaTimer* pLuaTimer = new CLuaTimer ( iLuaFunction, Arguments );
        pLuaTimer->SetStartTime ( CTickCount::Now () );
        pLuaTimer->SetDelay ( llTimeDelay );
        pLuaTimer->SetRepeats ( uiRepeats );
        m_TimerList.push_back ( pLuaTimer );
        return pLuaTimer;
    }

    return false;
}


void CLuaTimerManager::GetTimers ( CTickCount llTime, lua_State* luaVM )
{
    assert ( luaVM );

    CTickCount llCurrentTime = CTickCount::Now ();
    // Add all the timers to the table
    unsigned int uiIndex = 0;
    list < CLuaTimer* > ::const_iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end () ; iter++ )
    {
        // If the time left is less than the time specified, or the time specifed is 0
        CTickCount llTimeLeft = ( (*iter)->GetStartTime () + (*iter)->GetDelay () ) - llCurrentTime;
        if ( llTime.ToLongLong () == 0 || llTimeLeft <= llTime )
        {
            // Add it to the table
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushtimer ( luaVM, *iter );
            lua_settable ( luaVM, -3 );
        }
    }
}


void CLuaTimerManager::TakeOutTheTrash ( void )
{
    list < CLuaTimer* > ::iterator iter = m_TrashCan.begin ();
    for ( ; iter != m_TrashCan.end () ; iter++ )
    {
        // Delete the object
        CLuaTimer* pTimer = *iter;
        if ( Exists ( pTimer ) )
        {
            if ( !m_TimerList.empty() ) m_TimerList.remove ( pTimer );
            delete pTimer;
        }
    }
    m_TrashCan.clear ();
}

