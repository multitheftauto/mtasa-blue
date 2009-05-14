/// \file
/// \brief Derivation of the packet logger to defer the call to WriteLog until the user thread.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __THREADSAFE_PACKET_LOGGER_H
#define __THREADSAFE_PACKET_LOGGER_H

#include "PacketLogger.h"
#include "SingleProducerConsumer.h"

/// \ingroup PACKETLOGGER_GROUP
/// \brief Same as PacketLogger, but writes output in the user thread.
class RAK_DLL_EXPORT ThreadsafePacketLogger : public PacketLogger
{
public:
	ThreadsafePacketLogger();
	virtual ~ThreadsafePacketLogger();

	virtual void Update(void);

protected:
	virtual void AddToLog(const char *str);

	DataStructures::SingleProducerConsumer<char*> logMessages;
};

#endif
