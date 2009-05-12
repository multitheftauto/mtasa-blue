/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "SString.h"

//
// _vsnprintf with buffer full check
//
#define _VSNPRINTF( buffer, count, format, argptr ) \
    { \
        int iResult = _vsnprintf ( buffer, count, format, argptr ); \
        if( iResult == -1 || iResult == (count) ) \
            (buffer)[(count)-1] = 0; \
    }

#ifndef WIN32
    #define _vsnprintf vsnprintf
#endif


namespace SharedUtil
{
    //
    // Get startup directory as saved in the registry by the launcher
    // Used in the Win32 Client only
    //
    SString GetMTASABaseDir();

    //
    // Turns a relative MTASA path i.e. "MTA\file.dat"
    // into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
    //
    SString CalcMTASAPath ( const SString& strPath );

    //
    // Retrieves the number of milliseconds that have elapsed since the system was started.
    //
    // GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
    // an __int64 and will effectively never wrap. This is an emulated version for XP and down.
    // Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
    // 
    long long GetTickCount64_ ( void );

    //
    // Retrieves the number of seconds that have elapsed since the system was started.
    //
    double GetSecondCount ( void );


    //
    // Some templates
    //
    template < class T >
    T Min ( const T& a, const T& b )
    {
        return a < b ? a : b;
    }

    template < class T >
    T Max ( const T& a, const T& b )
    {
        return a > b ? a : b;
    }

    // Clamps a value between two other values ( min < a < max )
    template < class T >
    T Clamp ( const T& min, const T& a, const T& max )
    {
        return a < min ? min : a > max ? max : a;
    }

};

using namespace SharedUtil;

