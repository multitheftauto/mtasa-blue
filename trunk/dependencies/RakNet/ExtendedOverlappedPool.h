/// \file
/// \brief \b [Depreciated] This was used for IO completion ports.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


// No longer used as I no longer support IO Completion ports
/*
#ifdef __USE_IO_COMPLETION_PORTS
#ifndef __EXTENDED_OVERLAPPED_POOL
#define __EXTENDED_OVERLAPPED_POOL
#include "SimpleMutex.h"
#include "ClientContextStruct.h"
#include "DS_Queue.h"

/// Depreciated - for IO completion ports
class ExtendedOverlappedPool
{

public:
	ExtendedOverlappedPool();
	~ExtendedOverlappedPool();
	ExtendedOverlappedStruct* GetPointer( void );
	void ReleasePointer( ExtendedOverlappedStruct *p );
	static inline ExtendedOverlappedPool* Instance()
	{
		return & I;
	}

private:
	DataStructures::Queue<ExtendedOverlappedStruct*> pool;
	SimpleMutex poolMutex;
	static ExtendedOverlappedPool I;
};

#endif
#endif

*/
