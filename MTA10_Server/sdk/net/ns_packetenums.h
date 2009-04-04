/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/ns_packetenums.h
*  PURPOSE:     Net packet enumerations
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __NET_PACKETENUMS_H
#define __NET_PACKETENUMS_H

enum
{
	PACKET_ID_SERVER_JOIN = 0,
	PACKET_ID_SERVER_JOIN_DATA,
	PACKET_ID_SERVER_JOIN_COMPLETE,

	PACKET_ID_PLAYER_JOIN,
	PACKET_ID_PLAYER_JOINDATA,
	PACKET_ID_PLAYER_QUIT,
    PACKET_ID_PLAYER_TIMEOUT,

	PACKET_ID_MOD_NAME,
	PACKET_ID_MTA_RESERVED_02,
	PACKET_ID_MTA_RESERVED_03,
	PACKET_ID_MTA_RESERVED_04,
	PACKET_ID_MTA_RESERVED_05,
	PACKET_ID_MTA_RESERVED_06,
	PACKET_ID_MTA_RESERVED_07,
	PACKET_ID_MTA_RESERVED_08,
	PACKET_ID_MTA_RESERVED_09,
	PACKET_ID_MTA_RESERVED_10,
	PACKET_ID_MTA_RESERVED_11,
	PACKET_ID_MTA_RESERVED_12,
	PACKET_ID_MTA_RESERVED_13,
	PACKET_ID_MTA_RESERVED_14,
	PACKET_ID_MTA_RESERVED_15,

	PACKET_ID_END_OF_INTERNAL_PACKETS
};

#endif
