#include "Gen_RPC8.h"

unsigned int GenRPC::BuildStack(char *stack)
{
	char *stackPtr = (char*) stack;
	SerializeHeader(stackPtr, 0);
	return (unsigned int)(stackPtr-stack);
}

/// \internal
void GenRPC::Push( char*& p, char*const i, bool doEndianSwap) {
	(void) doEndianSwap;
	size_t len = strlen( i ) + 1;
	memcpy( (void*)p, i, len );
	p += len;
}

/// \internal
void GenRPC::Push( char*& p, const char*const i, bool doEndianSwap ) {
	(void) doEndianSwap;
	size_t len = strlen( i ) + 1;		
	memcpy( (void*)p, i, len );
	p += len;
}
/// \internal
unsigned GenRPC::D_type( const char*const )    { return STR_PARAM; }
/// \internal
unsigned GenRPC::D_type( char*const )          { return STR_PARAM; }

/// \internal
unsigned GenRPC::D_type( float )              { return REAL_PARAM; }
/// \internal
unsigned GenRPC::D_type( double )             { return REAL_PARAM; }
/// \internal
unsigned GenRPC::D_type( long double )        { return REAL_PARAM; }

/// \internal
size_t GenRPC::D_size( char*const str )       { return strlen( str ) + 1; }
/// \internal
size_t GenRPC::D_size( char const *const str ){ return strlen( str ) + 1; }

void GenRPC::SerializeHeader(char *&out, unsigned int numParams)
{
	*out = (char) numParams;
	out++;
	//out[*writeOffset]=(char) numParams;
	//*writeOffset+=sizeof(unsigned char);
}


//
// @params
//            call: [IN/OUT] workspace to build parameters that we will pass to function
//              in: [IN/OUT] is the serialized buffer - used as a temporary working for swapping
//                  parameters.
//        inLength: [IN] is the length of the above
//       lastParam: [IN] final parameter, added onto the list
//         thisPtr: [IN] if not zero - the value of this (added to start of list).
//
// @returns:
//         true: parameter list created successfully.
//        false: if deserialization fails for some reason.
//
bool GenRPC::DeserializeParametersAndBuildCall(
	CallParams &call,
	char *in, unsigned int inLength,
	void *lastParam, void *thisPtr
	) {

#if AUTO_RPC_ABI

		NaturalWord *intCallParam = call.intParams;

		char        *refParam = call.refParams;

#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
		HardwareReal *realCallParam = call.realParams;
#endif

#if AUTO_RPC_CREATE_FLOAT_MAP
		call.realMap = 0;
		call.numRealParams = 0;
#endif

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
		// large structure parameters have to be bumped up here - which corresponds with the start
		// of the parameters that *are* passed on the stack.
		NaturalWord *memParam = &call.intParams[ AUTO_RPC_INT_REG_PARAMS ];
#endif

		// this is first arg - assume space ;-)
#pragma warning(disable:4311) // pointer truncation
		if ( thisPtr )
			*(intCallParam++) = reinterpret_cast<NaturalWord>( thisPtr );

		unsigned int serializedArgs = *(unsigned char*)in;

		unsigned char* header = (unsigned char*)in + 1;

		unsigned char* data = &header[ serializedArgs * ( sizeof( unsigned int ) + 1 ) ];

		// check we have the entire header in buffer
		if ( data > (unsigned char*) &in[ inLength ] )
			return 0;

		for ( unsigned int i = 0; i < serializedArgs; i++ )
		{

			// read header entry
			int plen = *(unsigned int*)header;
			header += sizeof( unsigned int );
			unsigned char const flags = *( header++ );

			// Some bits are not used by the current implementation. So if we find them we bail because
			// we clearly are not equipped to handle the data - and is there no safe way to "fail".
			if ( flags & RESERVED_BITS )
				return 0;

#ifndef __BITSTREAM_NATIVE_END
			if (flags & DO_ENDIAN_SWAP)
			{
				RakNet::BitStream::ReverseBytesInPlace( (unsigned char*)&plen , sizeof( plen ) );
			}
#endif

			if ( !plen || data + plen > (unsigned char*)&in[ inLength ] )
				return 0;

			// handle null-terminated strings.
			if (  ( flags & PARAM_TYPE_MASK ) == STR_PARAM )
			{

				if ( intCallParam + 1 >= AUTO_RPC_ARRAY_END( call.intParams ) )
					return 0;

				// Check this has some sort of null termination. NB this assumes string is genuine Ascii
				// or UTF+8; using unicode (ie. UTF+16, UCS) could break this.
				if ( data[ plen - 1 ] != 0 )
					return 0;

				// The string doesn't need to be aligned, so we leave it in place; saving a copy, and
				// preventing clogging up of our buffers with data.

#pragma warning(disable:4311) // pointer truncation
				*( intCallParam++ ) = reinterpret_cast<NaturalWord>( data );

				data += plen;

				continue;
			}

#ifndef __BITSTREAM_NATIVE_END
			if (flags & DO_ENDIAN_SWAP)
			{
				RakNet::BitStream::ReverseBytesInPlace( (unsigned char*)data , plen );
			}
#endif

			// convert pointer to ref.
			if (  ( flags & PARAM_TYPE_MASK ) == REF_PARAM
#if AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD
				|| plen > AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD
#endif
				)
			{
				char *nextRefParam = refParam + AUTO_RPC__ALIGN_P2( plen, AUTO_RPC_REF_ALIGN );

				if ( nextRefParam >= AUTO_RPC_ARRAY_END( call.refParams ) || intCallParam + 1 >= AUTO_RPC_ARRAY_END( call.intParams ) )
					return 0;

				memcpy( refParam, data, plen );

#pragma warning(disable:4311) // pointer truncation
				*( intCallParam++ ) = reinterpret_cast<NaturalWord>( refParam );

				refParam = nextRefParam;

				data += plen;

				continue;
			}

			// Guarantee we have space on the output stack to accommodate the parameter.
			NaturalWord *nextParam = (NaturalWord*)( (char*)intCallParam + AUTO_RPC_ALIGN_P2( plen, NaturalWord ) );
			if ( nextParam >= AUTO_RPC_ARRAY_END( call.intParams ) )
				return 0;

#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
			if ( ( flags & PARAM_TYPE_MASK ) == REAL_PARAM
				// long doubles, of various sizes (10, 16), all get passed on the stack
				&& (size_t) plen <= sizeof(double)
				// once we've allocated all our floats, they get treated as ordinary int params
				&& realCallParam < AUTO_RPC_ARRAY_END( call.realParams )
				)
			{

				if ( plen != sizeof( float ) && plen != sizeof( double ) ) {
					printf("illegal float size %d\n", plen );
					// We can't handle it - it's not a real real :lol:
					return 0;
				}

#ifdef __BIG_ENDIAN__
				memcpy( (char*)( realCallParam + 1 ) - plen, data, plen );
#else
				memcpy( (char*)realCallParam, data, plen );
#endif

#if !AUTO_RPC_INT_SHADOW_OF_FLOATS
				// we didn't use the int slot, so don't allow an advance.
				nextParam = intCallParam;
#endif

				// next time, we use the next Real slot
				realCallParam++;
			}
#if !AUTO_RPC_INT_SHADOW_OF_FLOATS
			else
#endif
#endif  // AUTO_RPC_ALLOC_SEPARATE_FLOATS
			{
				// the processor can atomically zero-extend small types, so even with the test,
				// it should be faster than memcpy+memset.
				if ( plen == 1 )
					*intCallParam = *(uint8_t*)data;    // should resolve to movzx and a move
				else if ( plen == 2 )
					*intCallParam = *(uint16_t*)data;  // if network order replace use htons(), and skip EndianSwap()
				else if ( plen == 4 )
					*intCallParam = *(uint32_t*)data;  // if network order replace use htonl(), and skip EndianSwap()
#if AUTO_RPC_AUTORPC_WORD == 64
				else if ( plen == 8 )
					*intCallParam = *(uint64_t*)data;
#endif

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
				//
				// SYSV ABI: aggregates greater 16 bytes must go on the stack;
				// in practice, that means they can't come below AUTO_RPC_INT_REG_PARAMS when we call a function.
				//
				else if ( plen > 16 || ( plen > 8 && intCallParam == &call.intParams[ AUTO_RPC_INT_REG_PARAMS - 1] ) || ( flags & REAL_PARAM ) )
				{
					if ( intCallParam < memParam )
					{
						NaturalWord*const nextMemParam = (NaturalWord*)( (char*)memParam + AUTO_RPC_ALIGN_P2( plen, NaturalWord ) );

						if ( nextMemParam >= AUTO_RPC_ARRAY_END( call.intParams ) )
							return 0;

						memcpy( memParam, data, plen );

						// prevent advancing the ptr slot, since we didn't use it.
						nextParam = intCallParam;

						// but advance the memparam
						memParam = nextMemParam;
					}
					else
					{
						memcpy( (void*)intCallParam, data, plen );
					}
				}
#endif // AUTO_RPC_ABI_SYSV_AMD64
				else
				{
					// We don't need to worry about alignment, because any type that's not a whole multiple
					// of the natual word size will be an aggregate and that should be at the base of memory -
					// this is true for some PowerPC systems  (see [e]) but not all.  But hey, you
					// probably should be passing such structs by reference.
					//
					// Zeroing is also unecessary as code shouldn't be reading beyodn the bounds of the structure.
					//
					memcpy( (void*)intCallParam, data, plen );
				}

			}

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
			// skip over any stored "class MEMORY" (see [b]) parameters.
			if ( nextParam == &call.intParams[AUTO_RPC_INT_REG_PARAMS] )
				intCallParam = memParam;
			else
#endif
				// advance to next output param
				intCallParam = nextParam;

#if !AUTO_RPC_ALLOC_SEPARATE_FLOATS && AUTO_RPC_CREATE_FLOAT_MAP
			if ( ( flags & PARAM_TYPE_MASK ) == REAL_PARAM && i < AUTO_RPC_FLOAT_REG_PARAMS && ( plen == sizeof( double ) || plen == sizeof( float ) ) )			
			{
				call.numRealParams++;
				call.realMap |= ( 1 << i );
			}
#endif

			// advance to next input arg.
			data += plen;
		}

		// space for lastParam?
		if ( &intCallParam[1] >= AUTO_RPC_ARRAY_END( call.intParams ) )
			return 0;

#pragma warning(disable:4311) // pointer truncation
		*( intCallParam++ ) = reinterpret_cast<NaturalWord >( lastParam );

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
		// figure out how many args we have notched up.
		if ( memParam > &call.intParams[AUTO_RPC_INT_REG_PARAMS] && memParam > intCallParam )
			intCallParam = memParam;
#endif

		// convert from ptrdif_t to unsigned int; should be small enough, even if its 64-bit pointers.
		call.numIntParams = ( unsigned int )( intCallParam - call.intParams );

#if AUTO_RPC_FLOAT_REG_PARAMS && AUTO_RPC_ALLOC_SEPARATE_FLOATS
		call.numRealParams = ( unsigned int )( realCallParam - call.realParams );
#endif

		return 1;
#else // AUTO_RPC_ABI
		return 0;
#endif

}


//
// @params
//   callParams: [IN] parameter list
//  functionPtr: [IN] function to call.
//
// @returns:
//         true: function was called.
//        false: too many parameters, probably.
//
bool GenRPC::CallWithStack( CallParams& call, void *functionPtr ) {
#if AUTO_RPC_ABI
	// Are we x86-32?
#if !defined( AUTO_RPC_NO_ASM ) && ( defined(__i386__) || defined( _M_IX86 ) || defined( __INTEL__ ) )
#if !defined(__GNUC__)
	// Intel dialect assembly
	NaturalWord const paramc = call.numIntParams;
#pragma warning(disable:4311) // pointer truncation
	NaturalWord const paramv = reinterpret_cast<NaturalWord>( call.intParams );
	_asm
	{

			// Load numbytes.
			mov         ecx, paramc

			// allocate space on the stack for all these params
			lea         edi,dword ptr[ecx * 4]
			sub         esp,edi

			// setup source of copy
			mov         esi, paramv

			// Setup the destination of the copy: the return stack.
			mov         edi,esp

			// copy data
			rep movs    dword ptr es:[edi],dword ptr [esi]

			// call the function
			call        functionPtr

			// Restore the stack to its state, prior to our invocation.
			//
			// Detail: edi is one of the registers that must be preserved
			// across function calls. (The compiler should be saving it for us.)
			//
			// We left edi pointing to the end of the block copied; i.e. the state
			// of the stack prior to copying our params.  So by loading it
			// into the esp we can restore the return stack to the state prior
			// to the copy.
			//
			mov         esp,edi
	};
#else
	// GCC has its own form of asm block - so we'll always have to write two versions.
	// Therefore, as we're writing it twice, we use the ATT dialect, because only later
	// gcc support Intel dialect.  This one also aligns the stack to a multiple of 16 bytes; which
	// windows doesn't seem to care about.
	// Be aware, we can't use offset of to get the address, as gcc insists on sticking.
	// NaturalWord const paramv = reinterpret_cast<NaturalWord>( call.intParams );
	asm (\
		"lea    4(%%ecx),%%esi\n\
		mov    (%%ecx),%%ecx\n\
		lea    (,%%ecx,4),%%edi\n\
		sub    %%edi,%%esp\n\
		mov    $12,%%edx\n\
		and    %%esp,%%edx\n\
		sub    %%edx,%%esp\n\
		mov    %%esp,%%edi\n\
		rep movsl %%ds:(%%esi),%%es:(%%edi)\n\
		add    %%edx,%%edi\n\
		call   *%1\n\
		mov    %%edi,%%esp"\
		: /* no outputs */\
		: "c" ( &call ), "m" (functionPtr)\
		: "%edi" , "%esi", "%edx", "%eax"\
		);
#endif  // GNUC vs non GNUC
	return 1;
#elif !defined( AUTO_RPC_NO_ASM ) && ( defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_AMD64 ) )
#if AUTO_RPC_ABI == AUTO_RPC_ABI_WIN_AMD64
	NaturalWord const paramv = reinterpret_cast<NaturalWord>( call.intParams );
	_asm {
		// rcx := number of qwords to copy
		mov         rcx, paramc

			// r9 := 0
			sub         r9,r9

			// rsi => our parameter list.
			mov         rsi, paramv

			// r9 := -(number of qwords to copy)
			sub         r9,rcx

			// Preparation to align the stack to 16 byte boundary
			mov         rdx,8

			// rdi => projected bottom of stack
			lea         rdi,dword ptr[rsp + r9 * 8]

		// figure out if stack needs aligning
		and         rdx,rdi

			// save stack into rbx
			mov         rbx,rsp

			// align stack
			sub         rdi,rdx
			mov         rsp,rdi

			// rdx => our parameter list
			mov         rdx,rsi

			//
			// copy data - we copy all parameters, because we have to
			// create a shadow area; and this way its easiest.
			//
			rep movs     qword ptr es:[edi],qword ptr [esi]

		// load registers
		// rcx|xmm0, rdx|xmm1,r8|xmm2,r9|xmm3
		mov          rcx,qword ptr [rdx]
		mov          r8,qword ptr 16[rdx]
		movq         xmm0,rcx
			mov          r9,qword ptr 24[rdx]
		movq         xmm2,r8
			mov          rdx,qword ptr 8[rdx]
		movq         xmm3,r9
			movq         xmm1,rdx

			// call the function
			call        functionPtr

			// Restore the stack to its state, prior to our invocation -
			// saved in rbx.
			mov         rsp,rbx
	}
#elif AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
	//
	// GCC won't generate a stack frame on higher optimization levels, so we don't touch it.
	// on -O3 it inlines the code, breaking it because of the jump reference.
	//
	// I figure a 64-bit compiler will be recent enough to do Intel syntax. May need to change
	// my mind on that. NB. Structure members are hard coded into this.
	//
	asm (\
		".intel_syntax noprefix\n\
		push        rbx\n\
		mov         rax,rsi\n\
		push        r15\n\
		mov         ecx,dword ptr[rdi+8+8*8]\n\
		lea         rsi,[rdi+8+8*8+8]\n\
		mov         r15,rsp\n\
		lea         rbx,[rdi+8]\n\
		sub         r8,r8\n\
		sub         rcx,6\n\
		lea         r9,[rsi + 6 * 8]\n\
		jbe         .L1\n\
		sub         r8,rcx\n\
		mov         rdx,8\n\
		lea         rdi,qword ptr[rsp + r8 * 8]\n\
		and         rdx,rdi\n\
		mov         rsi,r9\n\
		sub         rdi,rdx\n\
		mov         rsp,rdi\n\
		rep movsq   \n\
		.L1:\n\
		movq         xmm0,[rbx]\n\
		movq         xmm1,[rbx+8]\n\
		movq         xmm2,[rbx+16]\n\
		movq         xmm3,[rbx+24]\n\
		movq         xmm4,[rbx+32]\n\
		movq         xmm5,[rbx+40]\n\
		movq         xmm6,[rbx+48]\n\
		movq         xmm7,[rbx+56]\n\
		mov          rdi,[r9-48]\n\
		mov          rsi,[r9-40]\n\
		mov          rdx,[r9-32]\n\
		mov          rcx,[r9-24]\n\
		mov          r8,[r9-16]\n\
		mov          r9,[r9-8]\n\
		call         rax\n\
		mov          rsp,r15\n\
		pop          r15\n\
		pop          rbx\n\
		.att_syntax prefix"\
		: /* no outputs */\
		: "D" ( &call ), "S" (functionPtr)\
		: "%rdx", "%rcx" , "%r8", "%r9", "%rax",\
		"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7" );
	// : "D", ( &call ), "c" ( &call.numIntParams ), "S" ( paramv ), "b" ( floatv ), "a" (functionPtr)
#else
#error unsupport ABI
#endif
	return 1;
#else
	// AUTO_RPC_NO_ASM or no x86-32/x86-64
	//
	//  4.  Passing the parameters.
	//
	//  The compiler knows how to call functions, so having sorted out the argument list,
	//  we just pass it to a function of the correct form - and let the compiler align stacks,
	//  load registers, place parameters where they should be.
	//
	//  This is particularly important as GCC has control over the stack frame, and it can
	//  improperly frame it - for instance utilising red zones to save args, rather than pushing them.
	//  On PowerPC it must create the parameter passing area, too.
	//
	//  The most brute force way, is to code a function call for every possible number of parameters
	//
	//  switch( paramc ) {
	//        case 1:   ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord)) myfunc)( callParam[0] ); break;
	//        case 2:   ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord)) myfunc)( callParam[0], callParam[1] ); break;
	//        ...
	//        case 64:  ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord)) myfunc)( callParam[0], callParam[1], ... , callParam[63] ); break;
	//  }
	//
	//  This is the only way to code WIN32 stdcall, for example, as the args must match exactly;
	//  and so the only way to call from C if you need to call WINAPI routines.
	//
	//  2) Fortunately most calling conventions allowing excessive args. So this means we could
	//  write something like below:
	//
	//     ( (void(*)(...)) myfunc )( args[0],...,args[63] );
	//
	//  And although this should work, its a huge performance penalty copying between memory
	//  locations for so many args.
	//
	//  So we compromise - and do a stepped sequence. Noticing that the WIN64 ABI alwys requires
	//  space for three args anyway.
	//
	//  And on SysV x64 systems, the first 6 args are passed in reg; so again, these are only
	//  copies into register, not memory copies.  And finally that if we load word[n], word[n+1]
	//  is loaded into the cache - thus the overhead for loading is not as big as it might be.
	//
	//  For most realistic cases, a dozen args would be excessive.  Obviously, if you have
	//  a tested assembler equivalent, its probably better to use that.
	//
	//
#if AUTO_RPC_FLOAT_REG_PARAMS
	if ( call.numRealParams == 0 )
#endif
	{
		if ( call.numIntParams <= 3 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_3)) functionPtr )( AUTO_RPC_INT_ARGS_3(  call ) );
			return 1;
		}
		if ( call.numIntParams <= 6 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_6)) functionPtr )( AUTO_RPC_INT_ARGS_6( call ) );
			return 1;
		}
		if ( call.numIntParams <= 9 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_9))functionPtr)( AUTO_RPC_INT_ARGS_9( call ) );
			return 1;
		}
		if ( call.numIntParams <= 12 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_12))functionPtr)( AUTO_RPC_INT_ARGS_12( call ) );
			return 1;
		}
		if ( call.numIntParams <= 32 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_32))functionPtr)( AUTO_RPC_INT_ARGS_32( call ) );
			return 1;
		}
		if ( call.numIntParams <= 64 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_64))functionPtr)( AUTO_RPC_INT_ARGS_64( call ) );
			return 1;
		}
	}
#if AUTO_RPC_FLOAT_REG_PARAMS && !AUTO_RPC_ALLOC_SEPARATE_FLOATS
	else
	{
		if ( call.numIntParams > 64 ) return 0;

		switch( call.realMap )
		{
			// case 0: - no floats, never happens here.

		case 1: ( (void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,NaturalWord,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
					call.realParams[0], call.intParams[1], call.intParams[2], call.intParams[3],
					AUTO_RPC_INT_ARGS_4_64( call )
					);
			break;

		case 2:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,NaturalWord,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.intParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 3:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,NaturalWord,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.intParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 4: ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
					call.intParams[0], call.intParams[1], call.realParams[2], call.intParams[3],
					AUTO_RPC_INT_ARGS_4_64( call )
					);
			break;

		case 5:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.intParams[1], call.realParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 6:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.realParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 7:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.realParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 8: ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
					call.intParams[0], call.intParams[1], call.intParams[2], call.realParams[3],
					AUTO_RPC_INT_ARGS_4_64( call )
					);
			break;

		case 9:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.intParams[1], call.intParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;
		case 10:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.intParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;


		case 11:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.intParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 12: ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
					 call.intParams[0], call.intParams[1], call.realParams[2], call.realParams[3],
					 AUTO_RPC_INT_ARGS_4_64( call )
					 );
			break;

		case 13:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.intParams[1], call.realParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 14:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.realParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 15:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.realParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		default: return 0;
		}
	}
#elif AUTO_RPC_FLOAT_REG_PARAMS
	else
	{
		// we pass FLOAT args last for powerpc compatibility. And although it means we pass them twice,
		// they should end up in the correct floating point register, with the rest of the integers in the
		// correct place...
		//
		// NB if you want to write inline asm for powerpc, you'll have to be put it in a separate
		// "naked" function to that uou can setup the parameter passing area and ensure its big enough.
		// (GCC will delete functions that are unused - it will delete the body of functions that
		// aren't called.)
		//
		if ( call.numIntParams <= 3 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_3,AUTO_RPC_FLOAT_REG_TYPE))functionPtr)( AUTO_RPC_INT_ARGS_3( call ), AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 6 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_6,AUTO_RPC_FLOAT_REG_TYPE))functionPtr)( AUTO_RPC_INT_ARGS_6( call ),AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 9 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_9,AUTO_RPC_FLOAT_REG_TYPE))functionPtr)( AUTO_RPC_INT_ARGS_9( call ),AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 12 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_12,AUTO_RPC_FLOAT_REG_TYPE)) functionPtr )( AUTO_RPC_INT_ARGS_12( call ), AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 64 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_64,AUTO_RPC_FLOAT_REG_TYPE)) functionPtr )( AUTO_RPC_INT_ARGS_64( call ), AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
	}
#endif   // AUTO_RPC_FLOAT_REG_PARAMS
	return 0;
#endif   // AUTO_RPC_NO_ASM
#else  // AUTO_RPC_ABI
	return 0;
#endif
}
// --8<---8<----8<----8<---END
