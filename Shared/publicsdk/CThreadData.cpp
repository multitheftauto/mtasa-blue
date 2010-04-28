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

#include "CThreadData.h"

CThreadData::CThreadData ( void )
{
    bAbortThread = false;

    // Initialize the mutexes
    #ifdef WIN32    // Win32 threads
        InitializeCriticalSection ( &MutexPrimary );
        InitializeCriticalSection ( &MutexLogical );
    #else           // POSIX threads
        pthread_mutex_init ( &MutexPrimary, NULL );
        pthread_mutex_init ( &MutexLogical, NULL );
    #endif
}

CThreadData::~CThreadData ( void )
{
    #ifdef WIN32
        DeleteCriticalSection ( &MutexPrimary );
        DeleteCriticalSection ( &MutexLogical );
    #else
    #endif
}
