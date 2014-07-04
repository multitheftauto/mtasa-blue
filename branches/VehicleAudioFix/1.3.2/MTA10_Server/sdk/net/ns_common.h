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

typedef bool (*PPACKETHANDLER) ( unsigned char, const NetServerPlayerID&, NetBitStreamInterface*, SNetExtraInfo* );

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

struct SBandwidthStatistics
{
    long long llOutgoingUDPByteCount;
    long long llIncomingUDPByteCount;
    long long llIncomingUDPByteCountBlocked;
    long long llOutgoingUDPPacketCount;
    long long llIncomingUDPPacketCount;
    long long llIncomingUDPPacketCountBlocked;
    long long llOutgoingUDPByteResentCount;
    long long llOutgoingUDPMessageResentCount;
    uint uiNetworkUpdateLoopProcessorNumber;
};

struct SNetPerformanceStatistics
{
    uint uiUpdateCyclePrepTimeAvgUs;
    uint uiUpdateCycleProcessTimeAvgUs;
    uint uiUpdateCyclePrepTimeMaxUs;
    uint uiUpdateCycleProcessTimeMaxUs;
    float fUpdateCycleDatagramsAvg;
    uint uiUpdateCycleDatagramsMax;
    uint uiUpdateCycleDatagramsLimit;
    uint uiUpdateCycleSendsLimitedTotal;
    float fUpdateCycleSendsLimitedPercent;
};

enum ePacketOrdering
{
    PACKET_ORDERING_DEFAULT = 0,
    PACKET_ORDERING_CHAT,
    PACKET_ORDERING_DATA_TRANSFER,
    PACKET_ORDERING_VOICE,
};

// Typedefs to make shared code easier
typedef NetServerPlayerID NetPlayerID;
typedef NetServerPacketPriority NetPacketPriority;
typedef NetServerPacketReliability NetPacketReliability;

#endif
