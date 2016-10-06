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

void CThreadCommandQueue::Add ( const char* szCommand )
{
    // Add command to queue thread-safe
    m_Mutex.lock ( );
    m_Commands.push ( szCommand );
    m_Mutex.unlock ( );
}


void CThreadCommandQueue::Process ( bool& bRequestedQuit, CModManagerImpl* pModManager )
{
    // Lock the critical section
    m_Mutex.lock ( );

    // Got commands to process?
    while ( !m_Commands.empty ( ) )
    {
        // Grab the string
        auto str = m_Commands.front ( );

        // Check for the most important command: quit and exit.
        if ( str == "quit" || str == "exit" )
            bRequestedQuit = true;
        else if ( pModManager )
            pModManager->HandleInput ( str.c_str ( ) );

        // Remove it from the queue
        m_Commands.pop ( );
    }

    // Unlock the critical section
    m_Mutex.unlock ( );
}

#endif
