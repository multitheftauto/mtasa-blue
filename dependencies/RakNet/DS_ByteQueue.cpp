#include "DS_ByteQueue.h"
#include <string.h> // Memmove
#include <stdlib.h> // realloc
#include <stdio.h>


using namespace DataStructures;

ByteQueue::ByteQueue()
{
	readOffset=writeOffset=lengthAllocated=0;
	data=0;
}
ByteQueue::~ByteQueue()
{
	Clear();
	

}
void ByteQueue::WriteBytes(const char *in, unsigned length)
{
	unsigned bytesWritten;
	bytesWritten=GetBytesWritten();
	if (lengthAllocated==0 || length > lengthAllocated-bytesWritten-1)
	{
		unsigned oldLengthAllocated=lengthAllocated;
		unsigned newAmountToAllocate=length*2;
		if (newAmountToAllocate<256)
			newAmountToAllocate=256;
		lengthAllocated=lengthAllocated + newAmountToAllocate;
		data=(char*)rakRealloc_Ex(data, lengthAllocated, __FILE__, __LINE__);
		if (writeOffset < readOffset)
		{
			if (writeOffset <= newAmountToAllocate)
			{
				memcpy(data + oldLengthAllocated, data, writeOffset);
				writeOffset=readOffset+bytesWritten;
			}
			else
			{
				memcpy(data + oldLengthAllocated, data, newAmountToAllocate);
				memmove(data, data+newAmountToAllocate, writeOffset-newAmountToAllocate);
                writeOffset-=newAmountToAllocate;
			}
		}
	}

	if (length <= lengthAllocated-writeOffset)
		memcpy(data+writeOffset, in, length);
	else
	{
		// Wrap
		memcpy(data+writeOffset, in, lengthAllocated-writeOffset);
		memcpy(data, in+(lengthAllocated-writeOffset), length-(lengthAllocated-writeOffset));
	}
	writeOffset=(writeOffset+length) % lengthAllocated;
}
bool ByteQueue::ReadBytes(char *out, unsigned maxLengthToRead, bool peek)
{
	unsigned bytesWritten = GetBytesWritten();
	unsigned bytesToRead = bytesWritten < maxLengthToRead ? bytesWritten : maxLengthToRead;
	if (bytesToRead==0)
		return false;
	if (writeOffset>=readOffset)
	{
		memcpy(out, data+readOffset, bytesToRead);
	}
	else
	{
		unsigned availableUntilWrap = lengthAllocated-readOffset;
		if (bytesToRead <= availableUntilWrap)
		{
			memcpy(out, data+readOffset, bytesToRead);
		}
		else
		{
			memcpy(out, data+readOffset, availableUntilWrap);
			memcpy(out+availableUntilWrap, data, bytesToRead-availableUntilWrap);
		}
	}

	if (peek==false)
		IncrementReadOffset(bytesToRead);
		
	return true;
}
void ByteQueue::Clear(void)
{
	if (lengthAllocated)
		rakFree_Ex(data, __FILE__, __LINE__ );
	readOffset=writeOffset=lengthAllocated=0;
	data=0;
}
unsigned ByteQueue::GetBytesWritten(void) const
{
	if (writeOffset>=readOffset)
		return writeOffset-readOffset;
	else
		return writeOffset+(lengthAllocated-readOffset);
}
void ByteQueue::IncrementReadOffset(unsigned length)
{
	readOffset=(readOffset+length) % lengthAllocated;
}
void ByteQueue::Print(void)
{
	unsigned i;
	for (i=readOffset; i!=writeOffset; i++)
		RAKNET_DEBUG_PRINTF("%i ", data[i]);
	RAKNET_DEBUG_PRINTF("\n");
}
