/// \file
/// \brief An interface for RakPeer.  Simply contains all user functions as pure virtuals.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __RAK_PEER_INTERFACE_H
#define __RAK_PEER_INTERFACE_H

#include "PacketPriority.h"
#include "RakNetTypes.h"
#include "RakMemoryOverride.h"
#include "Export.h"
#include "DS_List.h"

// Forward declarations
namespace RakNet
{
	class BitStream;
}
class PluginInterface2;
struct RPCMap;
struct RakNetStatistics;
class RouterInterface;
class NetworkIDManager;

/// The primary interface for RakNet, RakPeer contains all major functions for the library.
/// See the individual functions for what the class can do.
/// \brief The main interface for network communications
class RAK_DLL_EXPORT RakPeerInterface
{
public:
	///Destructor
	virtual ~RakPeerInterface()	{}

	// --------------------------------------------------------------------------------------------Major Low Level Functions - Functions needed by most users--------------------------------------------------------------------------------------------
	/// \brief Starts the network threads, opens the listen port.
	/// You must call this before calling Connect().
	/// Multiple calls while already active are ignored.  To call this function again with different settings, you must first call Shutdown().
	/// \note Call SetMaximumIncomingConnections if you want to accept incoming connections
	/// \note Set _RAKNET_THREADSAFE in RakNetDefines.h if you want to call RakNet functions from multiple threads (not recommended, as it is much slower and RakNet is already asynchronous).
	/// \param[in] maxConnections The maximum number of connections between this instance of RakPeer and another instance of RakPeer. Required so the network can preallocate and for thread safety. A pure client would set this to 1.  A pure server would set it to the number of allowed clients.- A hybrid would set it to the sum of both types of connections
	/// \param[in] localPort The port to listen for connections on.
	/// \param[in] _threadSleepTimer How many ms to Sleep each internal update cycle (30 to give the game priority, 0 for regular (recommended)
	/// \param[in] socketDescriptors An array of SocketDescriptor structures to force RakNet to listen on a particular IP address or port (or both).  Each SocketDescriptor will represent one unique socket.  Do not pass redundant structures.  To listen on a specific port, you can pass SocketDescriptor(myPort,0); such as for a server.  For a client, it is usually OK to just pass SocketDescriptor();
	/// \param[in] socketDescriptorCount The size of the \a socketDescriptors array.  Pass 1 if you are not sure what to pass.
	/// \return False on failure (can't create socket or thread), true on success.
	virtual bool Startup( unsigned short maxConnections, int _threadSleepTimer, SocketDescriptor *socketDescriptors, unsigned socketDescriptorCount )=0;

	/// Secures connections though a combination of SHA1, AES128, SYN Cookies, and RSA to prevent connection spoofing, replay attacks, data eavesdropping, packet tampering, and MitM attacks.
	/// There is a significant amount of processing and a slight amount of bandwidth overhead for this feature.
	/// If you accept connections, you must call this or else secure connections will not be enabled for incoming connections.
	/// If you are connecting to another system, you can call this with values for the (e and p,q) public keys before connecting to prevent MitM
	/// \pre Must be called while offline
	/// \param[in] pubKeyE A pointer to the public keys from the RSACrypt class.  
	/// \param[in] pubKeyN A pointer to the public keys from the RSACrypt class. 
	/// \param[in] privKeyP Public key generated from the RSACrypt class.  
	/// \param[in] privKeyQ Public key generated from the RSACrypt class.  If the private keys are 0, then a new key will be generated when this function is called@see the Encryption sample
	virtual void InitializeSecurity(const char *pubKeyE, const char *pubKeyN, const char *privKeyP, const char *privKeyQ )=0;

	/// Disables all security.
	/// \note Must be called while offline
	virtual void DisableSecurity( void )=0;

	/// If secure connections are on, do not use secure connections for a specific IP address.
	/// This is useful if you have a fixed-address internal server behind a LAN.
	/// \note Secure connections are determined by the recipient of an incoming connection. This has no effect if called on the system attempting to connect.
	/// \param[in] ip IP address to add. * wildcards are supported.
	virtual void AddToSecurityExceptionList(const char *ip)=0;

	/// Remove a specific connection previously added via AddToSecurityExceptionList
	/// \param[in] ip IP address to remove. Pass 0 to remove all IP addresses. * wildcards are supported.
	virtual void RemoveFromSecurityExceptionList(const char *ip)=0;

	/// Checks to see if a given IP is in the security exception list
	/// \param[in] IP address to check.
	virtual bool IsInSecurityExceptionList(const char *ip)=0;

	/// Sets how many incoming connections are allowed. If this is less than the number of players currently connected,
	/// no more players will be allowed to connect.  If this is greater than the maximum number of peers allowed,
	/// it will be reduced to the maximum number of peers allowed.
	/// Defaults to 0, meaning by default, nobody can connect to you
	/// \param[in] numberAllowed Maximum number of incoming connections allowed.
	virtual void SetMaximumIncomingConnections( unsigned short numberAllowed )=0;

	/// Returns the value passed to SetMaximumIncomingConnections()
	/// \return the maximum number of incoming connections, which is always <= maxConnections
	virtual unsigned short GetMaximumIncomingConnections( void ) const=0;

	/// Returns how many open connections there are at this time
	/// \return the number of open connections
	virtual unsigned short NumberOfConnections(void) const=0;

	/// Sets the password incoming connections must match in the call to Connect (defaults to none). Pass 0 to passwordData to specify no password
	/// This is a way to set a low level password for all incoming connections.  To selectively reject connections, implement your own scheme using CloseConnection() to remove unwanted connections
	/// \param[in] passwordData A data block that incoming connections must match.  This can be just a password, or can be a stream of data. Specify 0 for no password data
	/// \param[in] passwordDataLength The length in bytes of passwordData
	virtual void SetIncomingPassword( const char* passwordData, int passwordDataLength )=0;

	/// Gets the password passed to SetIncomingPassword
	/// \param[out] passwordData  Should point to a block large enough to hold the password data you passed to SetIncomingPassword()
	/// \param[in,out] passwordDataLength Maximum size of the array passwordData.  Modified to hold the number of bytes actually written
	virtual void GetIncomingPassword( char* passwordData, int *passwordDataLength  )=0;

	/// \brief Connect to the specified host (ip or domain name) and server port.
	/// Calling Connect and not calling SetMaximumIncomingConnections acts as a dedicated client.
	/// Calling both acts as a true peer. This is a non-blocking connection.
	/// You know the connection is successful when IsConnected() returns true or Receive() gets a message with the type identifier ID_CONNECTION_ACCEPTED.
	/// If the connection is not successful, such as a rejected connection or no response then neither of these things will happen.
	/// \pre Requires that you first call Initialize
	/// \param[in] host Either a dotted IP address or a domain name
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] passwordData A data block that must match the data block on the server passed to SetIncomingPassword.  This can be a string or can be a stream of data.  Use 0 for no password.
	/// \param[in] passwordDataLength The length in bytes of passwordData
	/// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
	/// \param[in] sendConnectionAttemptCount How many datagrams to send to the other system to try to connect.
	/// \param[in] timeBetweenSendConnectionAttemptsMS How often to send datagrams to the other system to try to connect. After this many times, ID_CONNECTION_ATTEMPT_FAILED is returned
	/// \return True on successful initiation. False on incorrect parameters, internal error, or too many existing peers.  Returning true does not mean you connected!
	virtual bool Connect( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, unsigned connectionSocketIndex=0, unsigned sendConnectionAttemptCount=7, unsigned timeBetweenSendConnectionAttemptsMS=500 )=0;

	/// \brief Connect to the specified network ID (Platform specific console function)
	/// Does built-in NAt traversal
	/// \param[in] passwordData A data block that must match the data block on the server passed to SetIncomingPassword.  This can be a string or can be a stream of data.  Use 0 for no password.
	/// \param[in] passwordDataLength The length in bytes of passwordData
	//virtual bool Console2LobbyConnect( void *networkServiceId, const char *passwordData, int passwordDataLength )=0;

	/// \brief Stops the network threads and closes all connections.
	/// \param[in] blockDuration How long, in milliseconds, you should wait for all remaining messages to go out, including ID_DISCONNECTION_NOTIFICATION.  If 0, it doesn't wait at all.
	/// \param[in] orderingChannel If blockDuration > 0, ID_DISCONNECTION_NOTIFICATION will be sent on this channel
	/// If you set it to 0 then the disconnection notification won't be sent
	virtual void Shutdown( unsigned int blockDuration, unsigned char orderingChannel=0 )=0;

	/// Returns if the network thread is running
	/// \return true if the network thread is running, false otherwise
	virtual bool IsActive( void ) const=0;

	/// Fills the array remoteSystems with the SystemAddress of all the systems we are connected to
	/// \param[out] remoteSystems An array of SystemAddress structures to be filled with the SystemAddresss of the systems we are connected to. Pass 0 to remoteSystems to only get the number of systems we are connected to
	/// \param[in, out] numberOfSystems As input, the size of remoteSystems array.  As output, the number of elements put into the array 
	virtual bool GetConnectionList( SystemAddress *remoteSystems, unsigned short *numberOfSystems ) const=0;

	/// Sends a block of data to the specified system that you are connected to.
	/// This function only works while the connected
	/// The first byte should be a message identifier starting at ID_USER_PACKET_ENUM
	/// \param[in] data The block of data to send
	/// \param[in] length The size in bytes of the data to send
	/// \param[in] priority What priority level to send on.  See PacketPriority.h
	/// \param[in] reliability How reliability to send this data.  See PacketPriority.h
	/// \param[in] orderingChannel When using ordered or sequenced messages, what channel to order these on. Messages are only ordered relative to other messages on the same stream
	/// \param[in] systemAddress Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
	/// \return False if we are not connected to the specified recipient.  True otherwise
	virtual bool Send( const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast )=0;

	/// "Send" to yourself rather than a remote system. The message will be processed through the plugins and returned to the game as usual
	/// This function works anytime
	/// The first byte should be a message identifier starting at ID_USER_PACKET_ENUM
	/// \param[in] data The block of data to send
	/// \param[in] length The size in bytes of the data to send
	virtual void SendLoopback( const char *data, const int length )=0;

	/// Sends a block of data to the specified system that you are connected to.  Same as the above version, but takes a BitStream as input.
	/// \param[in] bitStream The bitstream to send
	/// \param[in] priority What priority level to send on.  See PacketPriority.h
	/// \param[in] reliability How reliability to send this data.  See PacketPriority.h
	/// \param[in] orderingChannel When using ordered or sequenced messages, what channel to order these on. Messages are only ordered relative to other messages on the same stream
	/// \param[in] systemAddress Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
	/// \return False if we are not connected to the specified recipient.  True otherwise
	/// \note COMMON MISTAKE: When writing the first byte, bitStream->Write((unsigned char) ID_MY_TYPE) be sure it is casted to a byte, and you are not writing a 4 byte enumeration.
	virtual bool Send( const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast )=0;

	/// Sends multiple blocks of data, concatenating them automatically.
	///
	/// This is equivalent to:
	/// RakNet::BitStream bs;
	/// bs.WriteAlignedBytes(block1, blockLength1);
	/// bs.WriteAlignedBytes(block2, blockLength2);
	/// bs.WriteAlignedBytes(block3, blockLength3);
	/// Send(&bs, ...)
	///
	/// This function only works while the connected
	/// \param[in] data An array of pointers to blocks of data
	/// \param[in] lengths An array of integers indicating the length of each block of data
	/// \param[in] numParameters Length of the arrays data and lengths
	/// \param[in] priority What priority level to send on.  See PacketPriority.h
	/// \param[in] reliability How reliability to send this data.  See PacketPriority.h
	/// \param[in] orderingChannel When using ordered or sequenced messages, what channel to order these on. Messages are only ordered relative to other messages on the same stream
	/// \param[in] systemAddress Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
	/// \return False if we are not connected to the specified recipient.  True otherwise
	virtual bool SendList( char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast )=0;

	/// Gets a message from the incoming message queue.
	/// Use DeallocatePacket() to deallocate the message after you are done with it.
	/// User-thread functions, such as RPC calls and the plugin function PluginInterface::Update occur here.
	/// \return 0 if no packets are waiting to be handled, otherwise a pointer to a packet.
	/// \note COMMON MISTAKE: Be sure to call this in a loop, once per game tick, until it returns 0. If you only process one packet per game tick they will buffer up.
	/// sa RakNetTypes.h contains struct Packet
	virtual Packet* Receive( void )=0;

	/// Call this to deallocate a message returned by Receive() when you are done handling it.
	/// \param[in] packet The message to deallocate.	
	virtual void DeallocatePacket( Packet *packet )=0;

	/// Return the total number of connections we are allowed
	// TODO - rename for RakNet 3.0
	virtual unsigned short GetMaximumNumberOfPeers( void ) const=0;

	// --------------------------------------------------------------------------------------------Remote Procedure Call Functions - Functions to initialize and perform RPC--------------------------------------------------------------------------------------------
	/// \ingroup RAKNET_RPC
	/// \depreciated Use the AutoRPC plugin instead
	/// Register a C or static member function as available for calling as a remote procedure call
	/// \param[in] uniqueID A null-terminated unique string to identify this procedure.  See RegisterClassMemberRPC() for class member functions.
	/// \param[in] functionPointer(...) The name of the function to be used as a function pointer. This can be called whether active or not, and registered functions stay registered unless unregistered
	virtual void RegisterAsRemoteProcedureCall( const char* uniqueID, void ( *functionPointer ) ( RPCParameters *rpcParms ) )=0;

	/// \ingroup RAKNET_RPC
	/// \depreciated Use the AutoRPC plugin instead
	/// Register a C++ member function as available for calling as a remote procedure call.
	/// \param[in] uniqueID A null terminated string to identify this procedure. Recommended you use the macro REGISTER_CLASS_MEMBER_RPC to create the string.  Use RegisterAsRemoteProcedureCall() for static functions.
	/// \param[in] functionPointer The name of the function to be used as a function pointer. This can be called whether active or not, and registered functions stay registered unless unregistered with UnregisterAsRemoteProcedureCall
	/// \sa The sample ObjectMemberRPC.cpp
	virtual void RegisterClassMemberRPC( const char* uniqueID, void *functionPointer )=0;

	/// \ingroup RAKNET_RPC
	/// \depreciated Use the AutoRPC plugin instead
	/// Unregisters a C function as available for calling as a remote procedure call that was formerly registered with RegisterAsRemoteProcedureCall. Only call offline.
	/// \param[in] uniqueID A string of only letters to identify this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.
	virtual void UnregisterAsRemoteProcedureCall( const char* uniqueID )=0;

	/// \ingroup RAKNET_RPC
	/// Used by Object member RPC to lookup objects given that object's ID
	/// Also used by the ReplicaManager plugin
	/// \param[in] An instance of NetworkIDManager to use for the lookup.
	virtual void SetNetworkIDManager( NetworkIDManager *manager )=0;

	/// \return Returns the value passed to SetNetworkIDManager or 0 if never called.
	virtual NetworkIDManager *GetNetworkIDManager(void) const=0;

	/// \ingroup RAKNET_RPC
	/// Calls a C function on the remote system that was already registered using RegisterAsRemoteProcedureCall().
	/// \param[in] uniqueID A NULL terminated string identifying the function to call.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.
	/// \param[in] data The data to send
	/// \param[in] bitLength The number of bits of \a data
	/// \param[in] priority What priority level to send on. See PacketPriority.h.
	/// \param[in] reliability How reliability to send this data. See PacketPriority.h.
	/// \param[in] orderingChannel When using ordered or sequenced message, what channel to order these on.
	/// \param[in] systemAddress Who to send this message to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
	/// \param[in] includedTimestamp Pass a timestamp if you wish, to be adjusted in the usual fashion as per ID_TIMESTAMP.  Pass 0 to not include a timestamp.
	/// \param[in] networkID For static functions, pass UNASSIGNED_NETWORK_ID.  For member functions, you must derive from NetworkIDObject and pass the value returned by NetworkIDObject::GetNetworkID for that object.
	/// \param[in] replyFromTarget If 0, this function is non-blocking.  Otherwise it will block while waiting for a reply from the target procedure, which should be remotely written to RPCParameters::replyToSender and copied to replyFromTarget.  The block will return early on disconnect or if the sent packet is unreliable and more than 3X the ping has elapsed.
	/// \return True on a successful packet send (this does not indicate the recipient performed the call), false on failure
	virtual bool RPC( const char* uniqueID, const char *data, BitSize_t bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, RakNetTime *includedTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )=0;
	
	/// \ingroup RAKNET_RPC
	/// Calls a C function on the remote system that was already registered using RegisterAsRemoteProcedureCall.
	/// If you want that function to return data you should call RPC from that system in the same wayReturns true on a successful packet
	/// send (this does not indicate the recipient performed the call), false on failure
	/// \param[in] uniqueID A NULL terminated string identifying the function to call.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.
	/// \param[in] data The data to send
	/// \param[in] bitLength The number of bits of \a data
	/// \param[in] priority What priority level to send on. See PacketPriority.h.
	/// \param[in] reliability How reliability to send this data. See PacketPriority.h.
	/// \param[in] orderingChannel When using ordered or sequenced message, what channel to order these on.
	/// \param[in] systemAddress Who to send this message to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
	/// \param[in] includedTimestamp Pass a timestamp if you wish, to be adjusted in the usual fashion as per ID_TIMESTAMP.  Pass 0 to not include a timestamp.
	/// \param[in] networkID For static functions, pass UNASSIGNED_NETWORK_ID.  For member functions, you must derive from NetworkIDObject and pass the value returned by NetworkIDObject::GetNetworkID for that object.
	/// \param[in] replyFromTarget If 0, this function is non-blocking.  Otherwise it will block while waiting for a reply from the target procedure, which should be remotely written to RPCParameters::replyToSender and copied to replyFromTarget.  The block will return early on disconnect or if the sent packet is unreliable and more than 3X the ping has elapsed.
	/// \return True on a successful packet send (this does not indicate the recipient performed the call), false on failure
	virtual bool RPC( const char* uniqueID, const RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, RakNetTime *includedTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )=0;
	
	// -------------------------------------------------------------------------------------------- Connection Management Functions--------------------------------------------------------------------------------------------
	/// Close the connection to another host (if we initiated the connection it will disconnect, if they did it will kick them out).
	/// \param[in] target Which system to close the connection to.
	/// \param[in] sendDisconnectionNotification True to send ID_DISCONNECTION_NOTIFICATION to the recipient.  False to close it silently.
	/// \param[in] channel Which ordering channel to send the disconnection notification on, if any
	virtual void CloseConnection( const SystemAddress target, bool sendDisconnectionNotification, unsigned char orderingChannel=0 )=0;

	/// Cancel a pending connection attempt
	/// If we are already connected, the connection stays open
	/// \param[in] target Which system to cancel
	virtual void CancelConnectionAttempt( const SystemAddress target )=0;

	/// Returns if a particular systemAddress is connected to us (this also returns true if we are in the process of connecting)
	/// \param[in] systemAddress The SystemAddress we are referring to
	/// \param[in] includeInProgress If true, also return true for connections that are in progress but haven't completed
	/// \param[in] includeDisconnecting If true, also return true for connections that are in the process of disconnecting
	/// \return True if this system is connected and active, false otherwise.
	virtual bool IsConnected(const SystemAddress systemAddress, bool includeInProgress=false, bool includeDisconnecting=false)=0;

	/// Given a systemAddress, returns an index from 0 to the maximum number of players allowed - 1.
	/// \param[in] systemAddress The SystemAddress we are referring to
	/// \return The index of this SystemAddress or -1 on system not found.
	virtual int GetIndexFromSystemAddress( const SystemAddress systemAddress )=0;

	/// This function is only useful for looping through all systems
	/// Given an index, will return a SystemAddress.
	/// \param[in] index Index should range between 0 and the maximum number of players allowed - 1.
	/// \return The SystemAddress
	virtual SystemAddress GetSystemAddressFromIndex( int index )=0;

	/// Same as GetSystemAddressFromIndex but returns RakNetGUID
	/// \param[in] index Index should range between 0 and the maximum number of players allowed - 1.
	/// \return The RakNetGUID
	virtual RakNetGUID GetGUIDFromIndex( int index )=0;

	/// Same as calling GetSystemAddressFromIndex and GetGUIDFromIndex for all systems, but more efficient
	/// Indices match each other, so \a addresses[0] and \a guids[0] refer to the same system
	/// \param[out] addresses All system addresses. Size of the list is the number of connections. Size of the list will match the size of the \a guids list.
	/// \param[out] guids All guids. Size of the list is the number of connections. Size of the list will match the size of the \a addresses list.
	virtual void GetSystemList(DataStructures::List<SystemAddress> &addresses, DataStructures::List<RakNetGUID> &guids)=0;

	/// Bans an IP from connecting.  Banned IPs persist between connections but are not saved on shutdown nor loaded on startup.
	/// param[in] IP Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will ban all IP addresses starting with 128.0.0
	/// \param[in] milliseconds how many ms for a temporary ban.  Use 0 for a permanent ban
	virtual void AddToBanList( const char *IP, RakNetTime milliseconds=0 )=0;

	/// Allows a previously banned IP to connect. 
	/// param[in] Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will banAll IP addresses starting with 128.0.0
	virtual void RemoveFromBanList( const char *IP )=0;

	/// Allows all previously banned IPs to connect.
	virtual void ClearBanList( void )=0;

	/// Returns true or false indicating if a particular IP is banned.
	/// \param[in] IP - Dotted IP address.
	/// \return true if IP matches any IPs in the ban list, accounting for any wildcards. False otherwise.
	virtual bool IsBanned( const char *IP )=0;

	// --------------------------------------------------------------------------------------------Pinging Functions - Functions dealing with the automatic ping mechanism--------------------------------------------------------------------------------------------
	/// Send a ping to the specified connected system.
	/// \pre The sender and recipient must already be started via a successful call to Startup()
	/// \param[in] target Which system to ping
	virtual void Ping( const SystemAddress target )=0;

	/// Send a ping to the specified unconnected system. The remote system, if it is Initialized, will respond with ID_PONG followed by sizeof(RakNetTime) containing the system time the ping was sent.(Default is 4 bytes - See __GET_TIME_64BIT in RakNetTypes.h
	/// System should reply with ID_PONG if it is active
	/// \param[in] host Either a dotted IP address or a domain name.  Can be 255.255.255.255 for LAN broadcast.
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] onlyReplyOnAcceptingConnections Only request a reply if the remote system is accepting connections
	/// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
	/// \return true on success, false on failure (unknown hostname)
	virtual bool Ping( const char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections, unsigned connectionSocketIndex=0 )=0;

	/// Returns the average of all ping times read for the specific system or -1 if none read yet
	/// \param[in] systemAddress Which system we are referring to
	/// \return The ping time for this system, or -1
	virtual int GetAveragePing( const SystemAddress systemAddress )=0;

	/// Returns the last ping time read for the specific system or -1 if none read yet
	/// \param[in] systemAddress Which system we are referring to
	/// \return The last ping time for this system, or -1
	virtual int GetLastPing( const SystemAddress systemAddress ) const=0;

	/// Returns the lowest ping time read or -1 if none read yet
	/// \param[in] systemAddress Which system we are referring to
	/// \return The lowest ping time for this system, or -1
	virtual int GetLowestPing( const SystemAddress systemAddress ) const=0;

	/// Ping the remote systems every so often, or not. This is off by default.  Can be called anytime.
	/// \param[in] doPing True to start occasional pings.  False to stop them.
	virtual void SetOccasionalPing( bool doPing )=0;

	// --------------------------------------------------------------------------------------------Static Data Functions - Functions dealing with API defined synchronized memory--------------------------------------------------------------------------------------------
	/// Sets the data to send along with a LAN server discovery or offline ping reply.
	/// \a length should be under 400 bytes, as a security measure against flood attacks
	/// \param[in] data a block of data to store, or 0 for none
	/// \param[in] length The length of data in bytes, or 0 for none
	/// \sa Ping.cpp
	virtual void SetOfflinePingResponse( const char *data, const unsigned int length )=0;

	/// Returns pointers to a copy of the data passed to SetOfflinePingResponse
	/// \param[out] data A pointer to a copy of the data passed to \a SetOfflinePingResponse()
	/// \param[out] length A pointer filled in with the length parameter passed to SetOfflinePingResponse()
	/// \sa SetOfflinePingResponse
	virtual void GetOfflinePingResponse( char **data, unsigned int *length )=0;

	//--------------------------------------------------------------------------------------------Network Functions - Functions dealing with the network in general--------------------------------------------------------------------------------------------
	/// Return the unique address identifier that represents you or another system on the the network and is based on your local IP / port.
	/// \param[in] systemAddress Use UNASSIGNED_SYSTEM_ADDRESS to get your behind-LAN address. Use a connected system to get their behind-LAN address
	/// \param[in] index When you have multiple internal IDs, which index to return? Currently limited to MAXIMUM_NUMBER_OF_INTERNAL_IDS (so the maximum value of this variable is MAXIMUM_NUMBER_OF_INTERNAL_IDS-1)
	/// \return the identifier of your system internally, which may not be how other systems see if you if you are behind a NAT or proxy
	virtual SystemAddress GetInternalID( const SystemAddress systemAddress=UNASSIGNED_SYSTEM_ADDRESS, const int index=0 ) const=0;

	/// Return the unique address identifier that represents you on the the network and is based on your externalIP / port
	/// (the IP / port the specified player uses to communicate with you)
	/// \param[in] target Which remote system you are referring to for your external ID.  Usually the same for all systems, unless you have two or more network cards.
	virtual SystemAddress GetExternalID( const SystemAddress target ) const=0;

	/// Given a connected system, give us the unique GUID representing that instance of RakPeer.
	/// This will be the same on all systems connected to that instance of RakPeer, even if the external system addresses are different
	/// Currently O(log(n)), but this may be improved in the future. If you use this frequently, you may want to cache the value as it won't change.
	/// If \a input is UNASSIGNED_SYSTEM_ADDRESS, will return your own GUID
	/// \pre Call Startup() first, or the function will return UNASSIGNED_RAKNET_GUID
	/// \param[in] input The system address of the system we are connected to
	virtual RakNetGUID GetGuidFromSystemAddress( const SystemAddress input ) const=0;

	/// Given the GUID of a connected system, give us the system address of that system.
	/// The GUID will be the same on all systems connected to that instance of RakPeer, even if the external system addresses are different
	/// Currently O(log(n)), but this may be improved in the future. If you use this frequently, you may want to cache the value as it won't change.
	/// If \a input is UNASSIGNED_RAKNET_GUID, will return UNASSIGNED_SYSTEM_ADDRESS
	/// \param[in] input The RakNetGUID of the system we are checking to see if we are connected to
	virtual SystemAddress GetSystemAddressFromGuid( const RakNetGUID input ) const=0;

	/// Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable message.
	/// Default time is 10,000 or 10 seconds in release and 30,000 or 30 seconds in debug.
	/// \param[in] timeMS Time, in MS
	/// \param[in] target Which system to do this for. Pass UNASSIGNED_SYSTEM_ADDRESS for all systems.
	virtual void SetTimeoutTime( RakNetTime timeMS, const SystemAddress target )=0;

	/// Set the MTU per datagram.  It's important to set this correctly - otherwise packets will be needlessly split, decreasing performance and throughput.
	/// Maximum allowed size is MAXIMUM_MTU_SIZE.
	/// Too high of a value will cause packets not to arrive at worst and be fragmented at best.
	/// Too low of a value will split packets unnecessarily.
	/// Recommended size is 1500
	/// sa MTUSize.h
	/// \param[in] size The MTU size
	/// \param[in] target Which system to set this for.  UNASSIGNED_SYSTEM_ADDRESS to set the default, for new systems
	/// \pre Can only be called when not connected.
	/// \return false on failure (we are connected), else true
	virtual bool SetMTUSize( int size, const SystemAddress target=UNASSIGNED_SYSTEM_ADDRESS )=0;

	/// Returns the current MTU size
	/// \param[in] target Which system to get this for.  UNASSIGNED_SYSTEM_ADDRESS to get the default
	/// \return The current MTU size
	virtual int GetMTUSize( const SystemAddress target ) const=0;

	/// Returns the number of IP addresses this system has internally. Get the actual addresses from GetLocalIP()
	virtual unsigned GetNumberOfAddresses( void )=0;

	/// Returns an IP address at index 0 to GetNumberOfAddresses-1
	/// \param[in] index index into the list of IP addresses
	/// \return The local IP address at this index
	virtual const char* GetLocalIP( unsigned int index )=0;

	/// Is this a local IP?
	/// \param[in] An IP address to check, excluding the port
	/// \return True if this is one of the IP addresses returned by GetLocalIP
	virtual bool IsLocalIP( const char *ip )=0;

	/// Allow or disallow connection responses from any IP. Normally this should be false, but may be necessary
	/// when connecting to servers with multiple IP addresses.
	/// \param[in] allow - True to allow this behavior, false to not allow. Defaults to false. Value persists between connections
	virtual void AllowConnectionResponseIPMigration( bool allow )=0;

	/// Sends a one byte message ID_ADVERTISE_SYSTEM to the remote unconnected system.
	/// This will tell the remote system our external IP outside the LAN along with some user data.
	/// \pre The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] host Either a dotted IP address or a domain name
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] data Optional data to append to the packet.
	/// \param[in] dataLength length of data in bytes.  Use 0 if no data.
	/// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
	/// \return false if IsActive()==false or the host is unresolvable. True otherwise
	virtual bool AdvertiseSystem( const char *host, unsigned short remotePort, const char *data, int dataLength, unsigned connectionSocketIndex=0 )=0;

	/// Controls how often to return ID_DOWNLOAD_PROGRESS for large message downloads.
	/// ID_DOWNLOAD_PROGRESS is returned to indicate a new partial message chunk, roughly the MTU size, has arrived
	/// As it can be slow or cumbersome to get this notification for every chunk, you can set the interval at which it is returned.
	/// Defaults to 0 (never return this notification)
	/// \param[in] interval How many messages to use as an interval
	virtual void SetSplitMessageProgressInterval(int interval)=0;

	/// Returns what was passed to SetSplitMessageProgressInterval()
	/// \return What was passed to SetSplitMessageProgressInterval(). Default to 0.
	virtual int GetSplitMessageProgressInterval(void) const=0;

	/// Set how long to wait before giving up on sending an unreliable message
	/// Useful if the network is clogged up.
	/// Set to 0 or less to never timeout.  Defaults to 0.
	/// \param[in] timeoutMS How many ms to wait before simply not sending an unreliable message.
	virtual void SetUnreliableTimeout(RakNetTime timeoutMS)=0;

	/// Send a message to host, with the IP socket option TTL set to 3
	/// This message will not reach the host, but will open the router.
	/// Used for NAT-Punchthrough
	virtual void SendTTL( const char* host, unsigned short remotePort, int ttl, unsigned connectionSocketIndex=0 )=0;

	// --------------------------------------------------------------------------------------------Compression Functions - Functions related to the compression layer--------------------------------------------------------------------------------------------
	/// Enables or disables frequency table tracking.  This is required to get a frequency table, which is used in GenerateCompressionLayer()
	/// This value persists between connect calls and defaults to false (no frequency tracking)
	/// \pre You can call this at any time - however you SHOULD only call it when disconnected.  Otherwise you will only trackpart of the values sent over the network.
	/// \param[in] doCompile True to enable tracking 
	virtual void SetCompileFrequencyTable( bool doCompile )=0;

	/// Returns the frequency of outgoing bytes into output frequency table
	/// The purpose is to save to file as either a master frequency table from a sample game session for passing to
	/// GenerateCompressionLayer() 
	/// \pre You should only call this when disconnected. Requires that you first enable data frequency tracking by calling SetCompileFrequencyTable(true)
	/// \param[out] outputFrequencyTable  The frequency of each corresponding byte
	/// \return False (failure) if connected or if frequency table tracking is not enabled. Otherwise true (success)
	virtual bool GetOutgoingFrequencyTable( unsigned int outputFrequencyTable[ 256 ] )=0;

	/// This is an optional function to generate the compression layer based on the input frequency table.
	/// If you want to use it you should call this twice - once with inputLayer as true and once as false.
	/// The frequency table passed here with inputLayer=true should match the frequency table on the recipient with inputLayer=false.
	/// Likewise, the frequency table passed here with inputLayer=false should match the frequency table on the recipient with inputLayer=true.
	/// Calling this function when there is an existing layer will overwrite the old layer.
	/// \pre You should only call this when disconnected
	/// \param[in] inputFrequencyTable A frequency table for your data
	/// \param[in] inputLayer Is this the input layer?
	/// \return false (failure) if connected.  Otherwise true (success)
	/// \sa Compression.cpp
	virtual bool GenerateCompressionLayer( unsigned int inputFrequencyTable[ 256 ], bool inputLayer )=0;

	/// Delete the output or input layer as specified.  This is not necessary to call and is only valuable for freeing memory.
	/// \pre You should only call this when disconnected
	/// \param[in] inputLayer True to mean the inputLayer, false to mean the output layer
	/// \return false (failure) if connected.  Otherwise true (success)
	virtual bool DeleteCompressionLayer( bool inputLayer )=0;

	/// Returns the compression ratio. A low compression ratio is good.  Compression is for outgoing data
	/// \return The compression ratio
	virtual float GetCompressionRatio( void ) const=0;

	///Returns the decompression ratio.  A high decompression ratio is good.  Decompression is for incoming data
	/// \return The decompression ratio
	virtual float GetDecompressionRatio( void ) const=0;

	// -------------------------------------------------------------------------------------------- Plugin Functions--------------------------------------------------------------------------------------------
	/// Attatches a Plugin interface to run code automatically on message receipt in the Receive call
	/// \note If plugins have dependencies on each other then the order does matter - for example the router plugin should go first because it might route messages for other plugins
	/// \param[in] messageHandler Pointer to a plugin to attach
	virtual void AttachPlugin( PluginInterface2 *plugin )=0;

	/// Detaches a Plugin interface to run code automatically on message receipt
	/// \param[in] messageHandler Pointer to a plugin to detach
	virtual void DetachPlugin( PluginInterface2 *messageHandler )=0;

	// --------------------------------------------------------------------------------------------Miscellaneous Functions--------------------------------------------------------------------------------------------
	/// Put a message back at the end of the receive queue in case you don't want to deal with it immediately
	/// \param[in] packet The packet you want to push back.
	/// \param[in] pushAtHead True to push the packet so that the next receive call returns it.  False to push it at the end of the queue (obviously pushing it at the end makes the packets out of order)
	virtual void PushBackPacket( Packet *packet, bool pushAtHead )=0;

	/// \Internal
	// \param[in] routerInterface The router to use to route messages to systems not directly connected to this system.
	virtual void SetRouterInterface( RouterInterface *routerInterface )=0;

	/// \Internal
	// \param[in] routerInterface The router to use to route messages to systems not directly connected to this system.
	virtual void RemoveRouterInterface( RouterInterface *routerInterface )=0;

	/// \returns a packet for you to write to if you want to create a Packet for some reason.
	/// You can add it to the receive buffer with PushBackPacket
	/// \param[in] dataSize How many bytes to allocate for the buffer
	/// \return A packet you can write to
	virtual Packet* AllocatePacket(unsigned dataSize)=0;

	// --------------------------------------------------------------------------------------------Network Simulator Functions--------------------------------------------------------------------------------------------
	/// Adds simulated ping and packet loss to the outgoing data flow.
	/// To simulate bi-directional ping and packet loss, you should call this on both the sender and the recipient, with half the total ping and maxSendBPS value on each.
	/// You can exclude network simulator code with the _RELEASE #define to decrease code size
	/// \depreciated Use http://www.jenkinssoftware.com/raknet/forum/index.php?topic=1671.0 instead.
	/// \param[in] maxSendBPS Maximum bits per second to send.  Packetloss grows linearly.  0 to disable. (CURRENTLY BROKEN - ALWAYS DISABLED)
	/// \param[in] minExtraPing The minimum time to delay sends.
	/// \param[in] extraPingVariance The additional random time to delay sends.
	virtual void ApplyNetworkSimulator( double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance)=0;

	/// Limits how much outgoing bandwidth can be sent per-connection.
	/// This limit does not apply to the sum of all connections!
	/// Exceeding the limit queues up outgoing traffic
	/// \param[in] maxBitsPerSecond Maximum bits per second to send.  Use 0 for unlimited (default). Once set, it takes effect immedately and persists until called again.
	virtual void SetPerConnectionOutgoingBandwidthLimit( unsigned maxBitsPerSecond )=0;

	/// Returns if you previously called ApplyNetworkSimulator
	/// \return If you previously called ApplyNetworkSimulator
	virtual bool IsNetworkSimulatorActive( void )=0;

	// -------------------------------------------------------------------------------------------- Socket Functions--------------------------------------------------------------------------------------------
	/// Have RakNet use a socket you created yourself
	/// The socket should not be in use - it is up to you to either shutdown or close the connections using it. Otherwise existing connections on that socket will eventually disconnect
	/// This socket will be forgotten after calling Shutdown(), so rebind again if you need to.
	/// \param[in] s The socket to rebind.
	/// \param[in] haveRakNetCloseSocket If true, RakNet will call closeSocket on shutdown for this socket.
	/// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
	virtual void UseUserSocket( int socket, bool haveRakNetCloseSocket, unsigned connectionSocketIndex)=0;

	/// Have RakNet recreate a socket using a different port.
	/// The socket should not be in use - it is up to you to either shutdown or close the connections using it. Otherwise existing connections on that socket will eventually disconnect
	/// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
	/// \param[in] sd Address to bind on
	virtual void RebindSocketAddress(unsigned connectionSocketIndex, SocketDescriptor &sd)=0;

	// --------------------------------------------------------------------------------------------Statistical Functions - Functions dealing with API performance--------------------------------------------------------------------------------------------

	/// Returns a structure containing a large set of network statistics for the specified system.
	/// You can map this data to a string using the C style StatisticsToString() function
	/// \param[in] systemAddress: Which connected system to get statistics for
	/// \param[in] rns If you supply this structure, it will be written to it.  Otherwise it will use a static struct, which is not threadsafe
	/// \return 0 on can't find the specified system.  A pointer to a set of data otherwise.
	/// \sa RakNetStatistics.h
	virtual RakNetStatistics * const GetStatistics( const SystemAddress systemAddress, RakNetStatistics *rns=0 )=0;

	// --------------------------------------------------------------------------------------------EVERYTHING AFTER THIS COMMENT IS FOR INTERNAL USE ONLY--------------------------------------------------------------------------------------------
	/// \internal
	virtual char *GetRPCString( const char *data, const BitSize_t bitSize, const SystemAddress systemAddress)=0;

	/// \internal
	virtual bool SendOutOfBand(const char *host, unsigned short remotePort, MessageID header, const char *data, BitSize_t dataLength, unsigned connectionSocketIndex=0 )=0;

};

#endif
