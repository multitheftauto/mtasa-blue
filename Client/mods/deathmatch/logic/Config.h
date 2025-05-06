/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CScriptDebugging.cpp
 *  PURPOSE:     Script debugging
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// The almighty windows define and include
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>

// Type, function and normal defines
typedef int socklen_t;

#define MTA_OS_STRING "Windows"

/*** va_pass() (passing of ... variable length arguments ***/
template <BYTE count>
struct SVaPassNext
{
    SVaPassNext<count - 1> big;
    DWORD                  dw;
};
template <>
struct SVaPassNext<0>
{
};
// SVaPassNext - is generator of structure of any size at compile time.

class CVaPassNext
{
public:
    SVaPassNext<50> svapassnext;
    CVaPassNext(va_list& args)
    {
        try
        {            // to avoid access violation
            memcpy(&svapassnext, args, sizeof(svapassnext));
        }
        catch (...)
        {
        }
    }
};
#define va_pass(valist) CVaPassNext(valist).svapassnext
/*** va_pass() (passing of ... variable length arguments ***/

// Min and max number of characters in chat messages (the message itself, not including player nick)
#define MIN_CHAT_LENGTH 1
#define MAX_CHAT_LENGTH 255

// Min and max number of characters in a console command
#define MIN_COMMAND_LENGTH 1
#define MAX_COMMAND_LENGTH 255

// Min and max number of characters in chat echos
#define MIN_CHATECHO_LENGTH 1
#define MAX_CHATECHO_LENGTH (MAX_CHAT_LENGTH + MAX_PLAYER_NICK_LENGTH + 2) // +2 is for ": " between player nick and the message

// Min and max number of characters in outputChatBox from the server
#define MIN_OUTPUTCHATBOX_LENGTH 1
#define MAX_OUTPUTCHATBOX_LENGTH 256

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

// Couple of defines to ensure proper configuration
#if MAX_CHAT_LENGTH > 255
    #error MAX_CHAT_LENGTH "macro can't exceed 255"
#endif

// Max valid weather id
#define MAX_VALID_WEATHER 255

// Upper player limit
#define MAX_PLAYER_COUNT 4096
#if MAX_PLAYER_COUNT > 65535
    #error MAX_PLAYER_COUNT "macro can't exceed 65535"
#endif

// Max mapname length
#define MAX_MAPNAME_LENGTH 255

// Timeouts
#define NET_CONNECT_TIMEOUT 30000
#define CLIENT_VERIFICATION_TIMEOUT 10000

// Vehicle in-out delay (to prevent messed up like 1765/1956/1880
#define VEHICLE_INOUT_DELAY 1500
