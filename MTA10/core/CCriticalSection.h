/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCriticalSection.h
*  PURPOSE:		Header file for critical section class
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCRITICALSECTIONS_H
#define __CCRITICALSECTIONS_H

#include <windows.h>

class CCriticalSection  
{
    public:
    static void                 Initialize              ( void );
    static void                 Finalize                ( void );
    static void                 Lock                    ( void );
    static void                 Unlock                  ( void );

    private:
    static CRITICAL_SECTION     m_CriticalSection;
};

#endif
