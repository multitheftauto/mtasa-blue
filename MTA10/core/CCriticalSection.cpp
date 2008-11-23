/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCriticalSection.cpp
*  PURPOSE:		Easy interface to critical section locking
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CRITICAL_SECTION CCriticalSection::m_CriticalSection;

void CCriticalSection::Initialize ( void )
{
    InitializeCriticalSection ( &m_CriticalSection );
}


void CCriticalSection::Finalize ( void )
{
    DeleteCriticalSection ( &m_CriticalSection );
}


void CCriticalSection::Lock ( void)
{
    EnterCriticalSection ( &m_CriticalSection );
}


void CCriticalSection::Unlock ( void )
{
    LeaveCriticalSection ( &m_CriticalSection );
}
