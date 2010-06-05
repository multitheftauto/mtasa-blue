/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/Common.h
*  PURPOSE:		Common game layer include file
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COMMON
#define __CGAMESA_COMMON

#define CGAME_DLL

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <game/Common.h>

#undef DEBUG_LOG
#ifdef DEBUG_LOG
	#include <stdio.h>
	#include <time.h>

    static FILE* fDebugFile;

    static void OutputDebugText ( char * szDebugText )
    {
        #ifdef MTA_DEBUG
        char szDebug[500];
        sprintf ( szDebug, "%s\n", szDebugText );
        OutputDebugString ( szDebug );
        #endif
    }

    #ifdef MTA_DEBUG
	    #define DEBUG_TRACE(szText) \
            OutputDebugString(szText);
    #endif
#else
    #define DEBUG_TRACE(szText) // we do nothing with release versions
#endif

#endif