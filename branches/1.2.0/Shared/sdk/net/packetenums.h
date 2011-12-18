/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/sdk/net/packetenums.h
*  PURPOSE:     Network packet enumerations
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

enum
{
    PACKET_ID_SERVER_JOIN = 0,
    PACKET_ID_SERVER_JOIN_DATA,
    PACKET_ID_SERVER_JOIN_COMPLETE,

    PACKET_ID_PLAYER_JOIN,
    PACKET_ID_PLAYER_JOINDATA,
    PACKET_ID_PLAYER_QUIT,
    PACKET_ID_PLAYER_TIMEOUT,

    PACKET_ID_MOD_NAME,
    PACKET_ID_PACKET_PROGRESS,
    PACKET_ID_MTA_RESERVED_03,
    PACKET_ID_MTA_RESERVED_04,
    PACKET_ID_MTA_RESERVED_05,
    PACKET_ID_MTA_RESERVED_06,
    PACKET_ID_MTA_RESERVED_07,
    PACKET_ID_MTA_RESERVED_08,
    PACKET_ID_MTA_RESERVED_09,
    PACKET_ID_MTA_RESERVED_10,
    PACKET_ID_MTA_RESERVED_11,
    PACKET_ID_MTA_RESERVED_12,
    PACKET_ID_MTA_RESERVED_13,
    PACKET_ID_MTA_RESERVED_14,
    PACKET_ID_MTA_RESERVED_15,

    PACKET_ID_END_OF_INTERNAL_PACKETS
};

#define RAKNET_PACKET_COUNT RID_USER_PACKET_ENUM


/// You should not edit the file MessageIdentifiers.h as it is a part of RakNet static library
/// To define your own message id, define an enum following the code example that follows. 
///
/// \code
/// enum {
///   ID_MYPROJECT_MSG_1 = ID_USER_PACKET_ENUM,
///   ID_MYPROJECT_MSG_2, 
///    ... 
/// };
/// \endcode 
///
/// \note All these enumerations should be casted to (unsigned char) before writing them to RakNet::BitStream
enum
{
    //
	// RESERVED TYPES - DO NOT CHANGE THESE
	// All types from RakPeer
	//
	/// These types are never returned to the user.
	/// Ping from a connected system.  Update timestamps (internal use only)
	RID_CONNECTED_PING,  
	/// Ping from an unconnected system.  Reply but do not update timestamps. (internal use only)
	RID_UNCONNECTED_PING,
	/// Ping from an unconnected system.  Only reply if we have open connections. Do not update timestamps. (internal use only)
	RID_UNCONNECTED_PING_OPEN_CONNECTIONS,
	/// Pong from a connected system.  Update timestamps (internal use only)
	RID_CONNECTED_PONG,
	/// A reliable packet to detect lost connections (internal use only)
	RID_DETECT_LOST_CONNECTIONS,
	/// C2S: Initial query: Header(1), OfflineMesageID(16), Protocol number(1), Pad(toMTU), sent with no fragment set.
	/// If protocol fails on server, returns ID_INCOMPATIBLE_PROTOCOL_VERSION to client
	RID_OPEN_CONNECTION_REQUEST_1,
	/// S2C: Header(1), OfflineMesageID(16), server GUID(8), HasSecurity(1), Cookie(4, if HasSecurity)
	/// , public key (if do security is true), MTU(2). If public key fails on client, returns ID_PUBLIC_KEY_MISMATCH
	RID_OPEN_CONNECTION_REPLY_1,
	/// C2S: Header(1), OfflineMesageID(16), Cookie(4, if HasSecurity is true on the server), clientSupportsSecurity(1 bit),
	/// handshakeChallenge (if has security on both server and client), remoteBindingAddress(6), MTU(2), client GUID(8)
	/// Connection slot allocated if cookie is valid, server is not full, GUID and IP not already in use.
	RID_OPEN_CONNECTION_REQUEST_2,
	/// S2C: Header(1), OfflineMesageID(16), server GUID(8), mtu(2), doSecurity(1 bit), handshakeAnswer (if do security is true)
	RID_OPEN_CONNECTION_REPLY_2,
	/// C2S: Header(1), GUID(8), Timestamp, HasSecurity(1), Proof(32)
	RID_CONNECTION_REQUEST,
	/// RakPeer - Remote system requires secure connections, pass a public key to RakPeerInterface::Connect()
	RID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY,
	/// RakPeer - We passed a public key to RakPeerInterface::Connect(), but the other system did not have security turned on
	RID_OUR_SYSTEM_REQUIRES_SECURITY,
	/// RakPeer - Wrong public key passed to RakPeerInterface::Connect()
	RID_PUBLIC_KEY_MISMATCH,
	/// RakPeer - Same as ID_ADVERTISE_SYSTEM, but intended for internal use rather than being passed to the user.
	/// Second byte indicates type. Used currently for NAT punchthrough for receiver port advertisement. See ID_NAT_ADVERTISE_RECIPIENT_PORT
	RID_OUT_OF_BAND_INTERNAL,
	/// If RakPeerInterface::Send() is called where PacketReliability contains _WITH_ACK_RECEIPT, then on a later call to
	/// RakPeerInterface::Receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS. The message will be 5 bytes long,
	/// and bytes 1-4 inclusive will contain a number in native order containing a number that identifies this message.
	/// This number will be returned by RakPeerInterface::Send() or RakPeerInterface::SendList(). ID_SND_RECEIPT_ACKED means that
	/// the message arrived
	RID_SND_RECEIPT_ACKED,
	/// If RakPeerInterface::Send() is called where PacketReliability contains UNRELIABLE_WITH_ACK_RECEIPT, then on a later call to
	/// RakPeerInterface::Receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS. The message will be 5 bytes long,
	/// and bytes 1-4 inclusive will contain a number in native order containing a number that identifies this message. This number
	/// will be returned by RakPeerInterface::Send() or RakPeerInterface::SendList(). ID_SND_RECEIPT_LOSS means that an ack for the
	/// message did not arrive (it may or may not have been delivered, probably not). On disconnect or shutdown, you will not get
	/// ID_SND_RECEIPT_LOSS for unsent messages, you should consider those messages as all lost.
	RID_SND_RECEIPT_LOSS,
	

	//
	// USER TYPES - DO NOT CHANGE THESE
	//

	/// RakPeer - In a client/server environment, our connection request to the server has been accepted.
	RID_CONNECTION_REQUEST_ACCEPTED,
	/// RakPeer - Sent to the player when a connection request cannot be completed due to inability to connect. 
	RID_CONNECTION_ATTEMPT_FAILED,
	/// RakPeer - Sent a connect request to a system we are currently connected to.
	RID_ALREADY_CONNECTED,
	/// RakPeer - A remote system has successfully connected.
	RID_NEW_INCOMING_CONNECTION,
	/// RakPeer - The system we attempted to connect to is not accepting new connections.
	RID_NO_FREE_INCOMING_CONNECTIONS,
	/// RakPeer - The system specified in Packet::systemAddress has disconnected from us.  For the client, this would mean the
	/// server has shutdown. 
	RID_DISCONNECTION_NOTIFICATION,
	/// RakPeer - Reliable packets cannot be delivered to the system specified in Packet::systemAddress.  The connection to that
	/// system has been closed. 
	RID_CONNECTION_LOST,
	/// RakPeer - We are banned from the system we attempted to connect to.
	RID_CONNECTION_BANNED,
	/// RakPeer - The remote system is using a password and has refused our connection because we did not set the correct password.
	RID_INVALID_PASSWORD,
	// RAKNET_PROTOCOL_VERSION in RakNetVersion.h does not match on the remote system what we have on our system
	// This means the two systems cannot communicate.
	// The 2nd byte of the message contains the value of RAKNET_PROTOCOL_VERSION for the remote system
	RID_INCOMPATIBLE_PROTOCOL_VERSION,
	// Means that this IP address connected recently, and can't connect again as a security measure. See
	/// RakPeer::SetLimitIPConnectionFrequency()
	RID_IP_RECENTLY_CONNECTED,
	/// RakPeer - The sizeof(RakNetTime) bytes following this byte represent a value which is automatically modified by the difference
	/// in system times between the sender and the recipient. Requires that you call SetOccasionalPing.
	RID_TIMESTAMP,
    /// RakPeer - Pong from an unconnected system.  First byte is ID_UNCONNECTED_PONG, second sizeof(RakNet::TimeMS) bytes is the ping,
	/// following bytes is system specific enumeration data.
	/// Read using bitstreams
	RID_UNCONNECTED_PONG,
	/// RakPeer - Inform a remote system of our IP/Port. On the recipient, all data past ID_ADVERTISE_SYSTEM is whatever was passed to
	/// the data parameter
	RID_ADVERTISE_SYSTEM,
	// RakPeer - Downloading a large message. Format is ID_DOWNLOAD_PROGRESS (MessageID), partCount (unsigned int),
	///  partTotal (unsigned int),
	/// partLength (unsigned int), first part data (length <= MAX_MTU_SIZE). See the three parameters partCount, partTotal
	///  and partLength in OnFileProgress in FileListTransferCBInterface.h
	RID_DOWNLOAD_PROGRESS,
	
	/// ConnectionGraph2 plugin - In a client/server environment, a client other than ourselves has disconnected gracefully.
	///   Packet::systemAddress is modified to reflect the systemAddress of this client.
	RID_REMOTE_DISCONNECTION_NOTIFICATION,
	/// ConnectionGraph2 plugin - In a client/server environment, a client other than ourselves has been forcefully dropped.
	///  Packet::systemAddress is modified to reflect the systemAddress of this client.
	RID_REMOTE_CONNECTION_LOST,
	/// ConnectionGraph2 plugin: Bytes 1-4 = count. for (count items) contains {SystemAddress, RakNetGUID, 2 byte ping}
	RID_REMOTE_NEW_INCOMING_CONNECTION,

	/// FileListTransfer plugin - Setup data
	RID_FILE_LIST_TRANSFER_HEADER,
	/// FileListTransfer plugin - A file
	RID_FILE_LIST_TRANSFER_FILE,
	// Ack for reference push, to send more of the file
	RID_FILE_LIST_REFERENCE_PUSH_ACK,

	/// DirectoryDeltaTransfer plugin - Request from a remote system for a download of a directory
	RID_DDT_DOWNLOAD_REQUEST,
	
	/// RakNetTransport plugin - Transport provider message, used for remote console
	RID_TRANSPORT_STRING,

 	/// ReplicaManager plugin - Create an object
	RID_REPLICA_MANAGER_CONSTRUCTION,
 	/// ReplicaManager plugin - Changed scope of an object
 	RID_REPLICA_MANAGER_SCOPE_CHANGE,
 	/// ReplicaManager plugin - Serialized data of an object
	RID_REPLICA_MANAGER_SERIALIZE,
 	/// ReplicaManager plugin - New connection, about to send all world objects
	RID_REPLICA_MANAGER_DOWNLOAD_STARTED,
 	/// ReplicaManager plugin - Finished downloading all serialized objects
	RID_REPLICA_MANAGER_DOWNLOAD_COMPLETE,

	/// RakVoice plugin - Open a communication channel
	RID_RAKVOICE_OPEN_CHANNEL_REQUEST,
	/// RakVoice plugin - Communication channel accepted
	RID_RAKVOICE_OPEN_CHANNEL_REPLY,
	/// RakVoice plugin - Close a communication channel
	RID_RAKVOICE_CLOSE_CHANNEL,
	/// RakVoice plugin - Voice data
	RID_RAKVOICE_DATA,

	/// Autopatcher plugin - Get a list of files that have changed since a certain date
	RID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE,
	/// Autopatcher plugin - A list of files to create
	RID_AUTOPATCHER_CREATION_LIST,
	/// Autopatcher plugin - A list of files to delete
	RID_AUTOPATCHER_DELETION_LIST,
	/// Autopatcher plugin - A list of files to get patches for
	RID_AUTOPATCHER_GET_PATCH,
	/// Autopatcher plugin - A list of patches for a list of files
	RID_AUTOPATCHER_PATCH_LIST,
	/// Autopatcher plugin - Returned to the user: An error from the database repository for the autopatcher.
	RID_AUTOPATCHER_REPOSITORY_FATAL_ERROR,
	/// Autopatcher plugin - Finished getting all files from the autopatcher
	RID_AUTOPATCHER_FINISHED_INTERNAL,
	RID_AUTOPATCHER_FINISHED,
	/// Autopatcher plugin - Returned to the user: You must restart the application to finish patching.
	RID_AUTOPATCHER_RESTART_APPLICATION,

	/// NATPunchthrough plugin: internal
	RID_NAT_PUNCHTHROUGH_REQUEST,
	/// NATPunchthrough plugin: internal
	RID_NAT_GROUP_PUNCHTHROUGH_REQUEST,
	/// NATPunchthrough plugin: internal
	RID_NAT_GROUP_PUNCHTHROUGH_REPLY,
	/// NATPunchthrough plugin: internal
	RID_NAT_CONNECT_AT_TIME,
	/// NATPunchthrough plugin: internal
	RID_NAT_GET_MOST_RECENT_PORT,
	/// NATPunchthrough plugin: internal
	RID_NAT_CLIENT_READY,
	/// NATPunchthrough plugin: internal
	RID_NAT_GROUP_PUNCHTHROUGH_FAILURE_NOTIFICATION,

	/// NATPunchthrough plugin: Destination system is not connected to the server. Bytes starting at offset 1 contains the
	///  RakNetGUID destination field of NatPunchthroughClient::OpenNAT().
	RID_NAT_TARGET_NOT_CONNECTED,
	/// NATPunchthrough plugin: Destination system is not responding to ID_NAT_GET_MOST_RECENT_PORT. Possibly the plugin is not installed.
	///  Bytes starting at offset 1 contains the RakNetGUID  destination field of NatPunchthroughClient::OpenNAT().
	RID_NAT_TARGET_UNRESPONSIVE,
	/// NATPunchthrough plugin: The server lost the connection to the destination system while setting up punchthrough.
	///  Possibly the plugin is not installed. Bytes starting at offset 1 contains the RakNetGUID  destination
	///  field of NatPunchthroughClient::OpenNAT().
	RID_NAT_CONNECTION_TO_TARGET_LOST,
	/// NATPunchthrough plugin: This punchthrough is already in progress. Possibly the plugin is not installed.
	///  Bytes starting at offset 1 contains the RakNetGUID destination field of NatPunchthroughClient::OpenNAT().
	RID_NAT_ALREADY_IN_PROGRESS,
	/// NATPunchthrough plugin: This message is generated on the local system, and does not come from the network.
	///  packet::guid contains the destination field of NatPunchthroughClient::OpenNAT(). Byte 1 contains 1 if you are the sender, 0 if not
	RID_NAT_PUNCHTHROUGH_FAILED,
	/// NATPunchthrough plugin: Punchthrough succeeded. See packet::systemAddress and packet::guid. Byte 1 contains 1 if you are the sender,
	///  0 if not. You can now use RakPeer::Connect() or other calls to communicate with this system.
	RID_NAT_PUNCHTHROUGH_SUCCEEDED,
	/// NATPunchthrough plugin: OpenNATGroup failed.
	/// packet::guid contains the facilitator field of NatPunchthroughClient::OpenNAT()
	/// Data format starts at byte 1:<BR>
	/// (char) passedSystemsCount,<BR>
	/// (RakNetGuid, SystemAddress) (for passedSystemsCount),<BR>
	/// (char) ignoredSystemsCount (caused by ID_NAT_TARGET_NOT_CONNECTED, ID_NAT_CONNECTION_TO_TARGET_LOST, ID_NAT_TARGET_UNRESPONSIVE),<BR>
	/// RakNetGuid (for ignoredSystemsCount),<BR>
	/// (char) failedSystemsCount,<BR>
	/// RakNetGuid (for failedSystemsCount)<BR>
	RID_NAT_GROUP_PUNCH_FAILED,
	/// NATPunchthrough plugin: OpenNATGroup succeeded.
	/// packet::guid contains the facilitator field of NatPunchthroughClient::OpenNAT()
	/// See ID_NAT_GROUP_PUNCH_FAILED for data format
	RID_NAT_GROUP_PUNCH_SUCCEEDED,

	/// ReadyEvent plugin - Set the ready state for a particular system
	/// First 4 bytes after the message contains the id
	RID_READY_EVENT_SET,
	/// ReadyEvent plugin - Unset the ready state for a particular system
	/// First 4 bytes after the message contains the id
	RID_READY_EVENT_UNSET,
	/// All systems are in state ID_READY_EVENT_SET
	/// First 4 bytes after the message contains the id
	RID_READY_EVENT_ALL_SET,
	/// \internal, do not process in your game
	/// ReadyEvent plugin - Request of ready event state - used for pulling data when newly connecting
	RID_READY_EVENT_QUERY,

	/// Lobby packets. Second byte indicates type.
	RID_LOBBY_GENERAL,

	// RPC3, RPC4Plugin error
	RID_RPC_REMOTE_ERROR,
	/// Plugin based replacement for RPC system
	RID_RPC_PLUGIN,

	/// FileListTransfer transferring large files in chunks that are read only when needed, to save memory
	RID_FILE_LIST_REFERENCE_PUSH,
	/// Force the ready event to all set
	RID_READY_EVENT_FORCE_ALL_SET,

	/// Rooms function
	RID_ROOMS_EXECUTE_FUNC,
	RID_ROOMS_LOGON_STATUS,
	RID_ROOMS_HANDLE_CHANGE,

	/// Lobby2 message
	RID_LOBBY2_SEND_MESSAGE,
	RID_LOBBY2_SERVER_ERROR,

	/// Informs user of a new host GUID. Packet::Guid contains this new host RakNetGuid. The old host can be read out using BitStream->Read(RakNetGuid) starting on byte 1
	/// This is not returned until connected to a remote system
	/// If the oldHost is UNASSIGNED_RAKNET_GUID, then this is the first time the host has been determined
	RID_FCM2_NEW_HOST,
	/// \internal For FullyConnectedMesh2 plugin
	RID_FCM2_REQUEST_FCMGUID,
	/// \internal For FullyConnectedMesh2 plugin
	RID_FCM2_RESPOND_CONNECTION_COUNT,
	/// \internal For FullyConnectedMesh2 plugin
	RID_FCM2_INFORM_FCMGUID,
	/// \internal For FullyConnectedMesh2 plugin
	RID_FCM2_UPDATE_MIN_TOTAL_CONNECTION_COUNT,

	/// UDP proxy messages. Second byte indicates type.
	RID_UDP_PROXY_GENERAL,

	/// SQLite3Plugin - execute
	RID_SQLite3_EXEC,
	/// SQLite3Plugin - Remote database is unknown
	RID_SQLite3_UNKNOWN_DB,
	/// Events happening with SQLiteClientLoggerPlugin
	RID_SQLLITE_LOGGER,

	/// Sent to NatTypeDetectionServer
	RID_NAT_TYPE_DETECTION_REQUEST,
	/// Sent to NatTypeDetectionClient. Byte 1 contains the type of NAT detected.
	RID_NAT_TYPE_DETECTION_RESULT,

	/// Used by the router2 plugin
	RID_ROUTER_2_INTERNAL,
	/// No path is available or can be established to the remote system
	/// Packet::guid contains the endpoint guid that we were trying to reach
	RID_ROUTER_2_FORWARDING_NO_PATH,
	/// \brief You can now call connect, ping, or other operations to the destination system.
	///
	/// Connect as follows:
	///
	/// RakNet::BitStream bs(packet->data, packet->length, false);
	/// bs.IgnoreBytes(sizeof(MessageID));
	/// RakNetGUID endpointGuid;
	/// bs.Read(endpointGuid);
	/// unsigned short sourceToDestPort;
	/// bs.Read(sourceToDestPort);
	/// char ipAddressString[32];
	/// packet->systemAddress.ToString(false, ipAddressString);
	/// rakPeerInterface->Connect(ipAddressString, sourceToDestPort, 0,0);
	RID_ROUTER_2_FORWARDING_ESTABLISHED,
	/// The IP address for a forwarded connection has changed
	/// Read endpointGuid and port as per ID_ROUTER_2_FORWARDING_ESTABLISHED
	RID_ROUTER_2_REROUTED,

	/// \internal Used by the team balancer plugin
	RID_TEAM_BALANCER_INTERNAL,
	/// Cannot switch to the desired team because it is full. However, if someone on that team leaves, you will
	///  get ID_TEAM_BALANCER_SET_TEAM later. Byte 1 contains the team you requested to join. Following bytes contain NetworkID of which member.
	RID_TEAM_BALANCER_REQUESTED_TEAM_CHANGE_PENDING,
	/// Cannot switch to the desired team because all teams are locked. However, if someone on that team leaves,
	///  you will get ID_TEAM_BALANCER_SET_TEAM later. Byte 1 contains the team you requested to join.
	RID_TEAM_BALANCER_TEAMS_LOCKED,
	/// Team balancer plugin informing you of your team. Byte 1 contains the team you requested to join. Following bytes contain NetworkID of which member.
	RID_TEAM_BALANCER_TEAM_ASSIGNED,

	/// Gamebryo Lightspeed integration
	RID_LIGHTSPEED_INTEGRATION,

	/// XBOX integration
	RID_XBOX_LOBBY,

	/// The password we used to challenge the other system passed, meaning the other system has called TwoWayAuthentication::AddPassword() with the same password we passed to TwoWayAuthentication::Challenge()
	/// You can read the identifier used to challenge as follows:
	/// RakNet::BitStream bs(packet->data, packet->length, false); bs.IgnoreBytes(sizeof(RakNet::MessageID)); RakNet::RakString password; bs.Read(password);
	RID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS,
	RID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS,
	/// A remote system sent us a challenge using TwoWayAuthentication::Challenge(), and the challenge failed.
	/// If the other system must pass the challenge to stay connected, you should call RakPeer::CloseConnection() to terminate the connection to the other system. 
	RID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE,
	/// The other system did not add the password we used to TwoWayAuthentication::AddPassword()
	/// You can read the identifier used to challenge as follows:
	/// RakNet::BitStream bs(packet->data, packet->length, false); bs.IgnoreBytes(sizeof(MessageID)); RakNet::RakString password; bs.Read(password);
	RID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE,
	/// The other system did not respond within a timeout threshhold. Either the other system is not running the plugin or the other system was blocking on some operation for a long time.
	/// You can read the identifier used to challenge as follows:
	/// RakNet::BitStream bs(packet->data, packet->length, false); bs.IgnoreBytes(sizeof(MessageID)); RakNet::RakString password; bs.Read(password);
	RID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT,
	/// \internal
	RID_TWO_WAY_AUTHENTICATION_NEGOTIATION,

	/// CloudClient / CloudServer
	RID_CLOUD_POST_REQUEST,
	RID_CLOUD_RELEASE_REQUEST,
	RID_CLOUD_GET_REQUEST,
	RID_CLOUD_GET_RESPONSE,
	RID_CLOUD_UNSUBSCRIBE_REQUEST,
	RID_CLOUD_SERVER_TO_SERVER_COMMAND,
	RID_CLOUD_SUBSCRIPTION_NOTIFICATION,

	// So I can add more without changing user enumerations
	RID_RESERVED_1,
	RID_RESERVED_2,
	RID_RESERVED_3,
	RID_RESERVED_4,
	RID_RESERVED_5,
	RID_RESERVED_6,
	RID_RESERVED_7,
	RID_RESERVED_8,
	RID_RESERVED_9,

	// For the user to use.  Start your first enumeration at this value.
	RID_USER_PACKET_ENUM,
	//-------------------------------------------------------------------------------------------------------------
 
};

