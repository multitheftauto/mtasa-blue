#include "ThreadsafePacketLogger.h"
#include <string.h>

ThreadsafePacketLogger::ThreadsafePacketLogger()
{

}
ThreadsafePacketLogger::~ThreadsafePacketLogger()
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		rakFree_Ex((*msg), __FILE__, __LINE__ );
	}
}
void ThreadsafePacketLogger::Update(void)
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		WriteLog(*msg);
		rakFree_Ex((*msg), __FILE__, __LINE__ );
	}
}
void ThreadsafePacketLogger::AddToLog(const char *str)
{
	char **msg = logMessages.WriteLock();
	*msg = (char*) rakMalloc_Ex( strlen(str)+1, __FILE__, __LINE__ );
	strcpy(*msg, str);
	logMessages.WriteUnlock();
}
