/// \file
/// \brief \b [Internal] Datagram reliable, ordered, unordered and sequenced sends.  Flow control.  Message splitting, reassembly, and coalescence.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __RELIABILITY_LAYER_H
#define __RELIABILITY_LAYER_H

#include "RakMemoryOverride.h"
#include "MTUSize.h"
#include "DS_LinkedList.h"
#include "DS_List.h"
#include "SocketLayer.h"
#include "PacketPriority.h"
#include "DS_Queue.h"
#include "BitStream.h"
#include "InternalPacket.h"
#include "DataBlockEncryptor.h"
#include "RakNetStatistics.h"
#include "SHA1.h"
#include "DS_OrderedList.h"
#include "DS_RangeList.h"
#include "DS_BPlusTree.h"
#include "DS_MemoryPool.h"

class PluginInterface2;
class RakNetRandom;

/// Sizeof an UDP header in byte
#define UDP_HEADER_SIZE 28

/// Number of ordered streams available. You can use up to 32 ordered streams
#define NUMBER_OF_ORDERED_STREAMS 32 // 2^5

#define RESEND_TREE_ORDER 32

#define ACK_PING_SAMPLES_SIZE 256
#define ACK_PING_SAMPLES_BITS 8

#include "BitStream.h"

int SplitPacketIndexComp( SplitPacketIndexType const &key, InternalPacket* const &data );
struct SplitPacketChannel//<SplitPacketChannel>
{
	RakNetTimeUS lastUpdateTime;
	DataStructures::OrderedList<SplitPacketIndexType, InternalPacket*, SplitPacketIndexComp> splitPacketList;
};
int RAK_DLL_EXPORT SplitPacketChannelComp( SplitPacketIdType const &key, SplitPacketChannel* const &data );

/// Datagram reliable, ordered, unordered and sequenced sends.  Flow control.  Message splitting, reassembly, and coalescence.
class ReliabilityLayer//<ReliabilityLayer>
{
public:

	/// Constructor
	ReliabilityLayer();

	/// Destructor
	~ReliabilityLayer();

	/// Resets the layer for reuse
	void Reset( bool resetVariables );

	///Sets the encryption key.  Doing so will activate secure connections
	/// \param[in] key Byte stream for the encryption key
	void SetEncryptionKey( const unsigned char *key );

	/// Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable packet
	/// Default time is 10,000 or 10 seconds in release and 30,000 or 30 seconds in debug.
	/// \param[in] time Time, in MS
	void SetTimeoutTime( RakNetTime time );

	/// Returns the value passed to SetTimeoutTime. or the default if it was never called
	/// \param[out] the value passed to SetTimeoutTime
	RakNetTime GetTimeoutTime(void);

	/// Packets are read directly from the socket layer and skip the reliability layer because unconnected players do not use the reliability layer
	/// This function takes packet data after a player has been confirmed as connected.
	/// \param[in] buffer The socket data
	/// \param[in] length The length of the socket data
	/// \param[in] systemAddress The player that this data is from
	/// \param[in] messageHandlerList A list of registered plugins
	/// \param[in] MTUSize maximum datagram size
	/// \retval true Success
	/// \retval false Modified packet
	bool HandleSocketReceiveFromConnectedPlayer( const char *buffer, unsigned int length, SystemAddress systemAddress, DataStructures::List<PluginInterface2*> &messageHandlerList, int MTUSize );

	/// This allocates bytes and writes a user-level message to those bytes.
	/// \param[out] data The message
	/// \return Returns number of BITS put into the buffer
	BitSize_t Receive( unsigned char**data );

	/// Puts data on the send queue
	/// \param[in] data The data to send
	/// \param[in] numberOfBitsToSend The length of \a data in bits
	/// \param[in] priority The priority level for the send
	/// \param[in] reliability The reliability type for the send
	/// \param[in] orderingChannel 0 to 31.  Specifies what channel to use, for relational ordering and sequencing of packets.
	/// \param[in] makeDataCopy If true \a data will be copied.  Otherwise, only a pointer will be stored.
	/// \param[in] MTUSize maximum datagram size
	/// \param[in] currentTime Current time, as per RakNet::GetTime()
	/// \return True or false for success or failure.
	bool Send( char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, unsigned char orderingChannel, bool makeDataCopy, int MTUSize, RakNetTimeUS currentTime );

	/// Call once per game cycle.  Handles internal lists and actually does the send.
	/// \param[in] s the communication  end point
	/// \param[in] systemAddress The Unique Player Identifier who shouldhave sent some packets
	/// \param[in] MTUSize maximum datagram size
	/// \param[in] time current system time
	/// \param[in] maxBitsPerSecond if non-zero, enforces that outgoing bandwidth does not exceed this amount
	/// \param[in] messageHandlerList A list of registered plugins
	void Update(  SOCKET s, SystemAddress systemAddress, int MTUSize, RakNetTimeUS time, unsigned maxBitsPerSecond, DataStructures::List<PluginInterface2*> &messageHandlerList, RakNetRandom *rnr, bool isPS3LobbySocket );

	/// If Read returns -1 and this returns true then a modified packetwas detected
	/// \return true when a modified packet is detected
	bool IsCheater( void ) const;

	/// Were you ever unable to deliver a packet despite retries?
	/// \return true means the connection has been lost.  Otherwise not.
	bool IsDeadConnection( void ) const;

	/// Causes IsDeadConnection to return true
	void KillConnection(void);

	/// Get Statistics
	/// \return A pointer to a static struct, filled out with current statistical information.
	RakNetStatistics * const GetStatistics( RakNetStatistics *rns );

	///Are we waiting for any data to be sent out or be processed by the player?
	bool IsOutgoingDataWaiting(void);
	bool IsReliableOutgoingDataWaiting(void);
	bool AreAcksWaiting(void);

	// Set outgoing lag and packet loss properties
	void ApplyNetworkSimulator( double _maxSendBPS, RakNetTime _minExtraPing, RakNetTime _extraPingVariance );

	/// Returns if you previously called ApplyNetworkSimulator
	/// \return If you previously called ApplyNetworkSimulator
	bool IsNetworkSimulatorActive( void );

	void SetSplitMessageProgressInterval(int interval);
	void SetUnreliableTimeout(RakNetTime timeoutMS);
	/// Has a lot of time passed since the last ack
	bool AckTimeout(RakNetTimeUS curTime);
	RakNetTimeUS GetNextSendTime(void) const;
	RakNetTimeUS GetTimeBetweenPackets(void) const;
	RakNetTimeUS GetLastTimeBetweenPacketsDecrease(void) const;
	RakNetTimeUS GetLastTimeBetweenPacketsIncrease(void) const;
	RakNetTimeUS GetAckPing(void) const;

	// If true, will update time between packets quickly based on ping calculations
	//void SetDoFastThroughputReactions(bool fast);

	// Encoded as numMessages[unsigned int], message1BitLength[unsigned int], message1Data (aligned), ...
	//void GetUndeliveredMessages(RakNet::BitStream *messages, int MTUSize);

private:

	/// Generates a datagram (coalesced packets)
	/// \param[out] output The resulting BitStream
	/// \param[in] Current MTU size
	/// \param[out] reliableDataSent Set to true or false as a return value as to if reliable data was sent.
	/// \param[in] time Current time
	/// \param[in] systemAddress Who we are sending to
	/// \param[in] messageHandlerList A list of registered plugins
	/// \return If any data was sent
	bool GenerateDatagram( RakNet::BitStream *output, int MTUSize, bool *reliableDataSent, RakNetTimeUS time, SystemAddress systemAddress, bool *hitMTUCap, DataStructures::List<PluginInterface2*> &messageHandlerList );

	/// Send the contents of a bitstream to the socket
	/// \param[in] s The socket used for sending data
	/// \param[in] systemAddress The address and port to send to
	/// \param[in] bitStream The data to send.
	void SendBitStream( SOCKET s, SystemAddress systemAddress, RakNet::BitStream *bitStream, RakNetRandom *rnr, bool isPS3LobbySocket);

	///Parse an internalPacket and create a bitstream to represent this data
	/// \return Returns number of bits used
	BitSize_t WriteToBitStreamFromInternalPacket( RakNet::BitStream *bitStream, const InternalPacket *const internalPacket, RakNetTimeUS curTime );

	/// Parse a bitstream and create an internal packet to represent this data
	InternalPacket* CreateInternalPacketFromBitStream( RakNet::BitStream *bitStream, RakNetTimeUS time );

	/// Does what the function name says
	unsigned RemovePacketFromResendListAndDeleteOlderReliableSequenced( const MessageNumberType messageNumber, RakNetTimeUS time );

	/// Acknowledge receipt of the packet with the specified messageNumber
	void SendAcknowledgementPacket( const MessageNumberType messageNumber, RakNetTimeUS time );

	/// This will return true if we should not send at this time
	bool IsSendThrottled( int MTUSize );

	/// We lost a packet
	void UpdateWindowFromPacketloss( RakNetTimeUS time );

	/// Increase the window size
	void UpdateWindowFromAck( RakNetTimeUS time );

	/// Parse an internalPacket and figure out how many header bits would be written.  Returns that number
	int GetBitStreamHeaderLength( const InternalPacket *const internalPacket );

	/// Get the SHA1 code
	void GetSHA1( unsigned char * const buffer, unsigned int nbytes, char code[ SHA1_LENGTH ] );

	/// Check the SHA1 code
	bool CheckSHA1( char code[ SHA1_LENGTH ], unsigned char * const buffer, unsigned int nbytes );

	/// Search the specified list for sequenced packets on the specified ordering channel, optionally skipping those with splitPacketId, and delete them
	void DeleteSequencedPacketsInList( unsigned char orderingChannel, DataStructures::List<InternalPacket*>&theList, int splitPacketId = -1 );

	/// Search the specified list for sequenced packets with a value less than orderingIndex and delete them
	void DeleteSequencedPacketsInList( unsigned char orderingChannel, DataStructures::Queue<InternalPacket*>&theList );

	/// Returns true if newPacketOrderingIndex is older than the waitingForPacketOrderingIndex
	bool IsOlderOrderedPacket( OrderingIndexType newPacketOrderingIndex, OrderingIndexType waitingForPacketOrderingIndex );

	/// Split the passed packet into chunks under MTU_SIZE bytes (including headers) and save those new chunks
	void SplitPacket( InternalPacket *internalPacket, int MTUSize );

	/// Insert a packet into the split packet list
	void InsertIntoSplitPacketList( InternalPacket * internalPacket, RakNetTimeUS time );

	/// Take all split chunks with the specified splitPacketId and try to reconstruct a packet. If we can, allocate and return it.  Otherwise return 0
	InternalPacket * BuildPacketFromSplitPacketList( SplitPacketIdType splitPacketId, RakNetTimeUS time );

	/// Delete any unreliable split packets that have long since expired
	void DeleteOldUnreliableSplitPackets( RakNetTimeUS time );

	/// Creates a copy of the specified internal packet with data copied from the original starting at dataByteOffset for dataByteLength bytes.
	/// Does not copy any split data parameters as that information is always generated does not have any reason to be copied
	InternalPacket * CreateInternalPacketCopy( InternalPacket *original, int dataByteOffset, int dataByteLength, RakNetTimeUS time );

	/// Get the specified ordering list
	DataStructures::LinkedList<InternalPacket*> *GetOrderingListAtOrderingStream( unsigned char orderingChannel );

	/// Add the internal packet to the ordering list in order based on order index
	void AddToOrderingList( InternalPacket * internalPacket );

	/// Inserts a packet into the resend list in order
	void InsertPacketIntoResendList( InternalPacket *internalPacket, RakNetTimeUS time, bool makeCopyOfInternalPacket, bool firstResend );

	/// Memory handling
	void FreeMemory( bool freeAllImmediately );

	/// Memory handling
	void FreeThreadedMemory( void );

	/// Memory handling
	void FreeThreadSafeMemory( void );

	// Initialize the variables
	void InitializeVariables( void );

	/// Given the current time, is this time so old that we should consider it a timeout?
	bool IsExpiredTime(unsigned int input, RakNetTimeUS currentTime) const;

	// Make it so we don't do resends within a minimum threshold of time
	void UpdateNextActionTime(void);

	/// Does this packet number represent a packet that was skipped (out of order?)
	//unsigned int IsReceivedPacketHole(unsigned int input, RakNetTime currentTime) const;

	/// Skip an element in the received packets list
	//unsigned int MakeReceivedPacketHole(unsigned int input) const;

	/// How many elements are waiting to be resent?
	unsigned int GetResendListDataSize(void) const;

	/// Update all memory which is not threadsafe
	void UpdateThreadedMemory(void);

	void CalculateHistogramAckSize(void);

	// Used ONLY for RELIABLE_ORDERED
	// RELIABLE_SEQUENCED just returns the newest one
	DataStructures::List<DataStructures::LinkedList<InternalPacket*>*> orderingList;
	DataStructures::Queue<InternalPacket*> outputQueue;
	DataStructures::RangeList<MessageNumberType> acknowlegements;
	int splitMessageProgressInterval;
	RakNetTimeUS unreliableTimeout;
	
	// Resend list is a tree of packets we need to resend
	DataStructures::BPlusTree<MessageNumberType, InternalPacket*, RESEND_TREE_ORDER> resendList;
	// resend Queue holds the same pointers, but in order of when to send them.  nextActionTime is set to 0 when the packet is no longer needed.
	DataStructures::Queue<InternalPacket*> resendQueue;
	
	DataStructures::Queue<InternalPacket*> sendPacketSet[ NUMBER_OF_PRIORITIES ];
    DataStructures::OrderedList<SplitPacketIdType, SplitPacketChannel*, SplitPacketChannelComp> splitPacketChannelList;
	MessageNumberType sendMessageNumberIndex, internalOrderIndex;
	//unsigned int windowSize;
	RakNetTimeUS lastAckTime;
	RakNet::BitStream updateBitStream;
	OrderingIndexType waitingForOrderedPacketWriteIndex[ NUMBER_OF_ORDERED_STREAMS ], waitingForSequencedPacketWriteIndex[ NUMBER_OF_ORDERED_STREAMS ];
	
	// STUFF TO NOT MUTEX HERE (called from non-conflicting threads, or value is not important)
	OrderingIndexType waitingForOrderedPacketReadIndex[ NUMBER_OF_ORDERED_STREAMS ], waitingForSequencedPacketReadIndex[ NUMBER_OF_ORDERED_STREAMS ];
	bool deadConnection, cheater;
	// unsigned int lastPacketSendTime,retransmittedFrames, sentPackets, sentFrames, receivedPacketsCount, bytesSent, bytesReceived,lastPacketReceivedTime;
	SplitPacketIdType splitPacketId;
	RakNetTime timeoutTime; // How long to wait in MS before timing someone out
	//int MAX_AVERAGE_PACKETS_PER_SECOND; // Name says it all
//	int RECEIVED_PACKET_LOG_LENGTH, requestedReceivedPacketLogLength; // How big the receivedPackets array is
//	unsigned int *receivedPackets;
	unsigned int blockWindowIncreaseUntilTime;
	RakNetStatistics statistics;

	RakNetTimeUS histogramStart;
	unsigned histogramBitsSent;


	/// Memory-efficient receivedPackets algorithm:
	/// receivedPacketsBaseIndex is the packet number we are expecting
	/// Everything under receivedPacketsBaseIndex is a packet we already got
	/// Everything over receivedPacketsBaseIndex is stored in hasReceivedPacketQueue
	/// It stores the time to stop waiting for a particular packet number, where the packet number is receivedPacketsBaseIndex + the index into the queue
	/// If 0, we got got that packet.  Otherwise, the time to give up waiting for that packet.
	/// If we get a packet number where (receivedPacketsBaseIndex-packetNumber) is less than half the range of receivedPacketsBaseIndex then it is a duplicate
	/// Otherwise, it is a duplicate packet (and ignore it).
	DataStructures::Queue<RakNetTimeUS> hasReceivedPacketQueue;
	MessageNumberType receivedPacketsBaseIndex;
	bool resetReceivedPackets;

	RakNetTimeUS lastUpdateTime;
	RakNetTimeUS timeBetweenPackets, nextSendTime, ackPing;
	RakNetTimeUS ackPingSamples[ACK_PING_SAMPLES_SIZE]; // Must be range of unsigned char to wrap ackPingIndex properly
	RakNetTimeUS ackPingSum;
	unsigned char ackPingIndex;
	//RakNetTimeUS nextLowestPingReset;
	RemoteSystemTimeType remoteSystemTime;
	bool continuousSend;
	RakNetTimeUS lastTimeBetweenPacketsIncrease,lastTimeBetweenPacketsDecrease;
	// Limit changes in throughput to once per ping - otherwise even if lag starts we don't know about it
	// In the meantime the connection is flooded and overrun.
	RakNetTimeUS nextAllowedThroughputSample;

	// If Update::maxBitsPerSecond > 0, then throughputCapCountdown is used as a timer to prevent sends for some amount of time after each send, depending on
	// the amount of data sent
	long long throughputCapCountdown;

	DataBlockEncryptor encryptor;
	unsigned sendPacketCount, receivePacketCount;

	///This variable is so that free memory can be called by only the update thread so we don't have to mutex things so much
	bool freeThreadedMemoryOnNextUpdate;

	// If we backoff due to packetloss, don't remeasure until all waiting resends have gone out or else we overcount
	bool packetlossThisSample;
	int backoffThisSample;
	unsigned packetlossThisSampleResendCount;
	RakNetTimeUS lastPacketlossTime;

	//long double timeBetweenPacketsIncreaseMultiplier, timeBetweenPacketsDecreaseMultiplier;

#ifndef _RELEASE
	struct DataAndTime//<InternalPacket>
	{
		char data[ MAXIMUM_MTU_SIZE ];
		unsigned int length;
		RakNetTimeUS sendTime;
	};
	DataStructures::List<DataAndTime*> delayList;

	// Internet simulator
	double maxSendBPS;
	RakNetTime minExtraPing, extraPingVariance;
#endif

	// This has to be a member because it's not threadsafe when I removed the mutexes
	DataStructures::MemoryPool<InternalPacket> internalPacketPool;
};

#endif
