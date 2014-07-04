/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/net_common.h
*  PURPOSE:     Common network defines
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __NET_COMMON_H
#define __NET_COMMON_H

#ifdef WIN32
#include <windows.h>
#endif

#include "net/bitstream.h"
#include "net/packetenums.h"

#define CNET_DOWNLOAD_INVALID       65535

#define NET_CLIENT_PORT             0 // 0 will allow it to automatically choose a port, otherwise, use the value specific
#define NET_DISCONNECT_DELAY        30

#define NET_INVALID_PACKET_ID       255

typedef bool (*PPACKETHANDLER) ( unsigned char, NetBitStreamInterface& );

enum NetPacketPriority
{
    PACKET_PRIORITY_HIGH = 0,
    PACKET_PRIORITY_MEDIUM,
    PACKET_PRIORITY_LOW,
    PACKET_PRIORITY_COUNT
};

enum NetPacketReliability
{
    PACKET_RELIABILITY_UNRELIABLE = 0,          //  Can arrive out of order
    PACKET_RELIABILITY_UNRELIABLE_SEQUENCED,
    PACKET_RELIABILITY_RELIABLE,                //  Can arrive out of order
    PACKET_RELIABILITY_RELIABLE_ORDERED,
    PACKET_RELIABILITY_RELIABLE_SEQUENCED       //  Can drop packets
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
    int usingSlidingWindow;
};

enum ePacketOrdering
{
    PACKET_ORDERING_DEFAULT = 0,
    PACKET_ORDERING_CHAT,
    PACKET_ORDERING_FILETRANSFER,
    PACKET_ORDERING_LUA,
    PACKET_ORDERING_PURESYNC,
    PACKET_ORDERING_LIGHTSYNC,
    PACKET_ORDERING_OTHERSYNC
};

#endif
