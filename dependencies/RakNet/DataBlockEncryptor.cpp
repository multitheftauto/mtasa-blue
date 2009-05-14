/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "DataBlockEncryptor.h"
#include "CheckSum.h"
#include "GetTime.h"
#include "Rand.h"
#include "RakAssert.h"
#include <string.h>
#include "Rijndael.h"
//#include "Types.h"

DataBlockEncryptor::DataBlockEncryptor()
{
	keySet = false;
}

DataBlockEncryptor::~DataBlockEncryptor()
{}

bool DataBlockEncryptor::IsKeySet( void ) const
{
	return keySet;
}

void DataBlockEncryptor::SetKey( const unsigned char key[ 16 ] )
{
	keySet = true;
	//secretKeyAES128.set_key( key );
	makeKey(&keyEncrypt, DIR_ENCRYPT, 16, (char*)key);
	makeKey(&keyDecrypt, DIR_DECRYPT, 16, (char*)key);
	cipherInit(&cipherInst, MODE_ECB, 0); // ECB is not secure except that I chain manually farther down.
}

void DataBlockEncryptor::UnsetKey( void )
{
	keySet = false;
}

void DataBlockEncryptor::Encrypt( unsigned char *input, unsigned int inputLength, unsigned char *output, unsigned int *outputLength, RakNetRandom *rnr )
{
	unsigned index, byteIndex, lastBlock;
	unsigned int checkSum;
	unsigned char paddingBytes;
	unsigned char encodedPad;
	unsigned char randomChar;
	CheckSum checkSumCalculator;

#ifdef _DEBUG

	RakAssert( keySet );
#endif

	RakAssert( input && inputLength );


	// randomChar will randomize the data so the same data sent twice will not look the same
	randomChar = (unsigned char) rnr->RandomMT();

	// 16-(((x-1) % 16)+1)

	// # of padding bytes is 16 -(((input_length + extra_data -1) % 16)+1)
	paddingBytes = (unsigned char) ( 16 - ( ( ( inputLength + sizeof( randomChar ) + sizeof( checkSum ) + sizeof( encodedPad ) - 1 ) % 16 ) + 1 ) );

	// Randomize the pad size variable
	encodedPad = (unsigned char) rnr->RandomMT();
	encodedPad <<= 4;
	encodedPad |= paddingBytes;

	*outputLength = inputLength + sizeof( randomChar ) + sizeof( checkSum ) + sizeof( encodedPad ) + paddingBytes;

	// Write the data first, in case we are overwriting ourselves

	if ( input == output )
		memmove( output + sizeof( checkSum ) + sizeof( randomChar ) + sizeof( encodedPad ) + paddingBytes, input, inputLength );
	else
		memcpy( output + sizeof( checkSum ) + sizeof( randomChar ) + sizeof( encodedPad ) + paddingBytes, input, inputLength );

	// Write the random char
	memcpy( output + sizeof( checkSum ), ( char* ) & randomChar, sizeof( randomChar ) );

	// Write the pad size variable
	memcpy( output + sizeof( checkSum ) + sizeof( randomChar ), ( char* ) & encodedPad, sizeof( encodedPad ) );

	// Write the padding
	for ( index = 0; index < paddingBytes; index++ )
		*( output + sizeof( checkSum ) + sizeof( randomChar ) + sizeof( encodedPad ) + index ) = (unsigned char) rnr->RandomMT();

	// Calculate the checksum on the data
	checkSumCalculator.Add( output + sizeof( checkSum ), inputLength + sizeof( randomChar ) + sizeof( encodedPad ) + paddingBytes );

	checkSum = checkSumCalculator.Get();

	// Write checksum
#ifdef HOST_ENDIAN_IS_BIG
	output[0] = checkSum&0xFF;
	output[1] = (checkSum>>8)&0xFF;
	output[2] = (checkSum>>16)&0xFF;
	output[3] = (checkSum>>24)&0xFF;
#else
	memcpy( output, ( char* ) & checkSum, sizeof( checkSum ) );
#endif

	// AES on the first block
//	secretKeyAES128.encrypt16( output );
	blockEncrypt(&cipherInst, &keyEncrypt, output, 16, output);

	lastBlock = 0;

	// Now do AES on every other block from back to front
	for ( index = *outputLength - 16; index >= 16; index -= 16 )
	{
		for ( byteIndex = 0; byteIndex < 16; byteIndex++ )
			output[ index + byteIndex ] ^= output[ lastBlock + byteIndex ];

		//secretKeyAES128.encrypt16( output + index );
		blockEncrypt(&cipherInst, &keyEncrypt, output+index, 16, output+index);

		lastBlock = index;
	}
}

bool DataBlockEncryptor::Decrypt( unsigned char *input, unsigned int inputLength, unsigned char *output, unsigned int *outputLength )
{
	unsigned index, byteIndex, lastBlock;
	unsigned int checkSum;
	unsigned char paddingBytes;
	unsigned char encodedPad;
	unsigned char randomChar;
	CheckSum checkSumCalculator;
#ifdef _DEBUG

	RakAssert( keySet );
#endif

	if ( input == 0 || inputLength < 16 || ( inputLength % 16 ) != 0 )
	{
		return false;
	}

	// Unchain in reverse order
	for ( index = 16; index <= inputLength - 16;index += 16 )
	{
	//	secretKeyAES128.decrypt16( input + index );
		blockDecrypt(&cipherInst, &keyDecrypt, input + index, 16, output + index);

		for ( byteIndex = 0; byteIndex < 16; byteIndex++ )
		{
			if ( index + 16 == ( unsigned ) inputLength )
				output[ index + byteIndex ] ^= input[ byteIndex ];
			else
				output[ index + byteIndex ] ^= input[ index + 16 + byteIndex ];
		}

		lastBlock = index;
	};

	// Decrypt the first block
	//secretKeyAES128.decrypt16( input );
	blockDecrypt(&cipherInst, &keyDecrypt, input, 16, output);

	// Read checksum
#ifdef HOST_ENDIAN_IS_BIG
	checkSum = (unsigned int)output[0] | (unsigned int)(output[1]<<8) |
		(unsigned int)(output[2]<<16)|(unsigned int)(output[3]<<24);
#else
	memcpy( ( char* ) & checkSum, output, sizeof( checkSum ) );
#endif

	// Read the pad size variable
	memcpy( ( char* ) & encodedPad, output + sizeof( randomChar ) + sizeof( checkSum ), sizeof( encodedPad ) );

	// Ignore the high 4 bytes
	paddingBytes = encodedPad & 0x0F;


	// Get the data length
	*outputLength = inputLength - sizeof( randomChar ) - sizeof( checkSum ) - sizeof( encodedPad ) - paddingBytes;

	// Calculate the checksum on the data.
	checkSumCalculator.Add( output + sizeof( checkSum ), *outputLength + sizeof( randomChar ) + sizeof( encodedPad ) + paddingBytes );

	if ( checkSum != checkSumCalculator.Get() )
		return false;

	// Read the data
	//if ( input == output )
		memmove( output, output + sizeof( randomChar ) + sizeof( checkSum ) + sizeof( encodedPad ) + paddingBytes, *outputLength );
	//else
	//	memcpy( output, input + sizeof( randomChar ) + sizeof( checkSum ) + sizeof( encodedPad ) + paddingBytes, *outputLength );

	return true;
}
