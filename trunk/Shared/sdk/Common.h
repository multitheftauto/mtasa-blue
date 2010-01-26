/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/Common.h
*  PURPOSE:     Header for common definitions
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Min and max number of characters in player serial
#define MIN_SERIAL_LENGTH 1
#define MAX_SERIAL_LENGTH 32

// Network disconnection reason (message) size
#define NET_DISCONNECT_REASON_SIZE  256

// Element IDs
#define RESERVED_ELEMENT_ID	0xFFFFFFFE
#define INVALID_ELEMENT_ID 0xFFFFFFFF

// Element name characteristics
#define MAX_TYPENAME_LENGTH 32
#define MAX_ELEMENT_NAME_LENGTH 64

// Allow 100k server elements, 100k client elements and 100k xml nodes
#define MAX_SERVER_ELEMENTS 100000
#define MAX_CLIENT_ELEMENTS 100000
#define MAX_XML             100000
typedef unsigned long ElementID;

// Event name characteristics
#define MAX_EVENT_NAME_LENGTH 512

// LAN packet broadcasting port
#define SERVER_LIST_BROADCAST_PORT          34219

// Server game to query port offset (gameport + offset = queryport)
#define SERVER_LIST_QUERY_PORT_OFFSET       123
#define SERVER_LIST_QUERY_PORT_OFFSET_LAN   234

// LAN packet broadcasting string
#define SERVER_LIST_CLIENT_BROADCAST_STR    "MTA-CLIENT"
#define SERVER_LIST_SERVER_BROADCAST_STR    "MTA-SERVER"

// Defines the min/max size for the player nick for use in the core module
#define MIN_PLAYER_NICK_LENGTH          1
#define MAX_PLAYER_NICK_LENGTH          22

// Windows Specific stuff
#ifdef WIN32
#define _DECLSPEC_EX extern "C" _declspec(dllexport) 
#else
#define _DECLSPEC_EX extern "C" 
#endif
