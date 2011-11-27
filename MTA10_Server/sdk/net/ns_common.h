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

#include "net/bitstream.h"
#include "net/packetenums.h"
#include "ns_playerid.h"

const NetServerPlayerID NET_INVALID_PLAYER_ID;
#define NET_INVALID_PLAYER_INDEX 255;

#define NET_MAX_PLAYER_COUNT        64
#define NET_MAX_VEHICLE_COUNT       250

#define NET_MAX_PLAYER_NAME_LENGTH  32

typedef struct __static_client_data_t
{
    char szPlayerName[NET_MAX_PLAYER_NAME_LENGTH + 1];
} static_client_data_t, *pstatic_client_data_t;

typedef struct __static_server_data_t
{
} static_server_data_t, *pstatic_server_data_t;

typedef bool (*PPACKETHANDLER) ( unsigned char, NetServerPlayerID&, NetBitStreamInterface*, SNetExtraInfo* );

enum NetServerPacketPriority
{
    PACKET_PRIORITY_HIGH = 0,
    PACKET_PRIORITY_MEDIUM,
    PACKET_PRIORITY_LOW,
    PACKET_PRIORITY_COUNT
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

enum NSPerSecondMetrics
{
	NS_USER_MESSAGE_BYTES_PUSHED,
	NS_USER_MESSAGE_BYTES_SENT,
	NS_USER_MESSAGE_BYTES_RESENT,
	NS_USER_MESSAGE_BYTES_RECEIVED_PROCESSED,
	NS_USER_MESSAGE_BYTES_RECEIVED_IGNORED,
	NS_ACTUAL_BYTES_SENT,
	NS_ACTUAL_BYTES_RECEIVED,
	NS_PERSECOND_METRICS_COUNT
};

struct NetStatistics
{
	unsigned long long valueOverLastSecond[NS_PERSECOND_METRICS_COUNT];
	unsigned long long runningTotal[NS_PERSECOND_METRICS_COUNT];
	
	time_t connectionStartTime;

    unsigned int packetsSent, packetsReceived;

	unsigned long long BPSLimitByCongestionControl;
	bool isLimitedByCongestionControl;

	unsigned long long BPSLimitByOutgoingBandwidthLimit;
	bool isLimitedByOutgoingBandwidthLimit;

	unsigned int messageInSendBuffer[PACKET_PRIORITY_COUNT];
	double bytesInSendBuffer[PACKET_PRIORITY_COUNT];

	unsigned int messagesInResendBuffer;
	unsigned long long bytesInResendBuffer;

	float packetlossLastSecond, packetlossTotal;
    float compressionRatio, decompressionRatio;

    int encryptionStatus;
};

struct SBandwidthStatistics
{
    long long llOutgoingUDPByteCount;
    long long llIncomingUDPByteCount;
    long long llOutgoingUDPPacketCount;
    long long llIncomingUDPPacketCount;
};

#endif

