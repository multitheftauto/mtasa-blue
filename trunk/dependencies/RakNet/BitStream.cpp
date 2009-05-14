/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#if defined(_MSC_VER) && _MSC_VER < 1299 // VC6 doesn't support template specialization
#include "BitStream_NoTemplate.cpp"
#else

#include "BitStream.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_XBOX) || defined(X360)
#include "XBOX360Includes.h"
#elif defined(_WIN32)
#include <winsock2.h> // htonl
#include <memory.h>
#include <cmath>
#include <float.h>
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#include "PS3Includes.h"
#else
#include <arpa/inet.h>
#include <memory.h>
#include <cmath>
#include <float.h>
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

BitStream::BitStream( const unsigned int initialBytesToAllocate )
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
		data = ( unsigned char* ) rakMalloc_Ex( (size_t) initialBytesToAllocate, __FILE__, __LINE__ );
		numberOfBitsAllocated = initialBytesToAllocate << 3;
	}
#ifdef _DEBUG
	RakAssert( data );
#endif
	// memset(data, 0, initialBytesToAllocate);
	copyData = true;
}

BitStream::BitStream( unsigned char* _data, const unsigned int lengthInBytes, bool _copyData )
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
				data = ( unsigned char* ) rakMalloc_Ex( (size_t) lengthInBytes, __FILE__, __LINE__ );
			}
#ifdef _DEBUG
			RakAssert( data );
#endif
			memcpy( data, _data, (size_t) lengthInBytes );
		}
		else
			data = 0;
	}
	else
		data = ( unsigned char* ) _data;
}

// Use this if you pass a pointer copy to the constructor (_copyData==false) and want to overallocate to prevent reallocation
void BitStream::SetNumberOfBitsAllocated( const BitSize_t lengthInBits )
{
#ifdef _DEBUG
	RakAssert( lengthInBits >= ( BitSize_t ) numberOfBitsAllocated );
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
void BitStream::Write( const char* input, const unsigned int numberOfBytes )
{
	if (numberOfBytes==0)
		return;

	// Optimization:
	if ((numberOfBitsUsed & 7) == 0)
	{
		AddBitsAndReallocate( BYTES_TO_BITS(numberOfBytes) );
		memcpy(data+BITS_TO_BYTES(numberOfBitsUsed), input, (size_t) numberOfBytes);
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
void BitStream::Write( BitStream *bitStream, BitSize_t numberOfBits )
{
	AddBitsAndReallocate( numberOfBits );
	BitSize_t numberOfBitsMod8;

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
void BitStream::Write( BitStream &bitStream, BitSize_t numberOfBits )
{
	Write(&bitStream, numberOfBits);
}
void BitStream::Write( BitStream &bitStream )
{
	Write(&bitStream);
}
bool BitStream::Read( BitStream *bitStream, BitSize_t numberOfBits )
{
	if (GetNumberOfUnreadBits() < numberOfBits)
		return false;
	bitStream->Write(this, numberOfBits);
	return true;
}
bool BitStream::Read( BitStream *bitStream )
{
	bitStream->Write(this);
	return true;
}
bool BitStream::Read( BitStream &bitStream, BitSize_t numberOfBits )
{
	if (GetNumberOfUnreadBits() < numberOfBits)
		return false;
	bitStream.Write(this, numberOfBits);
	return true;
}
bool BitStream::Read( BitStream &bitStream )
{
	bitStream.Write(this);
	return true;
}

// Read an array or casted stream
bool BitStream::Read( char* output, const unsigned int numberOfBytes )
{
	// Optimization:
	if ((readOffset & 7) == 0)
	{
		if ( readOffset + ( numberOfBytes << 3 ) > numberOfBitsUsed )
			return false;

		// Write the data
		memcpy( output, data + ( readOffset >> 3 ), (size_t) numberOfBytes );

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
	
	BitSize_t numberOfBitsMod8 = numberOfBitsUsed & 7;
	
	if ( numberOfBitsMod8 == 0 )
		data[ numberOfBitsUsed >> 3 ] = 0x80;
	else
		data[ numberOfBitsUsed >> 3 ] |= 0x80 >> ( numberOfBitsMod8 ); // Set the bit to 1
		
	numberOfBitsUsed++;
}

// Returns true if the next data read is a 1, false if it is a 0
bool BitStream::ReadBit( void )
{
	bool result = ( data[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) ) !=0;
	readOffset++;
	return result;
}

// Align the bitstream to the byte boundary and then write the specified number of bits.
// This is faster than WriteBits but wastes the bits to do the alignment and requires you to call
// SetReadToByteAlignment at the corresponding read position
void BitStream::WriteAlignedBytes( const unsigned char* input, const unsigned int numberOfBytesToWrite )
{
	AlignWriteToByteBoundary();
	Write((const char*) input, numberOfBytesToWrite);
}

/// Aligns the bitstream, writes inputLength, and writes input. Won't write beyond maxBytesToWrite
void BitStream::WriteAlignedBytesSafe( const char *input, const unsigned int inputLength, const unsigned int maxBytesToWrite )
{
	if (input==0 || inputLength==0)
	{
		WriteCompressed((unsigned int)0);
		return;
	}
	WriteCompressed(inputLength);
	WriteAlignedBytes((const unsigned char*) input, inputLength < maxBytesToWrite ? inputLength : maxBytesToWrite);
}

// Read bits, starting at the next aligned bits. Note that the modulus 8 starting offset of the
// sequence must be the same as was used with WriteBits. This will be a problem with packet coalescence
// unless you byte align the coalesced packets.
bool BitStream::ReadAlignedBytes( unsigned char* output, const unsigned int numberOfBytesToRead )
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
	memcpy( output, data + ( readOffset >> 3 ), (size_t) numberOfBytesToRead );
	
	readOffset += numberOfBytesToRead << 3;
	
	return true;
}
bool BitStream::ReadAlignedBytesSafe( char *input, int &inputLength, const int maxBytesToRead )
{
	return ReadAlignedBytesSafe(input,(unsigned int&) inputLength,(unsigned int)maxBytesToRead);
}
bool BitStream::ReadAlignedBytesSafe( char *input, unsigned int &inputLength, const unsigned int maxBytesToRead )
{
	if (ReadCompressed(inputLength)==false)
		return false;
	if (inputLength > maxBytesToRead)
		inputLength=maxBytesToRead;
	if (inputLength==0)
		return true;
	return ReadAlignedBytes((unsigned char*) input, inputLength);
}
bool BitStream::ReadAlignedBytesSafeAlloc( char **input, int &inputLength, const unsigned int maxBytesToRead )
{
	return ReadAlignedBytesSafeAlloc(input,(unsigned int&) inputLength, maxBytesToRead);
}
bool BitStream::ReadAlignedBytesSafeAlloc( char **input, unsigned int &inputLength, const unsigned int maxBytesToRead )
{
	rakFree_Ex(*input, __FILE__, __LINE__ );
	*input=0;
	if (ReadCompressed(inputLength)==false)
		return false;
	if (inputLength > maxBytesToRead)
		inputLength=maxBytesToRead;
	if (inputLength==0)
		return true;
	*input = (char*) rakMalloc_Ex( (size_t) inputLength, __FILE__, __LINE__ );
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
void BitStream::WriteBits( const unsigned char* input, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits )
{
	if (numberOfBitsToWrite<=0)
		return;
	
	AddBitsAndReallocate( numberOfBitsToWrite );
	BitSize_t offset = 0;
	unsigned char dataByte;
	BitSize_t numberOfBitsUsedMod8;
	
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
		
		if (numberOfBitsToWrite>=8)
			numberOfBitsToWrite -= 8;
		else
			numberOfBitsToWrite=0;
		
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
	const unsigned int size, const bool unsignedData )
{
	BitSize_t currentByte = ( size >> 3 ) - 1; // PCs
	
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
bool BitStream::ReadBits( unsigned char *output, BitSize_t numberOfBitsToRead, const bool alignBitsToRight )
{
#ifdef _DEBUG
//	RakAssert( numberOfBitsToRead > 0 );
#endif
	if (numberOfBitsToRead<=0)
	  return false;
	
	if ( readOffset + numberOfBitsToRead > numberOfBitsUsed )
		return false;
		
	BitSize_t readOffsetMod8;
	
	BitSize_t offset = 0;
	
	memset( output, 0, (size_t) BITS_TO_BYTES( numberOfBitsToRead ) );
	
	readOffsetMod8 = readOffset & 7;
	
	while ( numberOfBitsToRead > 0 )
	{
		*( output + offset ) |= *( data + ( readOffset >> 3 ) ) << ( readOffsetMod8 ); // First half

		if ( readOffsetMod8 > 0 && numberOfBitsToRead > 8 - ( readOffsetMod8 ) )   // If we have a second half, we didn't read enough bytes in the first half
			*( output + offset ) |= *( data + ( readOffset >> 3 ) + 1 ) >> ( 8 - ( readOffsetMod8 ) ); // Second half (overlaps byte boundary)

		if (numberOfBitsToRead>=8)
		{
			numberOfBitsToRead -= 8;
			readOffset += 8;
			offset++;
		}
		else
		{
			int neg = (int) numberOfBitsToRead - 8;

			if ( neg < 0 )   // Reading a partial byte for the last byte, shift right so the data is aligned on the right
			{

				if ( alignBitsToRight )
					* ( output + offset ) >>= -neg;

				readOffset += 8 + neg;
			}
			else
				readOffset += 8;

			offset++;

			numberOfBitsToRead=0;
		}		
	}
	
	return true;
}

// Assume the input source points to a compressed native type. Decompress and read it
bool BitStream::ReadCompressed( unsigned char* output,
	const unsigned int size, const bool unsignedData )
{
	unsigned int currentByte = ( size >> 3 ) - 1;
	
	
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
void BitStream::AddBitsAndReallocate( const BitSize_t numberOfBitsToWrite )
{
	if (numberOfBitsToWrite <= 0)
		return;

	BitSize_t newNumberOfBitsAllocated = numberOfBitsToWrite + numberOfBitsUsed;
	
	if ( numberOfBitsToWrite + numberOfBitsUsed > 0 && ( ( numberOfBitsAllocated - 1 ) >> 3 ) < ( ( newNumberOfBitsAllocated - 1 ) >> 3 ) )   // If we need to allocate 1 or more new bytes
	{
#ifdef _DEBUG
		// If this assert hits then we need to specify true for the third parameter in the constructor
		// It needs to reallocate to hold all the data and can't do it unless we allocated to begin with
		// Often hits if you call Write or Serialize on a read-only bitstream
		RakAssert( copyData == true );
#endif

		// Less memory efficient but saves on news and deletes
		/// Cap to 1 meg buffer to save on huge allocations
		newNumberOfBitsAllocated = ( numberOfBitsToWrite + numberOfBitsUsed ) * 2;
		if (newNumberOfBitsAllocated - ( numberOfBitsToWrite + numberOfBitsUsed ) > 1048576 )
			newNumberOfBitsAllocated = numberOfBitsToWrite + numberOfBitsUsed + 1048576;

//		BitSize_t newByteOffset = BITS_TO_BYTES( numberOfBitsAllocated );
		// Use realloc and free so we are more efficient than delete and new for resizing
		BitSize_t amountToAllocate = BITS_TO_BYTES( newNumberOfBitsAllocated );
		if (data==(unsigned char*)stackData)
		{
			 if (amountToAllocate > BITSTREAM_STACK_ALLOCATION_SIZE)
			 {
				 data = ( unsigned char* ) rakMalloc_Ex( (size_t) amountToAllocate, __FILE__, __LINE__ );

				 // need to copy the stack data over to our new memory area too
				 memcpy ((void *)data, (void *)stackData, (size_t) BITS_TO_BYTES( numberOfBitsAllocated )); 
			 }
		}
		else
		{
			data = ( unsigned char* ) rakRealloc_Ex( data, (size_t) amountToAllocate, __FILE__, __LINE__ );
		}

#ifdef _DEBUG
		RakAssert( data ); // Make sure realloc succeeded
#endif
		//  memset(data+newByteOffset, 0,  ((newNumberOfBitsAllocated-1)>>3) - ((numberOfBitsAllocated-1)>>3)); // Set the new data block to 0
	}
	
	if ( newNumberOfBitsAllocated > numberOfBitsAllocated )
		numberOfBitsAllocated = newNumberOfBitsAllocated;
}
BitSize_t BitStream::GetNumberOfBitsAllocated(void) const
{
	return numberOfBitsAllocated;
}

// Should hit if reads didn't match writes
void BitStream::AssertStreamEmpty( void )
{
	RakAssert( readOffset == numberOfBitsUsed );
}
void BitStream::PrintBits( char *out ) const
{
	if ( numberOfBitsUsed <= 0 )
	{
		strcpy(out, "No bits\n" );
		return;
	}

	unsigned int strIndex=0;
	for ( BitSize_t counter = 0; counter < BITS_TO_BYTES( numberOfBitsUsed ); counter++ )
	{
		BitSize_t stop;

		if ( counter == ( numberOfBitsUsed - 1 ) >> 3 )
			stop = 8 - ( ( ( numberOfBitsUsed - 1 ) & 7 ) + 1 );
		else
			stop = 0;

		for ( BitSize_t counter2 = 7; counter2 >= stop; counter2-- )
		{
			if ( ( data[ counter ] >> counter2 ) & 1 )
				out[strIndex++]='1';
			else
				out[strIndex++]='0';

			if (counter2==0)
				break;
		}

		out[strIndex++]=' ';
	}

	out[strIndex++]='\n';

	out[strIndex++]=0;
}
void BitStream::PrintBits( void ) const
{
	char out[2048];
	PrintBits(out);
	RAKNET_DEBUG_PRINTF(out);
}
void BitStream::PrintHex( char *out ) const
{
	BitSize_t i;
	for ( i=0; i < GetNumberOfBytesUsed(); i++)
	{
		sprintf(out+i*3, "%02x ", data[i]);
	}
}
void BitStream::PrintHex( void ) const
{
	char out[2048];
	PrintHex(out);
	RAKNET_DEBUG_PRINTF(out);
}

// Exposes the data for you to look at, like PrintBits does.
// Data will point to the stream.  Returns the length in bits of the stream.
BitSize_t BitStream::CopyData( unsigned char** _data ) const
{
#ifdef _DEBUG
	RakAssert( numberOfBitsUsed > 0 );
#endif
	
	*_data = (unsigned char*) rakMalloc_Ex( (size_t) BITS_TO_BYTES( numberOfBitsUsed ), __FILE__, __LINE__ );
	memcpy( *_data, data, sizeof(unsigned char) * (size_t) ( BITS_TO_BYTES( numberOfBitsUsed ) ) );
	return numberOfBitsUsed;
}

// Ignore data we don't intend to read
void BitStream::IgnoreBits( const BitSize_t numberOfBits )
{
	readOffset += numberOfBits;
}

void BitStream::IgnoreBytes( const unsigned int numberOfBytes )
{
	IgnoreBits(BYTES_TO_BITS(numberOfBytes));
}

// Move the write pointer to a position on the array.  Dangerous if you don't know what you are doing!
// Doesn't work with non-aligned data!
void BitStream::SetWriteOffset( const BitSize_t offset )
{
	numberOfBitsUsed = offset;
}

/*
BitSize_t BitStream::GetWriteOffset( void ) const
{
	return numberOfBitsUsed;
}

// Returns the length in bits of the stream
BitSize_t BitStream::GetNumberOfBitsUsed( void ) const
{
	return GetWriteOffset();
}

// Returns the length in bytes of the stream
BitSize_t BitStream::GetNumberOfBytesUsed( void ) const
{
	return BITS_TO_BYTES( numberOfBitsUsed );
}

// Returns the number of bits into the stream that we have read
BitSize_t BitStream::GetReadOffset( void ) const
{
	return readOffset;
}


// Sets the read bit index
void BitStream::SetReadOffset( const BitSize_t newReadOffset )
{
	readOffset=newReadOffset;
}

// Returns the number of bits left in the stream that haven't been read
BitSize_t BitStream::GetNumberOfUnreadBits( void ) const
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
			unsigned char * newdata = ( unsigned char* ) rakMalloc_Ex( (size_t) BITS_TO_BYTES( numberOfBitsAllocated ), __FILE__, __LINE__ );
#ifdef _DEBUG
			
			RakAssert( data );
#endif
			
			memcpy( newdata, data, (size_t) BITS_TO_BYTES( numberOfBitsAllocated ) );
			data = newdata;
		}
		
		else
			data = 0;
	}
}
void BitStream::ReverseBytes(unsigned char *input, unsigned char *output, const unsigned int length)
{
	for (BitSize_t i=0; i < length; i++)
		output[i]=input[length-i-1];
}
void BitStream::ReverseBytesInPlace(unsigned char *data,const unsigned int length)
{
	unsigned char temp;
	BitSize_t i;
	for (i=0; i < (length>>1); i++)
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
#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
	return true;
#else
	static bool isNetworkOrder=(htonl(12345) == 12345);
	return isNetworkOrder;
#endif
}
bool BitStream::Read(char *var)
{
	return RakString::Deserialize(var,this);
}
bool BitStream::Read(unsigned char *var)
{
	return RakString::Deserialize((char*) var,this);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // #if _MSC_VER < 1299 
