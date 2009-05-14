#ifndef PLATFORM_HPP
#define PLATFORM_HPP


//// Platform tweaks ////

#include "RakNetTypes.h"

#ifdef _WIN32_WCE //makslane
#	define _byteswap_ushort(s) ((s >> 8) | (s << 8))
#	define _byteswap_ulong(s) (byteswap_ushort(s&0xffff)<<16) | (byteswap_ushort(s>>16))
#	define byteswap_ushort _byteswap_ushort
#	define byteswap_ulong _byteswap_ulong
#endif

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
# if defined(__sparc) || defined(__sparc__) || defined(__powerpc__) || \
	defined(__ppc__) || defined(__hppa) || defined(_MIPSEB) || defined(_POWER) || \
	defined(_M_PPC) || defined(_M_MPPC) || defined(_M_MRX000) || \
	defined(__POWERPC) || defined(m68k) || defined(powerpc) || \
	defined(sel) || defined(pyr) || defined(mc68000) || defined(is68k) || \
	defined(tahoe) || defined(ibm032) || defined(ibm370) || defined(MIPSEB) || \
	defined(__convex__) || defined(DGUX) || defined(hppa) || defined(apollo) || \
	defined(_CRAY) || defined(__hp9000) || defined(__hp9000s300) || defined(_AIX) || \
	defined(__AIX) || defined(__pyr__) || defined(hp9000s700) || defined(_IBMR2)
#  define BIG_ENDIAN
# elif defined(__i386__) || defined(i386) || defined(intel) || defined(_M_IX86) || \
	  defined(__alpha__) || defined(__alpha) || defined(__ia64) || defined(__ia64__) || \
	  defined(_M_ALPHA) || defined(ns32000) || defined(__ns32000__) || defined(sequent) || \
	  defined(MIPSEL) || defined(_MIPSEL) || defined(sun386) || defined(__sun386__) || \
	  defined(x86_64) || defined(__x86_64)
#  define LITTLE_ENDIAN
# else
#  error "Add your platform to the list"
# endif
#endif


//// Compiler tweaks ////

// Structure packing syntax
#if defined(__GNUC__)
#ifndef PACKED
# define PACKED __attribute__((__packed__))
#endif
# define ASSEMBLY_ATT_SYNTAX
# define ASSEMBLY_BLOCK asm
#elif defined(_WIN32)
# define PACKED
# define PRAGMA_PACK /* pragma push/pop */
#ifndef DEBUG
# define DEBUG _DEBUG
#endif
# if !defined(_M_X64)
#  define ASSEMBLY_INTEL_SYNTAX
#  define ASSEMBLY_BLOCK __asm
# endif
#elif defined(__BORLANDC__) // Borland
# define ASSEMBLY_INTEL_SYNTAX
# define ASSEMBLY_BLOCK _asm
#else
# error "Fix code for your compiler's packing syntax"
#endif

// Stringize
#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X


//// Elementary types ////

struct u128 { uint32_t u[4]; };

union Float32 {
	float f;
	uint32_t i;

	Float32(float n) { f = n; }
	Float32(uint32_t n) { i = n; }
};


//// Coordinate system ////

// Order of vertices in any quad
enum QuadCoords
{
	QUAD_UL, // upper left  (0,0)
	QUAD_LL, // lower left  (0,1)
	QUAD_LR, // lower right (1,1)
	QUAD_UR, // upper right (1,0)
};


//// Rotation macros ////

#ifndef ROL8
#define ROL8(n, r)  ( ((uint8_t)(n) << (r)) | ((uint8_t)(n) >> ( 8 - (r))) ) /* only works for uint8_t */
#endif
#ifndef ROR8
#define ROR8(n, r)  ( ((uint8_t)(n) >> (r)) | ((uint8_t)(n) << ( 8 - (r))) ) /* only works for uint8_t */
#endif
#ifndef ROL16
#define ROL16(n, r) ( ((uint16_t)(n) << (r)) | ((uint16_t)(n) >> (16 - (r))) ) /* only works for uint16_t */
#endif
#ifndef ROR16
#define ROR16(n, r) ( ((uint16_t)(n) >> (r)) | ((uint16_t)(n) << (16 - (r))) ) /* only works for uint16_t */
#endif
#ifndef ROL32
#define ROL32(n, r) ( ((uint32_t)(n) << (r)) | ((uint32_t)(n) >> (32 - (r))) ) /* only works for uint32_t */
#endif
#ifndef ROR32
#define ROR32(n, r) ( ((uint32_t)(n) >> (r)) | ((uint32_t)(n) << (32 - (r))) ) /* only works for uint32_t */
#endif
#ifndef ROL64
#define ROL64(n, r) ( ((uint64_t)(n) << (r)) | ((uint64_t)(n) >> (64 - (r))) ) /* only works for uint64_t */
#endif
#ifndef ROR64
#define ROR64(n, r) ( ((uint64_t)(n) >> (r)) | ((uint64_t)(n) << (64 - (r))) ) /* only works for uint64_t */
#endif

//// Byte-order swapping ////

#ifndef BOSWAP16
#define BOSWAP16(n) ROL16(n, 8)
#endif
#ifndef BOSWAP32
#define BOSWAP32(n) ( (ROL32(n, 8) & 0x00ff00ff) | (ROL32(n, 24) & 0xff00ff00) )
#endif
#ifndef BOSWAP64
#define BOSWAP64(n) ( ((uint64_t)BOSWAP32((uint32_t)n) << 32) | BOSWAP32((uint32_t)(n >> 32)) )
#endif

//// Miscellaneous bitwise macros ////

#define BITCLRHI8(reg, count) ((uint8_t)((uint8_t)(reg) << (count)) >> (count)) /* sets to zero a number of high bits in a byte */
#define BITCLRLO8(reg, count) ((uint8_t)((uint8_t)(reg) >> (count)) << (count)) /* sets to zero a number of low bits in a byte */
#define BITCLRHI16(reg, count) ((uint16_t)((uint16_t)(reg) << (count)) >> (count)) /* sets to zero a number of high bits in a 16-bit word */
#define BITCLRLO16(reg, count) ((uint16_t)((uint16_t)(reg) >> (count)) << (count)) /* sets to zero a number of low bits in a 16-bit word */
#define BITCLRHI32(reg, count) ((uint32_t)((uint32_t)(reg) << (count)) >> (count)) /* sets to zero a number of high bits in a 32-bit word */
#define BITCLRLO32(reg, count) ((uint32_t)((uint32_t)(reg) >> (count)) << (count)) /* sets to zero a number of low bits in a 32-bit word */


//// Integer macros ////

#define AT_LEAST_2_BITS(n) ( (n) & ((n) - 1) )
#define LEAST_SIGNIFICANT_BIT(n) ( (n) & -(n) ) /* 0 -> 0 */
#define IS_POWER_OF_2(n) ( n && !AT_LEAST_2_BITS(n) )

// Bump 'n' to the next unit of 'width'
// 0=CEIL_UNIT(0, 16), 1=CEIL_UNIT(1, 16), 1=CEIL_UNIT(16, 16), 2=CEIL_UNIT(17, 16)
#define CEIL_UNIT(n, width) ( ( (n) + (width) - 1 ) / (width) )
// 0=CEIL(0, 16), 16=CEIL(1, 16), 16=CEIL(16, 16), 32=CEIL(17, 16)
#define CEIL(n, width) ( CEIL_UNIT(n, width) * (width) )


//// String and buffer macros ////

// Same as strncpy() in all ways except that the result is guaranteed to
// be a nul-terminated C string
#ifndef STRNCPY
# define STRNCPY(dest, src, size) { strncpy(dest, src, size); (dest)[(size)-1] = '\0'; }
#endif

// Because memory clearing is a frequent operation
#define MEMCLR(dest, size) memset(dest, 0, size)

// Works for arrays, also
#define OBJCLR(object) memset(&(object), 0, sizeof(object))


//// Intrinsics ////

#if defined(_MSC_VER)

# include <cstdlib>

# pragma intrinsic(_lrotl)
# pragma intrinsic(_lrotr)
# pragma intrinsic(_byteswap_ushort)
# pragma intrinsic(_byteswap_ulong)

# undef ROL32
# undef ROR32
# undef BOSWAP16
# undef BOSWAP32

# define ROL32(n, r) _lrotl(n, r)
# define ROR32(n, r) _lrotr(n, r)
# define BOSWAP16(n) _byteswap_ushort(n)
# define BOSWAP32(n) _byteswap_ulong(n)

#endif


// getLE() converts from little-endian word to native byte-order word 
// getBE() converts from big-endian word to native byte-order word 

#if defined(LITTLE_ENDIAN)

# define swapLE(n)
# define getLE(n) (n)
# define getLE16(n) (n)
# define getLE32(n) (n)
# define getLE64(n) (n)

	inline void swapBE(uint16_t &n) { n = BOSWAP16(n); }
	inline void swapBE(uint32_t &n) { n = BOSWAP32(n); }
	inline void swapBE(uint64_t &n) { n = BOSWAP64(n); }
	inline uint16_t getBE(uint16_t n) { return BOSWAP16(n); }
	inline uint32_t getBE(uint32_t n) { return BOSWAP32(n); }
	inline uint64_t getBE(uint64_t n) { return BOSWAP64(n); }
	inline uint16_t getBE16(uint16_t n) { return BOSWAP16(n); }
	inline uint32_t getBE32(uint32_t n) { return BOSWAP32(n); }
	inline uint64_t getBE64(uint64_t n) { return BOSWAP64(n); }
	inline void swapBE(int16_t &n) { n = BOSWAP16((uint16_t)n); }
	inline void swapBE(int32_t &n) { n = BOSWAP32((uint32_t)n); }
	inline void swapBE(int64_t &n) { n = BOSWAP64((uint64_t)n); }
	inline int16_t getBE(int16_t n) { return BOSWAP16((uint16_t)n); }
	inline int32_t getBE(int32_t n) { return BOSWAP32((uint32_t)n); }
	inline int64_t getBE(int64_t n) { return BOSWAP64((uint64_t)n); }

	inline float getBE(float n) {
		Float32 c = n;
		c.i = BOSWAP32(c.i);
		return c.f;
	}

#else

# define swapBE(n)
# define getBE(n) (n)
# define getBE16(n) (n)
# define getBE32(n) (n)
# define getBE64(n) (n)

	inline void swapLE(uint16_t &n) { n = BOSWAP16(n); }
	inline void swapLE(uint32_t &n) { n = BOSWAP32(n); }
	inline void swapLE(uint64_t &n) { n = BOSWAP64(n); }
	inline uint16_t getLE(uint16_t n) { return BOSWAP16(n); }
	inline uint32_t getLE(uint32_t n) { return BOSWAP32(n); }
	inline uint64_t getLE(uint64_t n) { return BOSWAP64(n); }
	inline uint16_t getLE16(uint16_t n) { return BOSWAP16(n); }
	inline uint32_t getLE32(uint32_t n) { return BOSWAP32(n); }
	inline uint64_t getLE32(uint64_t n) { return BOSWAP64(n); }
	inline void swapLE(int16_t &n) { n = BOSWAP16((uint16_t)n); }
	inline void swapLE(int32_t &n) { n = BOSWAP32((uint32_t)n); }
	inline void swapLE(int64_t &n) { n = BOSWAP64((uint64_t)n); }
	inline int16_t getLE(int16_t n) { return BOSWAP16((uint16_t)n); }
	inline int32_t getLE(int32_t n) { return BOSWAP32((uint32_t)n); }
	inline int64_t getLE(int64_t n) { return BOSWAP64((uint64_t)n); }

	inline float getLE(float n) {
		Float32 c = n;
		c.i = BOSWAP32(c.i);
		return c.f;
	}

#endif


#endif // include guard
