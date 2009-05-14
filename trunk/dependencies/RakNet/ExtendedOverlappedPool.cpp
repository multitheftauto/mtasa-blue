/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


// No longer used as I no longer support IO Completion ports
/*
#ifdef __USE_IO_COMPLETION_PORTS
#include "ExtendedOverlappedPool.h"

ExtendedOverlappedPool ExtendedOverlappedPool::I;

ExtendedOverlappedPool::ExtendedOverlappedPool()
{}

ExtendedOverlappedPool::~ExtendedOverlappedPool()
{
	// The caller better have returned all the packets!
	ExtendedOverlappedStruct * p;
	poolMutex.Lock();

	while ( pool.Size() )
	{
		p = pool.Pop();
		RakNet::OP_DELETE(p, __FILE__, __LINE__);
	}

	poolMutex.Unlock();
}

ExtendedOverlappedStruct* ExtendedOverlappedPool::GetPointer( void )
{
	ExtendedOverlappedStruct * p = 0;
	poolMutex.Lock();

	if ( pool.Size() )
		p = pool.Pop();

	poolMutex.Unlock();

	if ( p )
		return p;

	return new ExtendedOverlappedStruct;
}

void ExtendedOverlappedPool::ReleasePointer( ExtendedOverlappedStruct *p )
{
	poolMutex.Lock();
	pool.Push( p );
	poolMutex.Unlock();
}

#endif

*/
