/// \file
/// \brief \b [Internal] Byte queue
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __BYTE_QUEUE_H
#define __BYTE_QUEUE_H

#include "RakMemoryOverride.h"
#include "Export.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	class ByteQueue
	{
	public:
		ByteQueue();
		~ByteQueue();
		void WriteBytes(const char *in, unsigned length);
		bool ReadBytes(char *out, unsigned maxLengthToRead, bool peek);
		unsigned GetBytesWritten(void) const;
		void IncrementReadOffset(unsigned length);
		void Clear(void);
		void Print(void);

	protected:
		char *data;
		unsigned readOffset, writeOffset, lengthAllocated;
	};
}

#endif
