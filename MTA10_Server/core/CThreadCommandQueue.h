/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CThreadCommandQueue.h
*  PURPOSE:     Thread command queue class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTHREADCOMMANDQUEUE_H
#define __CTHREADCOMMANDQUEUE_H

#ifdef WIN32

#include "CCriticalSection.h"
#include <list>
#include <string>

class CThreadCommandQueue
{
public:
    void                Add             ( const char* szCommand );
    void                Process         ( bool& bRequestedQuit, class CModManagerImpl* pModManager );

private:
    std::list < std::string >       m_Commands;
    CCriticalSection                m_Critical;
};

#endif

#endif
