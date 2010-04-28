/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/utils/CMD5Hasher.cpp
*  PURPOSE:     MD5 hasher utility class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CMD5Hasher.h"
#include <cstdio>
#include <cstring>

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

CMD5Hasher::CMD5Hasher ( void )
{

}


CMD5Hasher::~CMD5Hasher ( void )
{

}


bool CMD5Hasher::Calculate ( const char* szFilename, MD5& md5Result )
{
    //CRYPT_START
    // Try to load the file
    FILE* pFile = fopen ( szFilename, "rb" );
    if ( pFile )
    {
        // Init
        Init ();

        // Hash it
        unsigned char Buffer [1024];
        while ( unsigned int uiRead = static_cast < unsigned int > ( fread ( Buffer, 1, 1024, pFile ) ) )
        {
            Update ( Buffer, uiRead );
        }

        // Finalize
        Finalize ();

        // Close it
        fclose ( pFile );

        // Success
        memcpy ( md5Result, m_digest, 16 );
        return true;
    }

    // Failed
    return false;
    //CRYPT_END
}


bool CMD5Hasher::Calculate ( const void* pBuffer, size_t sizeLength, MD5& md5Result )
{
    //CRYPT_START
    // Hash it
    Init ();
    Update ( (unsigned char*) pBuffer, (unsigned int)sizeLength );
    Finalize ();

    // Copy out the MD5 and return success
    memcpy ( md5Result, m_digest, 16 );
    return true;
    //CRYPT_END
}


void CMD5Hasher::ConvertToHex ( const MD5& Input, char* pBuffer )
{
    for ( int i = 0; i < 16; i++ )
    {
        sprintf ( pBuffer+i*2, "%02X", Input [i] );
    }

    pBuffer [32] = '\0';
}


void CMD5Hasher::Init ( void )
{
  // Nothing counted, so count=0
  m_count[0] = 0;
  m_count[1] = 0;

  // Load magic initialization constants.
  m_state[0] = 0x67452301;
  m_state[1] = 0xefcdab89;
  m_state[2] = 0x98badcfe;
  m_state[3] = 0x10325476;
}


void CMD5Hasher::Update (unsigned char *input, unsigned int input_length) {

    //CRYPT_START
    unsigned long input_index, buffer_index;
    unsigned long buffer_space;                // how much space is left in buffer

    // Compute number of bytes mod 64
    buffer_index = (unsigned int)((m_count[0] >> 3) & 0x3F);

    // Update number of bits
    if (  (m_count[0] += ((unsigned long) input_length << 3))<((unsigned long) input_length << 3) )
    {
        m_count[1]++;
    }

    m_count[1] += ((unsigned long)input_length >> 29);


    buffer_space = 64 - buffer_index;  // how much space is left in buffer

    // Transform as many times as possible.
    if (input_length >= buffer_space) // ie. we have enough to fill the buffer
    {
        // fill the rest of the buffer and transform
        memcpy ( m_buffer + buffer_index, input, buffer_space );
        Transform ( m_buffer );

        // now, transform each 64-byte piece of the input, bypassing the buffer
        for (input_index = buffer_space; input_index + 63 < input_length; input_index += 64)
        {
            Transform (input+input_index);
        }

        buffer_index = 0;  // so we can buffer remaining
    }
    else
    {
        input_index = 0;     // so we can buffer the whole input
    }

    // and here we do the buffering:
    memcpy ( m_buffer + buffer_index, input + input_index, input_length - input_index );
    //CRYPT_END
}


void CMD5Hasher::Finalize ( void )
{
  unsigned char bits[8];
  unsigned int index, padLen;
  static unsigned char PADDING[64]={
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

  // Save number of bits
  Encode (bits, m_count, 8);

  // Pad out to 56 mod 64.
  index = (unsigned long) ((m_count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  Update (PADDING, padLen);

  // Append length (before padding)
  Update (bits, 8);

  // Store state in digest
  Encode ( m_digest, m_state, 16 );

  // Zeroize sensitive information
  memset ( m_buffer, 0, sizeof (*m_buffer) );
}


void CMD5Hasher::Transform ( unsigned char block [64] )
{
    unsigned long a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3], x[16];

    Decode (x, block, 64);

    // Round 1
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    // Round 2
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    // Round 3
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    // Round 4
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;

    // Zeroize sensitive information.
    memset ( (unsigned char*) x, 0, sizeof (x) );
}


void CMD5Hasher::Encode (unsigned char *output, unsigned long *input, unsigned long len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    output[j]   = (unsigned char)  (input[i] & 0xff);
    output[j+1] = (unsigned char) ((input[i] >> 8) & 0xff);
    output[j+2] = (unsigned char) ((input[i] >> 16) & 0xff);
    output[j+3] = (unsigned char) ((input[i] >> 24) & 0xff);
  }
}


void CMD5Hasher::Decode (unsigned long *output, unsigned char *input, unsigned long len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((unsigned long)input[j]) | (((unsigned long)input[j+1]) << 8) |
      (((unsigned long)input[j+2]) << 16) | (((unsigned long)input[j+3]) << 24);
}


inline unsigned long CMD5Hasher::RotateLeft (unsigned long x, unsigned long n )
{
  return (x << n) | (x >> (32-n));
}


inline unsigned long CMD5Hasher::F ( unsigned long x, unsigned long y, unsigned long z )
{
    return (x & y) | (~x & z);
}


inline unsigned long CMD5Hasher::G ( unsigned long x, unsigned long y, unsigned long z )
{
    return (x & z) | (y & ~z);
}


inline unsigned long CMD5Hasher::H ( unsigned long x, unsigned long y, unsigned long z )
{
    return x ^ y ^ z;
}


inline unsigned long CMD5Hasher::I ( unsigned long x, unsigned long y, unsigned long z )
{
    return y ^ (x | ~z);
}


inline void CMD5Hasher::FF ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long  s, unsigned long ac )
{
    a += F(b, c, d) + x + ac;
    a = RotateLeft (a, s) +b;
}


inline void CMD5Hasher::GG ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
{
    a += G(b, c, d) + x + ac;
    a = RotateLeft (a, s) +b;
}


inline void CMD5Hasher::HH ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
{
    a += H(b, c, d) + x + ac;
    a = RotateLeft (a, s) +b;
}


inline void CMD5Hasher::II ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
{
    a += I(b, c, d) + x + ac;
    a = RotateLeft (a, s) +b;
}
