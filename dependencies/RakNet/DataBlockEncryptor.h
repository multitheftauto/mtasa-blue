/// \file
/// \brief \b [Internal] Encrypts and decrypts data blocks.  Used as part of secure connections.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __DATA_BLOCK_ENCRYPTOR_H
#define __DATA_BLOCK_ENCRYPTOR_H

#include "Rijndael.h"
#include "RakMemoryOverride.h"
class RakNetRandom;

/// Encrypts and decrypts data blocks.
class DataBlockEncryptor
{

public:
	
	/// Constructor
	DataBlockEncryptor();
	
	/// Destructor
	~DataBlockEncryptor();
	
	/// \return true if SetKey has been called previously 
	bool IsKeySet( void ) const;
	
	/// Set the encryption key 
	/// \param[in] key The new encryption key 
	void SetKey( const unsigned char key[ 16 ] );
	
	/// Unset the encryption key 
	void UnsetKey( void );
	
	/// Encryption adds 6 data bytes and then pads the number of bytes to be a multiple of 16.  Output should be large enough to hold this.
	/// Output can be the same memory block as input
	/// \param[in] input the input buffer to encrypt 
	/// \param[in] inputLength the size of the @em input buffer 
	/// \param[in] output the output buffer to store encrypted data 
	/// \param[in] outputLength the size of the output buffer 
	void Encrypt( unsigned char *input, unsigned int inputLength, unsigned char *output, unsigned int *outputLength, RakNetRandom *rnr );
	
	/// Decryption removes bytes, as few as 6.  Output should be large enough to hold this.
	/// Output can be the same memory block as input
	/// \param[in] input the input buffer to decrypt 
	/// \param[in] inputLength the size of the @em input buffer 
	/// \param[in] output the output buffer to store decrypted data 
	/// \param[in] outputLength the size of the @em output buffer 
	/// \return False on bad checksum or input, true on success
	bool Decrypt( unsigned char *input, unsigned int inputLength, unsigned char *output, unsigned int *outputLength );
	
protected:
	
 	keyInstance keyEncrypt;
	keyInstance keyDecrypt;
	cipherInstance cipherInst;
	bool keySet;
};

#endif
