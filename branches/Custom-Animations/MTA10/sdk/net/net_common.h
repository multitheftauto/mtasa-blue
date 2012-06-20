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

// Copy of raknet statistics
struct NetRawStatistics
{
    unsigned messageSendBuffer[ 4 ];
    unsigned messagesSent[ 4 ];
    long long messageDataBitsSent[ 4 ];
    long long messageTotalBitsSent[ 4 ];
    unsigned packetsContainingOnlyAcknowlegements;
    unsigned acknowlegementsSent;
    unsigned acknowlegementsPending;
    long long acknowlegementBitsSent;
    unsigned packetsContainingOnlyAcknowlegementsAndResends;
    unsigned messageResends;
    long long messageDataBitsResent;
    long long messagesTotalBitsResent;
    unsigned messagesOnResendQueue;
    unsigned numberOfUnsplitMessages;
    unsigned numberOfSplitMessages;
    unsigned totalSplits;
    unsigned packetsSent;
    long long encryptionBitsSent;
    long long totalBitsSent;
    unsigned sequencedMessagesOutOfOrder;
    unsigned sequencedMessagesInOrder;
    unsigned orderedMessagesOutOfOrder;
    unsigned orderedMessagesInOrder;
    unsigned packetsReceived;
    unsigned packetsWithBadCRCReceived;
    long long bitsReceived;
    long long bitsWithBadCRCReceived;
    unsigned acknowlegementsReceived;
    unsigned duplicateAcknowlegementsReceived;
    unsigned messagesReceived;
    unsigned invalidMessagesReceived;
    unsigned duplicateMessagesReceived;
    unsigned messagesWaitingForReassembly;
    unsigned internalOutputQueueSize;
    double bitsPerSecond;
    long long connectionStartTime;
    bool bandwidthExceeded;
};

struct NetStatistics
{
    // Needed for getNetworkStats()
    unsigned long long  bytesReceived;
    unsigned long long  bytesSent;
    uint    packetsReceived;
    uint    packetsSent;
    float   packetlossTotal;
    float   packetlossLastSecond;
    uint    messagesInSendBuffer;
    uint    messagesInResendBuffer;
    bool    isLimitedByCongestionControl;
    bool    isLimitedByOutgoingBandwidthLimit;
    int     encryptionStatus;

    // Copy of raknet statistics
    NetRawStatistics raw;
};

enum ePacketOrdering
{
    PACKET_ORDERING_DEFAULT = 0,
    PACKET_ORDERING_CHAT,
    PACKET_ORDERING_DATA_TRANSFER,
    PACKET_ORDERING_VOICE,
};

// Typedefs to make shared code easier
typedef int NetPlayerID;

#endif
