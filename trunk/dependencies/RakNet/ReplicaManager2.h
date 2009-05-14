/// \file
/// \brief Contains the second iteration of the ReplicaManager class.  This system automatically creates and destroys objects, downloads the world to new players, manages players, and automatically serializes as needed.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __REPLICA_MANAGER_2_H
#define __REPLICA_MANAGER_2_H

#include "Export.h"
#include "RakNetTypes.h"
#include "DS_Map.h"
#include "PluginInterface2.h"
#include "NetworkIDObject.h"
#include "PacketPriority.h"
#include "GetTime.h"
#include "BitStream.h"
#include "DS_Queue.h"

namespace RakNet
{
class BitStream;
class Replica2;
class Connection_RM2;
class Connection_RM2Factory;

/// \defgroup REPLICA_MANAGER_2_GROUP ReplicaManager2
/// \ingroup PLUGINS_GROUP

/// \brief These are the types of events that can cause network data to be transmitted. 
/// \ingroup REPLICA_MANAGER_2_GROUP
typedef int SerializationType;
enum
{
	/// Serialization command initiated by the user
	SEND_SERIALIZATION_GENERIC_TO_SYSTEM,
	/// Serialization command initiated by the user
	BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM,
	/// Serialization command automatically called after sending construction of the object
	SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM,
	/// Serialization command automatically called after sending construction of the object
	BROADCAST_SERIALIZATION_CONSTRUCTION_TO_SYSTEM,
	/// Automatic serialization of data, based on Replica2::AddAutoSerializeTimer
	SEND_AUTO_SERIALIZE_TO_SYSTEM,
	/// Automatic serialization of data, based on Replica2::AddAutoSerializeTimer
	BROADCAST_AUTO_SERIALIZE_TO_SYSTEM,
	/// Received a serialization command, relaying to systems other than the sender
	RELAY_SERIALIZATION_TO_SYSTEMS,

	/// If SetAutoAddNewConnections is true, this is the command sent when sending all game objects to new connections automatically
	SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM,
	// Automatically sent message indicating if the replica is visible or not to a new connection
	SEND_VISIBILITY_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM,
	/// The data portion of the game download, preceeded by SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM
	SEND_DATA_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM,

	/// Default reason to send a destruction command
	SEND_DESTRUCTION_GENERIC_TO_SYSTEM,
	/// Triggered by ReplicaManager2::RecalculateVisibility - A replica is now never constructed, so needs to be destroyed
	SEND_DESTRUCTION_VISIBILITY_RECALCULATION_TO_SYSTEM,
	/// Triggered by Replica2::BroadcastDestruction
	BROADCAST_DESTRUCTION_GENERIC_TO_SYSTEM,
	/// Received destruction message, relaying to other systems
	RELAY_DESTRUCTION_TO_SYSTEMS,

	/// Default reason to send a construction command
	SEND_CONSTRUCTION_GENERIC_TO_SYSTEM,
	/// Triggered by ReplicaManager2::RecalculateVisibility - A replica is now always constructed, so needs to be created
	SEND_CONSTRUCTION_VISIBILITY_RECALCULATION_TO_SYSTEM,
	/// Triggered by Replica2::BroadcastConstruction()
	BROADCAST_CONSTRUCTION_GENERIC_TO_SYSTEM,
	/// Replica2::QueryIsConstructionAuthority()==false yet we called ReplicaManager2::SendConstruction()
	SEND_CONSTRUCTION_REQUEST_TO_SERVER,
	/// A non-authority object was created by a client, accepted, and is now relayed to all other connected systems
	BROADCAST_CONSTRUCTION_REQUEST_ACCEPTED_TO_SYSTEM,
	/// A non-authority object was created by a client, accepted
	SEND_CONSTRUCTION_REPLY_ACCEPTED_TO_CLIENT,
	/// A non-authority object was created by a client, denied
	SEND_CONSTRUCTION_REPLY_DENIED_TO_CLIENT,

	/// An object is visible
	SEND_VISIBILITY_TRUE_TO_SYSTEM,
	/// An object is visible
	BROADCAST_VISIBILITY_TRUE_TO_SYSTEM,
	/// An object is not visible
	SEND_VISIBILITY_FALSE_TO_SYSTEM,
	/// An object is not visible
	BROADCAST_VISIBILITY_FALSE_TO_SYSTEM,
	/// An object is visible, and we are telling other systems about this
	RELAY_VISIBILITY_TRUE_TO_SYSTEMS,
	/// An object is visible, and we are telling other systems about this
	RELAY_VISIBILITY_FALSE_TO_SYSTEMS,

	/// Calling Replica2::Serialize() for the purpose of reading memory to compare against later. This read will not be transmitted
	AUTOSERIALIZE_RESYNCH_ONLY,
	/// Calling Replica2::Serialize() to compare against a prior call. The serialization may be transmitted
	AUTOSERIALIZE_DEFAULT,

	/// Start your own reasons one unit past this enum
	UNDEFINED_REASON,
};

/// \brief A management system for your game objects and players to make serialization, scoping, and object creation and destruction easier.
///
/// Quick start:
/// 1. Create a class that derives from Connection_RM2, implementing the Construct() function. Construct() is a factory function that should return instances of your game objects, given a user-defined identifier.
/// 2. Create a class that derives from Connection_RM2Factory, implementing AllocConnection() and DeallocConnection() to return instances of the class from step 1.
/// 3. Attach ReplicaManager2 as a plugin
/// 4. Call ReplicaManager2::SetConnectionFactory with an instance of the class from step 2.
/// 5. For each of your game classes that use this system, derive from Replica2 and implement SerializeConstruction(), Serialize(), Deserialize(). The output of SerializeConstruction() is sent to Connection_RM2::Construct()
/// 6. When these classes are allocated, call Replica2::SetReplicaManager() with the instance of ReplicaManager2 class created in step 3 (this could be done automatically in the constructor)
/// 7. Creation: Use Replica2::SendConstruction() to create the object remotely, Replica2::SendDestruction() to delete the object remotely.
/// 8. Scoping: Override Replica2::QueryVisibility() and Replica2::QueryConstruction() to return BQR_YES or BQR_NO if an object should be visible and in scope to a given connection. Defaults to BQR_ALWAYS
/// 9. Automatic serialization: Call Replica2::AddAutoSerializeTimer() to automatically call Replica2::Serialize() at intervals, compare this to the last value, and broadcast out the object when the serialized variables change.
///
/// \pre Call RakPeer::SetNetworkIDManager()
/// \pre This system is a server or peer: Call NetworkIDManager::SetIsNetworkIDAuthority(true).
/// \pre This system is a client:  Call NetworkIDManager::SetIsNetworkIDAuthority(false).
/// \pre If peer to peer, NETWORK_ID_SUPPORTS_PEER_TO_PEER should be defined in RakNetDefines.h
/// \ingroup REPLICA_MANAGER_2_GROUP
class RAK_DLL_EXPORT ReplicaManager2 : public PluginInterface2
{
public:
	/// Constructor
	ReplicaManager2();

	/// Destructor
	virtual ~ReplicaManager2();

	/// Sets the factory class used to allocate connection objects
	/// \param[in] factory A pointer to an instance of a class that derives from Connection_RM2Factory. This pointer it saved and not copied, so the object should remain in memory.
	void SetConnectionFactory(Connection_RM2Factory *factory);

	/// \param[in] Default ordering channel to use when passing -1 to a function that takes orderingChannel as a parameter
	void SetDefaultOrderingChannel(char def);

	/// \param[in] Default packet priority to use when passing NUMBER_OF_PRIORITIES to a function that takes priority as a parameter
	void SetDefaultPacketPriority(PacketPriority def);

	/// \param[in] Default packet reliability to use when passing NUMBER_OF_RELIABILITIES to a function that takes reliability as a parameter
	void SetDefaultPacketReliability(PacketReliability def);

	/// Auto scope will track the prior construction and serialization visibility of each registered Replica2 class, for each connection.
	/// Per-tick, as the visibility or construction status of a replica changes, it will be constructed, destroyed, or the visibility will change as appropriate.
	/// \param[in] construction If true, Connection_RM2::SetConstructionByReplicaQuery will be called once per PluginInterface::Update tick. This will call Replica2::QueryConstruction to return if an object should be exist on a particular connection
	/// \param[in] visibility If true, Connection_RM2::SetConstructionSerializationByReplicaQuery will be called once per PluginInterface::Update tick. This will call Replica2::QuerySerialization to return if an object should be visible to a particular connection or not.
	void SetAutoUpdateScope(bool construction, bool visibility);

	/// Autoadd will cause a Connection_RM2 instance to be allocated for every connection.
	/// Defaults to true. Set this to false if you have connections which do not participate in the game (master server, etc).
	/// \param[in] autoAdd If true, all incoming connections are added as ReplicaManager2 connections.
	void SetAutoAddNewConnections(bool autoAdd);

	/// If SetAutoAddNewConnections() is false, you need to add connections manually
	/// connections are also created implicitly if needed
	/// \param[in] systemAddress The address of the new system
	/// \return false if the connection already exists
	bool AddNewConnection(SystemAddress systemAddress);

	/// Remove an existing connection. Also done automatically on ID_DISCONNECTION_NOTIFICATION and ID_CONNECTION_LOST
	/// \param[in] systemAddress The address of the system to remove the connection for
	/// \return false if the connection does not exist
	bool RemoveConnection(SystemAddress systemAddress);

	/// Is this connection registered with the system?
	/// \param[in] systemAddress The address of the system to check
	/// \return true if this address is registered, false otherwise
	bool HasConnection(SystemAddress systemAddress);

	/// If true, autoserialize timers added with Replica2::AddAutoSerializeTimer() will automatically decrement.
	/// If false, you must call Replica2::ElapseAutoSerializeTimers() manually.
	/// Defaults to true
	/// \param[in] autoUpdate True to automatically call ElapseAutoSerializeTimers(). Set to false if you need to control these timers.
	void SetDoReplicaAutoSerializeUpdate(bool autoUpdate);

	/// Sends a construction command to one or more systems, which will be relayed throughout the network.
	/// Recipient(s) will allocate the connection via Connection_RM2Factory::AllocConnection() if it does not already exist.
	/// Will trigger a call on the remote system(s) to Connection_RM2::Construct()
	/// \note If using peer-to-peer, NETWORK_ID_SUPPORTS_PEER_TO_PEER should be defined in RakNetDefines.h.
	/// \note This is a low level function. Beginners may wish to use Replica2::SendConstruction() or Replica2::BroadcastConstruction(). You can also override Replica2::QueryConstruction()
	/// \param[in] replica The class to construct remotely
	/// \param[in] replicaData User-defined serialized data representing how to construct the class. Could be the name of the class, a unique identifier, or other methods
	/// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. Connection_RM2Factory::AllocConnection will be called if this connection has not been previously used.
	/// \param[in] timestamp Timestamp to send with the message. Use 0 to not send a timestamp if you don't need it.
	/// \param[in] sendMessage True to actually send a network message. False to only register that the object exists on the remote system, useful for objects created outside ReplicaManager2, or objects that already existed in the world.
	/// \param[in] exclusionList Which systems to not send to. This list is carried with the messsage, and appended to at each node in the connection graph. This is used to prevent infinite cyclical sends.
	/// \param[in] localClientId If replica->QueryIsConstructionAuthority()==false, this number will be sent with SEND_CONSTRUCTION_REQUEST_TO_SERVER to the \a recipient. SEND_CONSTRUCTION_REPLY_ACCEPTED_TO_CLIENT or SEND_CONSTRUCTION_REPLY_DENIED_TO_CLIENT will be returned, and this number will be used to look up the local object in Replica2::clientPtrArray
	/// \param[in] type What kind of serialization operation this is. Use one of the pre-defined types, or create your own. This will be returned in \a type in Connection_RM2::Construct()
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by SetDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by SetDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by SetDefaultOrderingChannel()
	/// \pre Call SetConnectionFactory() with a derived instance of Connection_RM2Factory.
	void SendConstruction(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp, bool sendMessage,
		DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
		unsigned char localClientId, SerializationType type=SEND_CONSTRUCTION_GENERIC_TO_SYSTEM, 
		PacketPriority priority=NUMBER_OF_PRIORITIES, PacketReliability reliability=NUMBER_OF_RELIABILITIES, char orderingChannel=-1);

	/// Sends a destruction command to one or more systems, which will be relayed throughout the network.
	/// Recipient(s) will allocate the connection via Connection_RM2Factory::AllocConnection() if it does not already exist.
	/// Will trigger a call on the remote system(s) to Replica2::ReceiveDestruction() which in turn calls Replica2::DeserializeDestruction() with the value passed in \a replicaData
	/// Note: This is a low level function. Beginners may wish to use Replica2::SendDestruction() or Replica2::BroadcastDestruction().
	/// \param[in] replica The class to destroy remotely
	/// \param[in] replicaData User-defined serialized data. Passed to Replica2::ReceiveDestruction()
	/// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. Connection_RM2Factory::AllocConnection will be called if this connection has not been previously used.
	/// \param[in] timestamp Timestamp to send with the message. Use 0 to not send a timestamp if you don't need it.
	/// \param[in] sendMessage True to actually send a network message. False to only register that the object no longer exists on the remote system.
	/// \param[in] exclusionList Which systems to not send to. This list is carried with the messsage, and appended to at each node in the connection graph. This is used to prevent infinite cyclical sends.
	/// \param[in] type What kind of serialization operation this is. Use one of the pre-defined types, or create your own. This will be returned in \a type in Connection_RM2::Construct()
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by SetDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by SetDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by SetDefaultOrderingChannel()
	/// \pre Replica::QueryIsDestructionAuthority() must return true
	/// \pre Call SetConnectionFactory() with a derived instance of Connection_RM2Factory.
	void SendDestruction(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp, bool sendMessage,
		DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
		SerializationType type=SEND_DESTRUCTION_GENERIC_TO_SYSTEM,
		PacketPriority priority=NUMBER_OF_PRIORITIES, PacketReliability reliability=NUMBER_OF_RELIABILITIES, char orderingChannel=-1);

	/// Sends a serialized object to one or more systems, which will be relayed throughout the network.
	/// Recipient(s) will allocate the connection via Connection_RM2Factory::AllocConnection() if it does not already exist.
	/// Will trigger a call on the remote system(s) to Replica2::ReceiveSerialization() which in turn calls Replica2::Deserialize() with the value passed in \a replicaData
	/// Note: This is a low level function. Beginners may wish to use Replica2::SendSerialize() or Replica2::BroadcastSerialize().
	/// \param[in] replica The class to serialize
	/// \param[in] replicaData User-defined serialized data. Passed to Replica2::ReceiveSerialization()
	/// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. Connection_RM2Factory::AllocConnection will be called if this connection has not been previously used.
	/// \param[in] timestamp Timestamp to send with the message. Use 0 to not send a timestamp if you don't need it.
	/// \param[in] exclusionList Which systems to not send to. This list is carried with the messsage, and appended to at each node in the connection graph. This is used to prevent infinite cyclical sends.
	/// \param[in] type What kind of serialization operation this is. Use one of the pre-defined types, or create your own. This will be returned in \a type in Connection_RM2::Construct()
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by SetDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by SetDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by SetDefaultOrderingChannel()
	/// \pre Replica::QueryIsSerializationAuthority() must return true
	/// \pre Call SetConnectionFactory() with a derived instance of Connection_RM2Factory.
	void SendSerialize(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp,
		DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
		SerializationType type=SEND_SERIALIZATION_GENERIC_TO_SYSTEM, 
		PacketPriority priority=NUMBER_OF_PRIORITIES, PacketReliability reliability=NUMBER_OF_RELIABILITIES, char orderingChannel=-1);

	/// Sets the visibility status of an object. which will be relayed throughout the network.
	/// Objects that are not visible should be hidden in the game world, and will not send AutoSerialize updates
	/// Recipient(s) will allocate the connection via Connection_RM2Factory::AllocConnection() if it does not already exist.
	/// Will trigger a call on the remote system(s) to Connection_RM2::ReceiveVisibility()
	/// Note: This is a low level function. Beginners may wish to use Connection_RM2::SendVisibility() or override Replica2::QueryVisibility()
	/// \param[in] objectList The objects to send to the system.
	/// \param[in] replicaData User-defined serialized data. Read in Connection_RM2::ReceiveVisibility()
	/// \param[in] recipient Which system to send to. Use UNASSIGNED_SYSTEM_ADDRESS to send to all previously created connections. Connection_RM2Factory::AllocConnection will be called if this connection has not been previously used.
	/// \param[in] timestamp Timestamp to send with the message. Use 0 to not send a timestamp if you don't need it.
	/// \param[in] sendMessage True to actually send a network message. False to only register that the objects exist on the remote system
	/// \param[in] type What kind of serialization operation this is. Use one of the pre-defined types, or create your own. This will be returned in \a type in Connection_RM2::Construct()
	/// \param[in] priority PacketPriority to send with. Use NUMBER_OF_PRIORITIES to use the default defined by SetDefaultPacketPriority().
	/// \param[in] reliability PacketReliability to send with. Use NUMBER_OF_RELIABILITIES to use the default defined by SetDefaultPacketReliability();
	/// \param[in] orderingChannel ordering channel to send on. Use -1 to use the default defined by SetDefaultOrderingChannel()
	/// \pre Replica::QueryIsConstructionAuthority() must return true
	/// \pre Call SetConnectionFactory() with a derived instance of Connection_RM2Factory.
	void SendVisibility(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp,
		DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
		SerializationType type=SEND_VISIBILITY_TRUE_TO_SYSTEM, 
		PacketPriority priority=NUMBER_OF_PRIORITIES, PacketReliability reliability=NUMBER_OF_RELIABILITIES, char orderingChannel=-1);

	/// Returns how many Replica2 instances are registered.
	/// Replica2 instances are automatically registered when used, and unregistered when calling Deref (which is automatically done in the destructor).
	/// Used for GetReplicaAtIndex if you want to perform some object on all registered Replica objects.
	/// \return How many replica objects are in the list of replica objects
	unsigned GetReplicaCount(void) const;

	/// Returns a previously registered Replica2 *, from index 0 to GetReplicaCount()-1.
	/// Replica2* objects are returned in the order they were registered.
	/// \param[in] index An index, from 0 to GetReplicaCount()-1.
	/// \return A Replica2 pointer
	Replica2 *GetReplicaAtIndex(unsigned index);

	/// Returns the number of registered connections.
	/// Connections are registered implicitly when used.
	/// Connections are unregistered on disconnect.
	/// \return The number of registered connections
	unsigned GetConnectionCount(void) const;

	/// Returns a connection pointer previously implicitly added.
	/// \param[in] index An index, from 0 to GetConnectionCount()-1.
	/// \return A Connection_RM2 pointer
	Connection_RM2* GetConnectionAtIndex(unsigned index) const;

	/// Returns a connection pointer previously implicitly added.
	/// \param[in] systemAddress The system address of the connection to return
	/// \return A Connection_RM2 pointer
	Connection_RM2* GetConnectionBySystemAddress(SystemAddress systemAddress) const;

	/// Returns the index of a connection, by SystemAddress
	/// \param[in] systemAddress The system address of the connection index to return
	/// \return The connection index, or -1 if no such connection
	unsigned int GetConnectionIndexBySystemAddress(SystemAddress systemAddress) const;

	/// Call this when Replica2::QueryVisibility() or Replica2::QueryConstructionVisibility() changes from BQR_ALWAYS or BQR_NEVER to BQR_YES or BQR_NO
	/// Otherwise these two conditions are assumed to never change
	/// \param[in] Which replica to update
	void RecalculateVisibility(Replica2 *replica);

	/// \internal
	static int Replica2ObjectComp( RakNet::Replica2 * const &key, RakNet::Replica2 * const &data );
	/// \internal
	static int Replica2CompByNetworkID( const NetworkID &key, RakNet::Replica2 * const &data );
	/// \internal
	static int Connection_RM2CompBySystemAddress( const SystemAddress &key, RakNet::Connection_RM2 * const &data );

	/// Given a replica instance, return all connections that are believed to have this replica instantiated.
	/// \param[in] replica Which replica is being refered to
	/// \param[out] output List of connections, ordered by system address
	void GetConnectionsWithReplicaConstructed(Replica2 *replica, DataStructures::OrderedList<SystemAddress, Connection_RM2*, ReplicaManager2::Connection_RM2CompBySystemAddress> &output);

	/// Given a replica instance, return all connections that are believed to have this replica visible
	/// \param[in] replica Which replica is being refered to
	/// \param[out] output List of connections, ordered by system address
	void GetConnectionsWithSerializeVisibility(Replica2 *replica, DataStructures::OrderedList<SystemAddress, Connection_RM2*, ReplicaManager2::Connection_RM2CompBySystemAddress> &output);

	/// Gets the instance of RakPeerInterface that this plugin was attached to
	/// \return The instance of RakPeerInterface that this plugin was attached to
	RakPeerInterface *GetRakPeer(void) const;

	/// Internally starts tracking this replica
	/// \internal
	void Reference(Replica2* replica, bool *newReference);

	/// Stops tracking this replica. Call before deleting the Replica. Done automatically in ~Replica()
	/// \internal
	void Dereference(Replica2 *replica);

protected:
	// Plugin interface functions
	void OnAttach(void);
	PluginReceiveResult OnReceive(Packet *packet);
	void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	void OnShutdown(void);
	void Update(void);
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);

	PluginReceiveResult OnDownloadComplete(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp);
	PluginReceiveResult OnDownloadStarted(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp);
	PluginReceiveResult OnConstruction(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp);
	PluginReceiveResult OnDestruction(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp);
	PluginReceiveResult OnVisibilityChange(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp);
	PluginReceiveResult OnSerialize(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp);

	bool AddToAndWriteExclusionList(SystemAddress recipient, RakNet::BitStream *bs, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList);
	void WriteExclusionList(RakNet::BitStream *bs, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList);
	void CullByAndAddToExclusionList(
		DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> &inputList,
		DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> &culledOutput,
		DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList);
	void ReadExclusionList(RakNet::BitStream *bs, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList);

	void Send(RakNet::BitStream *bs, SystemAddress recipient, PacketPriority priority, PacketReliability reliability, char orderingChannel);
	void Clear(void);
	void DownloadToNewConnection(Connection_RM2* connection, RakNetTime timestamp, PacketPriority priority, PacketReliability reliability, char orderingChannel);

	Connection_RM2* CreateConnectionIfDoesNotExist(SystemAddress systemAddress, bool *newConnection);
	Connection_RM2* AutoCreateConnection(SystemAddress systemAddress, bool *newConnection);
	void AddConstructionReference(Connection_RM2* connection, Replica2* replica);
	void AddVisibilityReference(Connection_RM2* connection, Replica2* replica);
	void RemoveVisibilityReference(Connection_RM2* connection, Replica2* replica);
	void WriteHeader(RakNet::BitStream *bs, MessageID type, RakNetTime timestamp);

	friend class Connection_RM2;
	friend class Replica2;

	Connection_RM2Factory *connectionFactoryInterface;
	bool autoUpdateConstruction, autoUpdateVisibility;

	char defaultOrderingChannel;
	PacketPriority defaultPacketPriority;
	PacketReliability defaultPacketReliablity;
	bool autoAddNewConnections;
	bool doReplicaAutoUpdate;
	RakNetTime lastUpdateTime;

	DataStructures::List<Replica2*> fullReplicaUnorderedList;
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> fullReplicaOrderedList;
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> alwaysDoConstructReplicaOrderedList;
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> alwaysDoSerializeReplicaOrderedList;
	// Should only be in this list if QueryIsServer() is true
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> variableConstructReplicaOrderedList;
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> variableSerializeReplicaOrderedList;

	DataStructures::OrderedList<SystemAddress, Connection_RM2*, ReplicaManager2::Connection_RM2CompBySystemAddress> connectionList;
};

/// \brief The result of various scope and construction queries
/// \ingroup REPLICA_MANAGER_2_GROUP
enum RAK_DLL_EXPORT BooleanQueryResult
{
	/// The query is always true, for all systems. Certain optimizations are performed here, but you should not later return any other value without first calling ReplicaManager2::RecalculateVisibility
	BQR_ALWAYS,

	/// True
	BQR_YES,

	/// False
	BQR_NO,

	/// The query is never true, for all systems. Certain optimizations are performed here, but you should not later return any other value without first calling ReplicaManager2::RecalculateVisibility
	BQR_NEVER
};

/// \brief Contextual information about serialization, passed to some functions in Replica2
/// \ingroup REPLICA_MANAGER_2_GROUP
struct RAK_DLL_EXPORT SerializationContext
{
	SerializationContext() {}
	~SerializationContext() {}
	SerializationContext(SerializationType st, SystemAddress relay, SystemAddress recipient, RakNetTime _timestamp) {serializationType=st; relaySourceAddress=relay; recipientAddress=recipient; timestamp=_timestamp;}

	/// The system that sent the message to us.
	SystemAddress relaySourceAddress;

	/// The system that we are sending to.
	SystemAddress recipientAddress;

	/// Timestamp to send with the message. 0 means undefined. Set to non-zero to actually transmit using ID_TIMESTAMP
	RakNetTime timestamp;

	/// What type of serialization was performed
	SerializationType serializationType;

	/// General category of serialization
	static bool IsSerializationCommand(SerializationType r);
	/// General category of serialization
	static bool IsDownloadCommand(SerializationType r);
	/// General category of serialization
	static bool IsDestructionCommand(SerializationType r);
	/// General category of serialization
	static bool IsConstructionCommand(SerializationType r);
	/// General category of serialization
	static bool IsVisibilityCommand(SerializationType r);
	/// General category of serialization
	static bool IsVisible(SerializationType r);
};


/// \brief Base class for game objects that use the ReplicaManager2 system
/// All game objects that want to use the ReplicaManager2 functionality must inherit from Replica2.
/// Generally you will want to implement at a minimum Serialize(), Deserialize(), and SerializeConstruction()
/// \ingroup REPLICA_MANAGER_2_GROUP
class RAK_DLL_EXPORT Replica2 : public NetworkIDObject
{
public:
	/// Constructor
	Replica2();

	/// Destructor
	virtual ~Replica2();

	/// Sets the replica manager to use with this Replica.
	/// Will also set the NetworkIDManager associated with RakPeerInterface::SetNetworkIDManager()
	/// Call this before using any of the functions below!
	/// \param[in] rm A pointer to your instance of ReplicaManager 2
	void SetReplicaManager(ReplicaManager2* rm);

	/// Returns what was passed to SetReplicaManager(), or 0 if no value ever passed
	/// \return Registered instance of ReplicaManager2
	ReplicaManager2* GetReplicaManager(void) const;

	/// Construct this object on other systems
	/// Triggers a call to SerializeConstruction()
	/// \note If using peer-to-peer, NETWORK_ID_SUPPORTS_PEER_TO_PEER should be defined in RakNetDefines.h
	/// \param[in] recipientAddress Which system to send to
	/// \param[in] serializationType What type of command this is. Use UNDEFINED_REASON to have a type chosen automatically
	virtual void SendConstruction(SystemAddress recipientAddress, SerializationType serializationType=UNDEFINED_REASON);

	/// Destroy this object on other systems
	/// Triggers a call to SerializeDestruction()
	/// \param[in] recipientAddress Which system to send to
	/// \param[in] serializationType What type of command this is. Use UNDEFINED_REASON to have a type chosen automatically
	virtual void SendDestruction(SystemAddress recipientAddress, SerializationType serializationType=UNDEFINED_REASON);

	/// Serialize this object to another system
	/// Triggers a call to Serialize()
	/// \param[in] recipientAddress Which system to send to
	/// \param[in] serializationType What type of command this is. Use UNDEFINED_REASON to have a type chosen automatically
	virtual void SendSerialize(SystemAddress recipientAddress, SerializationType serializationType=UNDEFINED_REASON);

	/// Update the visibility of this object on another system
	/// Triggers a call to SerializeVisibility()
	/// \param[in] recipientAddress Which system to send to
	/// \param[in] serializationType What type of command this is. Use UNDEFINED_REASON to have a type chosen automatically
	virtual void SendVisibility(SystemAddress recipientAddress, SerializationType serializationType=UNDEFINED_REASON);

	/// Construct this object on other systems
	/// \param[in] serializationContext Which system to send to, an input timestamp, and the SerializationType. 0 to use defaults, no timestamp.
	virtual void BroadcastConstruction(SerializationContext *serializationContext=0);

	/// Serialize this object to all current connections
	/// Triggers a call to SerializeConstruction() for each connection (you can serialize differently per connection).
	/// \param[in] serializationContext Which system to send to, an input timestamp, and the SerializationType. 0 to use defaults, no timestamp.
	virtual void BroadcastSerialize(SerializationContext *serializationContext=0);

	/// Destroy this object on all current connections
	/// Triggers a call to SerializeDestruction() for each connection (you can serialize differently per connection).
	/// \param[in] serializationContext Which system to send to, an input timestamp, and the SerializationType. 0 to use defaults, no timestamp.
	virtual void BroadcastDestruction(SerializationContext *serializationContext=0);

	/// Update the visibility state of this object on all other systems
	/// Use SEND_VISIBILITY_TRUE_TO_SYSTEM or SEND_VISIBILITY_FALSE_TO_SYSTEM in \a serializationContext::serializationType
	/// Triggers a call to SerializeVisibility() for each connection (you can serialize differently per connection).
	/// \param[in] serializationContext Which system to send to, an input timestamp, and the SerializationType. 0 to use defaults, no timestamp, true visibility
	virtual void BroadcastVisibility(bool isVisible, SerializationContext *serializationContext=0);

	/// CALLBACK:
	/// Override in order to write to \a bitStream data identifying this class for the class factory. Will be received by Connection_RM2::Construct() to create an instance of this class.
	/// \param[out] bitStream Data used to identify this class, along with any data you also want to send when constructing the class
	/// \param[in] serializationContext serializationType passed to Replica2::SendConstruction(), along with destination system, and a timestamp you can write to.
	/// \return Return false to cancel the construction, true to process
	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)=0;

	/// CALLBACK:
	/// Override in order to write to \a bitStream data to send along with destruction requests. Will be received by DeserializeDestruction()
	/// \param[out] bitStream Data to send
	/// \param[in] serializationContext Describes which system we are sending to, and a timestamp as an out parameter
	/// \return Return false to cancel the operation, true to process
	virtual bool SerializeDestruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);

	/// CALLBACK:
	/// Override in order to write to \a bitStream data to send as regular class serialization, for normal per-tick data. Will be received by Deserialize()
	/// \param[out] bitStream Data to send
	/// \param[in] serializationContext Describes which system we are sending to, and a timestamp as an out parameter
	/// \return Return false to cancel the operation, true to process
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);

	/// CALLBACK:
	/// Override in order to write to \a bitStream data to send along with visibility changes. Will be received by DeserializeVisibility()
	/// \param[out] bitStream Data to send
	/// \param[in] serializationContext Describes which system we are sending to, and a timestamp as an out parameter
	/// \return Return false to cancel the operation, true to process
	virtual bool SerializeVisibility(RakNet::BitStream *bitStream, SerializationContext *serializationContext);

	/// CALLBACK:
	/// Receives data written by SerializeDestruction()
	/// \param[in] bitStream Data sent
	/// \param[in] serializationType SerializationContext::serializationType
	/// \param[in] sender Which system sent this message to us
	/// \param[in] timestamp If a timestamp was written, will be whatever was written adjusted to the local system time. 0 if not used.
	virtual void DeserializeDestruction(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	/// CALLBACK:
	/// Receives data written by Serialize()
	/// \param[in] bitStream Data sent
	/// \param[in] serializationType SerializationContext::serializationType
	/// \param[in] sender Which system sent this message to us
	/// \param[in] timestamp If a timestamp was written, will be whatever was written adjusted to the local system time. 0 if not used.
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	/// CALLBACK:
	/// Receives data written by SerializeVisibility()
	/// \param[in] bitStream Data sent
	/// \param[in] serializationType SerializationContext::serializationType
	/// \param[in] sender Which system sent this message to us
	/// \param[in] timestamp If a timestamp was written, will be whatever was written adjusted to the local system time. 0 if not used.
	virtual void DeserializeVisibility(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	/// CALLBACK:
	/// For a given connection, should this object exist?
	/// Checked every Update cycle if ReplicaManager2::SetAutoUpdateScope() parameter \a construction is true
	/// Defaults to BQR_ALWAYS
	/// \note This query is NOT used for ReplicaManager2::BroadcastConstruction() or SendConstruction(), which forces the operation to occur. If you DO want to use the query, use ReplicaManager2::Reference() and the next time RakPeer::Receive() is called it will occur.
	/// \param[in] connection Which connection we are referring to. 0 means unknown, in which case the system is checking for BQR_ALWAYS or BQR_NEVER as an optimization.
	/// \return BQR_NO and the object will be destroyed. BQR_YES and the object will be created. BQR_ALWAYS is YES for all connections, and is optimized to only be checked once.
	virtual BooleanQueryResult QueryConstruction(Connection_RM2 *connection);

	/// CALLBACK:
	/// For a given connection, should this object be visible (updatable?)
	/// Checked every Update cycle if ReplicaManager2::SetAutoUpdateScope() parameter \a serializationVisiblity is true
	/// Defaults to BQR_ALWAYS
	/// \note This query is NOT used for ReplicaManager2::BroadcastVisibility() or SendVisibility(), which forces the operation to occur. If you DO want to use the query, use ReplicaManager2::Reference() and the next time RakPeer::Receive() is called it will occur.
	/// \param[in] connection Which connection we are referring to. 0 means unknown, in which case the system is checking for BQR_ALWAYS or BQR_NEVER as an optimization.
	/// \return BQR_NO or BQR_YES and as this value changes per connection, you will get a call to DeserializeVisibility().
	virtual BooleanQueryResult QueryVisibility(Connection_RM2 *connection);

	/// CALLBACK:
	/// Does this system have control over construction of this object?
	/// While not strictly required, it is best to have this consistently return true for only one system. Otherwise systems may fight and override each other.
	/// Defaults to NetworkIDManager::IsNetworkIDAuthority();
	/// \return True if an authority over this operation, for this object instance
	virtual bool QueryIsConstructionAuthority(void) const;

	/// CALLBACK:
	/// Does this system have control over deletion of this object?
	/// While not strictly required, it is best to have this consistently return true for only one system. Otherwise systems may fight and override each other.
	/// Defaults to NetworkIDManager::IsNetworkIDAuthority();
	/// \return True if an authority over this operation, for this object instance
	virtual bool QueryIsDestructionAuthority(void) const;

	/// CALLBACK:
	/// Does this system have control over visibility of this object?
	/// While not strictly required, it is best to have this consistently return true for only one system. Otherwise systems may fight and override each other.
	/// Defaults to NetworkIDManager::IsNetworkIDAuthority();
	/// \return True if an authority over this operation, for this object instance
	virtual bool QueryIsVisibilityAuthority(void) const;

	/// CALLBACK:
	/// Does this system have control over serialization of object members of this object?
	/// It is reasonable to have this be true for more than one system, but you would want to serialize different variables so those systems do not conflict.
	/// Defaults to NetworkIDManager::IsNetworkIDAuthority();
	/// \return True if an authority over this operation, for this object instance
	virtual bool QueryIsSerializationAuthority(void) const;

	/// CALLBACK:
	/// If QueryIsConstructionAuthority() is false for a remote system, should that system be able to create this kind of object?
	/// \param[in] sender Which system sent this message to us? Also happens to be the system that is requesting to create an object
	/// \param[in] replicaData Construction data used to create this object
	/// \param[in] type Which type of serialization operation was performed
	/// \param[in] timestamp Written timestamp with the packet. 0 if not used.
	/// \return True to allow remote construction of this object. If true, we will reply with SEND_CONSTRUCTION_REPLY_ACCEPTED_TO_CLIENT and the network ID will be set on the requester.
	virtual bool AllowRemoteConstruction(SystemAddress sender, RakNet::BitStream *replicaData, SerializationType type, RakNetTime timestamp);

	/// Adds a timer that will elapse every \a countdown milliseconds, calling Serialize with AUTOSERIALIZE_DEFAULT or whatever value was passed to \a serializationType
	/// Every time this timer elapses, the value returned from Serialize() will be compared to the last value returned by Serialize().
	/// If different, SendSerialize() will be called automatically.
	/// It is possible to create your own AUTOSERIALIZE enumerations and thus control which parts of the object is serialized
	/// Use CancelAutoSerializeTimer() or ClearAutoSerializeTimers() to stop the timer.
	/// If this timer already exists, it will simply override the existing countdown
	/// This timer will automatically repeat every \a countdown milliseconds
	/// \note The same data is sent to all participants when the autoserialize timer completes. If the data sent depends on the system to be sent to, do not use autoserialize. This is an optimization to save memory.
	/// \param[in] interval Time in milliseconds between autoserialize ticks. Use 0 to process immediately, and every tick
	/// \param[in] serializationType User-defined identifier for what type of serialization operation to perform. Returned in Deserialize() as the \a serializationType parameter.
	/// \param[in] countdown Amount of time before doing the next autoserialize. Defaults to interval
	virtual void AddAutoSerializeTimer(RakNetTime interval, SerializationType serializationType=AUTOSERIALIZE_DEFAULT, RakNetTime countdown=(RakNetTime)-1 );

	/// Elapse time for all timers added with AddAutoSerializeTimer()
	/// Only necessary to call this if you call Replica2::SetDoReplicaAutoSerializeUpdate(false) (which defaults to true)
	/// \param[in] timeElapsed How many milliseconds have elapsed since the last call
	/// \param[in] resynchOnly True to only update what was considered the last send, without actually doing a send.
	virtual void ElapseAutoSerializeTimers(RakNetTime timeElapsed, bool resynchOnly);

	/// Returns how many milliseconds are remaining until the next autoserialize update
	/// \param[in] serializationType User-defined identifier for what type of serialization operation to perform. Returned in Deserialize() as the \a serializationType parameter.
	/// \return How many milliseconds are remaining until the next autoserialize update. Returns -1 if no such autoserialization timer is in place.
	RakNetTime GetTimeToNextAutoSerialize(SerializationType serializationType=AUTOSERIALIZE_DEFAULT);

	/// Do the actual send call when needed to support autoSerialize
	/// If you want to do different types of send calls (UNRELIABLE for example) override this function.
	/// \param[in] serializationContext Describes the recipient, sender. serializationContext::timestamp is an [out] parameter which if you write to, will be send along with the message
	/// \param[in] serializedObject Data to pass to ReplicaManager2::SendSerialize()
	virtual void BroadcastAutoSerialize(SerializationContext *serializationContext, RakNet::BitStream *serializedObject);

	/// Stop calling an autoSerialize timer previously setup with AddAutoSerializeTimer()
	/// \param[in] serializationType Corresponding value passed to serializationType
	virtual void CancelAutoSerializeTimer(SerializationType serializationType=AUTOSERIALIZE_DEFAULT);

	/// Remove and deallocate all previously added autoSerialize timers
	virtual void ClearAutoSerializeTimers(void);

	/// A timer has elapsed. Compare the last value sent to the current value, and if different, send the new value
	/// \internal
	virtual void OnAutoSerializeTimerElapsed(SerializationType serializationType, RakNet::BitStream *output, RakNet::BitStream *lastOutput, RakNetTime lastAutoSerializeCountdown, bool resynchOnly);

	/// Immediately elapse all autoserialize timers
	/// Used internally when a Deserialize() event occurs, so that the deserialize does not trigger an autoserialize itself
	/// \internal
	/// \param[in] resynchOnly If true, do not send a Serialize() message if the data has changed
	virtual void ForceElapseAllAutoserializeTimers(bool resynchOnly);

	/// A call to Connection_RM2 Construct() has completed and the object is now internally referenced
	/// \param[in] replicaData Whatever was written \a bitStream in Replica2::SerializeConstruction()
	/// \param[in] type Whatever was written \a serializationType in Replica2::SerializeConstruction()
	/// \param[in] replicaManager ReplicaManager2 instance that created this class.
	/// \param[in] timestamp timestamp sent with Replica2::SerializeConstruction(), 0 for none.
	/// \param[in] networkId NetworkID that will be assigned automatically to the new object after this function returns
	/// \param[in] networkIDCollision True if the network ID that should be assigned to this object is already in use. Usuallly this is because the object already exists, and you should just read your data and return 0.
	virtual void OnConstructionComplete(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision);

protected:

	virtual void ReceiveSerialize(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList );
	virtual void ReceiveDestruction(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList );
	virtual void DeleteOnReceiveDestruction(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList );
	virtual void ReceiveVisibility(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList);
	virtual Replica2 * ReceiveConstructionReply(SystemAddress sender, BitStream *replicaData, bool constructionAllowed);
	void DereferenceFromDestruction(void);

	friend class ReplicaManager2;
	friend class Connection_RM2;

	static unsigned char clientSharedID;
	static Replica2* clientPtrArray[256];

	bool hasClientID;
	unsigned char clientID;
	ReplicaManager2 *rm2;

	struct AutoSerializeEvent
	{
		SerializationType serializationType;
		RakNetTime initialCountdown;
		RakNetTime remainingCountdown;
		bool writeToResult1;
		RakNet::BitStream lastAutoSerializeResult1;
		RakNet::BitStream lastAutoSerializeResult2;
	};

	DataStructures::Map<SerializationType, AutoSerializeEvent*> autoSerializeTimers;

};

/// \brief Implement this factory class to return instances of your Connection_RM2 derived object. This is used as a class factory and exposes functionality related to the connection and the system
/// \ingroup REPLICA_MANAGER_2_GROUP
class RAK_DLL_EXPORT Connection_RM2Factory
{
public:
	Connection_RM2Factory() {}
	virtual ~Connection_RM2Factory() {}
	virtual Connection_RM2* AllocConnection(void) const=0;
	virtual void DeallocConnection(Connection_RM2* s) const=0;
};

/// \brief This class represents a connection between two instances of ReplicaManager2
/// Represents a connection. Allocated by user supplied factory interface Connection_RM2Factory
/// Implicitly created as needed
/// Generally you will want to implement at a minimum the Construct() function, used as a factory function to create your game objects
/// \ingroup REPLICA_MANAGER_2_GROUP
class RAK_DLL_EXPORT Connection_RM2
{
public:
	/// Constructor
	Connection_RM2();

	/// Destructor
	virtual ~Connection_RM2();

	/// Factory function, used to create instances of your game objects
	/// Encoding is entirely up to you. \a replicaData will hold whatever was written \a bitStream in Replica2::SerializeConstruction()
	/// One efficient way to do it is to use StringTable.h. This allows you to send predetermined strings over the network at a cost of 9 bits, up to 65536 strings
	/// \note The object is not yet referenced by ReplicaManager2 in this callback. Use Replica2::OnConstructionComplete() to perform functionality such as AutoSerialize()
	/// \param[in] replicaData Whatever was written \a bitStream in Replica2::SerializeConstruction()
	/// \param[in] type Whatever was written \a serializationType in Replica2::SerializeConstruction()
	/// \param[in] replicaManager ReplicaManager2 instance that created this class.
	/// \param[in] timestamp timestamp sent with Replica2::SerializeConstruction(), 0 for none.
	/// \param[in] networkId NetworkID that will be assigned automatically to the new object after this function returns
	/// \param[in] networkIDCollision True if the network ID that should be assigned to this object is already in use. Usuallly this is because the object already exists, and you should just read your data and return 0.
	/// \return Return 0 to signal that construction failed or was refused for this object. Otherwise return the object that was created. A reference will be held to this object, and SetNetworkID() and SetReplicaManager() will be called automatically.
	virtual Replica2* Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)=0;

	/// CALLBACK:
	/// Called before a download is sent to a new connection, called after ID_REPLICA_MANAGER_DOWNLOAD_STARTED is sent.
	/// Gives you control over the list of objects to be downloaded. For greater control, you can override ReplicaManager2::DownloadToNewConnection
	/// Defaults to send everything in the default order
	/// \param[in] fullReplicaUnorderedList The list of all known objects in the order they were originally known about by the system (the first time used by any function)
	/// \param[out] orderedDownloadList An empty list. Copy fullReplicaUnorderedList to this list to send everything. Leave elements out to not send them. Add them to the list in a different order to send them in that order.
	virtual void SortInitialDownload( const DataStructures::List<Replica2*> &orderedDownloadList, DataStructures::List<Replica2*> &initialDownloadList );


	/// CALLBACK:
	/// Called before a download is sent to a new connection
	/// \param[out] objectData What data you want to send to DeSerializeDownloadStarted()
	/// \param[in] replicaManager Which replica manager to use to perform the send
	/// \param[in/out] serializationContext Target recipient, optional timestamp, type of command
	virtual void SerializeDownloadStarted(RakNet::BitStream *objectData, ReplicaManager2 *replicaManager, SerializationContext *serializationContext);

	/// CALLBACK:
	/// Called after a download is sent to a new connection
	/// \param[out] objectData What data you want to send to DeSerializeDownloadComplete()
	/// \param[in] replicaManager Which replica manager to use to perform the send
	/// \param[in/out] serializationContext Target recipient, optional timestamp, type of command
	virtual void SerializeDownloadComplete(RakNet::BitStream *objectData, ReplicaManager2 *replicaManager, SerializationContext *serializationContext);

	/// CALLBACK:
	/// A new connection was added. All objects that are constructed and visible for this system will arrive immediately after this message.
	/// Write data to \a objectData by deriving from SerializeDownloadStarted()
	/// \note Only called if SetAutoUpdateScope is called with serializationVisiblity or construction true. (This is the default)
	/// \param[in] objectData objectData Data written through SerializeDownloadStarted()
	/// \param[in] replicaManager Which replica manager to use to perform the send
	/// \param[in] timestamp timestamp sent, 0 for none
	/// \param[in] serializationType Type of command
	virtual void DeserializeDownloadStarted(RakNet::BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType);

	/// CALLBACK:
	/// A new connection was added. All objects that are constructed and visible for this system have now arrived.
	/// Write data to \a objectData by deriving from SerializeDownloadComplete
	/// \note Only called if SetAutoUpdateScope is called with serializationVisiblity or construction true. (This is the default)
	/// \param[in] objectData objectData Data written through SerializeDownloadComplete()
	/// \param[in] replicaManager Which replica manager to use to perform the send
	/// \param[in] timestamp timestamp sent, 0 for none
	/// \param[in] serializationType Type of command
	virtual void DeserializeDownloadComplete(RakNet::BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType);

	/// Given a list of objects, compare it against lastConstructionList.
	/// BroadcastConstruct() is called for objects that only exist in the new list.
	/// BroadcastDestruct() is called for objects that only exist in the old list.
	/// This is used by SetConstructionByReplicaQuery() for all Replica2 that do not return BQR_ALWAYS from Replica2::QueryConstruction()
	/// If you want to pass your own, more efficient list to check against, call ReplicaManager2::SetAutoUpdateScope with construction=false and call this function yourself when desired
	/// \param[in] List of all objects that do not return BQR_ALWAYS from Replica2::QueryConstruction() that should currently be created on this system
	/// \param[in] replicaManager Which replica manager to use to perform the send
	virtual void SetConstructionByList(DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &currentVisibility, ReplicaManager2 *replicaManager);

	/// Given a list of objects, compare it against lastSerializationList.
	/// Replica2::BroadcastVisibility(true) is called for objects that only exist in the new list.
	/// Replica2::BroadcastVisibility(false) is called for objects that only exist in the old list.
	/// This is used by SetVisibilityByReplicaQuery() for all Replica2 that do not return BQR_ALWAYS from Replica2::QueryVisibility()
	/// If you want to pass your own, more efficient list to check against, call ReplicaManager2::SetAutoUpdateScope with construction=false and call this function yourself when desired
	/// \param[in] List of all objects that do not return BQR_ALWAYS from Replica2::QueryConstruction() that should currently be created on this system
	/// \param[in] replicaManager Which replica manager to use to perform the send
	virtual void SetVisibilityByList(DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &currentVisibility, ReplicaManager2 *replicaManager);

	/// Go through all registered Replica2 objects that do not return BQR_ALWAYS from Replica2::QueryConstruction()
	/// For each of these objects that return BQR_YES, pass them to currentVisibility in SetConstructionByList()
	/// Automatically called every tick if ReplicaManager2::SetAutoUpdateScope with construction=true is called (which is the default)
	/// \param[in] replicaManager Which replica manager to use to perform the send
	virtual void SetConstructionByReplicaQuery(ReplicaManager2 *replicaManager);

	/// Go through all registered Replica2 objects that do not return BQR_ALWAYS from Replica2::QueryVisibility()
	/// For each of these objects that return BQR_YES, pass them to currentVisibility in SetVisibilityByList()
	/// Automatically called every tick if ReplicaManager2::SetAutoUpdateScope with construction=true is called (which is the default)
	/// \param[in] replicaManager Which replica manager to use to perform the send
	virtual void SetVisibilityByReplicaQuery(ReplicaManager2 *replicaManager);

	/// Set the system address to use with this class instance. This is set internally when the object is created
	void SetSystemAddress(SystemAddress sa);

	/// Get the system address associated with this class instance.
	SystemAddress GetSystemAddress(void) const;

	/// Set the guid to use with this class instance. This is set internally when the object is created
	void SetGuid(RakNetGUID guid);

	/// Get the guid associated with this class instance.
	RakNetGUID GetGuid(void) const;
	
protected:
	void Deref(Replica2* replica);

	void CalculateListExclusivity(
		const DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &listOne,
		const DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &listTwo,
		DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &exclusiveToListOne, 
		DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &exclusiveToListTwo
		) const;

	virtual Replica2 * ReceiveConstruct(RakNet::BitStream *replicaData, NetworkID networkId, SystemAddress sender, unsigned char localClientId, SerializationType type,
		ReplicaManager2 *replicaManager, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList);

	friend class ReplicaManager2;

	// Address of this participant
	SystemAddress systemAddress;
	RakNetGUID rakNetGuid;

	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> lastConstructionList;
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> lastSerializationList;
};

}

#endif
