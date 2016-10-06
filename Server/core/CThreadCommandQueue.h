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

#pragma once

#ifdef WIN32

#include <mutex>
#include <queue>
#include <string>

class CThreadCommandQueue
{
public:
    void                Add             ( const char* szCommand );
    void                Process         ( bool& bRequestedQuit, class CModManagerImpl* pModManager );

private:
    std::queue < std::string >  m_Commands;
    std::mutex                  m_Mutex;
};

#endif
