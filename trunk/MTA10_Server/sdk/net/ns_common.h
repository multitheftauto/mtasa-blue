/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/ns_common.h
*  PURPOSE:     Net common defines
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __NS_COMMON_H
#define __NS_COMMON_H

#include "ns_bitstream.h"
#include "ns_packetenums.h"
#include "ns_playerid.h"

#define MTA_DM_NET_MODULE_VERSION 0x00003

const NetServerPlayerID NET_INVALID_PLAYER_ID;
#define NET_INVALID_PLAYER_INDEX 255;

#define NET_MAX_PLAYER_COUNT		64
#define NET_MAX_VEHICLE_COUNT		250

#define NET_MAX_PLAYER_NAME_LENGTH	32

typedef struct __static_client_data_t
{
	char szPlayerName[NET_MAX_PLAYER_NAME_LENGTH + 1];
} static_client_data_t, *pstatic_client_data_t;

typedef struct __static_server_data_t
{
} static_server_data_t, *pstatic_server_data_t;

typedef bool (*PPACKETHANDLER) ( unsigned char, NetServerPlayerID&, NetServerBitStreamInterface& );

enum NetServerPacketPriority
{
	PACKET_PRIORITY_HIGH = 0,
	PACKET_PRIORITY_MEDIUM,
	PACKET_PRIORITY_LOW
};

enum NetServerPacketReliability
{
	PACKET_RELIABILITY_UNRELIABLE = 0,
	PACKET_RELIABILITY_UNRELIABLE_SEQUENCED,
	PACKET_RELIABILITY_RELIABLE,
	PACKET_RELIABILITY_RELIABLE_ORDERED,
	PACKET_RELIABILITY_RELIABLE_SEQUENCED   //     Can drop packets
};

enum NetServerPacketOrdering
{
	PACKET_ORDERING_GAME = 0,
	PACKET_ORDERING_CHAT,
	PACKET_ORDERING_FILETRANSFER
};

#endif

