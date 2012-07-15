/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaTimerManager.cpp
*  PURPOSE:     Lua timer manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

void CLuaTimerManager::DoPulse ( CLuaMain* pLuaMain )
{
    assert ( m_ProcessQueue.empty () );
    assert ( m_DeleteList.empty () );
    assert ( !m_pProcessingTimer );

    CTickCount llCurrentTime = CTickCount::Now ();

    // Use a separate queue to avoid trouble
    m_ProcessQueue = std::deque < CLuaTimer* > ( m_TimerList.begin (), m_TimerList.end () );
    while ( !m_ProcessQueue.empty () )
    {
        m_pProcessingTimer = m_ProcessQueue.front ();
        m_ProcessQueue.pop_front ();

        CTickCount llStartTime = m_pProcessingTimer->GetStartTime ();
        CTickCount llDelay = m_pProcessingTimer->GetDelay ();
        unsigned int uiRepeats = m_pProcessingTimer->GetRepeats ();

        // Is the time up and is not being deleted
        if ( llCurrentTime >= ( llStartTime + llDelay ) )
        {
            m_pProcessingTimer->ExecuteTimer ( pLuaMain );

            // If this is the last repeat, remove
            if ( uiRepeats == 1 )
            {
                RemoveTimer ( m_pProcessingTimer );
            }
            else
            {
                // Decrease repeats if not infinite
                if ( uiRepeats != 0 )
                    m_pProcessingTimer->SetRepeats ( uiRepeats - 1 );

                m_pProcessingTimer->SetStartTime ( llCurrentTime );
            }
        }
    }
    m_pProcessingTimer = NULL;

    // Finally cleanup timers that were removed during their own processing
    for ( std::set < CLuaTimer* > ::const_iterator iter = m_DeleteList.begin () ; iter != m_DeleteList.end (); iter++ )
        delete *iter;
    m_DeleteList.clear ();
}


void CLuaTimerManager::RemoveTimer ( CLuaTimer* pLuaTimer )
{
    assert ( pLuaTimer );

    // Remove all references
    ListRemove ( m_TimerList, pLuaTimer );
    ListRemove ( m_ProcessQueue, pLuaTimer );
    if ( m_pProcessingTimer == pLuaTimer )
        m_DeleteList.insert ( pLuaTimer );
    else
        delete pLuaTimer;
}


void CLuaTimerManager::RemoveAllTimers ( void )
{
    // Delete all the timers
    std::vector < CLuaTimer* > ::const_iterator iter = m_TimerList.begin ();
    for ( ; iter != m_TimerList.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the timer list
    m_TimerList.clear ();
    m_ProcessQueue.clear ();
    m_DeleteList.clear ();
    m_pProcessingTimer = NULL;
}


void CLuaTimerManager::ResetTimer ( CLuaTimer* pLuaTimer )
{
    assert ( pLuaTimer );

    CTickCount llCurrentTime = CTickCount::Now ();
    pLuaTimer->SetStartTime ( llCurrentTime );
}


CLuaTimer* CLuaTimerManager::GetTimerFromScriptID ( uint uiScriptID )
{
    CLuaTimer* pLuaTimer = (CLuaTimer*) CIdArray::FindEntry ( uiScriptID, EIdClass::TIMER );
    if ( !pLuaTimer )
        return NULL;

    dassert ( ListContains ( m_TimerList, pLuaTimer ) );
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
