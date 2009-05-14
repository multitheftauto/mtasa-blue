/// \file
/// \brief \b [Internal] AES encoding / decoding
/// rijndael-alg-fst.h   v2.0   August '99
/// Optimised ANSI C code
///  taken from the 'aescrypt' project: www.sf.net/projects/aescrypt
///  See LICENSE-EST for the license applicable to this file


/// \note  Although the routines claim to support 192 and 256 bit blocks,
/// don't take your chances - stick to the 128 bit (16 byte) blocks unless
/// you've run tests to prove that 192 and 256 are correctly supported.
/// - Cirilo


#include <stdio.h>

#ifndef __RIJNDAEL_ALG_H
#define __RIJNDAEL_ALG_H

#define MAXKC				(256/32)
#define MAXROUNDS			14

typedef unsigned char		word8;	
typedef unsigned short		word16;	
typedef unsigned int		word32;

int rijndaelKeySched (word8 k[MAXKC][4], int keyBits,  
		word8 rk[MAXROUNDS+1][4][4]);
int rijndaelKeyEnctoDec (int keyBits, word8 W[MAXROUNDS+1][4][4]);
int rijndaelEncrypt (word8 a[16], word8 b[16], 
		word8 rk[MAXROUNDS+1][4][4]);
int rijndaelEncryptRound (word8 a[4][4],  
		word8 rk[MAXROUNDS+1][4][4], int rounds);
int rijndaelDecrypt (word8 a[16], word8 b[16],
		word8 rk[MAXROUNDS+1][4][4]);
int rijndaelDecryptRound (word8 a[4][4],  
		word8 rk[MAXROUNDS+1][4][4], int rounds);

#endif

// End of algorithm headers.  begin the AES API header defs


#ifndef __RIJNDAEL_API_H
#define __RIJNDAEL_API_H

// rijndael-api-fst.h   v2.0   August '99
// Optimised ANSI C code


//  Defines:
// Add any additional defines you need


#define     DIR_ENCRYPT     0    //  Are we encrpyting? 
#define     DIR_DECRYPT     1    //  Are we decrpyting? 
#define     MODE_ECB        1    //  Are we ciphering in ECB mode?  
#define     MODE_CBC        2    //  Are we ciphering in CBC mode?  
#define     MODE_CFB1       3    //  Are we ciphering in 1-bit CFB mode?
#ifndef TRUE
#define     TRUE            1
#endif
#ifndef FALSE
#define     FALSE           0
#endif
#define	BITSPERBLOCK		128		// Default number of bits in a cipher block

//  Error Codes - CHANGE POSSIBLE: inclusion of additional error codes 
#define     BAD_KEY_DIR        -1  //  Key direction is invalid, e.g., unknown value
#define     BAD_KEY_MAT        -2  //  Key material not of correct length
#define     BAD_KEY_INSTANCE   -3  //  Key passed is not valid 
#define     BAD_CIPHER_MODE    -4  //  Params struct passed to cipherInit invalid
#define     BAD_CIPHER_STATE   -5  //  Cipher in wrong state (e.g., not initialized)
#define     BAD_BLOCK_LENGTH   -6 
#define     BAD_CIPHER_INSTANCE   -7 


//  CHANGE POSSIBLE:  inclusion of algorithm specific defines 
// 14.Dec.2005 Cirilo: keys are now unsigned char rather than hex (ASCII)
#define     MAX_KEY_SIZE	32  // # of unsigned char's needed to represent a key
#define     MAX_IV_SIZE		16  // # bytes needed to represent an IV 

//  Typedefs:
// Typedef'ed data storage elements.  Add any algorithm specific parameters at the bottom of the structs as appropriate.

typedef    unsigned char    BYTE;

//  The structure for key information
typedef struct {
      BYTE  direction;	///  Key used for encrypting or decrypting?
      int   keyLen;	///  Length of the key 
      char  keyMaterial[MAX_KEY_SIZE+1];  ///  Raw key data in ASCII, e.g., user input or KAT values
      ///  The following parameters are algorithm dependent, replace or add as necessary 
      int   blockLen;   /// block length
      word8 keySched[MAXROUNDS+1][4][4];	/// key schedule
      } keyInstance;

//  The structure for cipher information
typedef struct {  // changed order of the components
      BYTE  mode;            /// MODE_ECB, MODE_CBC, or MODE_CFB1
      BYTE  IV[MAX_IV_SIZE]; /// A possible Initialization Vector for ciphering
      //  Add any algorithm specific parameters needed here 
      int   blockLen;    	/// Sample: Handles non-128 bit block sizes	(if available)
      } cipherInstance;


//  Function protoypes 
//  CHANGED: makeKey(): parameter blockLen added this parameter is absolutely necessary if you want to
// setup the round keys in a variable block length setting 
// cipherInit(): parameter blockLen added (for obvious reasons)		

int makeKey(keyInstance *key, BYTE direction, int keyLen, char *keyMaterial);

int cipherInit(cipherInstance *cipher, BYTE mode, char *IV);

int blockEncrypt(cipherInstance *cipher, keyInstance *key, BYTE *input, 
			int inputLen, BYTE *outBuffer);

int blockDecrypt(cipherInstance *cipher, keyInstance *key, BYTE *input,
			int inputLen, BYTE *outBuffer);
int cipherUpdateRounds(cipherInstance *cipher, keyInstance *key, BYTE *input, 
                        int inputLen, BYTE *outBuffer, int Rounds);


#endif // __RIJNDAEL_API_H  
