/// \file Types.h
/// \brief Used for types used by RSA
/// 
/// @verbatim
/// Fundamental tools & types
///
/// Catid(cat02e@fsu.edu)
///
/// 8/9/2004 Added SINGLE/ARRAY_RELEASE
/// 8/5/2004 Added COMPILER_ preprocessors
///    class NoCopies
/// 8/1/2004 Removed mask stuff
/// 7/29/2004 Added swapLE, swapBE, getLE, getBE
/// 7/28/2004 Automatic and AutoArray now compile in dev-c++
///    Added pre-processor conditions to support 
///    other compilers
///    Removed GETWORD and GETDWORD
/// 7/15/2004 Now using COM_RELEASE throughout CatGL3
/// 6/22/2004 Removed triple and pair
/// 6/12/2004 AutoDeallocate -> Automatic, AutoArray
/// 6/9/2004 OBJCLR
/// 5/2/2004 class AutoDeallocate
/// 5/1/2004 IS_POWER_OF_2, next_highest_power_of_2
/// 4/30/2004 Merged character manip macros
/// 2/23/2004 CEIL*
///    Removed MEMCOPY32 and MEMCLEAR32,
///    memcpy and memset are now faster
///    MAKE_MASK
/// 2/10/2004 LITTLE_ENDIAN
///    COUNT1BITS32
///    AT_LEAST_2_BITS
///    LEAST_SIGNIFICANT_BIT
/// X-mas/2003 [u/s]int?? -> [u/s]??
/// 7/3/2003 Added template triple, point->pair
/// 6/15/2003 Added template rect, point
/// 3/30/2003 Added RO?8, RO?16 and ?int64
///    Added MEMCOPY32 and MEMCLEAR32
/// 3/12/2003 Added GETWORD and GETDWORD
/// 1/16/2003 Formalized this library.
///
/// Tabs: 4 spaces
/// Dist: public
/// @endverbatim
///

#ifndef TYPES_H
#define TYPES_H

/// @brief Compatibility and Fundamental Tools and Types.  This
/// namespace contains compatibility tools and types and also
/// fundamental class. 
/// @section sec_compiler Compiler detection 
/// Things to consider for each compiler:
///
/// - BIG_ENDIAN / LITTLE_ENDIAN
/// - MULTITHREADED
/// - DEBUG
/// - HASINT64
/// - Basic types {u8-u64, s8-s64, f32, f64}
/// - WIN32
/// - ASSEMBLY_INTEL_SYNTAX / ASSEMBLY_ATT_SYNTAX, ASSEMBLY_BLOCK
/// - INLINE
/// - NO_TEMPLATE_INLINE_ASSEMBLY
/// - Fixes
/// 
/// Set depending which compiler is being used:
///
/// - COMPILER_MSVC
/// - COMPILER_GCC
/// - COMPILER_BORLANDC

// CB: Something strange happens with the system includes on
// Linux and BIG_ENDIAN ends up defined at some point, so
// we need to use HOST_ENDIAN_IS_BIG instead. This code is
// a copy of code further down, but keeps the def outside
// the cat namespace

#if !defined(HOST_ENDIAN_IS_BIG) && !defined(HOST_ENDIAN_IS_LITTLE)
#if defined(__sparc) || defined(__sparc__) || defined(__powerpc__) || \
	defined(__ppc__) || defined(__hppa) || defined(_MIPSEB) || defined(_POWER) || \
	defined(_M_PPC) || defined(_M_MPPC) || defined(_M_MRX000) || \
	defined(__POWERPC) || defined(m68k) || defined(powerpc) || \
	defined(sel) || defined(pyr) || defined(mc68000) || defined(is68k) || \
	defined(tahoe) || defined(ibm032) || defined(ibm370) || defined(MIPSEB) || \
	defined(__convex__) || defined(DGUX) || defined(hppa) || defined(apollo) || \
	defined(_CRAY) || defined(__hp9000) || defined(__hp9000s300) || defined(_AIX) || \
	defined(__AIX) || defined(__pyr__) || defined(hp9000s700) || defined(_IBMR2)

# define HOST_ENDIAN_IS_BIG

#elif defined(__i386__) || defined(i386) || defined(intel) || defined(_M_IX86) || defined(_M_X64) || \
	defined(__amd64) || defined(__amd64__)	|| \
	defined(__alpha__) || defined(__alpha) || defined(__ia64) || defined(__ia64__) || \
	defined(_M_ALPHA) || defined(ns32000) || defined(__ns32000__) || defined(sequent) || \
	defined(MIPSEL) || defined(_MIPSEL) || defined(sun386) || defined(__sun386__)

# define HOST_ENDIAN_IS_LITTLE

#else

# error "I can't tell what endian-ness to use for your architecture."

#endif
#endif


namespace cat
{

	//// endian-ness (ignoring NUXI) ////
	// Prevent an error message with gcc which define preprocessor symbol
	// LITTLE_ENDIAN or BIG_ENDIAN (endian.h)
	// dalfy
#if !defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)
#if defined(__sparc) || defined(__sparc__) || defined(__powerpc__) || \
	defined(__ppc__) || defined(__hppa) || defined(_MIPSEB) || defined(_POWER) || \
	defined(_M_PPC) || defined(_M_MPPC) || defined(_M_MRX000) || \
	defined(__POWERPC) || defined(m68k) || defined(powerpc) || \
	defined(sel) || defined(pyr) || defined(mc68000) || defined(is68k) || \
	defined(tahoe) || defined(ibm032) || defined(ibm370) || defined(MIPSEB) || \
	defined(__convex__) || defined(DGUX) || defined(hppa) || defined(apollo) || \
	defined(_CRAY) || defined(__hp9000) || defined(__hp9000s300) || defined(_AIX) || \
	defined(__AIX) || defined(__pyr__) || defined(hp9000s700) || defined(_IBMR2)

# define BIG_ENDIAN

#elif defined(__i386__) || defined(i386) || defined(intel) || defined(_M_IX86) || \
	defined(__amd64) || defined(__amd64__)	|| defined(_M_X64) || \
	defined(__alpha__) || defined(__alpha) || defined(__ia64) || defined(__ia64__) || \
	defined(_M_ALPHA) || defined(ns32000) || defined(__ns32000__) || defined(sequent) || \
	defined(MIPSEL) || defined(_MIPSEL) || defined(sun386) || defined(__sun386__)

# define LITTLE_ENDIAN

#else

# error "I can't tell what endian-ness to use for your architecture."

#endif
#endif

#ifndef FALSE
	/**
	* Define an alias between FALSE and false 
	*/
#define FALSE false
#endif
#ifndef TRUE
	/**
	* Define an alias between TRUE and true
	*/
#define TRUE true
#endif
	//// compiler-specific ////

#if defined(__COMO__) // Comeau C++

# error "Comeau C++ : I don't know your compiler"

#elif defined(__DMC__) // Digital Mars C++

# error "Digital Mars C++ : I don't know your compiler"

#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC) // Intel

// # error "Intel : I don't know your compiler"

#define HASINT64
#define INLINE inline

#if defined(_MT)
#define MULTITHREADED
#endif

#if defined(_DEBUG)
#define DEBUG
#endif

#elif (defined(__GNUC__)  || defined(__GCCXML__)) // GNU C++

#define COMPILER_GCC

# if defined(_MT)
#  define MULTITHREADED
# endif

# if defined(_DEBUG)
#  define DEBUG
# endif

# define INLINE inline
# define HASINT64

# define ASSEMBLY_ATT_SYNTAX
# define ASSEMBLY_BLOCK asm

#elif defined(__DJGPP__) // DJGPP

# error "DJGPP : I don't know your compiler"

# define ASSEMBLY_ATT_SYNTAX

#elif defined(__WATCOMC__) // WatcomC

# error "WatcomC : I don't know your compiler"

#elif defined(__KCC) // Kai C++

# error "Kai C++ : I don't know your compiler"

#elif defined(__sgi) // SGI MIPSpro C++

# error "SGI MIPSpro C++ : I don't know your compiler"

#elif defined(__SUNPRO_CC) // Sun Workshop Compiler C++

# error "Sun Workshop Compiler C++ : I don't know your compiler"

#elif defined(__HP_aCC) // HP aCC

# error "HP aCC : I don't know your compiler"

#elif defined(__DECCXX) // Compaq Tru64 Unix cxx

# error "Compaq Tru64 Unix cxx : I don't know your compiler"

#elif defined(__ghs) // Greenhills C++

# error "Greenhills C++ : I don't know your compiler"

#elif defined(__BORLANDC__) // Borland

# if (__BORLANDC__ >= 0x561)
#  define HASINT64
# endif

#define COMPILER_BORLANDC

# if defined(__MT__)
#  define MULTITHREADED
# endif

# if defined(_DEBUG)
#  define DEBUG
# endif


# define INLINE inline
# define ASSEMBLY_INTEL_SYNTAX
# define ASSEMBLY_BLOCK _asm
# define NO_TEMPLATE_INLINE_ASSEMBLY

#elif defined(__MWERKS__) // Metrowerks CodeWarrior

#define COMPILER_CODEWARRIOR
# define INLINE inline
# define HASINT64

#elif defined(__MRC__) || defined(__SC__) // MPW MrCpp or SCpp

# error "MPW MrCpp or SCpp : I don't know your compiler"

#elif defined(__IBMCPP__) // IBM Visual Age

# error "IBM Visual Age : I don't know your compiler"

#elif defined(_MSC_VER) // Microsoft VC++
	// must be last because many other compilers define this also

#define COMPILER_MSVC

# if (_MSC_VER >= 1200) /* 1200 == VC++ 6.0 */
#  define HASINT64
#  define INLINE __forceinline
# endif

# if defined(_MT)
#  define MULTITHREADED
# endif

# if defined(_DEBUG)
#  define DEBUG
# endif

# define BASIC_TYPES_ALREADY_DEFINED
	/**
	* Typename for 8 bits unsigned integer
	*/
	typedef unsigned __int8 u8;
	/**
	* Typename for 8  bits signed integer
	*/
	typedef signed __int8 s8;
	/**
	* Typename for 16 bits unsigned integer
	*/
	typedef unsigned __int16 u16;
	/**
	* Typename for 16 bits signed integer
	*/
	typedef signed __int16 s16;
	/**
	* Typename for 32 bits unsigned integer
	*/
	typedef unsigned __int32 u32;
	/**
	* Typename for 32 bits floatting point number
	*/
	typedef signed __int32 s32;
	/**
	* Typename for 32 bits floatting point number
	*/
	typedef float f32;
	/**
	* Typename for 64 bits floatting point number
	*/
	typedef double f64;
# if defined(HASINT64)
	/**
	* Typename for 64 bits unsigned integer
	*/
	typedef unsigned __int64 u64;
	/**
	* Typename for 64 bits signed integer
	*/
	typedef signed __int64 s64;
# endif

#if !defined(_M_X64)
# define ASSEMBLY_INTEL_SYNTAX
# define ASSEMBLY_BLOCK __asm
#endif

# if (_MSC_VER <= 1200)
#  pragma warning(disable : 4786) // truncation to 255 chars
# endif

#else

# error "Unknown : I don't know your compiler"

#endif // compilers


	// Generic basic types
#if !defined(BASIC_TYPES_ALREADY_DEFINED)
	/**
	* Typename for 8 bits unsigned integer 
	*/
	typedef unsigned char u8;
	/**
	* Typename for 8  bits signed integer 
	*/
	typedef signed char s8;
	/**
	* Typename for 16 bits unsigned integer
	*/
	typedef unsigned short u16;
	/**
	* Typename for 16 bits signed integer 
	*/
	typedef signed short s16;
	/**
	* Typename for 32 bits unsigned integer 
	*/
	typedef unsigned int u32;
	/**
	* Typename for 32 bits signed integer 
	*/
	typedef signed int s32;
	/**
	* Typename for 32 bits floatting point number 
	*/
	typedef float f32;
	/**
	* Typename for 64 bits floatting point number 
	*/
	typedef double f64;
# if defined(HASINT64)
	/**
	* Typename for 64 bits unsigned integer 
	*/
	typedef unsigned int long u64;
	/**
	* Typename for 64 bits signed integer 
	*/
	typedef signed long long s64;
# endif
#endif


	/**
	* Fixed-point types
	* hi-Siiiiiiiiiiiiiiiiiiiii-lo | hi-ffffffffff-lo
	*/
	typedef s32 sfp22_10;
	/**
	* Fixed-point types 
	* hi-iiiiiiiiiiiiiiiiiiiiii-lo | hi-ffffffffff-lo 
	*/
	typedef u32 ufp22_10;
	/**
	* Fixed point types 
	* hi-Siiiiiiiiiiiiiii-lo | hi-ffffffffffffffff-lo
	*/
	typedef s32 sfp16_16;
	/**
	* Fixed point types 
	* hi-iiiiiiiiiiiiiiii-lo | hi-ffffffffffffffff-lo
	*/
	typedef u32 ufp16_16;

	/*
	COMmy macros
	*/

#define COM_RELEASE(ref) if (ref) { (ref)->Release(); (ref) = 0; }
#define SINGLE_RELEASE(ref) if (ref) { delete (ref); (ref) = 0; }
#define ARRAY_RELEASE(ref) if (ref) { delete [](ref); (ref) = 0; }

	template <class T>

	class rect
	{

	public:
		rect() : x( 0 ), y( 0 ), w( 0 ), h( 0 )
		{}

		rect( T xx, T yy, T ww, T hh ) : x( xx ), y( yy ), w( ww ), h( hh )
		{}

		T x, y, w, h;
	};

	template <class T>

	class AutoArray
	{
		T *ptr;

	public:
		AutoArray( T *c_ptr )
		{
			ptr = c_ptr;
		}

		~AutoArray()
		{
			ARRAY_RELEASE( ptr );
		}

		INLINE void cancel()
		{
			ptr = 0;
		}
	};

	template <class T>

	class Automatic
	{
		T *ptr;

	public:
		Automatic( T *c_ptr )
		{
			ptr = c_ptr;
		}

		~Automatic()
		{
			SINGLE_RELEASE( ptr );
		}

		INLINE void cancel()
		{
			ptr = 0;
		}
	};

	// Derive from NoCopies to disallow copies of derived class

	class NoCopies
	{

	protected:
		NoCopies()
		{}

		~NoCopies()
		{}

	private:
		NoCopies( const NoCopies &cp );
		NoCopies &operator=( const NoCopies &cp );
	};


	// Byte-order swapping
#define BOSWAP32(n) ( ((n) << 24) | (((n) & 0x00ff0000) >> 8) | (((n) & 0x0000ff00) << 8) | ((n) >> 24) ) /* only works for u32 */
#define BOSWAP16(n) ( ((n) << 8) | ((n) >> 8) ) /* only works for u16 */

#ifdef LITTLE_ENDIAN
# define swapLE(n)
# define getLE(n) (n)

	INLINE void swapBE( u32 &n )
	{
		n = BOSWAP32( n );
	}

	INLINE void swapBE( u16 &n )
	{
		n = BOSWAP16( n );
	}

	INLINE u32 getBE( u32 n )
	{
		return BOSWAP32( n );
	}

	INLINE u16 getBE( u16 n )
	{
		return BOSWAP16( n );
	}

	INLINE void swapBE( s32 &n )
	{
		n = BOSWAP32( ( u32 ) n );
	}

	INLINE void swapBE( s16 &n )
	{
		n = BOSWAP16( ( u16 ) n );
	}

	INLINE s32 getBE( s32 n )
	{
		return BOSWAP32( ( u32 ) n );
	}

	INLINE s16 getBE( s16 n )
	{
		return BOSWAP16( ( u16 ) n );
	}

#else
# define swapBE(n)
# define getBE(n) (n)
	INLINE void swapLE( u32 &n )
	{
		n = BOSWAP32( n );
	}

	INLINE void swapLE( u16 &n )
	{
		n = BOSWAP16( n );
	}

	INLINE u32 getLE( u32 n )
	{
		return BOSWAP32( n );
	}

	INLINE u16 getLE( u16 n )
	{
		return BOSWAP16( n );
	}

	INLINE void swapLE( s32 &n )
	{
		n = BOSWAP32( ( u32 ) n );
	}

	INLINE void swapLE( s16 &n )
	{
		n = BOSWAP16( ( u16 ) n );
	}

	INLINE s32 getLE( s32 n )
	{
		return BOSWAP32( ( u32 ) n );
	}

	INLINE s16 getLE( s16 n )
	{
		return BOSWAP16( ( u16 ) n );
	}

#endif

} // namespace cat


// Rotation
#define ROL8(n, r)  ( ((n) << (r)) | ((n) >> ( 8 - (r))) ) /* only works for u8 */
#define ROR8(n, r)  ( ((n) >> (r)) | ((n) << ( 8 - (r))) ) /* only works for u8 */
#define ROL16(n, r) ( ((n) << (r)) | ((n) >> (16 - (r))) ) /* only works for u16 */
#define ROR16(n, r) ( ((n) >> (r)) | ((n) << (16 - (r))) ) /* only works for u16 */
#define ROL32(n, r) ( ((n) << (r)) | ((n) >> (32 - (r))) ) /* only works for u32 */
#define ROR32(n, r) ( ((n) >> (r)) | ((n) << (32 - (r))) ) /* only works for u32 */

/*
Add memory that is allocated on a 32-bit boundary
and has at least one block.
*/
#define MEMADD32(ptr, len, val) { \
	register u32 *__data = (u32*)(ptr); /* pointer to data to clear */ \
	register s32 __length = (len); /* number of 32-bit blocks */ \
	\
	switch (__length % 8) \
{ \
	case 0: do { *__data++ += (val); \
	case 7:   *__data++ += (val); \
	case 6:   *__data++ += (val); \
	case 5:   *__data++ += (val); \
	case 4:   *__data++ += (val); \
	case 3:   *__data++ += (val); \
	case 2:   *__data++ += (val); \
	case 1:   *__data++ += (val); \
	__length -= 8; \
			} while(__length > 0); \
} \
}

/**
* Safe null-terminated string -> char buffer copy
* \param dest the resulting string 
* \param src the string to copy 
* \param size the number of char to copy 
*/
#define STRNCPY(dest, src, size) { \
	strncpy(dest, src, size); \
	dest[size-1] = 0; \
}

/*
Because memory clearing is a frequent operation
*/
#define MEMCLR(dest, size) memset(dest, 0, size)

// Works for arrays, also
#define OBJCLR(object) memset(&(object), 0, sizeof(object))

/*
Fast binary method of counting bits

MIT Hackmem from X11

Only works for 32-bit integers
*/
#define _C1B_INTERMED(n) ( (n) - (((n) >> 1) & 033333333333) - (((n) >> 2) & 011111111111) )
#define COUNT1BITS32(n) ( ((_C1B_INTERMED(n) + (_C1B_INTERMED(n) >> 3)) & 030707070707) % 63 )

/*
Consider N an ordered set of 1/0 values.

LESSTHAN2BITS(n) := there are less than 2 bits in set N

Proof:

(N - 1) will clear the least significant of the bits in N.

Three cases exist concerning (N-1):

N contains 0 bits
An intersection with N would be trivially null.
N contains 1 bit
The least only existing bit was cleared,
thus an intersection with the original
set WOULD be null.
N contains more than 1 bit
A more significant bit remains in the set,
thus an intersection with the original
set WOULD NOT be null.
*/
#define AT_LEAST_2_BITS(n) ( (n) & ((n) - 1) )

#define LEAST_SIGNIFICANT_BIT(n) ( (n) & -(n) )

#define IS_POWER_OF_2(n) ( n && !AT_LEAST_2_BITS(n) )

INLINE cat::u32 next_highest_power_of_2( cat::u32 n )
{
	if ( IS_POWER_OF_2( n ) )
		return n;

	cat::u16 b = 2;

	while ( n >>= 1 )
		b <<= 1;

	return b;
}

/*
CEIL*

Bump 'n' to the next unit of 'width'.
*/
#define CEIL_UNIT(n, width) ( ( (n) + (width) - 1 ) / (width) )
#define CEIL(n, width) ( CEIL_UNIT(n, width) * (width) )

/*
Quick character manipulation
*/
//#define IS_ALPHA(ch) ( (((u8)(ch) & 0xc0) == 0x40) && ((((u8)(ch) - 1) & 0x1f) <= 0x19) )

//#define IS_NUM(ch) ( ((u8)(ch) - 0x30) < 10 )

//#define IS_ALPHANUM(ch) ( IS_ALPHA(ch) || IS_NUM(ch) )

//#define TO_LOWER(ch) (char)( (ch) | 0x20 ) /* must be upper/lower-case alpha */
//#define TO_UPPER(ch) (char)( (ch) & (~0x20) ) /* must be upper/lower-case alpha */


#endif // TYPES_H
