/// \file
/// \brief \b [Internal] A class which stores a user message, and all information associated with sending and receiving that message.
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

#ifndef __INTERNAL_PACKET_H
#define __INTERNAL_PACKET_H

#include "PacketPriority.h"
#include "RakNetTypes.h"
#include "RakMemoryOverride.h"

typedef unsigned short SplitPacketIdType;
typedef unsigned int SplitPacketIndexType;


/// This is the counter used for holding packet numbers, so we can detect duplicate packets.  It should be large enough that if the variables
/// were to wrap, the newly wrapped values would no longer be in use.  Warning: Too large of a value wastes bandwidth!
/// Use the smallest possible value, such that you send no more than rangeof(MessageNumberType) / GetTimeoutTime() packets per second
/// For the default value of 10 seconds, this is
/// unsigned char - 25.5 packets per second
/// unsigned short - 6553.5 packets per second
/// unsigned int - You'll run out of memory first.
typedef unsigned int MessageNumberType;

/// This is the counter used for holding ordered packet numbers, so we can detect out-of-order packets.  It should be large enough that if the variables
/// were to wrap, the newly wrapped values would no longer be in use.  Warning: Too large of a value wastes bandwidth!
typedef MessageNumberType OrderingIndexType;

typedef RakNetTime RemoteSystemTimeType;

/// Holds a user message, and related information
/// Don't use a constructor or destructor, due to the memory pool I am using
struct InternalPacket//<InternalPacket>
{
	///True if this is an acknowledgment packet
	//bool isAcknowledgement;
	
	/// A unique numerical identifier given to this user message. Used to identify messages on the network
	MessageNumberType messageNumber;
	/// Identifies the order in which this number was sent. Used locally
	MessageNumberType messageInternalOrder;
	/// Has this message number been assigned yet?  We don't assign until the message is actually sent.
	/// This fixes a bug where pre-determining message numbers and then sending a message on a different channel creates a huge gap.
	/// This causes performance problems and causes those messages to timeout.
	bool messageNumberAssigned;
	/// Used only for tracking packetloss and windowing internally, this is the aggreggate packet number that a message was last sent in
	unsigned packetNumber;
	/// Was this packet number used this update to track windowing drops or increases?  Each packet number is only used once per update.
//	bool allowWindowUpdate;
	///The priority level of this packet
	PacketPriority priority;
	///What type of reliability algorithm to use with this packet
	PacketReliability reliability;
	///What ordering channel this packet is on, if the reliability type uses ordering channels
	unsigned char orderingChannel;
	///The ID used as identification for ordering channels
	OrderingIndexType orderingIndex;
	///The ID of the split packet, if we have split packets.  This is the maximum number of split messages we can send simultaneously per connection.
	SplitPacketIdType splitPacketId;
	///If this is a split packet, the index into the array of subsplit packets
	SplitPacketIndexType splitPacketIndex;
	///The size of the array of subsplit packets
	SplitPacketIndexType splitPacketCount;
	///When this packet was created
	RakNetTimeUS creationTime;
	///The next time to take action on this packet
	RakNetTimeUS nextActionTime;
	// If this was a reliable packet, it included the ping time, to be sent back in an ack
	//RakNetTimeUS remoteSystemTime;
	//RemoteSystemTimeType remoteSystemTime;
	///How many bits the data is
	BitSize_t dataBitLength;
	///Buffer is a pointer to the actual data, assuming this packet has data at all
	unsigned char *data;
	// How many attempts we made at sending this message
	unsigned char timesSent;
};

#endif

