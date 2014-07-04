/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CThreadCommandQueue.cpp
*  PURPOSE:     Thread command queue class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#ifdef WIN32
#include "CThreadCommandQueue.h"
#include "CModManagerImpl.h"

using std::string;

void CThreadCommandQueue::Add ( const char* szCommand )
{
    // Lock the critical section, add it, then unlock
    m_Critical.Lock ();
    m_Commands.push_back ( szCommand );
    m_Critical.Unlock ();
}


void CThreadCommandQueue::Process ( bool& bRequestedQuit, CModManagerImpl* pModManager )
{
    // Lock the critical section
    m_Critical.Lock ();

    // Got commands to process?
    while ( m_Commands.size () > 0 )
    {
        // Grab the string
        const string& str = m_Commands.front ();

        // Check for the most important command: quit and exit.
        if ( str == "quit" || str == "exit" )
        {
            bRequestedQuit = true;
        }
        else
        {
            // Otherwise, pass the command to the mod's input handler
            pModManager->HandleInput ( str.c_str () );
        }

        // Remove it from the queue
        m_Commands.pop_front ();
    }

    // Unlock the critical section
    m_Critical.Unlock ();
}

#endif
