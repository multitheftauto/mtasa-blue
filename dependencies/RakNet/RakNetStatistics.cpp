/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "RakNetStatistics.h"
#include <stdio.h> // sprintf
#include "BitStream.h" // BITS_TO_BYTES
#include "GetTime.h"
#include "RakString.h"

using namespace RakNet;

// Verbosity level currently supports 0 (low), 1 (medium), 2 (high)
// Buffer must be hold enough to hold the output string.  See the source to get an idea of how many bytes will be output
void RAK_DLL_EXPORT StatisticsToString( RakNetStatistics *s, char *buffer, int verbosityLevel )
{
	if ( s == 0 )
	{
		sprintf( buffer, "stats is a NULL pointer in statsToString\n" );
		return ;
	}

	if ( verbosityLevel == 0 )
	{
		// Verbosity level 0
		sprintf( buffer,
			"Total bytes sent: %s\n"
			"Total bytes received: %s\n"
			"Packetloss: %.1f%%\n",
			RakString::ToString(BITS_TO_BYTES( s->totalBitsSent )),
			RakString::ToString(BITS_TO_BYTES( s->bitsReceived + s->bitsWithBadCRCReceived )),
			100.0f * ( float ) s->messagesTotalBitsResent / ( float ) (s->totalBitsSent+s->messagesTotalBitsResent) );
	}

	else if ( verbosityLevel == 1 )
	{
		RakNetTime time = RakNet::GetTime();
		double elapsedTime;
		double bpsSent;
		double bpsReceived;
		elapsedTime = (time-s->connectionStartTime) / 1000.0f;
		bpsSent = (double) s->totalBitsSent / elapsedTime;
		bpsReceived= (double) s->bitsReceived / elapsedTime;
		// Verbosity level 1

		sprintf( buffer,
			"Messages in Send buffer: %u\n"
			"Messages sent: %u\n"
			"Bytes sent: %s\n"
			"Acks sent: %u\n"
			"Acks in send buffer: %u\n"
			"Messages waiting for ack: %u\n"
			"Messages resent: %u\n"
			"Bytes resent: %s\n"
			"Packetloss: %.1f%%\n"
			"Messages received: %u\n"
			"Bytes received: %s\n"
			"Acks received: %u\n"
			"Duplicate acks received: %u\n"
			"Inst. KBits per second: %.1f\n"
			"KBits per second sent:\t\t\t%.1f\n"
			"KBits per second received:\t\t%.1f\n"
			"Bandwith exceeded:\t\t\t%i\n",
			s->messageSendBuffer[ SYSTEM_PRIORITY ] + s->messageSendBuffer[ HIGH_PRIORITY ] + s->messageSendBuffer[ MEDIUM_PRIORITY ] + s->messageSendBuffer[ LOW_PRIORITY ],
			s->messagesSent[ SYSTEM_PRIORITY ] + s->messagesSent[ HIGH_PRIORITY ] + s->messagesSent[ MEDIUM_PRIORITY ] + s->messagesSent[ LOW_PRIORITY ],
			RakString::ToString(BITS_TO_BYTES( s->totalBitsSent )),
			s->acknowlegementsSent,
			s->acknowlegementsPending,
			s->messagesOnResendQueue,
			s->messageResends,
			RakString::ToString(BITS_TO_BYTES( s->messagesTotalBitsResent )),
			100.0f * ( float ) s->messagesTotalBitsResent / ( float ) (s->totalBitsSent+s->messagesTotalBitsResent),
			s->duplicateMessagesReceived + s->invalidMessagesReceived + s->messagesReceived,
			RakString::ToString(BITS_TO_BYTES( s->bitsReceived + s->bitsWithBadCRCReceived )),
			s->acknowlegementsReceived,
			s->duplicateAcknowlegementsReceived,
			s->bitsPerSecond  / 1000.0,
			bpsSent / 1000.0,
			bpsReceived / 1000.0,
			s->bandwidthExceeded);
	}
	else
	{
		RakNetTime time = RakNet::GetTime();
		double elapsedTime;
		double bpsSent;
		double bpsReceived;
		elapsedTime = (time-s->connectionStartTime) / 1000.0f;
		bpsSent = (double) s->totalBitsSent / elapsedTime;
		bpsReceived= (double) s->bitsReceived / elapsedTime;

		// Verbosity level 2.
		sprintf( buffer,
			"Bytes sent:\t\t\t\t%s\n"
			"Messages in send buffer:\t\tSP:%u HP:%u MP:%u LP:%u\n"
			"Messages sent:\t\t\t\tSP:%u HP:%u MP:%u LP:%u\n"
			"Message data bytes sent:\t\tSP:%s HP:%s MP:%s LP:%s\n"
			"Message header bytes sent:\t\tSP:%s HP:%s MP:%s LP:%s\n"
			"Message total bytes sent:\t\tSP:%s HP:%s MP:%s LP:%s\n"
			"Bytes received:\t\t\t\tTtl:%s Good:%s Bad:%s\n"
			"Packets received:\t\t\tTtl:%u Good:%u Bad:%u\n"
			"Acks received:\t\t\t\tTtl:%u Good:%u Dup:%u\n"
			"Messages received:\t\t\tTotal:%u Valid:%u Invalid:%u Dup:%u\n"
			"Packetloss:\t\t\t\t%.1f%%\n"
			"Packets sent:\t\t\t\t%u\n"
			"Acks sent:\t\t\t\t%u\n"
			"Acks in send buffer:\t\t\t%u\n"
			"Messages waiting for ack:\t\t%u\n"
			"Ack bytes sent:\t\t\t\t%s\n"
			"Sent packets containing only acks:\t%u\n"
			"Sent packets w/only acks and resends:\t%u\n"
			"Reliable messages resent:\t\t%u\n"
			"Reliable message data bytes resent:\t%s\n"
			"Reliable message header bytes resent:\t%s\n"
			"Reliable message total bytes resent:\t%s\n"
			"Number of messages split:\t\t%u\n"
			"Number of messages unsplit:\t\t%u\n"
			"Message splits performed:\t\t%u\n"
			"Additional encryption bytes:\t\t%s\n"
			"Sequenced messages out of order:\t%u\n"
			"Sequenced messages in order:\t\t%u\n"
			"Ordered messages out of order:\t\t%u\n"
			"Ordered messages in of order:\t\t%u\n"
			"Split messages waiting for reassembly:\t%u\n"
			"Messages in internal output queue:\t%u\n"
			"Inst KBits per second:\t\t\t%.1f\n"
			"Elapsed time (sec):\t\t\t%.1f\n"
			"KBits per second sent:\t\t\t%.1f\n"
			"KBits per second received:\t\t%.1f\n"
			"Bandwith exceeded:\t\t\t%i\n",
			RakString::ToString(BITS_TO_BYTES( s->totalBitsSent )),
			s->messageSendBuffer[ SYSTEM_PRIORITY ], s->messageSendBuffer[ HIGH_PRIORITY ], s->messageSendBuffer[ MEDIUM_PRIORITY ], s->messageSendBuffer[ LOW_PRIORITY ],
			s->messagesSent[ SYSTEM_PRIORITY ], s->messagesSent[ HIGH_PRIORITY ], s->messagesSent[ MEDIUM_PRIORITY ], s->messagesSent[ LOW_PRIORITY ],
			RakString::ToString(BITS_TO_BYTES( s->messageDataBitsSent[ SYSTEM_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageDataBitsSent[ HIGH_PRIORITY ] )), RakString::ToString(( s->messageDataBitsSent[ MEDIUM_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageDataBitsSent[ LOW_PRIORITY ] )),
			RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ SYSTEM_PRIORITY ] - s->messageDataBitsSent[ SYSTEM_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ HIGH_PRIORITY ] - s->messageDataBitsSent[ HIGH_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ MEDIUM_PRIORITY ] - s->messageDataBitsSent[ MEDIUM_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ LOW_PRIORITY ] - s->messageDataBitsSent[ LOW_PRIORITY ] )),
			RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ SYSTEM_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ HIGH_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ MEDIUM_PRIORITY ] )), RakString::ToString(BITS_TO_BYTES( s->messageTotalBitsSent[ LOW_PRIORITY ] )),
			RakString::ToString(BITS_TO_BYTES( s->bitsReceived + s->bitsWithBadCRCReceived )), RakString::ToString(BITS_TO_BYTES( s->bitsReceived )), RakString::ToString(BITS_TO_BYTES( s->bitsWithBadCRCReceived )),
			s->packetsReceived + s->packetsWithBadCRCReceived, s->packetsReceived, s->packetsWithBadCRCReceived,
			s->acknowlegementsReceived + s->duplicateAcknowlegementsReceived, s->acknowlegementsReceived, s->duplicateAcknowlegementsReceived,
			s->messagesReceived + s->invalidMessagesReceived + s->duplicateMessagesReceived, s->messagesReceived, s->invalidMessagesReceived, s->duplicateMessagesReceived,
			100.0f * ( float ) s->messagesTotalBitsResent / ( float ) (s->totalBitsSent+s->messagesTotalBitsResent),
			s->packetsSent,
			s->acknowlegementsSent,
			s->acknowlegementsPending,
			s->messagesOnResendQueue,
			RakString::ToString(BITS_TO_BYTES( s->acknowlegementBitsSent )),
			s->packetsContainingOnlyAcknowlegements,
			s->packetsContainingOnlyAcknowlegementsAndResends,
			s->messageResends,
			RakString::ToString(BITS_TO_BYTES( s->messageDataBitsResent )),
			RakString::ToString(BITS_TO_BYTES( s->messagesTotalBitsResent - s->messageDataBitsResent )),
			RakString::ToString(BITS_TO_BYTES( s->messagesTotalBitsResent )),
			s->numberOfSplitMessages,
			s->numberOfUnsplitMessages,
			s->totalSplits,
			RakString::ToString(BITS_TO_BYTES( s->encryptionBitsSent )),
			s->sequencedMessagesOutOfOrder,
			s->sequencedMessagesInOrder,
			s->orderedMessagesOutOfOrder,
			s->orderedMessagesInOrder,
			s->messagesWaitingForReassembly,
			s->internalOutputQueueSize,
			s->bitsPerSecond/1000.0,
			elapsedTime,
			bpsSent / 1000.0,
			bpsReceived / 1000.0,
			s->bandwidthExceeded
			);
	}
}
