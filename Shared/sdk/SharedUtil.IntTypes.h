/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

/*************************************************************************
    Simplification of some 'unsigned' types
*************************************************************************/
//                                                  VS      GCC
//                      Actual sizes:      32bit   64bit   64bit
typedef unsigned long  ulong;             //  32      32      64
typedef unsigned int   uint;              //  32
typedef unsigned short ushort;            //  16
typedef unsigned char  uchar;             //  8

typedef unsigned long long uint64;            //  64
typedef unsigned int       uint32;            //  32
typedef unsigned short     uint16;            //  16
typedef unsigned char      uint8;             //  8

// signed types
typedef signed long long int64;            //  64
typedef signed int       int32;            //  32
typedef signed short     int16;            //  16
typedef signed char      int8;             //  8

// Windowsesq types
typedef unsigned char  BYTE;             //  8
typedef unsigned short WORD;             //  16
typedef unsigned long  DWORD;            //  32      32      64
typedef float          FLOAT;            //  32

// Type: considerations:
// a) long (and therefore DWORD) is 64 bits when compiled using 64 bit GCC
// b) char range can be -127 to 128 or 0 to 255 depending on compiler options/mood
