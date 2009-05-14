#include "PacketConsoleLogger.h"
#include "LogCommandParser.h"
#include <stdio.h>

PacketConsoleLogger::PacketConsoleLogger()
{
	logCommandParser=0;
}

void PacketConsoleLogger::SetLogCommandParser(LogCommandParser *lcp)
{
	logCommandParser=lcp;
	if (logCommandParser)
		logCommandParser->AddChannel("PacketConsoleLogger");
}
void PacketConsoleLogger::WriteLog(const char *str)
{
	if (logCommandParser)
		logCommandParser->WriteLog("PacketConsoleLogger", str);
}
