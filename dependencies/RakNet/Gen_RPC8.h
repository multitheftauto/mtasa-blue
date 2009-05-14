#ifndef __GEN_RPC8_H
#define __GEN_RPC8_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // memcpy
#include <typeinfo>
#if defined(_XBOX) || defined(X360)
#include <XBOX360Includes.h>
#elif defined (_WIN32)
#include "WindowsIncludes.h"
#endif
#include <stddef.h>
//#define ASSEMBLY_BLOCK asm
//#include "Types.h"
#include "BitStream.h"
// #define AUTO_RPC_NO_ASM

#ifdef _WIN64
#define AUTO_RPC_NO_ASM
#endif

namespace GenRPC
{

//#define __BITSTREAM_NATIVE_END 1


// -8<----8<----8<----BEGIN

//
// 0. References
//   a. Calling conventions for different C++ compilers and operating systems [http://www.agner.org/optimize]
//   b. System V Application Binary Interface AMD64 Architecture Processor Supplement
//     Used by 64-bit MAC and 64-bit Linux.
//   c. 32-bit PowerPC MAC calling conventions [http://developer.apple.com/documentation/DeveloperTools/Conceptual/LowLevelABI/Articles/32bitPowerPC.html#//apple_ref/doc/uid/TP40002438-SW20]
//   d. 32-bit IA MAC calling conventions [http://developer.apple.com/documentation/DeveloperTools/Conceptual/LowLevelABI/Articles/IA32.html#//apple_ref/doc/uid/TP40002492-SW4]
//   e. Calling conventions on 64-bit windows [http://msdn2.microsoft.com/en-us/library/zthk2dkh(VS.80).aspx]
//   f. 64-bit PowerPC MAC calling conventions [http://developer.apple.com/documentation/DeveloperTools/Conceptual/LowLevelABI/Articles/64bitPowerPC.html#//apple_ref/doc/uid/TP40002471]
//
// 1. General Introduction.
//
//   Quite a lot of code we write hinges on hidden assumptions. For instance, most code tacitly
//   assumes a char is 8 bits, even though it needn't be.  And how much code relies on
//   two's-a-compliment numbers, by, for example, assuming the only representation of zero
//   is all bits clear? Yet this too isn't mandated by the C standard - which allows for non
//   two's a compliment numbers.
//   And the switch to 64-bit will see us discovering how
//   much of our code assumes sizeof(int) == sizeof(long) == sizeof(void*)
//
//   These tradeoffs and compromises are made because we know the architectures
//   with CHAR_BITS != 8 are embedded systems of FPGAs and we don't extend our definition of
//   portability to those systems.  Or Windows code can
//   assume its running on a little-endian machine, without loss of generality.  In fact, it
//   often impossible to test our code in situatiosn where assumptions are not true.
//
//   The same is true of a lightweight RPC - to be possible at all, it will have make some
//   assumptions about the architecture on (like CHAR_BITS == 8) which limit portability, but
//   which are not unreasonable for the cases where its expected to run (modern personal computers)
//   and hopefully can be extended easily to meet new cases.
//
// 2. Parameter passing - Introduction
//
//   The C standard doesn't mandate how parameters are passed from one function to another.
//   That's down to the particular archictecture, normally laid out in the Application Binary
//   Interface (ABI).
//
//   On some architecture (e.g. 32bit X86) the parameters are all passed on the stack;
//   on some architectures (e.g. SPARC) there is no stack and they are all passed in register.
//   Sometimes the function must be passed the exact number of args it expects (e.g. WIN32
//   "stdcall"); somtimes it can take an arbitrary number of args (IA-32/64 linux).
//
//   But whatever the case, the compiler knows about all the details - it sorts them out every
//   time we write a call.  So to make it portable we must ensure we pass the compiler *sufficient*
//   information to be able to encode the call, in all the cases we're interested in.  To do this
//   we need some knowledge of the ABI, without getting our hands dirty writing assembler.
//   Not only because we can't all be experts at every particularl architecture with its necessary
//   prefetches, conditional moves and innate parralelism, but also because that allows the compiler to
//   code for the exact processors - rather than using a lowest-common denominator.
//
//
// 3. Preparing parameters and assumptions
//
//   We assume that the processor has a 32 bit or 64 bit "natural word size" - and that the
//   registers, stack entries (if a stack exists) and pointers all have this natural word
//   size.  We further assume (1) parameters smaller than this have to be padded out to meet this
//   size, and that (2) this can be done by zero-extended them, regardless of whether they are signed
//   or unsigned quanitites.
//
//   The SysV ABI for 64bit X86 [b] and that ABI for 64-bit windows [e] require that floating point
//   parameters are passed in floating points registers - so to work with these types we need to know
//   they are floats and alert the compiler.  A similar arrangement is true for both 32-bit and 64-bit
//   Power PC systems.
//
//   This can extend to structures ("aggregate types") containing floating point numbers - where
//   individual members can still be passed in register. (Although
//   on 64-bit windows, aggregates of size > 8 bytes are passed on the stack, so,
//   except for the pathological case of struct S { double F }; there are no problems with
//   structures containing floats.)
//
// ---

//
// AUTO_RPC_MAX_PARAMS:              Absolute number of stack words we can handle
// ABI:                     used to select features specific to ABI.
// AUTO_RPC_INT_REG_PARAMS:          Number of parameters passed in integer registers. (Only used by SYSV ABI)
// AUTO_RPC_FLOAT_REG_PARAMS:        Number of parameters passed in floating point registers.
// AUTO_RPC_INT_SHADOW_OF_FLOATS:    Create a copy of the floats in the integer/stack space.
// AUTO_RPC_ALLOC_SEPARATE_FLOATS:   Push floats to a separate contiguous floating point "stack".
//                          Ortherwise we rely on shadow.
//
// PARAMETER_REF_THRESHOLD: parameters bigger than this are replaced by a reference ( WIN64 )
//
//
#define AUTO_RPC_MAX_PARAMS 64

#define AUTO_RPC_ABI_NONE         0   // not supported - should fail.

#define AUTO_RPC_ABI_IA_32        1   // all parameters are passed on the stack.
// preserves: ebx,esi,edi,ebp

#define AUTO_RPC_ABI_WIN_AMD64    2   // first four parameters in either
// rcx|xmm0, rdx|xmm1,r8|xmm2,r9|xmm3
// preserves: rbx,rsi,rdi,rbp,r12-r15; xmm6-xmm15
// comments: aggregates > 8 passed by ref; reg params shadowed on stack

#define AUTO_RPC_ABI_SYSV_AMD64   3   // first six ints in: rdi,rsi,rdx,rcx,r8,r9
// first eight reals: in xmm0...xmm7
// preserves: rbx,rbp,r12-r15
// comments: aggregates > 16 bumped to stack

#define AUTO_RPC_ABI_PPC          4   // first 6 args (even if float) in int reg; first 13 floats in reg.
// parameter passing area with shadow area.


// Configure the parameters for the system.
#if defined(__i386__) || defined( _M_IX86 ) || defined( __INTEL__ )
// 32 bit system.
#define AUTO_RPC_AUTORPC_WORD 32

typedef unsigned int  NaturalWord;
typedef double        HardwareReal;

#define AUTO_RPC_INT_REG_PARAMS 0
#define AUTO_RPC_FLOAT_REG_PARAMS 0

#define AUTO_RPC_ABI AUTO_RPC_ABI_IA_32
#define AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD 0
#define AUTO_RPC_INT_SHADOW_OF_FLOATS  1
#define AUTO_RPC_ALLOC_SEPARATE_FLOATS 0
#define AUTO_RPC_CREATE_FLOAT_MAP 0

#elif defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_AMD64 )
#define AUTO_RPC_AUTORPC_WORD 64

#if defined( _WIN64 )
#define AUTO_RPC_FLOAT_REG_PARAMS 4
#define AUTO_RPC_INT_REG_PARAMS 4
#define AUTO_RPC_ABI AUTO_RPC_ABI_WIN_AMD64
#define AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD 8
#define AUTO_RPC_INT_SHADOW_OF_FLOATS  1
#define AUTO_RPC_ALLOC_SEPARATE_FLOATS 0
#define AUTO_RPC_CREATE_FLOAT_MAP 1
#else
#define AUTO_RPC_ABI AUTO_RPC_ABI_SYSV_AMD64
#define AUTO_RPC_INT_REG_PARAMS 6
#define AUTO_RPC_FLOAT_REG_PARAMS 8
#define AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD 0
#define AUTO_RPC_INT_SHADOW_OF_FLOATS  0
#define AUTO_RPC_ALLOC_SEPARATE_FLOATS 1
#define AUTO_RPC_CREATE_FLOAT_MAP 0
#endif

// NB OS's differ over.
typedef unsigned long long NaturalWord;
typedef double         HardwareReal;  // could be changed to __float128 on AMD64/nonwin

#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
typedef double HardwareReal;
typedef unsigned long long NaturalWord;
#define AUTO_RPC_AUTORPC_WORD 64
#define AUTO_RPC_INT_REG_PARAMS 8
#define AUTO_RPC_FLOAT_REG_PARAMS 13
#define AUTO_RPC_INT_SHADOW_OF_FLOATS 1
#define AUTO_RPC_ALLOC_SEPARATE_FLOATS 1
#define AUTO_RPC_CREATE_FLOAT_MAP 0
#define AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD 0
#define AUTO_RPC_ABI AUTO_RPC_ABI_PPC

#elif defined(_M_PPC) || defined( __POWERPC__ )

#include <limits.h>

/// PPC Mac doesn't support sizeof( long ) in an #if statement
#if defined (LONG_BIT)
	#if LONG_BIT == 64
		#define AUTORPC_WORD 64
		typedef double HardwareReal;
		typedef unsigned long long NaturalWord;
	#else
		#define AUTORPC_WORD 32
		typedef double HardwareReal;
		typedef unsigned int NaturalWord;
	#endif
#else
	#if defined(_XBOX) || defined(X360)
		#define AUTORPC_WORD 32
		typedef double HardwareReal;
		typedef unsigned int NaturalWord;
	#else
		#if sizeof( long ) == 8
			#define AUTORPC_WORD 64
			typedef double HardwareReal;
			typedef unsigned long long NaturalWord;
		#else
			#define AUTORPC_WORD 32
			typedef double HardwareReal;
			typedef unsigned int NaturalWord;
		#endif
	#endif
#endif

#define AUTO_RPC_INT_REG_PARAMS 8
#define AUTO_RPC_FLOAT_REG_PARAMS 13
#define AUTO_RPC_INT_SHADOW_OF_FLOATS 1
#define AUTO_RPC_ALLOC_SEPARATE_FLOATS 1
#define AUTO_RPC_CREATE_FLOAT_MAP 0
#define AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD 0
#define AUTO_RPC_ABI AUTO_RPC_ABI_PPC


#else
#ifdef __GNUC__
// gcc won't implemented message - so use #warning
#warning Unknown Architecture
#else
#pragma  message( Unknown architecture )
#endif

// defining AUTO_RPC_ABI_NONE, creates stub code that fails
#define AUTO_RPC_ABI AUTO_RPC_ABI_NONE
#endif

//
// Calling convention - we need to be explict on WIN32, so we do that here.  Everybody else
// has only one fixed, calling convention.
//
#ifdef _WIN32
#define AUTO_RPC_CALLSPEC WINAPIV
#else
#define AUTO_RPC_CALLSPEC
#endif

//
// useful macros; could be rewritten inline/inline templates
//
#define AUTO_RPC__ALIGN_P2( len, bytes )  ( ( len + bytes - 1 ) & ~( bytes - 1 ) )

// Return len rounded-up to an integral number of sizeof(type) - provided sizeof(type) is a power of 2
#define AUTO_RPC_ALIGN_P2( len, type )    AUTO_RPC__ALIGN_P2( len, sizeof( type ) )

// Return ptr to end of  'array[xxx]'
#define AUTO_RPC_ARRAY_END( array ) &array[ sizeof( array ) / sizeof( array[0] ) ]

// strip floating point params if there are no float regs.
#if !AUTO_RPC_FLOAT_REG_PARAMS

#if AUTO_RPC_CREATE_FLOAT_MAP
#undef AUTO_RPC_CREATE_FLOAT_MAP
#define AUTO_RPC_CREATE_FLOAT_MAP 0
#endif

#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
#undef AUTO_RPC_ALLOC_SEPARATE_FLOATS
#define AUTO_RPC_ALLOC_SEPARATE_FLOATS 0
#endif

#endif // FLOAT_REG_PARAM

#define AUTO_RPC_REF_ALIGN 16    // some structures must be memory aligned to 16 bytes, even, on 32-bit systems.
// stack simialrly must be aligned
#define AUTO_RPC_STACK_PADDING  ( sizeof(  NaturalWord ) / AUTO_RPC_REF_ALIGN )


// defining these externally makes the code a hell of a lot more readable.
#ifdef USE_VARADIC_CALL

#define AUTO_RPC_NW_3 ...
#define AUTO_RPC_NW_6 ...
#define AUTO_RPC_NW_9 ...
#define AUTO_RPC_NW_12 ...
#define AUTO_RPC_NW_64 ...

#else
#define AUTO_RPC_NW_3 NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_6 NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_9 NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_5 NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_4_9 AUTO_RPC_NW_5

#define AUTO_RPC_NW_12 NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_32 NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_64 NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_60 NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord,\
	NaturalWord,NaturalWord,NaturalWord,NaturalWord

#define AUTO_RPC_NW_4_64 AUTO_RPC_NW_60

#endif // USE VARADIC

#define AUTO_RPC_INT_ARGS_3(  call )  call.intParams[0],call.intParams[1],call.intParams[2]

#define AUTO_RPC_INT_ARGS_6( call )   call.intParams[0],call.intParams[1],call.intParams[2],call.intParams[3],\
	call.intParams[4],call.intParams[5]

#define AUTO_RPC_INT_ARGS_9( call )   call.intParams[0],call.intParams[1],call.intParams[2],call.intParams[3],\
	call.intParams[4],call.intParams[5],call.intParams[6],call.intParams[7],\
	call.intParams[8]

#define AUTO_RPC_INT_ARGS_4_9( call ) call.intParams[4],call.intParams[5],call.intParams[6],call.intParams[7],\
	call.intParams[8]

#define AUTO_RPC_INT_ARGS_12( call )  call.intParams[0],call.intParams[1],call.intParams[2],call.intParams[3],\
	call.intParams[4],call.intParams[5],call.intParams[6],call.intParams[7],\
	call.intParams[8],call.intParams[9],call.intParams[10],call.intParams[11]

#define AUTO_RPC_INT_ARGS_32( call )  call.intParams[0],call.intParams[1],call.intParams[2],call.intParams[3],\
	call.intParams[4],call.intParams[5],call.intParams[6],call.intParams[7],\
	call.intParams[8],call.intParams[9],call.intParams[10],call.intParams[11],\
	call.intParams[12],call.intParams[13],call.intParams[14],call.intParams[15],\
	call.intParams[16],call.intParams[17],call.intParams[18],call.intParams[19],\
	call.intParams[20],call.intParams[21],call.intParams[22],call.intParams[23],\
	call.intParams[24],call.intParams[25],call.intParams[26],call.intParams[27],\
	call.intParams[28],call.intParams[29],call.intParams[30],call.intParams[31]

#define AUTO_RPC_INT_ARGS_4_64( call ) call.intParams[4],call.intParams[5],call.intParams[6],call.intParams[7],\
	call.intParams[8],call.intParams[9],call.intParams[10],call.intParams[11],\
	call.intParams[12],call.intParams[13],call.intParams[14],call.intParams[15],\
	call.intParams[16],call.intParams[17],call.intParams[18],call.intParams[19],\
	call.intParams[20],call.intParams[21],call.intParams[22],call.intParams[23],\
	call.intParams[24],call.intParams[25],call.intParams[26],call.intParams[27],\
	call.intParams[28],call.intParams[29],call.intParams[30],call.intParams[31],\
	call.intParams[32],call.intParams[33],call.intParams[34],call.intParams[35],\
	call.intParams[36],call.intParams[37],call.intParams[38],call.intParams[39],\
	call.intParams[40],call.intParams[41],call.intParams[42],call.intParams[43],\
	call.intParams[44],call.intParams[45],call.intParams[46],call.intParams[47],\
	call.intParams[48],call.intParams[49],call.intParams[50],call.intParams[51],\
	call.intParams[52],call.intParams[53],call.intParams[54],call.intParams[55],\
	call.intParams[56],call.intParams[57],call.intParams[58],call.intParams[59],\
	call.intParams[60],call.intParams[61],call.intParams[62],call.intParams[63]

#define AUTO_RPC_INT_ARGS_64( call )  call.intParams[0],call.intParams[1],call.intParams[2],call.intParams[3],\
	call.intParams[4],call.intParams[5],call.intParams[6],call.intParams[7],\
	call.intParams[8],call.intParams[9],call.intParams[10],call.intParams[11],\
	call.intParams[12],call.intParams[13],call.intParams[14],call.intParams[15],\
	call.intParams[16],call.intParams[17],call.intParams[18],call.intParams[19],\
	call.intParams[20],call.intParams[21],call.intParams[22],call.intParams[23],\
	call.intParams[24],call.intParams[25],call.intParams[26],call.intParams[27],\
	call.intParams[28],call.intParams[29],call.intParams[30],call.intParams[31],\
	call.intParams[32],call.intParams[33],call.intParams[34],call.intParams[35],\
	call.intParams[36],call.intParams[37],call.intParams[38],call.intParams[39],\
	call.intParams[40],call.intParams[41],call.intParams[42],call.intParams[43],\
	call.intParams[44],call.intParams[45],call.intParams[46],call.intParams[47],\
	call.intParams[48],call.intParams[49],call.intParams[50],call.intParams[51],\
	call.intParams[52],call.intParams[53],call.intParams[54],call.intParams[55],\
	call.intParams[56],call.intParams[57],call.intParams[58],call.intParams[59],\
	call.intParams[60],call.intParams[61],call.intParams[62],call.intParams[63]

#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
#if AUTO_RPC_FLOAT_REG_PARAMS == 8

#define AUTO_RPC_FLOAT_REG_TYPE HardwareReal,HardwareReal,HardwareReal,HardwareReal,\
	HardwareReal,HardwareReal,HardwareReal,HardwareReal
#define AUTO_RPC_FLOAT_REG_ARGS( a ) a.realParams[0],a.realParams[1],a.realParams[2],a.realParams[3],\
	a.realParams[4],a.realParams[5],a.realParams[6],a.realParams[7]

#elif AUTO_RPC_FLOAT_REG_PARAMS == 4

#define AUTO_RPC_FLOAT_REG_TYPE HardwareReal,HardwareReal,HardwareReal,HardwareReal
#define AUTO_RPC_FLOAT_REG_ARGS( a ) a.realParams[0],a.realParams[1],a.realParams[2],a.realParams[3]

#elif AUTO_RPC_FLOAT_REG_PARAMS == 13

#define AUTO_RPC_FLOAT_REG_TYPE HardwareReal,HardwareReal,HardwareReal,HardwareReal,\
	HardwareReal,HardwareReal,HardwareReal,HardwareReal,\
	HardwareReal,HardwareReal,HardwareReal,HardwareReal,HardwareReal
#define AUTO_RPC_FLOAT_REG_ARGS( a ) a.realParams[0],a.realParams[1],a.realParams[2],a.realParams[3],\
	a.realParams[4],a.realParams[5],a.realParams[6],a.realParams[7],\
	a.realParams[8],a.realParams[9],a.realParams[10],a.realParams[11],a.realParams[12]

#elif AUTO_RPC_FLOAT_REG_PARAMS
#error Need FLOAT_REG_TYPE and AUTO_RPC_FLOAT_REG_ARGS setup
#endif


#endif // AUTO_RPC_ALLOC_SEPARATE_FLOATS

/// \internal 
/// Writes number of parameters to push on the stack
void SerializeHeader(char *&out, unsigned int numParams);

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
unsigned int BuildStack(char *stack);

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1>
unsigned int BuildStack(char *stack, P1 p1,
	bool es1=true)
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 1);
	PushHeader(stackPtr, p1, es1);
	Push( stackPtr, p1, es1 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2>
unsigned int BuildStack(char *stack, P1 p1, P2 p2,
	bool es1=true, bool es2=true)
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 2);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2, class P3>
unsigned int BuildStack(char *stack, P1 p1, P2 p2, P3 p3,
	bool es1=true, bool es2=true, bool es3=true )
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 3);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	PushHeader(stackPtr, p3, es3);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	Push( stackPtr, p3, es3 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2, class P3, class P4>
unsigned int BuildStack(char *stack, P1 p1, P2 p2, P3 p3, P4 p4,
	bool es1=true, bool es2=true, bool es3=true, bool es4=true )
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 4);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	PushHeader(stackPtr, p3, es3);
	PushHeader(stackPtr, p4, es4);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	Push( stackPtr, p3, es3 );
	Push( stackPtr, p4, es4 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2, class P3, class P4, class P5>
unsigned int BuildStack(char *stack, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5,
	bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true )
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 5);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	PushHeader(stackPtr, p3, es3);
	PushHeader(stackPtr, p4, es4);
	PushHeader(stackPtr, p5, es5);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	Push( stackPtr, p3, es3 );
	Push( stackPtr, p4, es4 );
	Push( stackPtr, p5, es5 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2, class P3, class P4, class P5, class P6>
unsigned int BuildStack(char *stack, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
	bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true, bool es6=true )
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 6);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	PushHeader(stackPtr, p3, es3);
	PushHeader(stackPtr, p4, es4);
	PushHeader(stackPtr, p5, es5);
	PushHeader(stackPtr, p6, es6);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	Push( stackPtr, p3, es3 );
	Push( stackPtr, p4, es4 );
	Push( stackPtr, p5, es5 );
	Push( stackPtr, p6, es6 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
unsigned int BuildStack(char *stack, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7,
	bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true, bool es6=true, bool es7=true )
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 7);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	PushHeader(stackPtr, p3, es3);
	PushHeader(stackPtr, p4, es4);
	PushHeader(stackPtr, p5, es5);
	PushHeader(stackPtr, p6, es6);
	PushHeader(stackPtr, p7, es7);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	Push( stackPtr, p3, es3 );
	Push( stackPtr, p4, es4 );
	Push( stackPtr, p5, es5 );
	Push( stackPtr, p6, es6 );
	Push( stackPtr, p7, es7 );
	return (unsigned int)(stackPtr-stack);
}

/// Builds up a function call and all parameters onto a stack
/// \param[out] Destination stack, which must be big enough to hold all parameters
template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
unsigned int BuildStack(char *stack, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8,
	bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true, bool es6=true, bool es7=true, bool es8=true )
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 8);
	PushHeader(stackPtr, p1, es1);
	PushHeader(stackPtr, p2, es2);
	PushHeader(stackPtr, p3, es3);
	PushHeader(stackPtr, p4, es4);
	PushHeader(stackPtr, p5, es5);
	PushHeader(stackPtr, p6, es6);
	PushHeader(stackPtr, p7, es7);
	PushHeader(stackPtr, p8, es8);
	Push( stackPtr, p1, es1 );
	Push( stackPtr, p2, es2 );
	Push( stackPtr, p3, es3 );
	Push( stackPtr, p4, es4 );
	Push( stackPtr, p5, es5 );
	Push( stackPtr, p6, es6 );
	Push( stackPtr, p7, es7 );
	Push( stackPtr, p8, es8 );
	return (unsigned int)(stackPtr-stack);
}

/// \internal
template <class item>
void Push( char*& p, item const i, bool doEndianSwap ) {
	memcpy( (void*)p, (void*)&i, sizeof( i ) );
	if (doEndianSwap && RakNet::BitStream::DoEndianSwap())
		RakNet::BitStream::ReverseBytesInPlace((unsigned char*) p,sizeof( i ));
	p += sizeof( i );
}

/// \internal
template <class item>
void Push( char*& p, item*const i, bool doEndianSwap) {
	memcpy( (void*)p, (void*)i, sizeof( *i ) );
	if (doEndianSwap && RakNet::BitStream::DoEndianSwap())
		RakNet::BitStream::ReverseBytesInPlace((unsigned char*) p,sizeof( i ));
	p += sizeof( *i );
}

/// \internal
template <class item>
void Push( char*& p, item const*const i, bool doEndianSwap) {
	memcpy( (void*)p, (void*)i, sizeof( *i ) );
	if (doEndianSwap && RakNet::BitStream::DoEndianSwap())
		RakNet::BitStream::ReverseBytesInPlace((unsigned char*) p,sizeof( i ));
	p += sizeof( *i );
}

/// \internal
void Push( char*& p, char*const i, bool doEndianSwap);

/// \internal
void Push( char*& p, const char*const i, bool doEndianSwap );

// THIS STRUCTURE LAYOUT IS HARDCODED INTO THE ASSEMBLY.  Unfortunately, that appears to be the
// only way to do it.
struct CallParams {
#if AUTO_RPC_ABI
#if AUTO_RPC_FLOAT_REG_PARAMS
	// on most platforms, just a bool telling us whether we need any floats.
	unsigned       numRealParams;

#if AUTO_RPC_CREATE_FLOAT_MAP
	//
	// bitmask: bit(n) set indicate parameter n is a float, not an int.
	//
	unsigned       realMap;
#endif

	// N.B. these may not have type HardwareReal - they're not promoted or converted.
#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
	HardwareReal   realParams[ AUTO_RPC_FLOAT_REG_PARAMS ];
#endif

#endif // AUTO_RPC_FLOAT_REG_PARAMS

	unsigned       numIntParams;
#if !AUTO_RPC_ALLOC_SEPARATE_FLOATS && AUTO_RPC_FLOAT_REG_PARAMS && AUTO_RPC_CREATE_FLOAT_MAP
	union {
		HardwareReal realParams[ AUTO_RPC_FLOAT_REG_PARAMS ];
#endif
		NaturalWord  intParams[ ( AUTO_RPC_MAX_PARAMS > AUTO_RPC_INT_REG_PARAMS ? AUTO_RPC_MAX_PARAMS : AUTO_RPC_INT_REG_PARAMS ) + AUTO_RPC_STACK_PADDING ];

#if !AUTO_RPC_ALLOC_SEPARATE_FLOATS && AUTO_RPC_FLOAT_REG_PARAMS && AUTO_RPC_CREATE_FLOAT_MAP
	};
#endif

	char      refParams[ AUTO_RPC_MAX_PARAMS * AUTO_RPC_REF_ALIGN ];
#endif // AUTO_RPC_ABI
};

/// Given a stack, the length of the stack, a possible last parameter, and a possible this pointer, build a call to a C or C++ function
bool DeserializeParametersAndBuildCall(
	CallParams &call,
	char *in, unsigned int inLength,
	void *lastParam, void *thisPtr);

// Given the output of DeserializeParametersAndBuildCall, actually call a function
bool CallWithStack( CallParams& call, void *functionPtr );

/// \internal
/// functions to return the size of the item.
template <class item>
size_t D_size( item const ) { return sizeof( item ); }

/// \internal
/// functions to return the size of the item.
template <class item>
size_t D_size( item const*const ) { return sizeof( item ); }

/// \internal
/// functions to return the size of the item.
template <class item>
size_t D_size( item*const ) { return sizeof( item ); }

/// \internal
size_t D_size( char*const str );
/// \internal
size_t D_size( char const*const str );

/// \internal
enum {
	// to maintain binary compatibility with a historical decision, bit 1 is not used
	// in defining the "well known param" types
	PARAM_TYPE_MASK = 0x5,
	INT_PARAM   = 0,  // pass by value an integer or structure composed of integers.
	REAL_PARAM  = 1,  // pass by value a SINGLE floating point parameter.
	REF_PARAM   = 4,  // pass a pointer or reference to data which must be aligned.
	STR_PARAM   = 5,  // pass a pointer to this data, which need not be unaligned;
	// but MUST be null terminated.
	// OBJECT_PARAM = 8, // TODO: pass by value an object, object id as first uint32_t of serialized data?
	// OBJECT_REF_PARAM = 9, // TODO: pass by reference an object, object id as first uint32_t of serialized data?
	// SC == "Shift count" (Bit index); which is always useful.
	ENDIAN_SWAP_SC = 1,   DO_ENDIAN_SWAP = 1 << ENDIAN_SWAP_SC,

	RESERVED_BITS = 0xf8,
};

/// \internal
template <class item>
unsigned D_type( item const )         { return INT_PARAM; }

/// \internal
template <class item>
unsigned D_type( item const*const )   { return REF_PARAM; }

/// \internal
template <class item>
unsigned D_type( item*const )         { return REF_PARAM; }

/// \internal
unsigned D_type( const char*const );
/// \internal
unsigned D_type( char*const );

/// \internal
unsigned D_type( float );
/// \internal
unsigned D_type( double );
/// \internal
unsigned D_type( long double );

/// \internal
template <class item>
void PushHeader( char*& p, item const i, bool endianSwap ) {
	unsigned int   s = (unsigned int) D_size( i );
	unsigned char  f = D_type( i ) | ( ((int) endianSwap) << ENDIAN_SWAP_SC );
	Push( p, s, endianSwap );
	Push( p, f, false );
}

}

#endif
