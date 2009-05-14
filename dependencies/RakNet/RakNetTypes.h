/// \file
/// \brief Types used by RakNet, most of which involve user code.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __NETWORK_TYPES_H
#define __NETWORK_TYPES_H

#include "RakNetDefines.h"
#include "NativeTypes.h"
#include "RakNetTime.h"
#include "Export.h"
#if !defined(_WIN32) && ((defined(__GNUC__)  || defined(__GCCXML__)))
#include "stdint.h"
#endif

/// Forward declaration
namespace RakNet
{
	class BitStream;
};

/// Given a number of bits, return how many bytes are needed to represent that.
#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

/// \sa NetworkIDObject.h
typedef unsigned char UniqueIDType;
typedef unsigned short SystemIndex;
typedef unsigned char RPCIndex;
const int MAX_RPC_MAP_SIZE=((RPCIndex)-1)-1;
const int UNDEFINED_RPC_INDEX=((RPCIndex)-1);

/// First byte of a network message
typedef unsigned char MessageID;

typedef uint32_t BitSize_t;

#if defined(_MSC_VER) && _MSC_VER > 0
#define PRINTF_TIME_MODIFIER "I64"
#else
#define PRINTF_TIME_MODIFIER "ll"
#endif

/// Describes the local socket to use for RakPeer::Startup
struct RAK_DLL_EXPORT SocketDescriptor
{
	SocketDescriptor();
	SocketDescriptor(unsigned short _port, const char *_hostAddress);

	/// The local port to bind to.  Pass 0 to have the OS autoassign a port.
	unsigned short port;

	/// The local network card address to bind to, such as "127.0.0.1".  Pass an empty string to use INADDR_ANY.
	char hostAddress[32];

	enum SocketType
	{
		UDP,
		PS3_LOBBY_UDP,
		NONE // Socket is not created, must be set with UseUserSocket to do anything
	};

	// Type of the socket. Defaults to UDP.
	SocketType socketType;
};
/// \brief Network address for a system
/// Corresponds to a network address
/// This is not necessarily a unique identifier. For example, if a system has both LAN and internet connections, the system may be identified by either one, depending on who is communicating
/// Use RakNetGUID for a unique per-instance of RakPeer to identify systems
struct RAK_DLL_EXPORT SystemAddress
{
	SystemAddress();
	explicit SystemAddress(const char *a, unsigned short b);
	explicit SystemAddress(unsigned int a, unsigned short b);

	///The peer address from inet_addr.
	unsigned int binaryAddress;
	///The port number
	unsigned short port;

	// Return the systemAddress as a string in the format <IP>:<Port>
	// Returns a static string
	// NOT THREADSAFE
	const char *ToString(bool writePort=true) const;

	// Return the systemAddress as a string in the format <IP>:<Port>
	// dest must be large enough to hold the output
	// THREADSAFE
	void ToString(bool writePort, char *dest) const;

	// Sets the binary address part from a string.  Doesn't set the port
	void SetBinaryAddress(const char *str);

	SystemAddress& operator = ( const SystemAddress& input )
	{
		binaryAddress = input.binaryAddress;
		port = input.port;
		return *this;
	}

	bool operator==( const SystemAddress& right ) const;
	bool operator!=( const SystemAddress& right ) const;
	bool operator > ( const SystemAddress& right ) const;
	bool operator < ( const SystemAddress& right ) const;
};


/// Size of SystemAddress data
#define SystemAddress_Size 6

class RakPeerInterface;

/// All RPC functions have the same parameter list - this structure.
/// \depreciated Use the AutoRPC or RPC3 plugin instead
struct RPCParameters
{
	/// The data from the remote system
	unsigned char *input;

	/// How many bits long \a input is
	BitSize_t numberOfBitsOfData;

	/// Which system called this RPC
	SystemAddress sender;

	/// Which instance of RakPeer (or a derived RakPeer or RakPeer) got this call
	RakPeerInterface *recipient;

	RakNetTime remoteTimestamp;

	/// The name of the function that was called.
	char *functionName;

	/// You can return values from RPC calls by writing them to this BitStream.
	/// This is only sent back if the RPC call originally passed a BitStream to receive the reply.
	/// If you do so and your send is reliable, it will block until you get a reply or you get disconnected from the system you are sending to, whichever is first.
	/// If your send is not reliable, it will block for triple the ping time, or until you are disconnected, or you get a reply, whichever is first.
	RakNet::BitStream *replyToSender;
};

/// Uniquely identifies an instance of RakPeer. Use RakPeer::GetGuidFromSystemAddress() and RakPeer::GetSystemAddressFromGuid() to go between SystemAddress and RakNetGUID
/// Use RakPeer::GetGuidFromSystemAddress(UNASSIGNED_SYSTSEM_ADDRESS) to get your own GUID
struct RAK_DLL_EXPORT RakNetGUID
{
	uint32_t g[4];

	// Return the GUID as a string
	// Returns a static string
	// NOT THREADSAFE
	const char *ToString(void) const;

	// Return the GUID as a string
	// dest must be large enough to hold the output
	// THREADSAFE
	void ToString(char *dest) const;

	bool FromString(const char *source);

	RakNetGUID& operator = ( const RakNetGUID& input )
	{
		g[0]=input.g[0];
		g[1]=input.g[1];
		g[2]=input.g[2];
		g[3]=input.g[3];
		return *this;
	}

	bool operator==( const RakNetGUID& right ) const;
	bool operator!=( const RakNetGUID& right ) const;
	bool operator > ( const RakNetGUID& right ) const;
	bool operator < ( const RakNetGUID& right ) const;
};

/// Index of an invalid SystemAddress
//const SystemAddress UNASSIGNED_SYSTEM_ADDRESS =
//{
//	0xFFFFFFFF, 0xFFFF
//};
const SystemAddress UNASSIGNED_SYSTEM_ADDRESS(0xFFFFFFFF, 0xFFFF);
const RakNetGUID UNASSIGNED_RAKNET_GUID = 
{
	{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}
};

struct RAK_DLL_EXPORT NetworkID
{
	NetworkID()
	{
#if defined NETWORK_ID_SUPPORTS_PEER_TO_PEER
		guid = UNASSIGNED_RAKNET_GUID;
		systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
#endif
		localSystemAddress=65535;
	}
	~NetworkID() {}

	/// \depreciated. Use NETWORK_ID_SUPPORTS_PEER_TO_PEER in RakNetDefines.h
	// Set this to true to use peer to peer mode for NetworkIDs.
	// Obviously the value of this must match on all systems.
	// True, and this will write the systemAddress portion with network sends.  Takes more bandwidth, but NetworkIDs can be locally generated
	// False, and only localSystemAddress is used.
//	static bool peerToPeerMode;

#if defined NETWORK_ID_SUPPORTS_PEER_TO_PEER
	// Depreciated: Use guid instead
	// In peer to peer, we use both systemAddress and localSystemAddress
	// In client / server, we only use localSystemAddress
	SystemAddress systemAddress;

	RakNetGUID guid;
#endif
	unsigned short localSystemAddress;

	NetworkID& operator = ( const NetworkID& input );

	static bool IsPeerToPeerMode(void);
	static void SetPeerToPeerMode(bool isPeerToPeer);
	bool operator==( const NetworkID& right ) const;
	bool operator!=( const NetworkID& right ) const;
	bool operator > ( const NetworkID& right ) const;
	bool operator < ( const NetworkID& right ) const;
};

/// This represents a user message from another system.
struct Packet
{
	/// Server only - this is the index into the player array that this systemAddress maps to
	SystemIndex systemIndex;

	/// The system that send this packet.
	SystemAddress systemAddress;

	/// A unique identifier for the system that sent this packet, regardless of IP address (internal / external / remote system)
	/// Only valid once a connection has been established (ID_CONNECTION_REQUEST_ACCEPTED, or ID_NEW_INCOMING_CONNECTION)
	/// Until that time, will be UNASSIGNED_RAKNET_GUID
	RakNetGUID guid;

	/// The length of the data in bytes
	/// \deprecated You should use bitSize.
	unsigned int length;

	/// The length of the data in bits
	BitSize_t bitSize;

	/// The data from the sender
	unsigned char* data;

	/// @internal
	/// Indicates whether to delete the data, or to simply delete the packet.
	bool deleteData;
};

///  Index of an unassigned player
const SystemIndex UNASSIGNED_PLAYER_INDEX = 65535;

/// Unassigned object ID
const NetworkID UNASSIGNED_NETWORK_ID;

const int PING_TIMES_ARRAY_SIZE = 5;

/// \brief RPC Function Implementation
///
/// The Remote Procedure Call Subsystem provide the RPC paradigm to
/// RakNet user. It consists in providing remote function call over the
/// network.  A call to a remote function require you to prepare the
/// data for each parameter (using BitStream) for example.
///
/// Use the following C function prototype for your callbacks
/// @code
/// void functionName(RPCParameters *rpcParms);
/// @endcode
/// If you pass input data, you can parse the input data in two ways.
/// 1.
/// Cast input to a struct (such as if you sent a struct)
/// i.e. MyStruct *s = (MyStruct*) input;
/// Make sure that the sizeof(MyStruct) is equal to the number of bytes passed!
/// 2.
/// Create a BitStream instance with input as data and the number of bytes
/// i.e. BitStream myBitStream(input, (numberOfBitsOfData-1)/8+1)
/// (numberOfBitsOfData-1)/8+1 is how convert from bits to bytes
/// Full example:
/// @code
/// void MyFunc(RPCParameters *rpcParms) {}
/// RakPeer *rakClient;
/// REGISTER_AS_REMOTE_PROCEDURE_CALL(rakClient, MyFunc);
/// This would allow MyFunc to be called from the server using  (for example)
/// rakServer->RPC("MyFunc", 0, clientID, false);
/// @endcode


/// \def REGISTER_STATIC_RPC
/// \ingroup RAKNET_RPC
/// \depreciated Use the AutoRPC plugin instead
/// Register a C function as a Remote procedure.
/// \param[in] networkObject Your instance of RakPeer, RakPeer, or RakPeer
/// \param[in] functionName The name of the C function to call
/// \attention 12/01/05 REGISTER_AS_REMOTE_PROCEDURE_CALL renamed to REGISTER_STATIC_RPC.  Delete the old name sometime in the future
//#pragma deprecated(REGISTER_AS_REMOTE_PROCEDURE_CALL)
//#define REGISTER_AS_REMOTE_PROCEDURE_CALL(networkObject, functionName) REGISTER_STATIC_RPC(networkObject, functionName)
#define REGISTER_STATIC_RPC(networkObject, functionName) (networkObject)->RegisterAsRemoteProcedureCall((#functionName),(functionName))

/// \def CLASS_MEMBER_ID
/// \ingroup RAKNET_RPC
/// \depreciated Use the AutoRPC plugin instead
/// \brief Concatenate two strings

/// \def REGISTER_CLASS_MEMBER_RPC
/// \depreciated Use the AutoRPC plugin instead
/// \ingroup RAKNET_RPC
/// \brief Register a member function of an instantiated object as a Remote procedure call.
/// RPC member Functions MUST be marked __cdecl!
/// \sa ObjectMemberRPC.cpp
/// \b CLASS_MEMBER_ID is a utility macro to generate a unique signature for a class and function pair and can be used for the Raknet functions RegisterClassMemberRPC(...) and RPC(...)
/// \b REGISTER_CLASS_MEMBER_RPC is a utility macro to more easily call RegisterClassMemberRPC
/// \param[in] networkObject Your instance of RakPeer, RakPeer, or RakPeer
/// \param[in] className The class containing the function
/// \param[in] functionName The name of the function (not in quotes, just the name)
#define CLASS_MEMBER_ID(className, functionName) #className "_" #functionName
#define REGISTER_CLASS_MEMBER_RPC(networkObject, className, functionName) {union {void (__cdecl className::*cFunc)( RPCParameters *rpcParms ); void* voidFunc;}; cFunc=&className::functionName; networkObject->RegisterClassMemberRPC(CLASS_MEMBER_ID(className, functionName),voidFunc);}

/// \def UNREGISTER_AS_REMOTE_PROCEDURE_CALL
/// \brief Only calls UNREGISTER_STATIC_RPC
/// \depreciated Use the AutoRPC plugin instead

/// \def UNREGISTER_STATIC_RPC
/// \ingroup RAKNET_RPC
/// \depreciated Use the AutoRPC plugin instead
/// Unregisters a remote procedure call
/// RPC member Functions MUST be marked __cdecl!  See the ObjectMemberRPC example.
/// \param[in] networkObject The object that manages the function
/// \param[in] functionName The function name
// 12/01/05 UNREGISTER_AS_REMOTE_PROCEDURE_CALL Renamed to UNREGISTER_STATIC_RPC.  Delete the old name sometime in the future
//#pragma deprecated(UNREGISTER_AS_REMOTE_PROCEDURE_CALL)
//#define UNREGISTER_AS_REMOTE_PROCEDURE_CALL(networkObject,functionName) UNREGISTER_STATIC_RPC(networkObject,functionName)
#define UNREGISTER_STATIC_RPC(networkObject,functionName) (networkObject)->UnregisterAsRemoteProcedureCall((#functionName))

/// \def UNREGISTER_CLASS_INST_RPC
/// \ingroup RAKNET_RPC
/// \depreciated Use the AutoRPC plugin instead
/// \brief Unregisters a member function of an instantiated object as a Remote procedure call.
/// \param[in] networkObject The object that manages the function
/// \param[in] className The className that was originally passed to REGISTER_AS_REMOTE_PROCEDURE_CALL
/// \param[in] functionName The function name
#define UNREGISTER_CLASS_MEMBER_RPC(networkObject, className, functionName) (networkObject)->UnregisterAsRemoteProcedureCall((#className "_" #functionName))

#endif
