/// \file
/// \brief This will write all incoming and outgoing network messages to the log command parser, which can be accessed through Telnet
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __PACKET_CONSOLE_LOGGER_H_
#define __PACKET_CONSOLE_LOGGER_H_

#include "PacketLogger.h"
class LogCommandParser;

/// \ingroup PACKETLOGGER_GROUP
/// \brief Packetlogger that logs to a remote command console
class RAK_DLL_EXPORT  PacketConsoleLogger : public PacketLogger
{
public:
	PacketConsoleLogger();
	// Writes to the command parser used for logging, which is accessed through a secondary communication layer (such as Telnet or RakNet) - See ConsoleServer.h
	virtual void SetLogCommandParser(LogCommandParser *lcp);
	virtual void WriteLog(const char *str);
protected:
	LogCommandParser *logCommandParser;
};

#endif
