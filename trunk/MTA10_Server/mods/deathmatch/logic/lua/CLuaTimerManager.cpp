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
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaTimerManager::DoPulse ( CLuaMain* pLuaMain )
{
    unsigned long ulCurrentTime = GetTime ();
    m_bIteratingList = true;
    list < CLuaTimer* > ::iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end (); )
    {
        CLuaTimer* pLuaTimer = *iter;
        unsigned long ulStartTime = pLuaTimer->GetStartTime ();
        unsigned long ulDelay = pLuaTimer->GetDelay ();
        unsigned int uiRepeats = pLuaTimer->GetRepeats ();

        // Is the time up
        if ( ulCurrentTime >= ( ulStartTime + ulDelay ) )
        {
            pLuaTimer->ExecuteTimer ( pLuaMain );

            // If this is the last repeat, remove
            if ( uiRepeats == 1 )
            {
                delete pLuaTimer;
                m_TimerList.erase ( iter++ );
            }
            else
            {
                // Decrease repeats if not infinite
                if ( uiRepeats != 0 )
                    (*iter)->SetRepeats ( uiRepeats - 1 );

                pLuaTimer->SetStartTime ( ulCurrentTime );

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
    list < CLuaTimer* > ::iterator iter = m_TimerList.begin ();
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

    unsigned long ulCurrentTime = GetTime ();
    pLuaTimer->SetStartTime ( ulCurrentTime );
}


bool CLuaTimerManager::Exists ( CLuaTimer* pLuaTimer )
{
    // Return true if we find it
    list < CLuaTimer* > ::iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end (); iter++ )
    {
        if ( *iter == pLuaTimer )
        {
            return true;
        }
    }

    // If not, false
    return false;
}


CLuaTimer* CLuaTimerManager::AddTimer ( lua_State* luaVM )
{
    if ( luaVM )
    {
        int iArgument2 = lua_type ( luaVM, 2 );
        int iArgument3 = lua_type ( luaVM, 3 );
        if ( lua_type ( luaVM, 1 ) &&
             (( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) ||
              ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) ))
        {
            // Grab the string argument, start-time, delay and repeats
            unsigned long ulTimeDelay = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
            unsigned int uiRepeats = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );

            // Check for the minimum interval
            if ( ulTimeDelay < LUA_TIMER_MIN_INTERVAL ) return NULL;

            // Grab the arguments from argument 4 and up
            CLuaArguments Arguments;
            Arguments.ReadArguments ( luaVM, 4 );

            int iLuaFunction = luaM_toref ( luaVM, 1 );

            if ( iLuaFunction != LUA_REFNIL )
            {
                // Add the timer
                CLuaTimer* pLuaTimer = new CLuaTimer ( iLuaFunction, Arguments );
                pLuaTimer->SetStartTime ( GetTime () );
                pLuaTimer->SetDelay ( ulTimeDelay );
                pLuaTimer->SetRepeats ( uiRepeats );
                m_TimerList.push_back ( pLuaTimer );
                return pLuaTimer;
            }
        }
    }
    return false;
}

void CLuaTimerManager::GetTimers ( unsigned long ulTime, CLuaMain* pLuaMain )
{
    assert ( pLuaMain );

    unsigned long ulCurrentTime = GetTime();
    // Add all the timers to the table
    unsigned int uiIndex = 0;
    list < CLuaTimer* > ::iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end () ; iter++ )
    {
        // If the time left is less than the time specified, or the time specifed is 0
        unsigned long ulTimeLeft = ( (*iter)->GetStartTime () + (*iter)->GetDelay () ) - ulCurrentTime;
        if ( ulTime == 0 || ulTimeLeft <= ulTime )
        {
            // Add it to the table
            lua_State* luaVM = pLuaMain->GetVirtualMachine ();
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

