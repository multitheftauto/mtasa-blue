/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        StdInc.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <string>

#ifndef WIN32
// Linux allocation tracking doesn't work in this module for some reason
    #define WITH_ALLOC_TRACKING 0
#endif
#include "SharedUtil.h"

#ifdef WIN32
//
// Windows
//
    #include <conio.h>
    #include <direct.h>
    #include <windows.h>
#else
//
// POSIX
//
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <dlfcn.h>
    #include <sys/time.h>
    #include <sys/times.h>

    #if defined(__APPLE__)
        #include <ncurses.h>
    #elif __has_include(<ncursesw/curses.h>)
        #include <ncursesw/curses.h>
    #else
        #include <ncurses.h>
    #endif

    #define MAX_PATH 255

    #ifndef stricmp
        #define stricmp strcasecmp
    #endif

    #ifndef strnicmp
        #define strnicmp strncasecmp
    #endif

    #ifndef Sleep
        #define Sleep(duration) usleep((duration)*1000)
    #endif
#endif
