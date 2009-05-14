#ifndef __RAKNET_DEFINES_H
#define __RAKNET_DEFINES_H

/// Define __GET_TIME_64BIT to have RakNetTime use a 64, rather than 32 bit value.  A 32 bit value will overflow after about 5 weeks.
/// However, this doubles the bandwidth use for sending times, so don't do it unless you have a reason to.
#define __GET_TIME_64BIT

/// Makes RakNet threadsafe
/// Define this if you use the same instance of RakPeer from multiple threads
/// Otherwise leave it undefined, since it makes things an order of magnitude slower.
/// Disabled by default
// #define _RAKNET_THREADSAFE

/// Define __BITSTREAM_NATIVE_END to NOT support endian swapping in the BitStream class.  This is faster and is what you should use
/// unless you actually plan to have different endianness systems connect to each other
/// Enabled by default.
#define __BITSTREAM_NATIVE_END

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#undef __BITSTREAM_NATIVE_END
#endif

/// Maximum (stack) size to use with _alloca before using new and delete instead.
#define MAX_ALLOCA_STACK_ALLOCATION 1048576

// Use WaitForSingleObject instead of sleep.
// Defining it plays nicer with other systems, and uses less CPU, but gives worse RakNet performance
// Undefining it uses more CPU time, but is more responsive and faster.
#ifndef _WIN32_WCE
#define USE_WAIT_FOR_MULTIPLE_EVENTS
#endif

/// Uncomment to use RakMemoryOverride for custom memory tracking
/// See RakMemoryOverride.h. 
#define _USE_RAK_MEMORY_OVERRIDE

/// If defined, RakNet will automatically try to determine available bandwidth and buffer accordingly (recommended)
/// If commented out, you will probably not be able to send large files and will get increased packetloss. However, responsiveness for the first 10 seconds or so will be improved.
#define _ENABLE_FLOW_CONTROL

/// If defined, OpenSSL is enabled for the class TCPInterface
/// This is necessary to use the SendEmail class with Google POP servers
/// Note that OpenSSL carries its own license restrictions that you should be aware of. If you don't agree, don't enable this define
/// This also requires that you enable header search paths to DependentExtensions\openssl-0.9.8g
/// #define OPEN_SSL_CLIENT_SUPPORT

/// Threshold at which to do a malloc / free rather than pushing data onto a fixed stack for the bitstream class
/// Arbitrary size, just picking something likely to be larger than most packets
#define BITSTREAM_STACK_ALLOCATION_SIZE 1024

// Redefine if you want to disable or change the target for debug RAKNET_DEBUG_PRINTF
#define RAKNET_DEBUG_PRINTF printf

// 16 * 4 * 8 = 512 bit. Used for InitializeSecurity()
#define RAKNET_RSA_FACTOR_LIMBS 16

// Enable to support peer to peer with NetworkIDs. Disable to save memory if doing client/server only
#define NETWORK_ID_SUPPORTS_PEER_TO_PEER

// O(1) instead of O(log2n) but takes more memory if less than 1/3 of the mappings are used.
// Only supported if NETWORK_ID_SUPPORTS_PEER_TO_PEER is commented out
// #define NETWORK_ID_USE_PTR_TABLE

// Maximum number of local IP addresses supported
#define MAXIMUM_NUMBER_OF_INTERNAL_IDS 10

// #define _DISABLE_RAKNET_ASSERTS
#if defined(_DEBUG) && !defined(_XBOX) && !defined(_DISABLE_RAKNET_ASSERTS)
	#include <assert.h>
	#define RakAssert(x) assert(x);
#else
	#define RakAssert(x) 
#endif

#endif // __RAKNET_DEFINES_H
