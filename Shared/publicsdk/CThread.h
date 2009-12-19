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

/** And remember.. threads on Win32 and POSIX are not similar at all. **/

#ifndef __CTHREAD_H
#define __CTHREAD_H

#ifdef WIN32			// Win32 threads
	#define _WIN32_WINNT 0x400
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	typedef HANDLE				ThreadHandle;
	typedef CRITICAL_SECTION	ThreadMutex;
#else					// POSIX threads
	#include <stdio.h>
	#include <pthread.h>

	typedef pthread_t			ThreadHandle;
	typedef pthread_mutex_t		ThreadMutex;
#endif

#include "CThreadData.h"

// Base thread class
class CThread
{
public:
                        CThread         ( void );
				virtual	~CThread        ( void );

    bool                Start           ( CThreadData *pData );
    void                Stop            ( void );

	static bool			TryLock			( ThreadMutex * Mutex );
	static void			Lock			( ThreadMutex * Mutex );
	static void			Unlock			( ThreadMutex * Mutex );

protected:
    int                 Run             ( CThreadData* arg );

    virtual int			Execute         ( CThreadData* pData ) = 0;

    CThreadData*		Arg             ( void ) const;
    void				Arg             ( CThreadData* pData );

#ifdef WIN32	// Win32 threads
    static DWORD WINAPI EntryPoint      ( void* );
#else			// POSIX threads
	static void*		EntryPoint		( void* );
#endif

private:
    void*               m_pArg;
	CThreadData*		m_pThreadData;
	ThreadHandle		m_hThread;
};

#endif
