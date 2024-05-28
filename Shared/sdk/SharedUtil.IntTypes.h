/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <cstdint>

/*************************************************************************
    Simplification of some 'unsigned' types
*************************************************************************/
//                                                  VS      GCC
//                      Actual sizes:      32bit   64bit   64bit
using ulong = unsigned long;       //       32      32      64
using uint = std::uint32_t;        //       32      32      32
using ushort = std::uint16_t;      //       16      16      16
using uchar = std::uint8_t;        //        8       8       8

using uint64 = std::uint64_t;      //       64      64      64
using uint32 = std::uint32_t;      //       32      32      32
using uint16 = std::uint16_t;      //       16      16      16
using uint8 = std::uint8_t;        //        8       8       8

// signed types
using int64 = std::int64_t;        //       64
using int32 = std::int32_t;        //       32
using int16 = std::int16_t;        //       16
using int8 = std::int8_t;          //        8

// Windowsesq types
// DWORD must be set to uint32 and not unsigned long
// due to the nature of DWORD ("A DWORD is a 32-bit unsigned integer")
using DWORD = std::uint32_t;       //       32      32      32
using WORD = std::uint16_t;        //       16      16      16
using BYTE = std::uint8_t;         //        8       8       8
using FLOAT = float;

using AnimationId = std::uint32_t;
using AssocGroupId = std::uint32_t;
