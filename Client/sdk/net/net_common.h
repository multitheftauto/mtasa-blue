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

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "net/bitstream.h"
#include "net/packetenums.h"

#define CNET_DOWNLOAD_INVALID       65535

#define NET_CLIENT_PORT             0 // 0 will allow it to automatically choose a port, otherwise, use the value specific
#define NET_DISCONNECT_DELAY        30

#define NET_INVALID_PACKET_ID       255

typedef bool (*PPACKETHANDLER)(std::uint8_t, NetBitStreamInterface&);

enum NetPacketPriority
{
    PACKET_PRIORITY_HIGH = 0,
    PACKET_PRIORITY_MEDIUM,
    PACKET_PRIORITY_LOW,
    PACKET_PRIORITY_COUNT
};

enum NetPacketReliability
{
    PACKET_RELIABILITY_UNRELIABLE = 0,            //  Can arrive out of order
    PACKET_RELIABILITY_UNRELIABLE_SEQUENCED,
    PACKET_RELIABILITY_RELIABLE,            //  Can arrive out of order
    PACKET_RELIABILITY_RELIABLE_ORDERED,
    PACKET_RELIABILITY_RELIABLE_SEQUENCED            //  Can drop packets
};

// Copy of raknet statistics
struct NetRawStatistics
{
    unsigned  messageSendBuffer[4];
    unsigned  messagesSent[4];
    std::int64_t messageDataBitsSent[4];
    std::int64_t messageTotalBitsSent[4];
    unsigned  packetsContainingOnlyAcknowlegements;
    unsigned  acknowlegementsSent;
    unsigned  acknowlegementsPending;
    std::int64_t acknowlegementBitsSent;
    unsigned  packetsContainingOnlyAcknowlegementsAndResends;
    unsigned  messageResends;
    std::int64_t messageDataBitsResent;
    std::int64_t messagesTotalBitsResent;
    unsigned  messagesOnResendQueue;
    unsigned  numberOfUnsplitMessages;
    unsigned  numberOfSplitMessages;
    unsigned  totalSplits;
    unsigned  packetsSent;
    std::int64_t encryptionBitsSent;
    std::int64_t totalBitsSent;
    unsigned  sequencedMessagesOutOfOrder;
    unsigned  sequencedMessagesInOrder;
    unsigned  orderedMessagesOutOfOrder;
    unsigned  orderedMessagesInOrder;
    unsigned  packetsReceived;
    unsigned  packetsWithBadCRCReceived;
    std::int64_t bitsReceived;
    std::int64_t bitsWithBadCRCReceived;
    unsigned  acknowlegementsReceived;
    unsigned  duplicateAcknowlegementsReceived;
    unsigned  messagesReceived;
    unsigned  invalidMessagesReceived;
    unsigned  duplicateMessagesReceived;
    unsigned  messagesWaitingForReassembly;
    unsigned  internalOutputQueueSize;
    double    bitsPerSecond;
    std::int64_t connectionStartTime;
    bool      bandwidthExceeded;
};

struct NetStatistics
{
    // Needed for getNetworkStats()
    std::uint64_t bytesReceived;
    std::uint64_t bytesSent;
    uint               packetsReceived;
    uint               packetsSent;
    float              packetlossTotal;
    float              packetlossLastSecond;
    uint               messagesInSendBuffer;
    uint               messagesInResendBuffer;
    bool               isLimitedByCongestionControl;
    bool               isLimitedByOutgoingBandwidthLimit;

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
