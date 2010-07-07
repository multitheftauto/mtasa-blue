/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptDebugging.cpp
*  PURPOSE:     Script debugging
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H
#ifdef WIN32
    /* Win32 platforms */

    // The almighty windows define and include
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <cstdio>

    // Type, function and normal defines
    #define socklen_t int

    #ifndef snprintf
    #define snprintf _snprintf
    #endif

    #ifndef vsnprintf
    #define vsnprintf _vsnprintf
    #endif

    #define MTA_OS_STRING "Windows"
#else
    /* POSIX-compatible */

    // Types and std
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <string.h>

    // Curses
    #include <curses.h>

    // Net and error includes
    #include <arpa/inet.h>
    #include <errno.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/time.h>
    #include <sys/timeb.h>
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <time.h>
    #include <fcntl.h>
    #include <dirent.h>

    // Type, function and normal defines
    #define MAX_PATH 255

    // Function defines
    #define Sleep(duration) usleep(duration * 1000)

    #ifndef stricmp
    #define stricmp(x, y) strcasecmp(x, y)
    #endif

    #ifndef strnicmp
    #define strnicmp(x, y, z) strncasecmp(x, y, z)
    #endif

    #ifdef LINUX
        #define MTA_OS_STRING "Linux"
    #endif

    #ifdef FREEBSD
        #define MTA_OS_STRING "FreeBSD"
    #endif

    #ifdef OPENBSD
        #define MTA_OS_STRING "OpenBSD"
    #endif

    // define GetProcAddress(a,b) as dlsym(a,b)
    #define GetProcAddress dlsym
#endif

/*** va_pass() (passing of ... variable length arguments ***/
template<BYTE count>
struct SVaPassNext{
    SVaPassNext<count-1> big;
    DWORD dw;
};
template<> struct SVaPassNext<0>{};
//SVaPassNext - is generator of structure of any size at compile time.

class CVaPassNext{
public:
    SVaPassNext<50> svapassnext;
    CVaPassNext(va_list & args){
        try{//to avoid access violation
            memcpy(&svapassnext, args, sizeof(svapassnext));
        } catch (...) {}
    }
};
#define va_pass(valist) CVaPassNext(valist).svapassnext
/*** va_pass() (passing of ... variable length arguments ***/

// Min and max number of characters in chat messages
#define MIN_CHAT_LENGTH 1
#define MAX_CHAT_LENGTH 96

// Min and max number of characters in chat echos
#define MIN_CHATECHO_LENGTH 1
#define MAX_CHATECHO_LENGTH 128

// Min and max number of characters in debug echos
#define MIN_DEBUGECHO_LENGTH 1
#define MAX_DEBUGECHO_LENGTH 256

// Min and max number of characters in console echos
#define MIN_CONSOLEECHO_LENGTH 1
#define MAX_CONSOLEECHO_LENGTH 1000

// Max servermessage size (this must match the client's)
#define MIN_SERVERMESSAGE_LENGTH 1
#define MAX_SERVERMESSAGE_LENGTH 64

// Max disconnect reason length that can be sent with the "you were disconnected" packet
#define MIN_DISCONNECT_REASON_LENGTH 1
#define MAX_DISCONNECT_REASON_LENGTH 127

// Max kick string length that can be sent
#define MIN_KICK_REASON_LENGTH 1
#define MAX_KICK_REASON_LENGTH 64

// Max ban string length that can be sent
#define MIN_BAN_REASON_LENGTH 1
#define MAX_BAN_REASON_LENGTH 64


// Couple of defines to ensure proper configuration
#if MAX_CHAT_LENGTH > 255
    #error MAX_CHAT_LENGTH "macro can't exceed 255"
#endif

// Defines how long the whowas list can be
#define MAX_WHOWAS_LENGTH 1024

// Max valid weather id
#define MAX_VALID_WEATHER 255

// Upper player limit
#define MAX_PLAYER_COUNT 250
#if MAX_PLAYER_COUNT > 254
    #error MAX_PLAYER_COUNT "macro can't exceed 254"
#endif

// Max mapname length
#define MAX_MAPNAME_LENGTH 255

// Timeouts
#define NET_CONNECT_TIMEOUT 30000
#define CLIENT_VERIFICATION_TIMEOUT 10000

// Vehicle in-out delay (to prevent messed up like 1765/1956/1880
#define VEHICLE_INOUT_DELAY 1500

#endif
