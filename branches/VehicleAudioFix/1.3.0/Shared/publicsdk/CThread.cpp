/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

#include "CThread.h"

CThread::CThread ( void )
{
    m_pThreadData = NULL;
    m_pArg = NULL;
#ifdef WIN32    // Win32 threads
    m_hThread = NULL;
#endif
}


CThread::~CThread ( void )
{
    Stop ();
}


bool CThread::Start ( CThreadData *pData )
{
    if ( pData == NULL )    // pData HAS to be valid
        return false;

    Stop ();
    Arg ( pData );

    #ifdef WIN32    // Win32 threads
        m_hThread = CreateThread ( NULL, 0, &CThread::EntryPoint, this, 0, NULL );
        return m_hThread != NULL;
    #else           // POSIX threads
        if ( pthread_create ( &m_hThread, NULL, CThread::EntryPoint, this ) )
        {
            // something bad happened
            return false;
        }
    #endif
    return true;
}


void CThread::Stop ( void )
{
    if ( m_hThread )
    {
//      TerminateThread ( m_hThread, 0 );
        if ( m_pThreadData != NULL )
        m_pThreadData->bAbortThread = true;
    }
}


bool CThread::TryLock ( ThreadMutex * Mutex )
{
    #ifdef WIN32
        if ( TryEnterCriticalSection ( Mutex ) != 0 )
            return true;
    #else
        if ( pthread_mutex_trylock ( Mutex ) == 0 )
            return true;
    #endif
    return false;
}


void CThread::Lock ( ThreadMutex * Mutex )
{
    #ifdef WIN32    // Win32 threads
        EnterCriticalSection ( Mutex );
    #else           // POSIX threads
        pthread_mutex_lock ( Mutex );
    #endif
}


void CThread::Unlock ( ThreadMutex * Mutex )
{
    #ifdef WIN32    // Win32 threads
        LeaveCriticalSection ( Mutex );
    #else           // POSIX threads
        pthread_mutex_unlock ( Mutex );
    #endif
}


int CThread::Run ( CThreadData* arg )
{
    return Execute ( arg );
}


#ifdef WIN32    // Win32 threads
DWORD CThread::EntryPoint ( void* pThis )
{
    CThread* pt = (CThread*)pThis;
    return pt->Run ( pt->Arg () );
}
#else           // POSIX threads
void* CThread::EntryPoint ( void* pThis )
{
    CThread* pt = (CThread*)pThis;
    pt->Run ( pt->Arg () );
    return NULL;
}
#endif


CThreadData* CThread::Arg ( void ) const
{
    return m_pThreadData;
}


void CThread::Arg ( CThreadData* pData )
{
    m_pThreadData = pData;
}
