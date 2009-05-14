/// \file
/// \brief \b [Internal] Passes queued data between threads using a circular buffer with read and write pointers
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __SINGLE_PRODUCER_CONSUMER_H
#define __SINGLE_PRODUCER_CONSUMER_H

#include "RakAssert.h"

static const int MINIMUM_LIST_SIZE=8;

#include "RakMemoryOverride.h"
#include "Export.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	/// \brief A single producer consumer implementation without critical sections.
	template <class SingleProducerConsumerType>
	class RAK_DLL_EXPORT SingleProducerConsumer
	{
	public:
		/// Constructor
		SingleProducerConsumer();

		/// Destructor
		~SingleProducerConsumer();

		/// WriteLock must be immediately followed by WriteUnlock.  These two functions must be called in the same thread.
		/// \return A pointer to a block of data you can write to.
		SingleProducerConsumerType* WriteLock(void);

		/// Call if you don't want to write to a block of data from WriteLock() after all.
		/// Cancelling locks cancels all locks back up to the data passed.  So if you lock twice and cancel using the first lock, the second lock is ignored
		/// \param[in] cancelToLocation Which WriteLock() to cancel.
		void CancelWriteLock(SingleProducerConsumerType* cancelToLocation);

		/// Call when you are done writing to a block of memory returned by WriteLock()
		void WriteUnlock(void);

		/// ReadLock must be immediately followed by ReadUnlock. These two functions must be called in the same thread.
		/// \retval 0 No data is availble to read
		/// \retval Non-zero The data previously written to, in another thread, by WriteLock followed by WriteUnlock.
		SingleProducerConsumerType* ReadLock(void);

		// Cancelling locks cancels all locks back up to the data passed.  So if you lock twice and cancel using the first lock, the second lock is ignored
		/// param[in] Which ReadLock() to cancel.
		void CancelReadLock(SingleProducerConsumerType* cancelToLocation);

		/// Signals that we are done reading the the data from the least recent call of ReadLock.
		/// At this point that pointer is no longer valid, and should no longer be read.
		void ReadUnlock(void);

		/// Clear is not thread-safe and none of the lock or unlock functions should be called while it is running.
		void Clear(void);

		/// This function will estimate how many elements are waiting to be read.  It's threadsafe enough that the value returned is stable, but not threadsafe enough to give accurate results.
		/// \return An ESTIMATE of how many data elements are waiting to be read
		int Size(void) const;

		/// Make sure that the pointer we done reading for the call to ReadUnlock is the right pointer.
		/// param[in] A previous pointer returned by ReadLock()
		bool CheckReadUnlockOrder(const SingleProducerConsumerType* data) const;

		/// Returns if ReadUnlock was called before ReadLock
		/// \return If the read is locked
		bool ReadIsLocked(void) const;

	private:
		struct DataPlusPtr
		{
			DataPlusPtr () {readyToRead=false;}
			SingleProducerConsumerType object;

			// Ready to read is so we can use an equality boolean comparison, in case the writePointer var is trashed while context switching.
			volatile bool readyToRead;
			volatile DataPlusPtr *next;
		};
		volatile DataPlusPtr *readAheadPointer;
		volatile DataPlusPtr *writeAheadPointer;
		volatile DataPlusPtr *readPointer;
		volatile DataPlusPtr *writePointer;
		unsigned readCount, writeCount;
	};

	template <class SingleProducerConsumerType>
		SingleProducerConsumer<SingleProducerConsumerType>::SingleProducerConsumer()
	{
		// Preallocate
		readPointer = RakNet::OP_NEW<DataPlusPtr>( __FILE__, __LINE__ );
		writePointer=readPointer;
		readPointer->next = RakNet::OP_NEW<DataPlusPtr>( __FILE__, __LINE__ );
		int listSize;
#ifdef _DEBUG
		RakAssert(MINIMUM_LIST_SIZE>=3);
#endif
		for (listSize=2; listSize < MINIMUM_LIST_SIZE; listSize++)
		{
			readPointer=readPointer->next;
			readPointer->next = RakNet::OP_NEW<DataPlusPtr>( __FILE__, __LINE__ );
		}
		readPointer->next->next=writePointer; // last to next = start
		readPointer=writePointer;
		readAheadPointer=readPointer;
		writeAheadPointer=writePointer;
		readCount=writeCount=0;
	}

	template <class SingleProducerConsumerType>
		SingleProducerConsumer<SingleProducerConsumerType>::~SingleProducerConsumer()
	{
		volatile DataPlusPtr *next;
		readPointer=writeAheadPointer->next;
		while (readPointer!=writeAheadPointer)
		{
			next=readPointer->next;
			RakNet::OP_DELETE((char*) readPointer, __FILE__, __LINE__);
			readPointer=next;
		}
		RakNet::OP_DELETE((char*) readPointer, __FILE__, __LINE__);
	}

	template <class SingleProducerConsumerType>
		SingleProducerConsumerType* SingleProducerConsumer<SingleProducerConsumerType>::WriteLock( void )
	{
		if (writeAheadPointer->next==readPointer ||
			writeAheadPointer->next->readyToRead==true)
		{
			volatile DataPlusPtr *originalNext=writeAheadPointer->next;
			writeAheadPointer->next=new DataPlusPtr;
			RakAssert(writeAheadPointer->next);
			writeAheadPointer->next->next=originalNext;
		}

		volatile DataPlusPtr *last;
		last=writeAheadPointer;
		writeAheadPointer=writeAheadPointer->next;

		return (SingleProducerConsumerType*) last;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::CancelWriteLock( SingleProducerConsumerType* cancelToLocation )
	{
		writeAheadPointer=(DataPlusPtr *)cancelToLocation;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::WriteUnlock( void )
	{
		//	DataPlusPtr *dataContainer = (DataPlusPtr *)structure;

#ifdef _DEBUG
		RakAssert(writePointer->next!=readPointer);
		RakAssert(writePointer!=writeAheadPointer);
#endif

		writeCount++;
		// User is done with the data, allow send by updating the write pointer
		writePointer->readyToRead=true;
		writePointer=writePointer->next;
	}

	template <class SingleProducerConsumerType>
		SingleProducerConsumerType* SingleProducerConsumer<SingleProducerConsumerType>::ReadLock( void )
	{
			if (readAheadPointer==writePointer ||
				readAheadPointer->readyToRead==false)
			{
				return 0;
			}

			volatile DataPlusPtr *last;
			last=readAheadPointer;
			readAheadPointer=readAheadPointer->next;
			return (SingleProducerConsumerType*)last;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::CancelReadLock( SingleProducerConsumerType* cancelToLocation )
	{
#ifdef _DEBUG
		RakAssert(readPointer!=writePointer);
#endif
		readAheadPointer=(DataPlusPtr *)cancelToLocation;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::ReadUnlock( void )
	{
#ifdef _DEBUG
		RakAssert(readAheadPointer!=readPointer); // If hits, then called ReadUnlock before ReadLock
		RakAssert(readPointer!=writePointer); // If hits, then called ReadUnlock when Read returns 0
#endif
		readCount++;

		// Allow writes to this memory block
		readPointer->readyToRead=false;
		readPointer=readPointer->next;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::Clear( void )
	{
		// Shrink the list down to MINIMUM_LIST_SIZE elements
		volatile DataPlusPtr *next;
		writePointer=readPointer->next;

		int listSize=1;
		next=readPointer->next;
		while (next!=readPointer)
		{
			listSize++;
			next=next->next;
		}

		while (listSize-- > MINIMUM_LIST_SIZE)
		{
			next=writePointer->next;
#ifdef _DEBUG
			RakAssert(writePointer!=readPointer);
#endif
			RakNet::OP_DELETE((char*) writePointer, __FILE__, __LINE__);
			writePointer=next;
		}

		readPointer->next=writePointer;
		writePointer=readPointer;
		readAheadPointer=readPointer;
		writeAheadPointer=writePointer;
		readCount=writeCount=0;
	}

	template <class SingleProducerConsumerType>
		int SingleProducerConsumer<SingleProducerConsumerType>::Size( void ) const
	{
		return writeCount-readCount;
	}

	template <class SingleProducerConsumerType>
		bool SingleProducerConsumer<SingleProducerConsumerType>::CheckReadUnlockOrder(const SingleProducerConsumerType* data) const
	{
		return const_cast<const SingleProducerConsumerType *>(&readPointer->object) == data;
	}


	template <class SingleProducerConsumerType>
		bool SingleProducerConsumer<SingleProducerConsumerType>::ReadIsLocked(void) const
	{
		return readAheadPointer!=readPointer;
	}	
}

#endif

/*
#include "SingleProducerConsumer.h"
#include <process.h>
#include "RakAssert.h"
#include <stdio.h>
#include "WindowsIncludes.h"
#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#include <math.h>
#else
#include <cmath>
#endif
#include <stdlib.h>

#define READ_COUNT_ITERATIONS 10000000

DataStructures::SingleProducerConsumer<unsigned long> spc;
unsigned long readCount;

unsigned __stdcall ProducerThread( LPVOID arguments )
{
unsigned long producerCount;
unsigned long *writeBlock;
producerCount=0;
while (readCount < READ_COUNT_ITERATIONS)
{
writeBlock=spc.WriteLock();
*writeBlock=producerCount;
spc.WriteUnlock();
producerCount++;
if ((producerCount%1000000)==0)
{
RAKNET_DEBUG_PRINTF("WriteCount: %i. BufferSize=%i\n", producerCount, spc.Size());
}
}
RAKNET_DEBUG_PRINTF("PRODUCER THREAD ENDED!\n");
return 0;
}

unsigned __stdcall ConsumerThread( LPVOID arguments )
{
unsigned long *readBlock;
while (readCount < READ_COUNT_ITERATIONS)
{
if ((readBlock=spc.ReadLock())!=0)
{
if (*readBlock!=readCount)
{
RAKNET_DEBUG_PRINTF("Test failed! Expected %i got %i!\n", readCount, *readBlock);
readCount = READ_COUNT_ITERATIONS;
RakAssert(0);
}
spc.ReadUnlock();
readCount++;
if ((readCount%1000000)==0)
{
RAKNET_DEBUG_PRINTF("ReadCount:  %i. BufferSize=%i\n", readCount, spc.Size());
}
}
}
RAKNET_DEBUG_PRINTF("CONSUMER THREAD ENDED!\n");
return 0;
}

void main(void)
{
readCount=0;
unsigned threadId1 = 0;
unsigned threadId2 = 0;
HANDLE thread1Handle, thread2Handle;
unsigned long startTime = timeGetTime();

thread1Handle=(HANDLE)_beginthreadex( NULL, 0, ProducerThread, 0, 0, &threadId1 );
thread2Handle=(HANDLE)_beginthreadex( NULL, 0, ConsumerThread, 0, 0, &threadId1 );

while (readCount < READ_COUNT_ITERATIONS)
{
Sleep(0);
}
char str[256];
RAKNET_DEBUG_PRINTF("Elapsed time = %i milliseconds. Press Enter to continue\n", timeGetTime() - startTime);
fgets(str, sizeof(str), stdin);
}
*/
