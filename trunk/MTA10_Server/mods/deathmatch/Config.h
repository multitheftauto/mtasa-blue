/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/Config.h
*  PURPOSE:     Header for platform specific functions, types and defines
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Header file for platform specific functions, types and defines

#ifndef __CONFIG_H
#define __CONFIG_H

#include <Platform.h>
#include <Common.h>

/*** va_pass() (passing of ... variable length arguments ***/
template<unsigned char count>
struct SVaPassNext{
    SVaPassNext<count-1> big;
    unsigned long dw;
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

// Version stuff
#define MTA_DM_VERSION 0x0100
#define MTA_DM_VERSIONSTRING "1.0"
#define MTA_DM_FULL_STRING "MTA:SA Server"
#define MTA_DM_NETCODE_VERSION 0x012B

// Min and max number of characters in player nicknames (this must match the client's)
#define MIN_NICK_LENGTH 1
#define MAX_NICK_LENGTH 22

// Min and max number of characters in passwords
#define MIN_PASSWORD_LENGTH 0
#define MAX_PASSWORD_LENGTH 30

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
#define MAX_PLAYER_COUNT 128
#if MAX_PLAYER_COUNT > 254
	#error MAX_PLAYER_COUNT "macro can't exceed 254"
#endif

// Banlist filename
#define FILENAME_BANLIST	"banlist.xml"

// Settings filename
#define FILENAME_SETTINGS	"settings.xml"

// Settings root-node
#define ROOTNODE_SETTINGS	"settings"

// Settings prefixes and delimiters
#define SETTINGS_PREFIX_PRIVATE		'@'						// Private prefix
#define SETTINGS_PREFIX_PROTECTED	'#'						// Protected prefix
#define SETTINGS_PREFIX_PUBLIC		'*'						// Public prefix
#define SETTINGS_NO_PREFIX			CSettings::Private		// Access type when no prefix is used
#define SETTINGS_DELIMITER			'.'						// Delimiter

// Maximum length for names and values in settings registry
#define MAX_SETTINGS_LENGTH			256

// Maximum resource length
#define MAX_RESOURCE_LENGTH			64

// SQL variable placeholder (replaces with actual variable content)
#define	SQL_VARIABLE_PLACEHOLDER	'?'

// Security script path
#define LUA_SECURITY_SCRIPT			"security.lua"

// Server FPS limit (in milliseconds)
#define FPS_LIMIT					16

// Default MTU size
#define MTU_SIZE_DEFAULT			1264

// Max garages
#define MAX_GARAGES                 50

// Game-monitor.com query URL (use %u for port input)
#define QUERY_URL_GAME_MONITOR		"http://master.game-monitor.com/heartbeat.php?p=%u&e=3"

#endif
