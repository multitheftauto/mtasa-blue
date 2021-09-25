/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/Platform.cpp
 *  PURPOSE:     Platform-specific defines and methods
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

extern "C" bool g_bSilent;
extern "C" bool g_bNoTopBar;

/** Operating system identifiers **/
#if defined(WIN32)
    #ifdef _WIN64
        #define MTA_OS_STRING   "Windows x64"
    #else
        #define MTA_OS_STRING   "Windows"
    #endif
    #define MTA_LIB_EXTENSION   ".dll"
#elif defined(__linux__)
    #ifdef __x86_64__
        #define MTA_OS_STRING   "GNU/Linux x64"
    #else
        #define MTA_OS_STRING   "GNU/Linux"
    #endif
    #define MTA_LIB_EXTENSION   ".so"
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    #define MTA_OS_STRING       "BSD"
    #define MTA_LIB_EXTENSION   ".so"
#elif defined(__APPLE__)
    #define MTA_OS_STRING       "macOS"
    #define MTA_LIB_EXTENSION   ".dylib"
#else
    #error "Unsupported operating system"
#endif

#if defined(MTA_DEBUG)
    #define MTA_LIB_SUFFIX  "_d"
#else
    #define MTA_LIB_SUFFIX
#endif

/** Multi-platform defines **/
#ifdef WIN32
/* Win32 */

// Define includes
    #include <conio.h>
    #include <direct.h>
    #include <windows.h>

// Define types
typedef int socklen_t;

// Define keys
    #define KEY_BACKSPACE   0x08
    #define KEY_EXTENDED    0xE0
    #define KEY_LEFT    0x4B
    #define KEY_RIGHT   0x4D
    #define KEY_UP      0x48
    #define KEY_DOWN    0x50
#else
/* POSIX */

// Define includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <string>
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
        #define Sleep(duration) usleep((duration) * 1000)
    #endif

#endif
