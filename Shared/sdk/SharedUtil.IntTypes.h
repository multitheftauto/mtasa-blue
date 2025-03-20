/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/SharedUtil.IntTypes.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <cstdint>

/*************************************************************************
    Simplification of some 'unsigned' types
*************************************************************************/
//                                                  VS      GCC
//                      Actual sizes:      32bit   64bit   64bit
using ulong = unsigned long;           //  32      32      64
using uint = std::uint32_t;            //  32
using ushort = std::uint16_t;          //  16
using uchar = std::uint8_t;            //  8

using uint64 = std::uint64_t;          //  64
using uint32 = std::uint32_t;          //  32
using uint16 = std::uint16_t;          //  16
using uint8 = std::uint8_t;            //  8

// signed types
using int64 = std::int64_t;            //  64
using int32 = std::int32_t;            //  32
using int16 = std::int16_t;            //  16
using int8 = std::int8_t;              //  8

// Windowsesq types
using BYTE = std::uint8_t;             //  8
using WORD = std::uint16_t;            //  16
using DWORD = unsigned long;           //  32      32      64
using FLOAT = float;                   //  32

// Type: considerations:
// a) long (and therefore DWORD) is 64 bits when compiled using 64 bit GCC
// b) char range can be -127 to 128 or 0 to 255 depending on compiler options/mood
