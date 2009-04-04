/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/net/net_common.h
*  PURPOSE:		Common network defines
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __NET_COMMON_H
#define __NET_COMMON_H

#ifdef WIN32
#include <windows.h>
#endif

#include "net_bitstream.h"
#include "net_packetenums.h"

#define MTA_DM_NET_MODULE_VERSION   0x00004

#define CNET_DOWNLOAD_INVALID		65535

#define NET_CLIENT_PORT				0 // 0 will allow it to automatically choose a port, otherwise, use the value specific
#define NET_DISCONNECT_DELAY		30

#define NET_INVALID_PACKET_ID		255

typedef bool (*PPACKETHANDLER) ( unsigned char, NetBitStreamInterface& );

enum NetPacketPriority
{
	PACKET_PRIORITY_HIGH = 0,
	PACKET_PRIORITY_MEDIUM,
	PACKET_PRIORITY_LOW
};

enum NetPacketReliability
{
	PACKET_RELIABILITY_UNRELIABLE = 0,
	PACKET_RELIABILITY_UNRELIABLE_SEQUENCED,
	PACKET_RELIABILITY_RELIABLE,
	PACKET_RELIABILITY_RELIABLE_ORDERED,
	PACKET_RELIABILITY_RELIABLE_SEQUENCED   //     Can drop packets
};

enum NetPacketOrdering
{
	PACKET_ORDERING_GAME = 0,
	PACKET_ORDERING_CHAT,
	PACKET_ORDERING_FILETRANSFER
};

#endif
