/// \file
/// \brief \b [Internal] Used for RSA generation.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// (128)2^7-bit to (32768)2^14-bit signed 2's complement & unsigned extended arithmetic
///
/// catid(cat02e@fsu.edu)
///
/// 7/30/2004 Fixed VS6 compat
/// 7/28/2004 Fixed macros so they can be used outside of the big namespace
///    Now using pre-processor definitions from types.h for inline assembly
/// 7/26/2004 Removed a lot of assembly, made add/sub assembly optional
/// 7/25/2004 Merged the wrapper class Int<T> from older code
/// 7/24/2004 Replaced trivial assembly code with std:: functions
///    Refined some assembly code with Art of Assembly chapter 9
///    Added binary ops
/// 7/23/2004 Finished assembly coding
///    Removed Int<T> class, for now
///    Added old C++ code back in with USEASSEMBLY
/// 7/22/2004 Signed arithmetic (needed for ext. Euclidean algo)
///    Cleaned up coding style
///    Began rewriting parts in assembly
/// 7/21/2004 Began writing
///
/// Tabs: 4 spaces
/// Dist: public

#ifndef BIGTYPES_H
#define BIGTYPES_H

#if !defined(_XBOX360)

#include "Types.h"

//#define BIG_USES_STRINGS /* undefining this means you cannot convert bigs to strings or from strings */

#ifdef BIG_USES_STRINGS
# include <string>
#endif


#ifdef _MSC_VER
#pragma warning( push )
#endif

namespace big
{

	using namespace cat;
	
	//// basic definitions ////
	
	// word size
	typedef u32 word; // assembly implementation is for 32-bit word size
	const u32 WORDBITS = sizeof( word ) * 8;
	const u32 HALFWORDBITS = sizeof( word ) * 8 / 2;
	const word WORDHIGHBIT = ( word ) 1 << ( WORDBITS - 1 );
	const word WORDALLBITS = ( word ) 0 - 1;
	const word WORDLOBITS = ( ( word ) 1 << HALFWORDBITS ) - 1;
	const word WORDHIBITS = WORDALLBITS ^ WORDLOBITS;
#define BIGHIGHBIT(n) ((n)[sizeof(n) / sizeof(big::word) - 1] & WORDHIGHBIT)
	
	// template operator parameter modes
#define BIGONETYPE template<class T> /* supports only one class */
#define BIGTWOTYPES template<class T, class Bigger> /* sizeof Bigger >= sizeof T */
#define BIGSMALLTYPE template<class Smaller> /* sizeof self >= sizeof Smaller */
	
	
	//// big types ////
	
#define BIGWORDCOUNT_FROMBITCOUNT(bits) ((bits) / 8 / sizeof(big::word))
#define BIGWORDCOUNT(T) (sizeof(T) / sizeof(big::word))
#define BIGBITCOUNT(T) (sizeof(T) * 8)
	
	// low words -- [0] < [1] < [2] < [3] -- high words
	typedef word u128[ BIGWORDCOUNT_FROMBITCOUNT( 128 ) ];
	typedef word u256[ BIGWORDCOUNT_FROMBITCOUNT( 256 ) ];
	typedef word u512[ BIGWORDCOUNT_FROMBITCOUNT( 512 ) ];
	typedef word u1024[ BIGWORDCOUNT_FROMBITCOUNT( 1024 ) ];
	typedef word u2048[ BIGWORDCOUNT_FROMBITCOUNT( 2048 ) ];
	typedef word u4096[ BIGWORDCOUNT_FROMBITCOUNT( 4096 ) ];
	typedef word u8192[ BIGWORDCOUNT_FROMBITCOUNT( 8192 ) ];
	typedef word u16384[ BIGWORDCOUNT_FROMBITCOUNT( 16384 ) ];
	typedef word u32768[ BIGWORDCOUNT_FROMBITCOUNT( 32768 ) ];
	
	// use these macros to create temporary variables when
	// those variables are to be twice/half the size of another
	// variable of varying size.
#define BIGDOUBLESIZE(T, var_name) big::word (var_name)[BIGWORDCOUNT(T) * 2] /* WARNING: invalid w/ u32768 */
#define BIGHALFSIZE(T, var_name) big::word (var_name)[BIGWORDCOUNT(T) / 2] /* WARNING: invalid w/ u128 */
	
	
	//// library summary ////
	
	// assignment
	BIGONETYPE INLINE void zero( T &n ); // n = 0
	BIGONETYPE INLINE void usetw( T &a, word b ); // a = b, zero-extend
	BIGONETYPE INLINE void ssetw( T &a, word b ); // a = b, sign-extend
	
	BIGONETYPE INLINE void set ( T &a, T &b )
	
	; // a = b
	BIGTWOTYPES INLINE void usetlow( Bigger &a, T &b ); // a_low = b (zero-extend)
	
	BIGTWOTYPES INLINE void ssetlow( Bigger &a, T &b ); // a_low = b (sign-extend)
	
	BIGTWOTYPES INLINE void sethigh( Bigger &a, T &b ); // a_high = b
	
	BIGTWOTYPES INLINE void takelow( T &a, Bigger &b ); // a = b_low
	
	BIGTWOTYPES INLINE void takehigh( T &a, Bigger &b ); // a = b_high
	
	// comparison
	BIGONETYPE bool ugreater( T &a, T &b ); // a > b (unsigned)
	
	BIGONETYPE bool ugreaterOrEqual( T &a, T &b ); // a >= b (unsigned)
	
	BIGONETYPE bool sgreater( T &a, T &b ); // a > b (signed)
	
	BIGONETYPE bool sgreaterOrEqual( T &a, T &b ); // a >= b (signed)
	
	BIGONETYPE INLINE bool equal( T &a, T &b ); // a == b
	
	BIGONETYPE INLINE bool isZero( T &n ); // a == 0
	
	// binary
	BIGONETYPE void bAND( T &a, T &b ); // a &= b
	
	BIGONETYPE void bOR( T &a, T &b ); // a |= b
	
	BIGONETYPE void bXOR( T &a, T &b ); // a ^= b
	
	BIGONETYPE void bNOT( T &n ); // n = ~n
	
	// shifting
	BIGONETYPE void shiftLeft1( T &n ); // n <<= 1
	
	BIGONETYPE void shiftLeft( T &n, u32 s ); // n <<= s (s <= WORDBITS)
	
	BIGONETYPE void ushiftRight1( T &n ); // n >>= 1 (unsigned)
	
	BIGONETYPE void ushiftRight( T &n, u32 s ); // n >>= s (unsigned) (s <= WORDBITS)
	
	BIGONETYPE void sshiftRight1( T &n ); // n >>= 1 (signed)
	
	BIGONETYPE void sshiftRight( T &n, u32 s ); // n >>= s (signed) (s <= WORDBITS)
	
	// addition/subtraction
	BIGONETYPE void add ( T &a, T &b )
	
	; // a += b
	BIGONETYPE void increment( T &n ); // ++n
	
	BIGONETYPE void subtract( T &a, T &b ); // a -= b
	
	BIGONETYPE void decrement( T &n ); // --n
	
	// negation
	BIGONETYPE void negate( T &n ); // n = -n
	
	// multiplication
	BIGONETYPE void usquare( T &a ); // a *= a, signed
	
	BIGTWOTYPES void umultiply( T &a, T &b, Bigger &m ); // m = a * b (&a != &b != &m), unsigned
	
	BIGTWOTYPES void umultiply( Bigger &a, T &b ); // a *= b (&a != &b), unsigned
	
	BIGONETYPE void ssquare( T &a ); // a *= a, signed
	
	BIGTWOTYPES void smultiply( T &a, T &b, Bigger &m ); // m = a * b (&a != &b != &m), signed
	
	BIGTWOTYPES void smultiply( Bigger &a, T &b ); // a *= b (&a != &b), signed
	
	// division/remainder
	BIGONETYPE void udivide( T &a, T &b, T &q, T &r ); // {q, r} = a / b (&q != &r), unsigned
	
	BIGONETYPE void umodulo( T &a, T &b, T &r ); // r = a Mod b, unsigned
	
	BIGONETYPE void sdivide( T &a, T &b, T &q, T &r ); // {q, r} = a / b (&q != &r), signed
	
	BIGONETYPE void smodulo( T &a, T &b, T &r ); // r = a Mod b, signed
	
#ifdef BIG_USES_STRINGS
	// converting to/from strings
	BIGONETYPE std::string toString( T &n, bool sign, u16 radix ); // n -> string
	
	BIGONETYPE void fromString( std::string s, T &n, bool sign, u16 radix ); // s -> n
	
#endif
	
	
	//////// wrapper class ////////
	
#define BIGINTFAST INLINE Int<T> & /* operation is done to self, returns itself */
#define BIGINTSLOW Int<T> /* new object is created and returned */
	
	BIGONETYPE class Int
	{
	
	protected:
		T raw;
		
	public:
		operator T &(); // automatic casting to T: you may use BigInt classes as parameters to the functions
		
	public:
		Int();
		Int( word n );
#ifdef BIG_USES_STRINGS
		
		Int( std::string &s );
#endif
		
		Int( T &n );
		
	public:
		BIGINTFAST zero();
		BIGINTFAST operator=( word n );
		BIGINTFAST operator=( T &n );
		
	public:
		BIGINTFAST operator<<=( u32 s );
		BIGINTSLOW operator<<( u32 s );
		BIGINTFAST operator>>=( u32 s );
		BIGINTSLOW operator>>( u32 s );
		
	public:
		BIGINTFAST operator+=( T &n );
		BIGINTSLOW operator+( T &n );
		BIGINTFAST operator-=( T &n );
		BIGINTSLOW operator-( T &n );
		BIGINTFAST operator++(); // prefix
		BIGINTSLOW operator++( int ); // postfix
		BIGINTFAST operator--(); // prefix
		BIGINTSLOW operator--( int ); // postfix
		
	public:
		BIGINTSLOW operator-( int ); // negation
		
	public:
		BIGSMALLTYPE BIGINTFAST operator*=( Smaller &n )
		{
			smultiply( raw, n );
			return *this;
		}
		
		BIGINTSLOW operator*( T &n );
		BIGINTFAST square();
		
	public:
		BIGINTFAST operator/=( T &n );
		BIGINTSLOW operator/( T &n );
		BIGINTFAST operator%=( T &n );
		BIGINTSLOW operator%( T &n );
		
	public:
		/* fast */
		bool operator>( T &n );
		/* fast */
		bool operator>=( T &n );
		/* fast */
		bool operator<( T &n );
		/* fast */
		bool operator<=( T &n );
		/* fast */
		bool operator==( T &n );
		/* fast */
		bool operator!=( T &n );
		/* fast */
		bool operator!();
		
	public:
#ifdef BIG_USES_STRINGS
		/* fast */
		std::string str();
		BIGINTFAST operator=( std::string &s );
		BIGINTFAST operator=( const char *s );
#endif
		
	};
	
	
	//////// assignment ////////
	
	// n = 0
	BIGONETYPE INLINE void zero( T &n )
	{
		memset( n, 0, sizeof( T ) );
	}
	
	// a = b, zero-extend
	BIGONETYPE INLINE void usetw( T &a, word b )
	{
		a[ 0 ] = b;
		memset( a + 1, 0, sizeof( T ) - sizeof( word ) );
	}
	
	// a = b, sign-extend
	BIGONETYPE INLINE void ssetw( T &a, word b )
	{
		a[ 0 ] = b;
		memset( a + 1, ( b & WORDHIGHBIT ) ? WORDALLBITS : 0, sizeof( T ) - sizeof( word ) );
	}
	
	// a = b
	BIGONETYPE INLINE void set ( T &a, T &b )
	{
		memcpy( a, b, sizeof( T ) );
	}
	
	// a_low = b (zero-extend)
	BIGTWOTYPES INLINE void usetlow( Bigger &a, T &b )
	{
		memcpy( a, b, sizeof( T ) );
#ifdef _MSC_VER
#pragma warning( disable : 4318 ) // warning C4318: passing constant zero as the length to memset
#endif
		memset( a + BIGWORDCOUNT( T ), 0, sizeof( Bigger ) - sizeof( T ) );
	}
	
	// a_low = b (sign-extend)
	BIGTWOTYPES INLINE void ssetlow( Bigger &a, T &b )
	{
		memcpy( a, b, sizeof( T ) );
		memset( a + BIGWORDCOUNT( T ), BIGHIGHBIT( b ) ? WORDALLBITS : 0, sizeof( Bigger ) - sizeof( T ) );
	}
	
	// a_high = b
	BIGTWOTYPES INLINE void sethigh( Bigger &a, T &b )
	{
		memcpy( a + BIGWORDCOUNT( Bigger ) - BIGWORDCOUNT( T ), b, sizeof( T ) );
		memset( a, 0, sizeof( Bigger ) - sizeof( T ) );
	}
	
	// a = b_low
	BIGTWOTYPES INLINE void takelow( T &a, Bigger &b )
	{
		memcpy( a, b, sizeof( T ) );
	}
	
	// a = b_high
	BIGTWOTYPES INLINE void takehigh( T &a, Bigger &b )
	{
		memcpy( a, b + BIGWORDCOUNT( Bigger ) - BIGWORDCOUNT( T ), sizeof( T ) );
	}
	
	
	//////// comparison ////////
	
	// a > b
	BIGONETYPE bool ugreater( T &a, T &b )
	{
		for ( s32 ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
		{
			if ( a[ ii ] > b[ ii ] )
				return true;
				
			if ( a[ ii ] < b[ ii ] )
				return false;
		}
		
		return false;
	}
	
	// a >= b
	BIGONETYPE bool ugreaterOrEqual( T &a, T &b )
	{
		for ( s32 ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
		{
			if ( a[ ii ] > b[ ii ] )
				return true;
				
			if ( a[ ii ] < b[ ii ] )
				return false;
		}
		
		return true;
	}
	
	// a > b
	BIGONETYPE bool sgreater( T &a, T &b )
	{
		for ( s32 ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
		{
			if ( a[ ii ] > b[ ii ] )
				return BIGHIGHBIT( a ) == 0;
				
			if ( a[ ii ] < b[ ii ] )
				return BIGHIGHBIT( b ) != 0;
		}
		
		return false;
	}
	
	// a >= b
	BIGONETYPE bool sgreaterOrEqual( T &a, T &b )
	{
		for ( s32 ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
		{
			if ( a[ ii ] > b[ ii ] )
				return BIGHIGHBIT( a ) == 0;
				
			if ( a[ ii ] < b[ ii ] )
				return BIGHIGHBIT( b ) != 0;
		}
		
		return true;
	}
	
	// a == b
	BIGONETYPE INLINE bool equal( T &a, T &b )
	{
		return memcmp( a, b, sizeof( T ) ) == 0;
	}
	
	// a == 0
	BIGONETYPE INLINE bool isZero( T &n )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			if ( n[ ii ] )
				return false;
				
		return true;
	}
	
	
	//////// binary ////////
	
	// a &= b
	BIGONETYPE void bAND( T &a, T &b )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			a[ ii ] &= b[ ii ];
	}
	
	// a |= b
	BIGONETYPE void bOR( T &a, T &b )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			a[ ii ] |= b[ ii ];
	}
	
	// a ^= b
	BIGONETYPE void bXOR( T &a, T &b )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			a[ ii ] ^= b[ ii ];
	}
	
	// n = ~n
	BIGONETYPE void bNOT( T &n )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			n[ ii ] = ~n[ ii ];
	}
	
	
	//////// shifting ////////
	
	// n <<= 1
	BIGONETYPE void shiftLeft1( T &n )
	{
		register word w_i, carry = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			w_i = n[ ii ];
			
			n[ ii ] = ( w_i << 1 ) | carry;
			carry = w_i >> ( WORDBITS - 1 );
		}
	}
	
	// n <<= s (s <= WORDBITS)
	BIGONETYPE void shiftLeft( T &n, u32 s )
	{
		register s32 ii;
		register u32 bases = s / WORDBITS;
		u32 bits = s % WORDBITS;
		
		// move whole bases first
		
		if ( bases )
		{
			// shift bases
			
			for ( ii = BIGWORDCOUNT( T ) - 1 - bases; ii >= 0; --ii )
				n[ ii + bases ] = n[ ii ];
				
			// clear the original locii of those bases
			memset( n, 0, bases * sizeof( word ) );
		}
		
		if ( bits )
		{
			register word w_i, carry = 0;
			
			for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			{
				w_i = n[ ii ];
				
				n[ ii ] = ( w_i << bits ) | carry;
				carry = w_i >> ( WORDBITS - bits );
			}
		}
	}
	
	// n >>= 1 (unsigned)
	BIGONETYPE void ushiftRight1( T &n )
	{
		register word w_i, carry = 0;
		
		for ( s32 ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
		{
			w_i = n[ ii ];
			
			n[ ii ] = carry | ( w_i >> 1 );
			carry = w_i << ( WORDBITS - 1 );
		}
	}
	
	// n >>= s (unsigned) (s <= WORDBITS)
	BIGONETYPE void ushiftRight( T &n, u32 s )
	{
		register s32 ii;
		register u32 bases = s / WORDBITS;
		register u32 bits = s % WORDBITS;
		
		// move whole bases first
		
		if ( bases )
		{
			// shift bases
			
			for ( ii = bases; ii < BIGWORDCOUNT( T ); ++ii )
				n[ ii - bases ] = n[ ii ];
				
			// clear the original locii of those bases
			memset( n + BIGWORDCOUNT( T ) - bases, 0, bases * sizeof( word ) );
		}
		
		if ( bits )
		{
			register word w_i, carry = 0;
			
			for ( ii = BIGWORDCOUNT( T ) - 1 - bases; ii >= 0; --ii )
			{
				w_i = n[ ii ];
				
				n[ ii ] = carry | ( w_i >> bits );
				carry = w_i << ( WORDBITS - bits );
			}
		}
	}
	
	// n >>= 1 (signed)
	BIGONETYPE void sshiftRight1( T &n )
	{
		register word w_i, carry = BIGHIGHBIT( n ) ? 1 : 0;
		
		for ( s32 ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
		{
			w_i = n[ ii ];
			
			n[ ii ] = carry | ( w_i >> 1 );
			carry = w_i << ( WORDBITS - 1 );
		}
	}
	
	// n >>= s (signed) (s <= WORDBITS)
	BIGONETYPE void sshiftRight( T &n, u32 s )
	{
		register s32 ii;
		register u32 bases = s / WORDBITS;
		register u32 bits = s % WORDBITS;
		
		word filler = BIGHIGHBIT( n ) ? WORDALLBITS : 0;
		
		// move whole bases first
		
		if ( bases )
		{
			// shift bases
			
			for ( ii = bases; ii < BIGWORDCOUNT( T ); ++ii )
				n[ ii - bases ] = n[ ii ];
				
			// clear the original locii of those bases
			memset( n + BIGWORDCOUNT( T ) - bases, filler, bases * sizeof( word ) );
		}
		
		if ( bits )
		{
			register word w_i, carry = filler << ( WORDBITS - bits );
			
			for ( ii = BIGWORDCOUNT( T ) - 1 - bases; ii >= 0; --ii )
			{
				w_i = n[ ii ];
				
				n[ ii ] = carry | ( w_i >> bits );
				carry = w_i << ( WORDBITS - bits );
			}
		}
	}
	
	
	//////// addition/subtraction ////////
	
#if defined(NO_TEMPLATE_INLINE_ASSEMBLY) && defined(ASSEMBLY_INTEL_SYNTAX)
	void BorlandAdd( void *a, void *b, u32 c )
	{
		ASSEMBLY_BLOCK // BorlandC, x86, 32-bit words
		{
			mov esi, b
			mov edi, a
			mov ecx, c
		        
			// unrolled loop since word count is a multiple of 4 >= 4
			mov eax, [ esi ]
			xor edx, edx  // edx used later to index later words >= 4
		        
			add [ edi ]
			, eax  // and now we can use ADD instead of ADC on the first addition
			mov eax, [ esi + 4 ]
			adc [ edi + 4 ], eax
			mov eax, [ esi + 8 ]
			adc [ edi + 8 ], eax
			mov eax, [ esi + 12 ]
			adc [ edi + 12 ], eax
		        
			jecxz done_already
		        
		next_word:
			inc edx
			inc edx
		        
			// unrolled loop since word count is a multiple of 4 >= 4
			mov eax, [ esi + edx * 8 ]
			adc [ edi + edx * 8 ], eax
			mov eax, [ esi + edx * 8 + 4 ]
			adc [ edi + edx * 8 + 4 ], eax
			mov eax, [ esi + edx * 8 + 8 ]
			adc [ edi + edx * 8 + 8 ], eax
			mov eax, [ esi + edx * 8 + 12 ]
			adc [ edi + edx * 8 + 12 ], eax
		        
			loop next_word
		        
		done_already:
		}
	}
	
#endif
	
	// a += b
	BIGONETYPE void add ( T &a, T &b )
	{
#if defined(NO_TEMPLATE_INLINE_ASSEMBLY) && defined(ASSEMBLY_INTEL_SYNTAX)
		const u32 qc1 = BIGWORDCOUNT( T ) / 4 - 1;
		
		BorlandAdd( a, b, qc1 );
		
#elif defined(ASSEMBLY_INTEL_SYNTAX)
		
		const u32 qc1 = BIGWORDCOUNT( T ) / 4 - 1;
		
		ASSEMBLY_BLOCK // VS.NET, x86, 32-bit words
		{
		        mov esi, b
		        mov edi, a
		        mov ecx, qc1
		
		        // unrolled loop since word count is a multiple of 4 >= 4
		        mov eax, [ esi ]
		        xor edx, edx  // edx used later to index later words >= 4
		
		        add [ edi ], eax  // and now we can use ADD instead of ADC on the first addition
		        mov eax, [ esi + 4 ]
		        adc [ edi + 4 ], eax
		        mov eax, [ esi + 8 ]
		        adc [ edi + 8 ], eax
		        mov eax, [ esi + 12 ]
		        adc [ edi + 12 ], eax
		
		        jecxz done_already
		
	        next_word:
		        inc edx
		        inc edx
		
		        // unrolled loop since word count is a multiple of 4 >= 4
		        mov eax, [ esi + edx * 8 ]
		        adc [ edi + edx * 8 ], eax
		        mov eax, [ esi + edx * 8 + 4 ]
		        adc [ edi + edx * 8 + 4 ], eax
		        mov eax, [ esi + edx * 8 + 8 ]
		        adc [ edi + edx * 8 + 8 ], eax
		        mov eax, [ esi + edx * 8 + 12 ]
		        adc [ edi + edx * 8 + 12 ], eax
		
		        loop next_word
		
	        done_already:
		}
#else
		register word carry = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word a_i = a[ ii ];
			word b_i = b[ ii ];
			a[ ii ] += b_i + carry;
		
			carry = ( ( a_i & ( WORDALLBITS >> 1 ) ) + ( b_i & ( WORDALLBITS >> 1 ) ) + carry ) >> ( WORDBITS - 1 );
			carry += ( a_i >> ( WORDBITS - 1 ) ) + ( b_i >> ( WORDBITS - 1 ) );
			carry >>= 1;
		}
		
#endif
		
	}
	
	// ++n
	BIGONETYPE void increment( T &n )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			if ( ++n[ ii ] )
				break;
	}
	
#if defined(NO_TEMPLATE_INLINE_ASSEMBLY) && defined(ASSEMBLY_INTEL_SYNTAX)
	void BorlandSubtract( void *a, void *b, u32 c )
	{
		ASSEMBLY_BLOCK // BorlandC, x86, 32-bit words
		{
			mov esi, b
			mov edi, a
			mov ecx, c
		        
			// unrolled loop since word count is a multiple of 4 >= 4
			mov eax, [ esi ]
			xor edx, edx  // edx used later to index later words >= 4
		        
			add [ edi ]
			, eax  // and now we can use ADD instead of ADC on the first addition
			mov eax, [ esi + 4 ]
			adc [ edi + 4 ], eax
			mov eax, [ esi + 8 ]
			adc [ edi + 8 ], eax
			mov eax, [ esi + 12 ]
			adc [ edi + 12 ], eax
		        
			jecxz done_already
		        
		next_word:
			inc edx
			inc edx
		        
			// unrolled loop since word count is a multiple of 4 >= 4
			mov eax, [ esi + edx * 8 ]
			adc [ edi + edx * 8 ], eax
			mov eax, [ esi + edx * 8 + 4 ]
			adc [ edi + edx * 8 + 4 ], eax
			mov eax, [ esi + edx * 8 + 8 ]
			adc [ edi + edx * 8 + 8 ], eax
			mov eax, [ esi + edx * 8 + 12 ]
			adc [ edi + edx * 8 + 12 ], eax
		        
			loop next_word
		        
		done_already:
		}
	}
	
#endif
	
	// a -= b
	BIGONETYPE void subtract( T &a, T &b )
	{
#if defined(NO_TEMPLATE_INLINE_ASSEMBLY) && defined(ASSEMBLY_INTEL_SYNTAX)
		const u32 qc1 = BIGWORDCOUNT( T ) / 4 - 1;
		
		BorlandSubtract( a, b, qc1 );
		
#elif defined(ASSEMBLY_INTEL_SYNTAX)
		
		const u32 qc1 = BIGWORDCOUNT( T ) / 4 - 1;
		
		ASSEMBLY_BLOCK // VS.NET, x86, 32-bit words
		{
		        mov esi, b
		        mov edi, a
		        mov ecx, qc1
		
		        // unrolled loop since word count is a multiple of 4 >= 4
		        mov eax, [ esi ]
		        xor edx, edx  // edx used later to index later words >= 4
		        sub [ edi ], eax  // first subtraction doesn't need to borrow
		        mov eax, [ esi + 4 ]
		        sbb [ edi + 4 ], eax
		        mov eax, [ esi + 8 ]
		        sbb [ edi + 8 ], eax
		        mov eax, [ esi + 12 ]
		        sbb [ edi + 12 ], eax
		
		        jecxz done_already
		
	        next_word:
		        inc edx
		        inc edx
		
		        // unrolled loop since word count is a multiple of 4 >= 4
		        mov eax, [ esi + edx * 8 ]
		        sbb [ edi + edx * 8 ], eax
		        mov eax, [ esi + edx * 8 + 4 ]
		        sbb [ edi + edx * 8 + 4 ], eax
		        mov eax, [ esi + edx * 8 + 8 ]
		        sbb [ edi + edx * 8 + 8 ], eax
		        mov eax, [ esi + edx * 8 + 12 ]
		        sbb [ edi + edx * 8 + 12 ], eax
		
		        loop next_word
		
	        done_already:
		}
#else
		register word borrow = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word a_i = a[ ii ];
			word b_i = b[ ii ];
			a[ ii ] -= b_i + borrow;
		
			borrow = ( ( a_i & ( WORDALLBITS >> 1 ) ) - ( b_i & ( WORDALLBITS >> 1 ) ) - borrow ) >> ( WORDBITS - 1 );
			borrow += ( b_i >> ( WORDBITS - 1 ) ) - ( a_i >> ( WORDBITS - 1 ) );
			++borrow;
			borrow >>= 1;
		}
		
#endif
		
	}
	
	// --n
	BIGONETYPE void decrement( T &n )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			if ( n[ ii ] -- )
				break;
	}
	
	
	//////// negation ////////
	
	// n = -n
	BIGONETYPE void negate( T &n )
	{
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
			n[ ii ] = ~n[ ii ];
			
		increment( n );
	}
	
	
	//////// multiplication ////////
	
	// a *= a, unsigned
	BIGONETYPE void usquare( T &a )
	{
		T a0, a1;
		
		set ( a0, a )
		
		;
		set ( a1, a )
		
		;
		zero( a );
		
		u32 shifts = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word w_i = a0[ ii ];
			
			u16 ctr = WORDBITS;
			
			while ( w_i )
			{
				if ( w_i & 1 )
				{
					if ( shifts )
					{
						shiftLeft( a1, shifts );
						shifts = 0;
					}
					
					add ( a, a1 )
					
					;
				}
				
				w_i >>= 1;
				++shifts;
				--ctr;
			}
			
			shifts += ctr;
		}
	}
	
	// m = a * b (&a != &b != &m), unsigned
	BIGTWOTYPES void umultiply( T &a0, T &b0, Bigger &m )
	{
		Bigger a;
		usetlow( a, a0 );
		
		zero( m );
		
		u32 shifts = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word w_i = b0[ ii ];
			
			u16 ctr = WORDBITS;
			
			while ( w_i )
			{
				if ( w_i & 1 )
				{
					if ( shifts )
					{
						shiftLeft( a, shifts );
						shifts = 0;
					}
					
					add ( m, a )
					
					;
				}
				
				w_i >>= 1;
				++shifts;
				--ctr;
			}
			
			shifts += ctr;
		}
	}
	
	// a *= b (&a != &b), unsigned
	BIGTWOTYPES void umultiply( Bigger &a0, T &b0 )
	{
		Bigger a;
		
		set ( a, a0 )
		
		;
		
		zero( a0 );
		
		u32 shifts = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word w_i = b0[ ii ];
			
			u16 ctr = WORDBITS;
			
			while ( w_i )
			{
				if ( w_i & 1 )
				{
					if ( shifts )
					{
						shiftLeft( a, shifts );
						shifts = 0;
					}
					
					add ( a0, a )
					
					;
				}
				
				w_i >>= 1;
				++shifts;
				--ctr;
			}
			
			shifts += ctr;
		}
	}
	
	// a *= a, signed
	BIGONETYPE void ssquare( T &a )
	{
		T a0, a1;
		
		if ( BIGHIGHBIT( a ) )
			negate( a );
			
		set ( a0, a )
		
		;
		set ( a1, a )
		
		;
		zero( a );
		
		u32 shifts = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word w_i = a0[ ii ];
			
			u16 ctr = WORDBITS;
			
			while ( w_i )
			{
				if ( w_i & 1 )
				{
					if ( shifts )
					{
						shiftLeft( a1, shifts );
						shifts = 0;
					}
					
					add ( a, a1 )
					
					;
				}
				
				w_i >>= 1;
				++shifts;
				--ctr;
			}
			
			shifts += ctr;
		}
	}
	
	// m = a * b (&a != &b != &m), signed
	BIGTWOTYPES void smultiply( T &a0, T &b0, Bigger &m )
	{
		Bigger a;
		ssetlow( a, a0 );
		
		word sign_a = BIGHIGHBIT( a );
		
		if ( sign_a )
			negate( a );
			
		T b;
		
		set ( b, b0 )
		
		;
		
		word sign_b = BIGHIGHBIT( b );
		
		if ( sign_b )
			negate( b );
			
		zero( m );
		
		u32 shifts = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word w_i = b[ ii ];
			
			u16 ctr = WORDBITS;
			
			while ( w_i )
			{
				if ( w_i & 1 )
				{
					if ( shifts )
					{
						shiftLeft( a, shifts );
						shifts = 0;
					}
					
					add ( m, a )
					
					;
				}
				
				w_i >>= 1;
				++shifts;
				--ctr;
			}
			
			shifts += ctr;
		}
		
		if ( sign_a ^ sign_b )
			negate( m );
	}
	
	// a *= b (&a != &b), signed
	BIGTWOTYPES void smultiply( Bigger &a0, T &b0 )
	{
		Bigger a;
		ssetlow( a, a0 );
		
		word sign_a = BIGHIGHBIT( a );
		
		if ( sign_a )
			negate( a );
			
		T b;
		
		set ( b, b0 )
		
		;
		
		word sign_b = BIGHIGHBIT( b );
		
		if ( sign_b )
			negate( b );
			
		zero( a0 );
		
		u32 shifts = 0;
		
		for ( u32 ii = 0; ii < BIGWORDCOUNT( T ); ++ii )
		{
			word w_i = b[ ii ];
			
			u16 ctr = WORDBITS;
			
			while ( w_i )
			{
				if ( w_i & 1 )
				{
					if ( shifts )
					{
						shiftLeft( a, shifts );
						shifts = 0;
					}
					
					add ( a0, a )
					
					;
				}
				
				w_i >>= 1;
				++shifts;
				--ctr;
			}
			
			shifts += ctr;
		}
		
		if ( sign_a ^ sign_b )
			negate( a0 );
	}
	
	
	//////// division/remainder ////////
	
	// {q, r} = a / b (&q != &r), unsigned
	BIGONETYPE void udivide( T &a, T &b0, T &q, T &r )
	{
		T b;
		
		set ( b, b0 )
		
		;
		set ( r, a )
		
		;
		zero( q );
		
		u32 shifts = 1;
		
		// sort of: shift b left until b > r, then shift back one
		if ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
		{
			s32 ii, jj;
			
			// shift by words if possible
			
			for ( ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
				if ( r[ ii ] )
					break;
					
			for ( jj = ii; jj >= 0; --jj )
				if ( b[ jj ] )
					break;
					
			if ( ii != jj )
			{
				shifts = ( ii - jj ) * WORDBITS;
				shiftLeft( b, shifts );
				++shifts;
			}
			
			while ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
			{
				shiftLeft1( b );
				
				++shifts;
			}
			
			while ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
		}
		
		else
			if ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
			
		u32 qshifts = 0;
		
		while ( shifts-- )
		{
			++qshifts;
			
			if ( !ugreater( b, r ) )
			{
				subtract( r, b );
				
				shiftLeft( q, qshifts );
				qshifts = 0;
				
				q[ 0 ] |= 1;
			}
			
			ushiftRight1( b );
		}
		
		shiftLeft( q, qshifts );
	}
	
	// r = a Mod b, unsigned
	BIGONETYPE void umodulo( T &a, T &b0, T &r )
	{
		T b;
		u32 shifts = 1;
		
		set ( b, b0 )
		
		;
		set ( r, a )
		
		;
		
		if ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
		{
			s32 ii, jj;
			
			// shift by words if possible
			
			for ( ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
				if ( r[ ii ] )
					break;
					
			for ( jj = ii; jj >= 0; --jj )
				if ( b[ jj ] )
					break;
					
			if ( ii != jj )
			{
				shifts = ( ii - jj ) * WORDBITS;
				shiftLeft( b, shifts );
				++shifts;
			}
			
			while ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
			{
				shiftLeft1( b );
				
				++shifts;
			}
			
			while ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
		}
		
		else
			if ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
			
		while ( shifts-- )
		{
			if ( !ugreater( b, r ) )
				subtract( r, b );
				
			ushiftRight1( b );
		}
	}
	
	// {q, r} = a / b (&q != &r), signed
	BIGONETYPE void sdivide( T &a, T &b0, T &q, T &r )
	{
		T b;
		
		set ( b, b0 )
		
		;
		set ( r, a )
		
		;
		zero( q );
		
		word sign_a = BIGHIGHBIT( a );
		
		if ( sign_a )
			negate( r );
			
		word sign_b = BIGHIGHBIT( b );
		
		if ( sign_b )
			negate( b );
			
		u32 shifts = 1;
		
		if ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
		{
			s32 ii, jj;
			
			// shift by words if possible
			
			for ( ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
				if ( r[ ii ] )
					break;
					
			for ( jj = ii; jj >= 0; --jj )
				if ( b[ jj ] )
					break;
					
			if ( ii != jj )
			{
				shifts = ( ii - jj ) * WORDBITS;
				shiftLeft( b, shifts );
				++shifts;
			}
			
			while ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
			{
				shiftLeft1( b );
				
				++shifts;
			}
			
			while ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
		}
		
		else
			if ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
			
		u32 qshifts = 0;
		
		while ( shifts-- )
		{
			++qshifts;
			
			if ( !ugreater( b, r ) )
			{
				subtract( r, b );
				
				shiftLeft( q, qshifts );
				qshifts = 0;
				
				q[ 0 ] |= 1;
			}
			
			ushiftRight1( b );
		}
		
		shiftLeft( q, qshifts );
		
		if ( sign_a ^ sign_b )
			negate( q );
			
		if ( sign_a )
			negate( r );
	}
	
	// r = a Mod b, signed
	BIGONETYPE void smodulo( T &a, T &b0, T &r )
	{
		T b;
		u32 shifts = 1;
		
		set ( b, b0 )
		
		;
		set ( r, a )
		
		;
		
		word sign_a = BIGHIGHBIT( a );
		
		if ( sign_a )
			negate( r );
			
		word sign_b = BIGHIGHBIT( b );
		
		if ( sign_b )
			negate( b );
			
		if ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
		{
			s32 ii, jj;
			
			// shift by words if possible
			
			for ( ii = BIGWORDCOUNT( T ) - 1; ii >= 0; --ii )
				if ( r[ ii ] )
					break;
					
			for ( jj = ii; jj >= 0; --jj )
				if ( b[ jj ] )
					break;
					
			if ( ii != jj )
			{
				shifts = ( ii - jj ) * WORDBITS;
				shiftLeft( b, shifts );
				++shifts;
			}
			
			while ( !BIGHIGHBIT( b ) && ugreater( r, b ) )
			{
				shiftLeft1( b );
				
				++shifts;
			}
			
			while ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
		}
		
		else
			if ( ugreater( b, r ) )
			{
				ushiftRight1( b );
				
				--shifts;
			}
			
		while ( shifts-- )
		{
			if ( !ugreater( b, r ) )
				subtract( r, b );
				
			ushiftRight1( b );
		}
		
		if ( sign_a )
			negate( r );
	}
	
	
	//////// converting to/from strings ////////
	
#ifdef BIG_USES_STRINGS
	
	// n -> string
	BIGONETYPE std::string toString( T &n0, bool sign = true, u16 radix = 10 )
	{
		T n, base, r;
		std::string s;
		
		set ( n, n0 )
		
		;
		usetw( base, radix );
		
		word sign_n = 0;
		
		if ( sign && ( sign_n = BIGHIGHBIT( n ) ) )
			negate( n );
			
		do // always allow first iteration for zero
		{
			// {q, r} = n / base
			udivide( n, base, n, r );
			
			char ch = ( char ) r[ 0 ];
			
			if ( ch >= 10 )
				ch += 'a' - 10;
			else
				ch += '0';
				
			// insert character
			s = ch + s;
		}
		
		while ( !isZero( n ) );
		
		if ( sign_n )
			s = '-' + s;
			
		return s;
	}
	
	// s -> n, signed
	BIGONETYPE void fromString( std::string s, T &n, bool sign = true, u16 radix = 10 )
	{
		T acc, base, temp;
		
		usetw( acc, 1 );
		usetw( base, radix );
		zero( n );
		
		u32 len = ( u32 ) s.length();
		const char *citer = s.c_str() + len;
		
		while ( len-- )
		{
			char ch = *( --citer );
			
			if ( IS_ALPHA( ch ) )   // assumes alpha characters only up to radix
				ch = TO_LOWER( ch ) - 'a' + 10;
			else
				if ( sign && ch == '-' )   // '-' should be first character
				{
					negate( n );
					break;
				}
				else // assumes it's alphanumeric/-
					ch -= '0';
					
			usetw( temp, ch );
			
			umultiply( temp, acc );
			
			add ( n, temp )
			
			;
			
			umultiply( acc, base );
		}
	}
	
#endif // BIG_USES_STRINGS
	
	
	//////// class wrapper ////////
	
	BIGONETYPE INLINE Int<T>::Int()
	{
		big::zero( raw );
	}
	
	BIGONETYPE INLINE Int<T>::Int( word n )
	{
		ssetw( raw, n );
	}
	
#ifdef BIG_USES_STRINGS
	
	BIGONETYPE INLINE Int<T>::Int( std::string &s )
	{
		fromString( s, raw );
	}
	
#endif
	
	BIGONETYPE INLINE Int<T>::Int( T &n )
	{
		set ( raw, n )
		
		;
	}
	
	BIGONETYPE INLINE Int<T>::operator T &()
	{
		return raw;
	}
	
	
	BIGONETYPE BIGINTFAST Int<T>::zero()
	{
		big::zero( raw );
		return *this;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator=( word n )
	{
		ssetw( raw, n );
		return *this;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator=( T &n )
	{
		set ( raw, n )
		
		;
		return *this;
	}
	
	
	BIGONETYPE BIGINTFAST Int<T>::operator<<=( u32 s )
	{
		shiftLeft( raw, s );
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator<<( u32 s )
	{
		Int<T> temp( raw );
		return temp <<= s;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator>>=( u32 s )
	{
		shiftRight( raw, s );
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator>>( u32 s )
	{
		Int<T> temp( raw );
		return temp >>= s;
	}
	
	
	BIGONETYPE BIGINTFAST Int<T>::operator+=( T &n )
	{
		add ( raw, n )
		
		;
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator+( T &n )
	{
		Int<T> temp( raw );
		return temp += n;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator-=( T &n )
	{
		subtract( raw, n );
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator-( T &n )
	{
		Int<T> temp( raw );
		return temp -= n;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator++()   // prefix
	{
		increment( raw );
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator++( int )   // postfix
	{
		Int<T> temp( raw );
		increment( raw );
		return temp;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator--()   // prefix
	{
		decrement( raw );
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator--( int )   // postfix
	{
		Int<T> temp( raw );
		decrement( raw );
		return temp;
	}
	
	
	BIGONETYPE BIGINTSLOW Int<T>::operator-( int )   // negation
	{
		Int<T> temp( raw );
		negate( temp );
		return temp;
	}
	
	
	BIGONETYPE BIGINTSLOW Int<T>::operator*( T &n )
	{
		Int<T> temp( raw );
		return temp *= n;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::square()
	{
		square( raw );
		return *this;
	}
	
	
	BIGONETYPE BIGINTFAST Int<T>::operator/=( T &n )
	{
		T discard;
		divide( raw, n, raw, discard );
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator/( T &n )
	{
		Int<T> temp( raw );
		return temp /= n;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator%=( T &n )
	{
#if (defined(__GNUC__)  || defined(__GCCXML__))
		smodulus( raw, n, raw );
#else
		modulus( raw, n, raw );
#endif
		return *this;
	}
	
	BIGONETYPE BIGINTSLOW Int<T>::operator%( T &n )
	{
		Int<T> temp( raw );
		return temp %= n;
	}
	
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator>( T &n )
	{
		return sgreater( raw, n );
	}
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator>=( T &n )
	{
		return sgreaterOrEqual( raw, n );
	}
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator<( T &n )
	{
		return !sgreaterOrEqual( raw, n );
	}
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator<=( T &n )
	{
		return !sgreater( raw, n );
	}
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator==( T &n )
	{
		return equal( raw, n );
	}
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator!=( T &n )
	{
		return !equal( raw, n );
	}
	
	BIGONETYPE INLINE /* fast */ bool Int<T>::operator!()
	{
		return isZero( raw );
	}
	
#ifdef BIG_USES_STRINGS
	
	BIGONETYPE INLINE /* fast */ std::string Int<T>::str()
	{
		return toString( raw );
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator=( std::string &s )
	{
		fromString( s, raw );
		return *this;
	}
	
	BIGONETYPE BIGINTFAST Int<T>::operator=( const char *s )
	{
		fromString( std::string( s ), raw );
		return *this;
	}
	
#endif // BIG_USES_STRINGS
}

#endif // #if !defined(_XBOX360)

#endif // BIGTYPES_H

#ifdef _MSC_VER
#pragma warning( pop )
#endif
