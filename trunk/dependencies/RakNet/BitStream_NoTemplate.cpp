/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#if defined(_MSC_VER) && _MSC_VER < 1299 // VC6 doesn't support template specialization

#include "BitStream.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <float.h>
#if defined(_XBOX) || defined(X360)
#include "XBOX360Includes.h"
#elif defined(_WIN32)
#include <winsock2.h> // htonl
#elif defined(_CONSOLE_2)
#include "PS3Includes.h"
#else
#include <arpa/inet.h>
#endif

// MSWin uses _copysign, others use copysign...
#ifndef _WIN32
#define _copysign copysign
#endif



using namespace RakNet;

#ifdef _MSC_VER
#pragma warning( push )
#endif

BitStream::BitStream()
{
	numberOfBitsUsed = 0;
	//numberOfBitsAllocated = 32 * 8;
	numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
	readOffset = 0;
	//data = ( unsigned char* ) rakMalloc_Ex( 32, __FILE__, __LINE__ );
	data = ( unsigned char* ) stackData;

#ifdef _DEBUG
//	RakAssert( data );
#endif
	//memset(data, 0, 32);
	copyData = true;
}

BitStream::BitStream( int initialBytesToAllocate )
{
	numberOfBitsUsed = 0;
	readOffset = 0;
	if (initialBytesToAllocate <= BITSTREAM_STACK_ALLOCATION_SIZE)
	{
		data = ( unsigned char* ) stackData;
		numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
	}
	else
	{
		data = ( unsigned char* ) rakMalloc_Ex( initialBytesToAllocate, __FILE__, __LINE__ );
		numberOfBitsAllocated = initialBytesToAllocate << 3;
	}
#ifdef _DEBUG
	RakAssert( data );
#endif
	// memset(data, 0, initialBytesToAllocate);
	copyData = true;
}

BitStream::BitStream( unsigned char* _data, unsigned int lengthInBytes, bool _copyData )
{
	numberOfBitsUsed = lengthInBytes << 3;
	readOffset = 0;
	copyData = _copyData;
	numberOfBitsAllocated = lengthInBytes << 3;

	if ( copyData )
	{
		if ( lengthInBytes > 0 )
		{
			if (lengthInBytes < BITSTREAM_STACK_ALLOCATION_SIZE)
			{
				data = ( unsigned char* ) stackData;
				numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE << 3;
			}
			else
			{
				data = ( unsigned char* ) rakMalloc_Ex( lengthInBytes, __FILE__, __LINE__ );
			}
#ifdef _DEBUG
			RakAssert( data );
#endif
			memcpy( data, _data, lengthInBytes );
		}
		else
			data = 0;
	}
	else
		data = ( unsigned char* ) _data;
}

// Use this if you pass a pointer copy to the constructor (_copyData==false) and want to overallocate to prevent reallocation
void BitStream::SetNumberOfBitsAllocated( const unsigned int lengthInBits )
{
#ifdef _DEBUG
	RakAssert( lengthInBits >= ( unsigned int ) numberOfBitsAllocated );
#endif
	numberOfBitsAllocated = lengthInBits;
}

BitStream::~BitStream()
{
	if ( copyData && numberOfBitsAllocated > (BITSTREAM_STACK_ALLOCATION_SIZE << 3))
		rakFree_Ex( data , __FILE__, __LINE__ );  // Use realloc and free so we are more efficient than delete and new for resizing
}

void BitStream::Reset( void )
{
	// Note:  Do NOT reallocate memory because BitStream is used
	// in places to serialize/deserialize a buffer. Reallocation
	// is a dangerous operation (may result in leaks).

	if ( numberOfBitsUsed > 0 )
	{
		//  memset(data, 0, BITS_TO_BYTES(numberOfBitsUsed));
	}

	// Don't free memory here for speed efficiency
	//free(data);  // Use realloc and free so we are more efficient than delete and new for resizing
	numberOfBitsUsed = 0;

	//numberOfBitsAllocated=8;
	readOffset = 0;

	//data=(unsigned char*)rakMalloc_Ex(1, __FILE__, __LINE__);
	// if (numberOfBitsAllocated>0)
	//  memset(data, 0, BITS_TO_BYTES(numberOfBitsAllocated));
}

// Write an array or casted stream
void BitStream::Write( const char* input, const int numberOfBytes )
{
	if (numberOfBytes==0)
		return;

	// Optimization:
	if ((numberOfBitsUsed & 7) == 0)
	{
		AddBitsAndReallocate( BYTES_TO_BITS(numberOfBytes) );
		memcpy(data+BITS_TO_BYTES(numberOfBitsUsed), input, numberOfBytes);
		numberOfBitsUsed+=BYTES_TO_BITS(numberOfBytes);
	}
	else
	{
		WriteBits( ( unsigned char* ) input, numberOfBytes * 8, true );
	}

}
void BitStream::Write( BitStream *bitStream)
{
	Write(bitStream, bitStream->GetNumberOfBitsUsed());
}
void BitStream::Write( BitStream *bitStream, int numberOfBits )
{
	AddBitsAndReallocate( numberOfBits );
	int numberOfBitsMod8;

	while (numberOfBits-->0 && bitStream->readOffset + 1 <= bitStream->numberOfBitsUsed)
	{
		numberOfBitsMod8 = numberOfBitsUsed & 7;
		if ( numberOfBitsMod8 == 0 )
		{
			// New byte
			if (bitStream->data[ bitStream->readOffset >> 3 ] & ( 0x80 >> ( bitStream->readOffset & 7 ) ) )
			{
				// Write 1
				data[ numberOfBitsUsed >> 3 ] = 0x80;
			}
			else
			{
				// Write 0
				data[ numberOfBitsUsed >> 3 ] = 0;
			}

		}
		else
		{
			// Existing byte
			if (bitStream->data[ bitStream->readOffset >> 3 ] & ( 0x80 >> ( bitStream->readOffset & 7 ) ) )
				data[ numberOfBitsUsed >> 3 ] |= 0x80 >> ( numberOfBitsMod8 ); // Set the bit to 1
			// else 0, do nothing
		}

		bitStream->readOffset++;
		numberOfBitsUsed++;
	}
}

// Read an array or casted stream
bool BitStream::Read( char* output, const int numberOfBytes )
{
	// Optimization:
	if ((readOffset & 7) == 0)
	{
		if ( readOffset + ( numberOfBytes << 3 ) > numberOfBitsUsed )
			return false;

		// Write the data
		memcpy( output, data + ( readOffset >> 3 ), numberOfBytes );

		readOffset += numberOfBytes << 3;
		return true;
	}
	else
	{
		return ReadBits( ( unsigned char* ) output, numberOfBytes * 8 );
	}
}

// Sets the read pointer back to the beginning of your data.
void BitStream::ResetReadPointer( void )
{
	readOffset = 0;
}

// Sets the write pointer back to the beginning of your data.
void BitStream::ResetWritePointer( void )
{
	numberOfBitsUsed = 0;
}

// Write a 0
void BitStream::Write0( void )
{
	AddBitsAndReallocate( 1 );

	// New bytes need to be zeroed
	if ( ( numberOfBitsUsed & 7 ) == 0 )
		data[ numberOfBitsUsed >> 3 ] = 0;

	numberOfBitsUsed++;
}

// Write a 1
void BitStream::Write1( void )
{
	AddBitsAndReallocate( 1 );

	int numberOfBitsMod8 = numberOfBitsUsed & 7;

	if ( numberOfBitsMod8 == 0 )
		data[ numberOfBitsUsed >> 3 ] = 0x80;
	else
		data[ numberOfBitsUsed >> 3 ] |= 0x80 >> ( numberOfBitsMod8 ); // Set the bit to 1

	numberOfBitsUsed++;
}

#ifdef _MSC_VER
#pragma warning( disable : 4800 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
// Returns true if the next data read is a 1, false if it is a 0
bool BitStream::ReadBit( void )
{
	bool result = ( data[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) );
	readOffset++;
	return result;
}

// Align the bitstream to the byte boundary and then write the specified number of bits.
// This is faster than WriteBits but wastes the bits to do the alignment and requires you to call
// SetReadToByteAlignment at the corresponding read position
void BitStream::WriteAlignedBytes( void* input, const int numberOfBytesToWrite )
{
#ifdef _DEBUG
	if (numberOfBytesToWrite<=0)
	{
		RakAssert( numberOfBytesToWrite > 0 );
	}
#endif

	AlignWriteToByteBoundary();
	Write((const char*) input, numberOfBytesToWrite);
}

/// Aligns the bitstream, writes inputLength, and writes input. Won't write beyond maxBytesToWrite
void BitStream::WriteAlignedBytesSafe( void *input, const int inputLength, const int maxBytesToWrite )
{
	if (input==0 || inputLength==0)
	{
		WriteCompressed((unsigned int)0);
		return;
	}
	WriteCompressed(inputLength);
	WriteAlignedBytes((void*) input, inputLength < maxBytesToWrite ? inputLength : maxBytesToWrite);
}

// Read bits, starting at the next aligned bits. Note that the modulus 8 starting offset of the
// sequence must be the same as was used with WriteBits. This will be a problem with packet coalescence
// unless you byte align the coalesced packets.
bool BitStream::ReadAlignedBytes( void* output, const int numberOfBytesToRead )
{
#ifdef _DEBUG
	RakAssert( numberOfBytesToRead > 0 );
#endif

	if ( numberOfBytesToRead <= 0 )
		return false;

	// Byte align
	AlignReadToByteBoundary();

	if ( readOffset + ( numberOfBytesToRead << 3 ) > numberOfBitsUsed )
		return false;

	// Write the data
	memcpy( output, data + ( readOffset >> 3 ), numberOfBytesToRead );

	readOffset += numberOfBytesToRead << 3;

	return true;
}
bool BitStream::ReadAlignedBytesSafe( void *input, int &inputLength, const int maxBytesToRead )
{
	if (ReadCompressed(inputLength)==false)
		return false;
	if (inputLength > maxBytesToRead)
		inputLength=maxBytesToRead;
	if (inputLength==0)
		return true;
	return ReadAlignedBytes((unsigned char*) input, inputLength);
}
bool BitStream::ReadAlignedBytesSafeAlloc( char **input, int &inputLength, const int maxBytesToRead )
{
	rakFree_Ex(*input, __FILE__, __LINE__ );
	*input=0;
	if (ReadCompressed(inputLength)==false)
		return false;
	if (inputLength > maxBytesToRead)
		inputLength=maxBytesToRead;
	if (inputLength==0)
		return true;
	*input = (char*) rakMalloc_Ex( BITS_TO_BYTES( inputLength ), __FILE__, __LINE__ );
	return ReadAlignedBytes((unsigned char*) *input, inputLength);
}
// Align the next write and/or read to a byte boundary.  This can be used to 'waste' bits to byte align for efficiency reasons
void BitStream::AlignWriteToByteBoundary( void )
{
	if ( numberOfBitsUsed )
		numberOfBitsUsed += 8 - ( (( numberOfBitsUsed - 1 ) & 7) + 1 );
}

// Align the next write and/or read to a byte boundary.  This can be used to 'waste' bits to byte align for efficiency reasons
void BitStream::AlignReadToByteBoundary( void )
{
	if ( readOffset )
		readOffset += 8 - ( (( readOffset - 1 ) & 7 ) + 1 );
}

// Write numberToWrite bits from the input source
void BitStream::WriteBits( const unsigned char *input, int numberOfBitsToWrite, const bool rightAlignedBits )
{
	if (numberOfBitsToWrite<=0)
		return;

	AddBitsAndReallocate( numberOfBitsToWrite );
	int offset = 0;
	unsigned char dataByte;
	int numberOfBitsUsedMod8;

	numberOfBitsUsedMod8 = numberOfBitsUsed & 7;

	// Faster to put the while at the top surprisingly enough
	while ( numberOfBitsToWrite > 0 )
		//do
	{
		dataByte = *( input + offset );

		if ( numberOfBitsToWrite < 8 && rightAlignedBits )   // rightAlignedBits means in the case of a partial byte, the bits are aligned from the right (bit 0) rather than the left (as in the normal internal representation)
			dataByte <<= 8 - numberOfBitsToWrite;  // shift left to get the bits on the left, as in our internal representation

		// Writing to a new byte each time
		if ( numberOfBitsUsedMod8 == 0 )
			* ( data + ( numberOfBitsUsed >> 3 ) ) = dataByte;
		else
		{
			// Copy over the new data.
			*( data + ( numberOfBitsUsed >> 3 ) ) |= dataByte >> ( numberOfBitsUsedMod8 ); // First half

			if ( 8 - ( numberOfBitsUsedMod8 ) < 8 && 8 - ( numberOfBitsUsedMod8 ) < numberOfBitsToWrite )   // If we didn't write it all out in the first half (8 - (numberOfBitsUsed%8) is the number we wrote in the first half)
			{
				*( data + ( numberOfBitsUsed >> 3 ) + 1 ) = (unsigned char) ( dataByte << ( 8 - ( numberOfBitsUsedMod8 ) ) ); // Second half (overlaps byte boundary)
			}
		}

		if ( numberOfBitsToWrite >= 8 )
			numberOfBitsUsed += 8;
		else
			numberOfBitsUsed += numberOfBitsToWrite;

		numberOfBitsToWrite -= 8;

		offset++;
	}
	// } while(numberOfBitsToWrite>0);
}

// Set the stream to some initial data.  For internal use
void BitStream::SetData( unsigned char *input )
{
	data=input;
	copyData=false;
}

// Assume the input source points to a native type, compress and write it
void BitStream::WriteCompressed( const unsigned char* input,
	const int size, const bool unsignedData )
{
	int currentByte = ( size >> 3 ) - 1; // PCs

	unsigned char byteMatch;

	if ( unsignedData )
	{
		byteMatch = 0;
	}

	else
	{
		byteMatch = 0xFF;
	}

	// Write upper bytes with a single 1
	// From high byte to low byte, if high byte is a byteMatch then write a 1 bit. Otherwise write a 0 bit and then write the remaining bytes
	while ( currentByte > 0 )
	{
		if ( input[ currentByte ] == byteMatch )   // If high byte is byteMatch (0 of 0xff) then it would have the same value shifted
		{
			bool b = true;
			Write( b );
		}
		else
		{
			// Write the remainder of the data after writing 0
			bool b = false;
			Write( b );

			WriteBits( input, ( currentByte + 1 ) << 3, true );
			//  currentByte--;


			return ;
		}

		currentByte--;
	}

	// If the upper half of the last byte is a 0 (positive) or 16 (negative) then write a 1 and the remaining 4 bits.  Otherwise write a 0 and the 8 bites.
	if ( ( unsignedData && ( ( *( input + currentByte ) ) & 0xF0 ) == 0x00 ) ||
		( unsignedData == false && ( ( *( input + currentByte ) ) & 0xF0 ) == 0xF0 ) )
	{
		bool b = true;
		Write( b );
		WriteBits( input + currentByte, 4, true );
	}

	else
	{
		bool b = false;
		Write( b );
		WriteBits( input + currentByte, 8, true );
	}
}

// Read numberOfBitsToRead bits to the output source
// alignBitsToRight should be set to true to convert internal bitstream data to userdata
// It should be false if you used WriteBits with rightAlignedBits false
bool BitStream::ReadBits( unsigned char* output, int numberOfBitsToRead, const bool alignBitsToRight )
{
#ifdef _DEBUG
//	RakAssert( numberOfBitsToRead > 0 );
#endif
	if (numberOfBitsToRead<=0)
	  return false;

	if ( readOffset + numberOfBitsToRead > numberOfBitsUsed )
		return false;

	int readOffsetMod8;

	int offset = 0;

	memset( output, 0, BITS_TO_BYTES( numberOfBitsToRead ) );

	readOffsetMod8 = readOffset & 7;

	// do
	// Faster to put the while at the top surprisingly enough
	while ( numberOfBitsToRead > 0 )
	{
		*( output + offset ) |= *( data + ( readOffset >> 3 ) ) << ( readOffsetMod8 ); // First half

		if ( readOffsetMod8 > 0 && numberOfBitsToRead > 8 - ( readOffsetMod8 ) )   // If we have a second half, we didn't read enough bytes in the first half
			*( output + offset ) |= *( data + ( readOffset >> 3 ) + 1 ) >> ( 8 - ( readOffsetMod8 ) ); // Second half (overlaps byte boundary)

		numberOfBitsToRead -= 8;

		if ( numberOfBitsToRead < 0 )   // Reading a partial byte for the last byte, shift right so the data is aligned on the right
		{

			if ( alignBitsToRight )
				* ( output + offset ) >>= -numberOfBitsToRead;

			readOffset += 8 + numberOfBitsToRead;
		}
		else
			readOffset += 8;

		offset++;

	}

	//} while(numberOfBitsToRead>0);

	return true;
}

// Assume the input source points to a compressed native type. Decompress and read it
bool BitStream::ReadCompressed( unsigned char* output,
	const int size, const bool unsignedData )
{
	int currentByte = ( size >> 3 ) - 1;


	unsigned char byteMatch, halfByteMatch;

	if ( unsignedData )
	{
		byteMatch = 0;
		halfByteMatch = 0;
	}

	else
	{
		byteMatch = 0xFF;
		halfByteMatch = 0xF0;
	}

	// Upper bytes are specified with a single 1 if they match byteMatch
	// From high byte to low byte, if high byte is a byteMatch then write a 1 bit. Otherwise write a 0 bit and then write the remaining bytes
	while ( currentByte > 0 )
	{
		// If we read a 1 then the data is byteMatch.

		bool b;

		if ( Read( b ) == false )
			return false;

		if ( b )   // Check that bit
		{
			output[ currentByte ] = byteMatch;
			currentByte--;
		}
		else
		{
			// Read the rest of the bytes

			if ( ReadBits( output, ( currentByte + 1 ) << 3 ) == false )
				return false;

			return true;
		}
	}

	// All but the first bytes are byteMatch.  If the upper half of the last byte is a 0 (positive) or 16 (negative) then what we read will be a 1 and the remaining 4 bits.
	// Otherwise we read a 0 and the 8 bytes
	//RakAssert(readOffset+1 <=numberOfBitsUsed); // If this assert is hit the stream wasn't long enough to read from
	if ( readOffset + 1 > numberOfBitsUsed )
		return false;

	bool b;

	if ( Read( b ) == false )
		return false;

	if ( b )   // Check that bit
	{

		if ( ReadBits( output + currentByte, 4 ) == false )
			return false;

		output[ currentByte ] |= halfByteMatch; // We have to set the high 4 bits since these are set to 0 by ReadBits
	}
	else
	{
		if ( ReadBits( output + currentByte, 8 ) == false )
			return false;
	}

	return true;
}

// Reallocates (if necessary) in preparation of writing numberOfBitsToWrite
void BitStream::AddBitsAndReallocate( const int numberOfBitsToWrite )
{
	if (numberOfBitsToWrite <= 0)
		return;

	int newNumberOfBitsAllocated = numberOfBitsToWrite + numberOfBitsUsed;

	if ( numberOfBitsToWrite + numberOfBitsUsed > 0 && ( ( numberOfBitsAllocated - 1 ) >> 3 ) < ( ( newNumberOfBitsAllocated - 1 ) >> 3 ) )   // If we need to allocate 1 or more new bytes
	{
#ifdef _DEBUG
		// If this assert hits then we need to specify true for the third parameter in the constructor
		// It needs to reallocate to hold all the data and can't do it unless we allocated to begin with
		RakAssert( copyData == true );
#endif

		// Less memory efficient but saves on news and deletes
		newNumberOfBitsAllocated = ( numberOfBitsToWrite + numberOfBitsUsed ) * 2;
//		int newByteOffset = BITS_TO_BYTES( numberOfBitsAllocated );
		// Use realloc and free so we are more efficient than delete and new for resizing
		int amountToAllocate = BITS_TO_BYTES( newNumberOfBitsAllocated );
		if (data==(unsigned char*)stackData)
		{
			 if (amountToAllocate > BITSTREAM_STACK_ALLOCATION_SIZE)
			 {
				 data = ( unsigned char* ) rakMalloc_Ex( amountToAllocate, __FILE__, __LINE__ );

				 // need to copy the stack data over to our new memory area too
				 memcpy ((void *)data, (void *)stackData, BITS_TO_BYTES( numberOfBitsAllocated ));
			 }
		}
		else
		{
			data = ( unsigned char* ) rakRealloc_Ex( data, amountToAllocate, __FILE__, __LINE__ );
		}

#ifdef _DEBUG
		RakAssert( data ); // Make sure realloc succeeded
#endif
		//  memset(data+newByteOffset, 0,  ((newNumberOfBitsAllocated-1)>>3) - ((numberOfBitsAllocated-1)>>3)); // Set the new data block to 0
	}

	if ( newNumberOfBitsAllocated > numberOfBitsAllocated )
		numberOfBitsAllocated = newNumberOfBitsAllocated;
}
unsigned int BitStream::GetNumberOfBitsAllocated(void) const
{
	return numberOfBitsAllocated;
}

// Should hit if reads didn't match writes
void BitStream::AssertStreamEmpty( void )
{
	RakAssert( readOffset == numberOfBitsUsed );
}

void BitStream::PrintBits( void ) const
{
	if ( numberOfBitsUsed <= 0 )
	{
		RAKNET_DEBUG_PRINTF( "No bits\n" );
		return ;
	}

	for ( int counter = 0; counter < BITS_TO_BYTES( numberOfBitsUsed ); counter++ )
	{
		int stop;

		if ( counter == ( numberOfBitsUsed - 1 ) >> 3 )
			stop = 8 - ( ( ( numberOfBitsUsed - 1 ) & 7 ) + 1 );
		else
			stop = 0;

		for ( int counter2 = 7; counter2 >= stop; counter2-- )
		{
			if ( ( data[ counter ] >> counter2 ) & 1 )
				putchar( '1' );
			else
				putchar( '0' );
		}

		putchar( ' ' );
	}

	putchar( '\n' );
}


// Exposes the data for you to look at, like PrintBits does.
// Data will point to the stream.  Returns the length in bits of the stream.
int BitStream::CopyData( unsigned char** _data ) const
{
#ifdef _DEBUG
	RakAssert( numberOfBitsUsed > 0 );
#endif

	*_data = (unsigned char*) rakMalloc_Ex( BITS_TO_BYTES( numberOfBitsUsed ), __FILE__, __LINE__ );
	memcpy( *_data, data, sizeof(unsigned char) * ( BITS_TO_BYTES( numberOfBitsUsed ) ) );
	return numberOfBitsUsed;
}

// Ignore data we don't intend to read
void BitStream::IgnoreBits( const int numberOfBits )
{
	readOffset += numberOfBits;
}

void BitStream::IgnoreBytes( const int numberOfBytes )
{
	IgnoreBits(BYTES_TO_BITS(numberOfBytes));
}

// Move the write pointer to a position on the array.  Dangerous if you don't know what you are doing!
void BitStream::SetWriteOffset( const int offset )
{
	numberOfBitsUsed = offset;
}

/*
int BitStream::GetWriteOffset( void ) const
{
	return numberOfBitsUsed;
}

// Returns the length in bits of the stream
int BitStream::GetNumberOfBitsUsed( void ) const
{
	return GetWriteOffset();
}

// Returns the length in bytes of the stream
int BitStream::GetNumberOfBytesUsed( void ) const
{
	return BITS_TO_BYTES( numberOfBitsUsed );
}

// Returns the number of bits into the stream that we have read
int BitStream::GetReadOffset( void ) const
{
	return readOffset;
}


// Sets the read bit index
void BitStream::SetReadOffset( int newReadOffset )
{
	readOffset=newReadOffset;
}

// Returns the number of bits left in the stream that haven't been read
int BitStream::GetNumberOfUnreadBits( void ) const
{
	return numberOfBitsUsed - readOffset;
}
// Exposes the internal data
unsigned char* BitStream::GetData( void ) const
{
	return data;
}

*/
// If we used the constructor version with copy data off, this makes sure it is set to on and the data pointed to is copied.
void BitStream::AssertCopyData( void )
{
	if ( copyData == false )
	{
		copyData = true;

		if ( numberOfBitsAllocated > 0 )
		{
			unsigned char * newdata = ( unsigned char* ) rakMalloc_Ex( BITS_TO_BYTES( numberOfBitsAllocated ), __FILE__, __LINE__ );
#ifdef _DEBUG

			RakAssert( data );
#endif

			memcpy( newdata, data, BITS_TO_BYTES( numberOfBitsAllocated ) );
			data = newdata;
		}

		else
			data = 0;
	}
}
void BitStream::ReverseBytes(unsigned char *input, unsigned char *output, int length)
{
	for (int i=0; i < length; i++)
		output[i]=input[length-i-1];
}
void BitStream::ReverseBytesInPlace(unsigned char *data, int length)
{
	unsigned char temp;
	int i;
	for (i=0; i < length; i++)
	{
		temp = data[i];
		data[i]=data[length-i-1];
		data[length-i-1]=temp;
	}
}
bool BitStream::DoEndianSwap(void)
{
#ifndef __BITSTREAM_NATIVE_END
	return IsNetworkOrder()==false;
#else
	return false;
#endif
}
bool BitStream::IsBigEndian(void)
{
	return IsNetworkOrder();
}
bool BitStream::IsNetworkOrder(void)
{
	static bool isNetworkOrder=(htonl(12345) == 12345);
	return isNetworkOrder;
}
void BitStream::WriteNormVector( float x, float y, float z ){
#ifdef _DEBUG
	RakAssert(x <= 1.01 && y <= 1.01 && z <= 1.01 && x >= -1.01 && y >= -1.01 && z >= -1.01);
#endif
	if (x>1.0)
		x=1.0;
	if (y>1.0)
		y=1.0;
	if (z>1.0)
		z=1.0;
	if (x<-1.0)
		x=-1.0;
	if (y<-1.0)
		y=-1.0;
	if (z<-1.0)
		z=-1.0;

	Write((bool) (x < 0.0));
	if (y==0.0)
		Write(true);
	else
	{
		Write(false);
		WriteCompressed((float)y);
		//Write((unsigned short)((y+1.0f)*32767.5f));
	}
	if (z==0.0)
		Write(true);
	else
	{
		Write(false);
		WriteCompressed((float)z);
		//Write((unsigned short)((z+1.0f)*32767.5f));
	}
}
void BitStream::WriteVector( float x, float y, float z )
{
	float magnitude = sqrt(x * x + y * y + z * z);
	Write((float)magnitude);
	if (magnitude > 0.0)
	{
		WriteCompressed((float)(x/magnitude));
		WriteCompressed((float)(y/magnitude));
		WriteCompressed((float)(z/magnitude));
	}
}
void BitStream::WriteNormQuat( float w, float x, float y, float z){
	Write((bool)(w<0.0));
	Write((bool)(x<0.0));
	Write((bool)(y<0.0));
	Write((bool)(z<0.0));
	Write((unsigned short)(fabs(x)*65535.0));
	Write((unsigned short)(fabs(y)*65535.0));
	Write((unsigned short)(fabs(z)*65535.0));
	// Leave out w and calculate it on the target
}
void BitStream::WriteOrthMatrix(
					 double m00, double m01, double m02,
					 double m10, double m11, double m12,
					 double m20, double m21, double m22 ){
						 double qw;
						 double qx;
						 double qy;
						 double qz;

#ifdef _MSC_VER
#pragma warning(disable:4100)   // m10, m01 : unreferenced formal parameter
#endif

						 // Convert matrix to quat
						 // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
						 double sum;
						 sum = 1 + m00 + m11 + m22;
						 if (sum < 0.0f) sum=0.0f;
						 qw = sqrt( sum  ) / 2;
						 sum = 1 + m00 - m11 - m22;
						 if (sum < 0.0f) sum=0.0f;
						 qx = sqrt( sum  ) / 2;
						 sum = 1 - m00 + m11 - m22;
						 if (sum < 0.0f) sum=0.0f;
						 qy = sqrt( sum  ) / 2;
						 sum = 1 - m00 - m11 + m22;
						 if (sum < 0.0f) sum=0.0f;
						 qz = sqrt( sum  ) / 2;
						 if (qw < 0.0) qw=0.0;
						 if (qx < 0.0) qx=0.0;
						 if (qy < 0.0) qy=0.0;
						 if (qz < 0.0) qz=0.0;
						 qx = _copysign( qx, m21 - m12 );
						 qy = _copysign( qy, m02 - m20 );
						 qz = _copysign( qz, m10 - m01 );

						 WriteNormQuat(qw,qx,qy,qz);
}
bool BitStream::ReadNormVector( float &x, float &y, float &z ){
	//	unsigned short sy, sz;
	bool yZero, zZero;
	bool xNeg;
	float cy,cz;

	Read(xNeg);

	Read(yZero);
	if (yZero)
		y=0.0;
	else
	{
		ReadCompressed(cy);
		y=cy;
		//Read(sy);
		//y=((float)sy / 32767.5f - 1.0f);
	}

	if (!Read(zZero))
		return false;

	if (zZero)
		z=0.0;
	else
	{
		//	if (!Read(sz))
		//		return false;

		//	z=((float)sz / 32767.5f - 1.0f);
		if (!ReadCompressed(cz))
			return false;
		z=cz;
	}

	x = float (sqrtf((float)1.0 - y*y - z*z));
	if (xNeg)
		x=-x;
	return true;
}
bool BitStream::ReadVector( float x, float y, float z ){
	float magnitude;
	if (!Read(magnitude))
		return false;
	if (magnitude!=0.0)
	{
		float cx,cy,cz;
		ReadCompressed(cx);
		ReadCompressed(cy);
		if (!ReadCompressed(cz))
			return false;
		x=cx;
		y=cy;
		z=cz;
		x*=magnitude;
		y*=magnitude;
		z*=magnitude;
	}
	else
	{
		x=0.0;
		y=0.0;
		z=0.0;
	}
	return true;
}
bool BitStream::ReadNormQuat( double &w, double &x, double &y, double &z){
	bool cwNeg, cxNeg, cyNeg, czNeg;
	unsigned short cx,cy,cz;
	Read(cwNeg);
	Read(cxNeg);
	Read(cyNeg);
	Read(czNeg);
	Read(cx);
	Read(cy);
	if (!Read(cz))
		return false;

	// Calculate w from x,y,z
	x=(double)(cx/65535.0);
	y=(double)(cy/65535.0);
	z=(double)(cz/65535.0);
	if (cxNeg) x=-x;
	if (cyNeg) y=-y;
	if (czNeg) z=-z;
	double difference = 1.0 - x*x - y*y - z*z;
	if (difference < 0.0f)
		difference=0.0f;
	w = (double)(sqrt(difference));
	if (cwNeg)
		w=-w;
	return true;
}
bool BitStream::ReadOrthMatrix(
					float &m00, float &m01, float &m02,
					float &m10, float &m11, float &m12,
					float &m20, float &m21, float &m22 )
{
	double _m00, _m01, _m02, _m10, _m11, _m12, _m20, _m21, _m22;
	bool b=ReadOrthMatrix(
		_m00,  _m01,  _m02,
		_m10,  _m11,  _m12,
		_m20,  _m21,  _m22);
	m00=(float)_m00; m01=(float)_m01; m02=(float)_m02;
	m10=(float)_m10; m11=(float)_m11; m12=(float)_m12;
	m20=(float)_m20; m21=(float)_m21; m22=(float)_m22;
	return b;
}
bool BitStream::ReadOrthMatrix(
					double &m00, double &m01, double &m02,
					double &m10, double &m11, double &m12,
					double &m20, double &m21, double &m22 ){
						float qw,qx,qy,qz;
						if (!ReadNormQuat(qw,qx,qy,qz))
							return false;

						// Quat to orthogonal rotation matrix
						// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
						double sqw = (double)qw*(double)qw;
						double sqx = (double)qx*(double)qx;
						double sqy = (double)qy*(double)qy;
						double sqz = (double)qz*(double)qz;
						m00 =  (sqx - sqy - sqz + sqw); // since sqw + sqx + sqy + sqz =1
						m11 = (-sqx + sqy - sqz + sqw);
						m22 = (-sqx - sqy + sqz + sqw);

						double tmp1 = (double)qx*(double)qy;
						double tmp2 = (double)qz*(double)qw;
						m10 = (2.0 * (tmp1 + tmp2));
						m01 = (2.0 * (tmp1 - tmp2));

						tmp1 = (double)qx*(double)qz;
						tmp2 = (double)qy*(double)qw;
						m20 = (2.0 * (tmp1 - tmp2));
						m02 = (2.0 * (tmp1 + tmp2));
						tmp1 = (double)qy*(double)qz;
						tmp2 = (double)qx*(double)qw;
						m21 = (2.0 * (tmp1 + tmp2));
						m12 = (2.0 * (tmp1 - tmp2));

						return true;
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
