/*
 * Discord erlpack - tidied up for D++, Craig Edwards 2021.
 * 
 * MessagePack system dependencies modified for erlpack.
 * 
 * Copyright (C) 2008-2010 FURUHASHI Sadayuki
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#if defined(__linux__)
#include <endian.h>
#endif

#ifdef _WIN32

#ifdef __cplusplus
/* numeric_limits<T>::min,max */
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#endif

#else
#include <arpa/inet.h>  /* __BYTE_ORDER */
#endif

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __LITTLE_ENDIAN__
#elif __BYTE_ORDER == __BIG_ENDIAN
#define __BIG_ENDIAN__
#elif _WIN32
#define __LITTLE_ENDIAN__
#endif
#endif


#ifdef __LITTLE_ENDIAN__

#ifdef _WIN32
#  if defined(ntohs)
#    define etf_byte_order_16(x) ntohs(x)
#  elif defined(_byteswap_ushort) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#    define etf_byte_order_16(x) ((uint16_t)_byteswap_ushort((unsigned short)x))
#  else
#    define etf_byte_order_16(x) ( \
        ((((uint16_t)x) <<  8) ) | \
        ((((uint16_t)x) >>  8) ) )
#  endif
#else
#  define etf_byte_order_16(x) ntohs(x)
#endif

#ifdef _WIN32
#  if defined(ntohl)
#    define etf_byte_order_32(x) ntohl(x)
#  elif defined(_byteswap_ulong) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#    define etf_byte_order_32(x) ((uint32_t)_byteswap_ulong((unsigned long)x))
#  else
#    define etf_byte_order_32(x) \
        ( ((((uint32_t)x) << 24)               ) | \
          ((((uint32_t)x) <<  8) & 0x00ff0000U ) | \
          ((((uint32_t)x) >>  8) & 0x0000ff00U ) | \
          ((((uint32_t)x) >> 24)               ) )
#  endif
#else
#  define etf_byte_order_32(x) ntohl(x)
#endif

#if defined(_byteswap_uint64) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#  define etf_byte_order_64(x) (_byteswap_uint64(x))
#elif defined(bswap_64)
#  define etf_byte_order_64(x) bswap_64(x)
#elif defined(__DARWIN_OSSwapInt64)
#  define etf_byte_order_64(x) __DARWIN_OSSwapInt64(x)
#elif defined(__linux__)
#  define etf_byte_order_64(x) be64toh(x)
#else
#  define etf_byte_order_64(x) \
    ( ((((uint64_t)x) << 56)                         ) | \
      ((((uint64_t)x) << 40) & 0x00ff000000000000ULL ) | \
      ((((uint64_t)x) << 24) & 0x0000ff0000000000ULL ) | \
      ((((uint64_t)x) <<  8) & 0x000000ff00000000ULL ) | \
      ((((uint64_t)x) >>  8) & 0x00000000ff000000ULL ) | \
      ((((uint64_t)x) >> 24) & 0x0000000000ff0000ULL ) | \
      ((((uint64_t)x) >> 40) & 0x000000000000ff00ULL ) | \
      ((((uint64_t)x) >> 56)                         ) )
#endif

#else
#define etf_byte_order_16(x) (x)
#define etf_byte_order_32(x) (x)
#define etf_byte_order_64(x) (x)
#endif

#define store_16_bits(to, num) \
    do { uint16_t val = etf_byte_order_16(num); memcpy(to, &val, 2); } while(0)
#define store_32_bits(to, num) \
    do { uint32_t val = etf_byte_order_32(num); memcpy(to, &val, 4); } while(0)
#define store_64_bits(to, num) \
    do { uint64_t val = etf_byte_order_64(num); memcpy(to, &val, 8); } while(0)
