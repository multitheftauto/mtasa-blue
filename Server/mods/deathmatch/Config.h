/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/Config.h
 *  PURPOSE:     Header for platform specific functions, types and defines
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// Header file for platform specific functions, types and defines

#pragma once

#include <MTAPlatform.h>
#include <Common.h>

// Min and max number of characters in player nicknames (this must match the client's)
#define MIN_PLAYER_NICK_LENGTH 1
#define MAX_PLAYER_NICK_LENGTH 22

// Min and max number of characters in player nametags (this must match the client's)
#define MIN_PLAYER_NAMETAG_LENGTH 1
#define MAX_PLAYER_NAMETAG_LENGTH 64

#define MAX_TEAM_NAME_LENGTH 255

// Min number of characters in passwords
#define MIN_PASSWORD_LENGTH 1

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

// Max kick string length that can be sent
#define MIN_KICK_REASON_LENGTH      1
#define MAX_KICK_REASON_LENGTH      128
#define MAX_KICK_RESPONSIBLE_LENGTH 30

// Max ban string length that can be sent
#define MIN_BAN_REASON_LENGTH      1
#define MAX_BAN_REASON_LENGTH      128
#define MAX_BAN_RESPONSIBLE_LENGTH 30

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

// Game Type
#define GAME_TYPE_STRING "MTA:SA"

// Banlist filename
#define FILENAME_BANLIST    "banlist.xml"

// Settings filename
#define FILENAME_SETTINGS   "settings.xml"

// Settings root-node
#define ROOTNODE_SETTINGS   "settings"

// Settings prefixes and delimiters
#define SETTINGS_PREFIX_PRIVATE     '@'                     // Private prefix
#define SETTINGS_PREFIX_PROTECTED   '#'                     // Protected prefix
#define SETTINGS_PREFIX_PUBLIC      '*'                     // Public prefix
#define SETTINGS_NO_PREFIX          CSettings::Private      // Access type when no prefix is used
#define SETTINGS_DELIMITER          '.'                     // Delimiter

// Maximum length for names and values in settings registry
#define MAX_SETTINGS_LENGTH         256

// Maximum resource length
#define MAX_RESOURCE_LENGTH         64

// SQL variable placeholder (replaces with actual variable content)
#define SQL_VARIABLE_PLACEHOLDER    '?'

// Server FPS limit (in milliseconds)
#define FPS_LIMIT                   16

// Max garages
#define MAX_GARAGES                 50

// MTA master server query URL (Inputs: game port, ase port, http port, version, extra, serverip)
#define QUERY_URL_MTA_MASTER_SERVER "https://master.mtasa.com/ase/add.php?g=%GAME%&a=%ASE%&h=%HTTP%&v=%VER%&x=%EXTRA%&ip=%IP%"

// MTA port tester URL
#define PORT_TESTER_URL             "https://nightly.mtasa.com/ports/"

// MTA minclientversion auto update and others
#define HQCOMMS_URL                 "https://updatesa.multitheftauto.com/sa/server/hqcomms/"
