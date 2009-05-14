/// \file
/// \brief This file contains enumerations for packet priority and reliability enumerations.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __PACKET_PRIORITY_H
#define __PACKET_PRIORITY_H 

/// These enumerations are used to describe when packets are delivered.
enum PacketPriority
{
	/// \internal Used by RakNet to send above-high priority messages.
	SYSTEM_PRIORITY,

	/// High priority messages are send before medium priority messages.
	HIGH_PRIORITY,

	/// Medium priority messages are send before low priority messages.
	MEDIUM_PRIORITY,   

	/// Low priority messages are only sent when no other messages are waiting.
	LOW_PRIORITY,

	/// \internal
	NUMBER_OF_PRIORITIES
};

/// These enumerations are used to describe how packets are delivered.
/// \note  Note to self: I write this with 3 bits in the stream.  If I add more remember to change that
enum PacketReliability
{
	/// Same as regular UDP, except that it will also discard duplicate datagrams.  RakNet adds (6 to 17) + 21 bits of overhead, 16 of which is used to detect duplicate packets and 6 to 17 of which is used for message length.
	UNRELIABLE,

	/// Regular UDP with a sequence counter.  Out of order messages will be discarded.  This adds an additional 13 bits on top what is used for UNRELIABLE.
	UNRELIABLE_SEQUENCED,

	/// The message is sent reliably, but not necessarily in any order.  Same overhead as UNRELIABLE.
	RELIABLE,

	/// This message is reliable and will arrive in the order you sent it.  Messages will be delayed while waiting for out of order messages.  Same overhead as UNRELIABLE_SEQUENCED.
	RELIABLE_ORDERED,

	/// This message is reliable and will arrive in the sequence you sent it.  Out or order messages will be dropped.  Same overhead as UNRELIABLE_SEQUENCED.
	RELIABLE_SEQUENCED,

	/// \internal
	NUMBER_OF_RELIABILITIES
};

#endif
