/// \file
/// \brief \b [Internal] Depreciated, back from when I supported IO Completion ports.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __CLIENT_CONTEXT_STRUCT_H
#define __CLIENT_CONTEXT_STRUCT_H

#if defined(_XBOX) || defined(X360)
#elif defined(_WIN32)
//#include "WindowsIncludes.h"
#endif
#include "RakNetTypes.h"
#include "MTUSize.h"

class RakPeer;

#ifdef __USE_IO_COMPLETION_PORTS

struct ClientContextStruct
{
	HANDLE handle; // The socket, also used as a file handle
};

struct ExtendedOverlappedStruct
{
	OVERLAPPED overlapped;
	char data[ MAXIMUM_MTU_SIZE ]; // Used to hold data to send
	int length; // Length of the actual data to send, always under MAXIMUM_MTU_SIZE
	unsigned int binaryAddress;
	unsigned short port;
	RakPeer *rakPeer;
	bool read; // Set to true for reads, false for writes
};

#endif

#endif
