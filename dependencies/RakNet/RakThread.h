#ifndef __RAK_THREAD_H
#define __RAK_THREAD_H

#if defined(_WIN32_WCE)
#include "WindowsIncludes.h"
#endif

#include "Export.h"

namespace RakNet
{

/// To define a thread, use RAK_THREAD_DECLARATION(functionName);
#if defined(_WIN32_WCE)
#define RAK_THREAD_DECLARATION(functionName) DWORD WINAPI functionName(LPVOID arguments)
#elif defined(_WIN32)
#define RAK_THREAD_DECLARATION(functionName) unsigned __stdcall functionName( void* arguments )
#else
#define RAK_THREAD_DECLARATION(functionName) void* functionName( void* arguments )
#endif

class RAK_DLL_EXPORT RakThread
{
public:

	/// Create a thread, simplified to be cross platform without all the extra junk
	/// To then start that thread, call RakCreateThread(functionName, arguments);
	/// \param[in] start_address Function you want to call
	/// \param[in] arglist Arguments to pass to the function
	/// \return 0=success. >0 = error code
#if defined(_WIN32_WCE)
	static int Create( LPTHREAD_START_ROUTINE start_address, void *arglist);
#elif defined(_WIN32)
	static int Create( unsigned __stdcall start_address( void* ), void *arglist);
#else
	static int Create( void* start_address( void* ), void *arglist);
#endif
};

}

#endif
