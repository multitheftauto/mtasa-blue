/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CAntiCheat.cpp
*  PURPOSE:     Anti-cheat handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

// Turn off optimizations in this file because we're encrypting stuff
#pragma optimize( "", off )

#define ANTICHEAT_CHECKS_PER_CYCLE 5

CAntiCheat::CAntiCheat ( void )
{
    // Start at the first index
    m_LastEntry = m_Entries.begin ();
}


CAntiCheat::~CAntiCheat ( void )
{
    // All modules should've been removed now
    assert ( m_Entries.size () == 0 );
}


void CAntiCheat::AddModule ( CAntiCheatModule& Module )
{
    m_Entries.push_back ( &Module );
}


void CAntiCheat::RemoveModule ( CAntiCheatModule& Module )
{
    // Find the entry
    auto iter = std::find(m_Entries.begin(), m_Entries.end(), &Module);
    // Did we find it?
    if (iter == m_Entries.end())
        return;

    // Does this iterator match our last index?
    if (m_LastEntry == iter)
    {
        // Make it our last entry. This might be end(), but it'll be valid.
        m_LastEntry = m_Entries.erase(iter);
        return;
    }
    else
    {
        // Just erase it and return. Our last entry should be valid.
        m_Entries.erase(iter);
        return;
    }

}


void CAntiCheat::ClearModules ( void )
{
    m_Entries.clear ();
}


bool CAntiCheat::PerformChecks ( void )
{
    // We have entries in our vector?
    if ( m_Entries.size () > 0 )
    {
        // Do this a number of times
        for ( int i = 0; i < ANTICHEAT_CHECKS_PER_CYCLE; i++ )
        {
            // Is the current index at the end? Start over.
            if ( m_LastEntry == m_Entries.end () )
            {
                m_LastEntry = m_Entries.begin ();
            }

            // Perform checks on the given element, as long as it returns
            // true it means the player does not cheat.
            if ( !( *m_LastEntry )->PerformChecks () )
            {
                return false;
            }
        }
    }

    // He does not cheat
    return true;
}

// Turn optimizations back on if enabled in project settings
#pragma optimize( "", on )
