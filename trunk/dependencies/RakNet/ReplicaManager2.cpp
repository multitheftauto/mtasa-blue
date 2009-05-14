#include "ReplicaManager2.h"
#include "MessageIdentifiers.h"
#include "RakAssert.h"
#include "RakPeerInterface.h"
#include "NetworkIDManager.h"

using namespace RakNet;

unsigned char Replica2::clientSharedID=0;
Replica2* Replica2::clientPtrArray[256];


#ifdef _MSC_VER
#pragma warning( push )
#endif

bool SerializationContext::IsSerializationCommand(SerializationType r)
{
	return r>=SEND_SERIALIZATION_GENERIC_TO_SYSTEM && r<=RELAY_SERIALIZATION_TO_SYSTEMS;
}
bool SerializationContext::IsDownloadCommand(SerializationType r)
{
	return r>=SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM && r<=SEND_DATA_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM;
}
bool SerializationContext::IsDestructionCommand(SerializationType r)
{
	return r>=SEND_DESTRUCTION_GENERIC_TO_SYSTEM && r<=RELAY_DESTRUCTION_TO_SYSTEMS;
}
bool SerializationContext::IsConstructionCommand(SerializationType r)
{
	return r>=SEND_CONSTRUCTION_GENERIC_TO_SYSTEM && r<=SEND_CONSTRUCTION_REPLY_DENIED_TO_CLIENT;
}
bool SerializationContext::IsVisibilityCommand(SerializationType r)
{
	return r>=SEND_VISIBILITY_TRUE_TO_SYSTEM && r<=RELAY_VISIBILITY_FALSE_TO_SYSTEMS;
}
bool SerializationContext::IsVisible(SerializationType r)
{
	return r==SEND_VISIBILITY_TRUE_TO_SYSTEM || r==BROADCAST_VISIBILITY_TRUE_TO_SYSTEM || r==RELAY_VISIBILITY_TRUE_TO_SYSTEMS;
}

int ReplicaManager2::Replica2CompByNetworkID( const NetworkID &key, RakNet::Replica2 * const &data )
{
	if (key < data->GetNetworkID())
		return -1;
	if (key == data->GetNetworkID())
		return 0;
	return 1;
}

int ReplicaManager2::Replica2ObjectComp( RakNet::Replica2 * const &key, RakNet::Replica2 * const &data )
{
	if (key->GetAllocationNumber()<data->GetAllocationNumber())
		return -1;
	if (key->GetAllocationNumber()==data->GetAllocationNumber())
		return 0;
	return 1;
}

int ReplicaManager2::Connection_RM2CompBySystemAddress( const SystemAddress &key, RakNet::Connection_RM2 * const &data )
{
	if (key < data->GetSystemAddress())
		return -1;
	if (key == data->GetSystemAddress())
		return 0;
	return 1;
}

ReplicaManager2::ReplicaManager2()
{
	connectionFactoryInterface=0;
	defaultOrderingChannel=0;
	defaultPacketPriority=HIGH_PRIORITY;
	defaultPacketReliablity=RELIABLE_ORDERED;
	autoUpdateConstruction=true;
	autoUpdateVisibility=true;
	autoAddNewConnections=true;
	doReplicaAutoUpdate=true;
	DataStructures::OrderedList<SystemAddress,SystemAddress>::IMPLEMENT_DEFAULT_COMPARISON();
}
ReplicaManager2::~ReplicaManager2()
{
}
void ReplicaManager2::SendConstruction(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp, bool sendMessage,
						DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
						unsigned char localClientId, SerializationType type, 
						PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (replica==0)
		return;

	// Why would you request an object you already have?
	if (replica->GetNetworkID()!=UNASSIGNED_NETWORK_ID && replica->QueryIsConstructionAuthority()==false)
		return;

	if (recipient==UNASSIGNED_SYSTEM_ADDRESS && connectionList.Size()==0)
		return;

	RakAssert(replica->QueryConstruction(0)!=BQR_NEVER);

	bool newConnection;
	Connection_RM2* connection;

	// Add to list of replicas if not already there
	bool newReference;
	Reference(replica, &newReference);
	
	RakNet::BitStream bs;
	WriteHeader(&bs, ID_REPLICA_MANAGER_CONSTRUCTION, timestamp);
	bs.Write((unsigned char) type);
	bs.Write(replica->GetNetworkID());
	bs.Write(localClientId);
	
	if (recipient!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		connection = AutoCreateConnection(recipient, &newConnection);
		if (connection==0)
			return;

		if (newConnection)
		{
			// If a new connection, the replica was constructed automatically anyway
			DownloadToNewConnection(connection, timestamp, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);
			return;
		}

		if (AddToAndWriteExclusionList(recipient, &bs, exclusionList)==false)
			return;
		bs.AlignWriteToByteBoundary();
		bs.Write(replicaData);

		// Lookup connection by target. If does not exist, create
		
		AddConstructionReference(connection, replica);
		if (sendMessage)
		{
			// Send the packet
			Send(&bs, recipient, priority, reliability, orderingChannel);

			if (newReference && replica->QueryVisibility(connection)==BQR_ALWAYS)
			{
//				SerializationContext sc;
//				sc.recipientAddress=recipient;
//				sc.timestamp=timestamp;
//				sc.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
//				sc.serializationType=SEND_SERIALIZATION_GENERIC_TO_SYSTEM;
				replica->SendSerialize(recipient, SEND_SERIALIZATION_CONSTRUCTION_TO_SYSTEM);
			}
		}
	}
	else
	{
		DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> culledOutput;
		CullByAndAddToExclusionList(connectionList, culledOutput, exclusionList);
		WriteExclusionList(&bs, exclusionList);
		bs.AlignWriteToByteBoundary();
		bs.Write(replicaData);

		unsigned i;
		for (i=0; i < culledOutput.Size(); i++)
		{
			connection=culledOutput[i];
			AddConstructionReference(connection, replica);

			if (sendMessage)
				Send(&bs, connection->GetSystemAddress(), priority, reliability, orderingChannel);

			if (newReference && replica->QueryIsSerializationAuthority() && replica->QueryVisibility(connection)==BQR_ALWAYS)
			{
//				SerializationContext sc;
//				sc.recipientAddress=connection->GetSystemAddress();
//				sc.timestamp=0;
//				sc.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
//				sc.serializationType=BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM;
				replica->SendSerialize(connection->GetSystemAddress(), BROADCAST_SERIALIZATION_CONSTRUCTION_TO_SYSTEM);
			}
		}
	}	
}

void ReplicaManager2::SendDestruction(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp, bool sendMessage,
						DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
						SerializationType type, 
						PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (replica==0)
		return;

	if (recipient==UNASSIGNED_SYSTEM_ADDRESS && connectionList.Size()==0)
		return;

	if (replica->QueryIsDestructionAuthority()==false)
		return;

	if (recipient!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		bool newConnection;

		// Lookup connection by target. If does not exist, create
		Connection_RM2* connection = AutoCreateConnection(recipient, &newConnection);
		if (connection==0)
			return;

		// Have this system stop referencing the replica object
		connection->Deref(replica);

		if (newConnection)
		{
			// If a new connection, the object didn't exist anyway
			DownloadToNewConnection(connection, timestamp, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);
			return;
		}
	}

	if (sendMessage && replica->GetNetworkID()!=UNASSIGNED_NETWORK_ID)
	{
		// Send the packet
		RakNet::BitStream bs;
		WriteHeader(&bs, ID_REPLICA_MANAGER_DESTRUCTION, timestamp);
		bs.Write((unsigned char) type);
		bs.Write(replica->GetNetworkID());
		
		if (recipient!=UNASSIGNED_SYSTEM_ADDRESS)
		{
			if (AddToAndWriteExclusionList(recipient, &bs, exclusionList)==false)
				return;
			bs.AlignWriteToByteBoundary();
			bs.Write(replicaData);

			Send(&bs, recipient, priority, reliability, orderingChannel);
		}
		else
		{
			DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> output, culledOutput;
			GetConnectionsWithReplicaConstructed(replica, output);
			CullByAndAddToExclusionList(output, culledOutput, exclusionList);
			WriteExclusionList(&bs, exclusionList);
			bs.AlignWriteToByteBoundary();
			bs.Write(replicaData);

			unsigned i;
			for (i=0; i < output.Size(); i++)
				Send(&bs, output[i]->GetSystemAddress(), priority, reliability, orderingChannel);
		}
	}
}

void ReplicaManager2::SendSerialize(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp,
						DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
						SerializationType type, 
						 PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (replica==0)
		return;

	if (recipient==UNASSIGNED_SYSTEM_ADDRESS && connectionList.Size()==0)
		return;

	if (replica->GetNetworkID()==UNASSIGNED_NETWORK_ID)
		return;

	if (replica->QueryIsSerializationAuthority()==false)
		return;

	// Add to list of replicas if not already there
	bool newReference;
	Reference(replica, &newReference);

	if (newReference && replica->QueryConstruction(0)==BQR_ALWAYS)
	{
		replica->BroadcastConstruction();
	}

	bool newConnection;
	Connection_RM2* connection;

	RakNet::BitStream bs;
	WriteHeader(&bs, ID_REPLICA_MANAGER_SERIALIZE, timestamp);
	bs.Write((unsigned char) type);
	bs.Write(replica->GetNetworkID());

	if (recipient!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		connection = AutoCreateConnection(recipient, &newConnection);
		if (connection==0)
			return;
		if (newConnection)
			DownloadToNewConnection(connection, timestamp, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);

		if (AddToAndWriteExclusionList(recipient, &bs, exclusionList)==false)
			return;
		bs.AlignWriteToByteBoundary();
		bs.Write(replicaData);

		// Send the packet
		Send(&bs, recipient, priority, reliability, orderingChannel);
	}
	else
	{
		DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> output, culledOutput;
		GetConnectionsWithSerializeVisibility(replica, output);

		CullByAndAddToExclusionList(output, culledOutput, exclusionList);
		WriteExclusionList(&bs, exclusionList);
		bs.AlignWriteToByteBoundary();
		bs.Write(replicaData);

		unsigned i;
		for (i=0; i < culledOutput.Size(); i++)
		{
			connection=culledOutput[i];	
			Send(&bs, connection->GetSystemAddress(), priority, reliability, orderingChannel);
		}
	}	
}
void ReplicaManager2::SendVisibility(Replica2 *replica, BitStream *replicaData, SystemAddress recipient, RakNetTime timestamp,
							 DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList,
							 SerializationType type, 
							 PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (replica==0)
		return;

	bool newConnection;
	Connection_RM2* connection;

	if (recipient==UNASSIGNED_SYSTEM_ADDRESS && connectionList.Size()==0)
		return;

	if (replica->GetNetworkID()==UNASSIGNED_NETWORK_ID)
		return;

	// Add to list of replicas if not already there
	bool newReference;
	Reference(replica, &newReference);

	if (newReference && replica->QueryConstruction(0)==BQR_ALWAYS)
	{
		replica->BroadcastConstruction();
	}

	RakNet::BitStream bs;
	WriteHeader(&bs, ID_REPLICA_MANAGER_SCOPE_CHANGE, timestamp);
	bs.Write((unsigned char) type);
	bs.Write(replica->GetNetworkID());

	if (recipient!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		if (AddToAndWriteExclusionList(recipient, &bs, exclusionList)==false)
			return;

		connection = AutoCreateConnection(recipient, &newConnection);
		if (connection==0)
			return;
		if (newConnection)
			DownloadToNewConnection(connection, timestamp, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);

		bs.AlignWriteToByteBoundary();
		bs.Write(replicaData);

		if (SerializationContext::IsVisibilityCommand(type))
		{
			if (SerializationContext::IsVisible(type))
				AddVisibilityReference(connection, replica);
			else
				RemoveVisibilityReference(connection, replica);
		}
		// Send the packet
		Send(&bs, recipient, priority, reliability, orderingChannel);
	}
	else
	{
		DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> culledOutput;
		CullByAndAddToExclusionList(connectionList, culledOutput, exclusionList);
		WriteExclusionList(&bs, exclusionList);
		bs.AlignWriteToByteBoundary();
		bs.Write(replicaData);

		unsigned i;
		for (i=0; i < culledOutput.Size(); i++)
		{
			connection=culledOutput[i];
			if (SerializationContext::IsVisible(type))
				AddVisibilityReference(connection, replica);
			else
				RemoveVisibilityReference(connection, replica);
			Send(&bs, connection->GetSystemAddress(), priority, reliability, orderingChannel);
		}
	}	
}
void ReplicaManager2::Dereference(Replica2 *replica)
{
	unsigned i;
	if (replica==0)
		return;

	for (i=0; i < connectionList.Size(); i++)
	{
		connectionList[i]->lastConstructionList.RemoveIfExists(replica);
		connectionList[i]->lastSerializationList.RemoveIfExists(replica);
	}

	for (i=0; i < fullReplicaUnorderedList.Size(); i++)
	{
		if (fullReplicaUnorderedList[i]==replica)
		{
			fullReplicaUnorderedList.RemoveAtIndex(i);
			break;
		}
	}

	fullReplicaOrderedList.RemoveIfExists(replica);
	alwaysDoConstructReplicaOrderedList.RemoveIfExists(replica);
	alwaysDoSerializeReplicaOrderedList.RemoveIfExists(replica);
	variableConstructReplicaOrderedList.RemoveIfExists(replica);
	variableSerializeReplicaOrderedList.RemoveIfExists(replica);
}
void ReplicaManager2::Update(void)
{
	unsigned i;

	if (autoUpdateConstruction || autoUpdateVisibility)
	{
		for (i=0; i < connectionList.Size(); i++)
		{
			if (autoUpdateConstruction)
				connectionList[i]->SetConstructionByReplicaQuery(this);
			if (autoUpdateVisibility)
				connectionList[i]->SetVisibilityByReplicaQuery(this);
		}
	}

	if (doReplicaAutoUpdate)
	{
		RakNetTime currentTime = RakNet::GetTime();
		for (i=0; i < fullReplicaUnorderedList.Size(); i++)
		{
			fullReplicaUnorderedList[i]->ElapseAutoSerializeTimers(currentTime-lastUpdateTime,false);
		}
		lastUpdateTime=currentTime;
	}

}
unsigned ReplicaManager2::GetReplicaCount(void) const
{
	return fullReplicaUnorderedList.Size();
}
Replica2 *ReplicaManager2::GetReplicaAtIndex(unsigned index)
{
	return fullReplicaUnorderedList[index];
}
void ReplicaManager2::SetAutoAddNewConnections(bool autoDownload)
{
	autoAddNewConnections=autoDownload;
}
void ReplicaManager2::SetDoReplicaAutoSerializeUpdate(bool autoUpdate)
{
	doReplicaAutoUpdate=autoUpdate;
}
void ReplicaManager2::SetConnectionFactory(Connection_RM2Factory *factory)
{
	RakAssert(factory);
	connectionFactoryInterface=factory;
}
unsigned ReplicaManager2::GetConnectionCount(void) const
{
	return connectionList.Size();
}
Connection_RM2* ReplicaManager2::GetConnectionAtIndex(unsigned index) const
{
	return connectionList[index];
}
Connection_RM2* ReplicaManager2::GetConnectionBySystemAddress(SystemAddress systemAddress) const
{
	bool objectExists;
	unsigned index = connectionList.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists)
		return connectionList[index];
	return 0;
}
unsigned int ReplicaManager2::GetConnectionIndexBySystemAddress(SystemAddress systemAddress) const
{
	bool objectExists;
	unsigned index = connectionList.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists)
		return index;
	return (unsigned int) -1;
}
void ReplicaManager2::SetDefaultOrderingChannel(char def)
{
	defaultOrderingChannel=def;
}
void ReplicaManager2::SetDefaultPacketPriority(PacketPriority def)
{
	defaultPacketPriority=def;
}
void ReplicaManager2::SetDefaultPacketReliability(PacketReliability def)
{
	defaultPacketReliablity=def;
}
void ReplicaManager2::SetAutoUpdateScope(bool construction, bool visibility)
{
	autoUpdateConstruction=construction;
	autoUpdateVisibility=visibility;
}
void ReplicaManager2::RecalculateVisibility(Replica2 *replica)
{
	Dereference(replica);
	bool newReference;
	Reference(replica, &newReference);

	if (replica->QueryConstruction(0)==BQR_NEVER && autoUpdateConstruction)
	{
		// Destroy on all systems
		replica->SendDestruction(UNASSIGNED_SYSTEM_ADDRESS, SEND_DESTRUCTION_VISIBILITY_RECALCULATION_TO_SYSTEM);
	}
	if (replica->QueryConstruction(0)==BQR_ALWAYS && autoUpdateConstruction)
	{
		// Create on all systems
		replica->SendConstruction(UNASSIGNED_SYSTEM_ADDRESS, SEND_CONSTRUCTION_VISIBILITY_RECALCULATION_TO_SYSTEM);
	}
	if (replica->QueryVisibility(0)==BQR_ALWAYS && autoUpdateVisibility)
	{
		// Set visibility and creation on all systems
		replica->SendVisibility(UNASSIGNED_SYSTEM_ADDRESS, UNDEFINED_REASON);
		replica->SendSerialize(UNASSIGNED_SYSTEM_ADDRESS, UNDEFINED_REASON);
	}
}
void ReplicaManager2::GetConnectionsWithReplicaConstructed(Replica2 *replica, DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> &output)
{
	BooleanQueryResult res;
	res = replica->QueryConstruction(0);
	if (res==BQR_ALWAYS)
	{
		output=connectionList;
	}
	else if (res!=BQR_NEVER)
	{
		unsigned i;
		for (i=0; i < connectionList.Size(); i++)
		{
			if (connectionList[i]->lastConstructionList.HasData(replica))
				output.Insert(connectionList[i]->GetSystemAddress(),connectionList[i], false);
		}
	}
}
void ReplicaManager2::GetConnectionsWithSerializeVisibility(Replica2 *replica, DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> &output)
{
	BooleanQueryResult res;
	res = replica->QueryVisibility(0);
	if (res==BQR_ALWAYS)
	{
		GetConnectionsWithReplicaConstructed(replica, output);
	}
	else if (res!=BQR_NEVER)
	{
		unsigned i;
		for (i=0; i < connectionList.Size(); i++)
		{
			if (connectionList[i]->lastSerializationList.HasData(replica))
				output.Insert(connectionList[i]->GetSystemAddress(),connectionList[i], false);
		}
	}
}
RakPeerInterface *ReplicaManager2::GetRakPeer(void) const
{
	return rakPeerInterface;
}
Connection_RM2* ReplicaManager2::AutoCreateConnection(SystemAddress systemAddress, bool *newConnection)
{
	if (autoAddNewConnections)
		return CreateConnectionIfDoesNotExist(systemAddress, newConnection);
	else
	{
		bool objectExists;
		unsigned index = connectionList.GetIndexFromKey(systemAddress, &objectExists);
		*newConnection=false;
		if (objectExists==false)
		{
			return 0;
		}
		return connectionList[index];
	}
}
Connection_RM2* ReplicaManager2::CreateConnectionIfDoesNotExist(SystemAddress systemAddress, bool *newConnection)
{
	bool objectExists;
	unsigned index = connectionList.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists==false)
	{
		// If it crashes here, you need to call SetConnection_RM2Factory
		Connection_RM2 *connection = connectionFactoryInterface->AllocConnection();
		connection->SetSystemAddress(systemAddress);
		connection->SetGuid(rakPeerInterface->GetGuidFromSystemAddress(systemAddress));
		connectionList.Insert(systemAddress, connection, false);
		*newConnection=true;
		return connection;
	}
	*newConnection=false;
	return connectionList[index];
}
bool ReplicaManager2::AddNewConnection(SystemAddress systemAddress)
{
	bool newConnection;
	Connection_RM2* connection = CreateConnectionIfDoesNotExist(systemAddress, &newConnection);
	if (newConnection)
		DownloadToNewConnection(connection, 0, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);
	return newConnection;
}
bool ReplicaManager2::RemoveConnection(SystemAddress systemAddress)
{
	unsigned int index = GetConnectionIndexBySystemAddress(systemAddress);
	if (index!=(unsigned int) -1)
	{
		connectionFactoryInterface->DeallocConnection(connectionList[index]);
		connectionList.RemoveAtIndex(index);
		return true;
	}
	return false;
}
bool ReplicaManager2::HasConnection(SystemAddress systemAddress)
{
	unsigned int index = GetConnectionIndexBySystemAddress(systemAddress);
	return index!=(unsigned int) -1;
}
void ReplicaManager2::Reference(Replica2* replica, bool *newReference)
{
	replica->SetReplicaManager(this);

	bool objectExists;
	unsigned index = fullReplicaOrderedList.GetIndexFromKey(replica,&objectExists);
	if (objectExists==false)
	{
		fullReplicaUnorderedList.Insert(replica, __FILE__, __LINE__);
		fullReplicaOrderedList.InsertAtIndex(replica, index);

		BooleanQueryResult queryResult;
		queryResult = replica->QueryConstruction(0);
		if (queryResult==BQR_ALWAYS)
			alwaysDoConstructReplicaOrderedList.Insert(replica,replica, false);
		else if (queryResult!=BQR_NEVER)
			variableConstructReplicaOrderedList.Insert(replica,replica, false);
		queryResult = replica->QueryVisibility(0);
		if (queryResult==BQR_ALWAYS)
			alwaysDoSerializeReplicaOrderedList.Insert(replica,replica, false);
		else if (queryResult!=BQR_NEVER)
			variableSerializeReplicaOrderedList.Insert(replica,replica, false);

		if (newReference)
			*newReference=true;

		return;
	}
	if (newReference)
		*newReference=false;
}
void ReplicaManager2::AddConstructionReference(Connection_RM2* connection, Replica2* replica)
{
	if (replica->QueryIsConstructionAuthority() && replica->QueryConstruction(0)!=BQR_ALWAYS && replica->QueryConstruction(0)!=BQR_NEVER)
		connection->lastConstructionList.Insert(replica, replica, false);
}
void ReplicaManager2::AddVisibilityReference(Connection_RM2* connection, Replica2* replica)
{
	if (replica->QueryIsVisibilityAuthority() && replica->QueryVisibility(0)!=BQR_ALWAYS && replica->QueryVisibility(0)!=BQR_NEVER)
		connection->lastSerializationList.Insert(replica, replica, false);
}
void ReplicaManager2::RemoveVisibilityReference(Connection_RM2* connection, Replica2* replica)
{
	if (replica->QueryIsVisibilityAuthority() && replica->QueryVisibility(0)!=BQR_ALWAYS && replica->QueryVisibility(0)!=BQR_NEVER)
		connection->lastSerializationList.RemoveIfExists(replica);
}
void ReplicaManager2::WriteHeader(RakNet::BitStream *bs, MessageID type, RakNetTime timestamp)
{
	if (timestamp!=0)
	{
		bs->Write((MessageID)ID_TIMESTAMP);
		bs->Write(timestamp);
	}
	bs->Write(type);
}
void ReplicaManager2::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) systemAddress;
	(void) rakNetGUID;
	(void) lostConnectionReason;

	RemoveConnection(systemAddress);
}
void ReplicaManager2::OnShutdown(void)
{
	Clear();
}
void ReplicaManager2::OnAttach(void)
{
	lastUpdateTime=RakNet::GetTime();
}
void ReplicaManager2::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) systemAddress;
	(void) rakNetGUID;
	(void) isIncoming;

	if (autoAddNewConnections)
		AddNewConnection(systemAddress);
}
PluginReceiveResult ReplicaManager2::OnReceive(Packet *packet)
{
	RakNetTime timestamp=0;
	unsigned char packetIdentifier, packetDataOffset;
	if ( ( unsigned char ) packet->data[ 0 ] == ID_TIMESTAMP )
	{
		if ( packet->length > sizeof( unsigned char ) + sizeof( RakNetTime ) )
		{
			packetIdentifier = ( unsigned char ) packet->data[ sizeof( unsigned char ) + sizeof( RakNetTime ) ];
			// Required for proper endian swapping
			RakNet::BitStream tsBs(packet->data+sizeof(MessageID),packet->length-1,false);
			tsBs.Read(timestamp);
			packetDataOffset=sizeof( unsigned char )*2 + sizeof( RakNetTime );
		}
		else
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}
	else
	{
		packetIdentifier = ( unsigned char ) packet->data[ 0 ];
		packetDataOffset=sizeof( unsigned char );
	}

	switch (packetIdentifier)
	{
	case ID_REPLICA_MANAGER_DOWNLOAD_STARTED:
		return OnDownloadStarted(packet->data+packetDataOffset, packet->length-packetDataOffset, packet->systemAddress, timestamp);
	case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
		return OnDownloadComplete(packet->data+packetDataOffset, packet->length-packetDataOffset, packet->systemAddress, timestamp);
	case ID_REPLICA_MANAGER_CONSTRUCTION:
		return OnConstruction(packet->data+packetDataOffset, packet->length-packetDataOffset, packet->systemAddress, timestamp);
	case ID_REPLICA_MANAGER_DESTRUCTION:
		return OnDestruction(packet->data+packetDataOffset, packet->length-packetDataOffset, packet->systemAddress, timestamp);
	case ID_REPLICA_MANAGER_SCOPE_CHANGE:
		return OnVisibilityChange(packet->data+packetDataOffset, packet->length-packetDataOffset, packet->systemAddress, timestamp);
	case ID_REPLICA_MANAGER_SERIALIZE:
		return OnSerialize(packet->data+packetDataOffset, packet->length-packetDataOffset, packet->systemAddress, timestamp);
	}

	return RR_CONTINUE_PROCESSING;
}
PluginReceiveResult ReplicaManager2::OnDownloadStarted(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp)
{
	RakNet::BitStream incomingBitstream(packetData, packetDataLength, false);
	bool newConnection;
	Connection_RM2* connection = AutoCreateConnection(sender, &newConnection);
	if (connection==0)
		return RR_CONTINUE_PROCESSING;
	SerializationType serializationType;
	unsigned char c;
	incomingBitstream.Read(c);
	serializationType=(SerializationType) c;
	incomingBitstream.AlignReadToByteBoundary();
	connection->DeserializeDownloadStarted(&incomingBitstream, sender, this, timestamp, serializationType);

	if (newConnection)
		DownloadToNewConnection(connection, timestamp, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
PluginReceiveResult ReplicaManager2::OnDownloadComplete(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp)
{
	RakNet::BitStream incomingBitstream(packetData, packetDataLength, false);
	bool newConnection;
	Connection_RM2* connection = AutoCreateConnection(sender, &newConnection);
	if (connection==0)
		return RR_CONTINUE_PROCESSING;
	SerializationType serializationType;
	unsigned char c;
	incomingBitstream.Read(c);
	serializationType=(SerializationType) c;
	incomingBitstream.AlignReadToByteBoundary();
	connection->DeserializeDownloadComplete(&incomingBitstream, sender, this, timestamp, serializationType);

	if (newConnection)
		DownloadToNewConnection(connection, timestamp, defaultPacketPriority, defaultPacketReliablity, defaultOrderingChannel);
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}

PluginReceiveResult ReplicaManager2::OnConstruction(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp)
{
	RakNet::BitStream incomingBitstream(packetData, packetDataLength, false);
	bool newConnection;
	Connection_RM2* connection = AutoCreateConnection(sender, &newConnection);
	if (connection==0)
		return RR_CONTINUE_PROCESSING;
	SerializationType serializationType;
	unsigned char c;
	incomingBitstream.Read(c);
	serializationType=(SerializationType) c;
	NetworkID networkId=UNASSIGNED_NETWORK_ID;
	unsigned char localClientId=255;
	bool success;
	incomingBitstream.Read(networkId);
	success=incomingBitstream.Read(localClientId);
	RakAssert(success);

	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	ReadExclusionList(&incomingBitstream, exclusionList);
	exclusionList.Insert(sender,sender, false);

	Replica2* replica;
	// The prefix misaligns the data from the send, which is a problem if the user uses aligned data
	incomingBitstream.AlignReadToByteBoundary();
	replica = connection->ReceiveConstruct(&incomingBitstream, networkId, sender, localClientId, serializationType, this, timestamp,exclusionList);
	if (replica)
	{
		// Register this object on this connection
		AddConstructionReference(connection, replica);
	}
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
PluginReceiveResult ReplicaManager2::OnDestruction(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp)
{
	if (HasConnection(sender)==false)
		return RR_CONTINUE_PROCESSING;

	RakNet::BitStream incomingBitstream(packetData, packetDataLength, false);
	SerializationType serializationType;
	unsigned char c;
	incomingBitstream.Read(c);
	serializationType=(SerializationType) c;
	NetworkID networkId;
	incomingBitstream.Read(networkId);
	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	ReadExclusionList(&incomingBitstream, exclusionList);
	exclusionList.Insert(sender,sender, false);
	Replica2 * replica = rakPeerInterface->GetNetworkIDManager()->GET_OBJECT_FROM_ID<Replica2*>( networkId );
	if (replica)
	{
		// Verify that this is a registered object, so it actually is a Replica2
		if (fullReplicaOrderedList.HasData((Replica2 *)replica)==false)
		{
			// This object is not registered
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}

		// The prefix misaligns the data from the send, which is a problem if the user uses aligned data
		incomingBitstream.AlignReadToByteBoundary();
		replica->ReceiveDestruction(sender, &incomingBitstream, serializationType, timestamp,exclusionList );
	}
	// else this object is unknown

	
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
PluginReceiveResult ReplicaManager2::OnVisibilityChange(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp)
{
	RakNet::BitStream incomingBitstream(packetData, packetDataLength, false);
	bool newConnection;
	Connection_RM2* connection = AutoCreateConnection(sender, &newConnection);
	if (connection==0)
		return RR_CONTINUE_PROCESSING;
	SerializationType serializationType;
	
	unsigned char c;
	incomingBitstream.Read(c);
	serializationType=(SerializationType) c;
	NetworkID networkId;
	incomingBitstream.Read(networkId);
	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	ReadExclusionList(&incomingBitstream, exclusionList);
	exclusionList.Insert(sender,sender, false);
	
	Replica2 *replica = rakPeerInterface->GetNetworkIDManager()->GET_OBJECT_FROM_ID<Replica2 *>( networkId );
	if (replica)
	{
		// Verify that this is a registered object, so it actually is a Replica2
		if (fullReplicaOrderedList.HasData((Replica2 *)replica)==false)
		{
			RakAssert(0);
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}

		// The prefix misaligns the data from the send, which is a problem if the user uses aligned data
		incomingBitstream.AlignReadToByteBoundary();
		replica->ReceiveVisibility(sender, &incomingBitstream, serializationType, timestamp,exclusionList);

		AddConstructionReference(connection, replica);

		// Update the last known visibility list
		if (SerializationContext::IsVisibilityCommand(serializationType))
		{
			if (SerializationContext::IsVisible(serializationType))
				AddVisibilityReference(connection, replica);
			else
				RemoveVisibilityReference(connection, replica);
		}
	}
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
PluginReceiveResult ReplicaManager2::OnSerialize(unsigned char *packetData, int packetDataLength, SystemAddress sender, RakNetTime timestamp)
{
	RakNet::BitStream incomingBitstream(packetData, packetDataLength, false);
	Connection_RM2* connection = GetConnectionBySystemAddress(sender);
	if (connection==0)
		return RR_CONTINUE_PROCESSING;
	SerializationType serializationType;
	unsigned char c;
	incomingBitstream.Read(c);
	serializationType=(SerializationType) c;
	NetworkID networkId;
	incomingBitstream.Read(networkId);
	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	ReadExclusionList(&incomingBitstream, exclusionList);
	exclusionList.Insert(sender,sender, false);

	Replica2 *replica = rakPeerInterface->GetNetworkIDManager()->GET_OBJECT_FROM_ID<Replica2 *>( networkId );
	if (replica)
	{
		// Verify that this is a registered object, so it actually is a Replica2
		if (fullReplicaOrderedList.HasData((Replica2 *)replica)==false)
		{
			RakAssert(0);
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}

		exclusionList.Insert(sender,sender, false);

		// The prefix misaligns the data from the send, which is a problem if the user uses aligned data
		incomingBitstream.AlignReadToByteBoundary();
		replica->ReceiveSerialize(sender, &incomingBitstream, serializationType, timestamp,exclusionList);

		AddConstructionReference(connection, replica);
	}
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
bool ReplicaManager2::AddToAndWriteExclusionList(SystemAddress recipient, RakNet::BitStream *bs, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList)
{
	if (exclusionList.HasData(recipient))
		return false;
	exclusionList.Insert(recipient,recipient,true);
	WriteExclusionList(bs,exclusionList);
	return true;
}
void ReplicaManager2::WriteExclusionList(RakNet::BitStream *bs, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList)
{
	bs->WriteCompressed(exclusionList.Size());
	for (unsigned exclusionListIndex=0; exclusionListIndex < exclusionList.Size(); exclusionListIndex++ )
		bs->Write(exclusionList[exclusionListIndex]);
}

void ReplicaManager2::CullByAndAddToExclusionList(
								 DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> &inputList,
								 DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> &culledOutput,
								 DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList)
{
	Connection_RM2* connection;
	unsigned i;
	unsigned exclusionListIndex=0;
	for (i=0; i < inputList.Size(); i++)
	{
		connection=inputList[i];
		while (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex] < connection->GetSystemAddress())
			exclusionListIndex++;
		if (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex]==connection->GetSystemAddress())
		{
			exclusionListIndex++;
			continue;
		}
		culledOutput.InsertAtEnd(connection);
	}

	for (i=0; i < culledOutput.Size(); i++)
		exclusionList.Insert(culledOutput[i]->GetSystemAddress(),culledOutput[i]->GetSystemAddress(),true);
}
void ReplicaManager2::ReadExclusionList(RakNet::BitStream *bs, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList)
{
	unsigned int exclusionListSize;
	bs->ReadCompressed(exclusionListSize);
	for (unsigned exclusionListIndex=0; exclusionListIndex < exclusionListSize; exclusionListIndex++)
	{
		SystemAddress systemToExclude;
		bs->Read(systemToExclude);
		exclusionList.InsertAtEnd(systemToExclude);
	}
}
void ReplicaManager2::Send(RakNet::BitStream *bs, SystemAddress recipient, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (priority==NUMBER_OF_PRIORITIES)
		priority=defaultPacketPriority;
	if (reliability==NUMBER_OF_RELIABILITIES)
		reliability=defaultPacketReliablity;
	if (orderingChannel==-1)
		orderingChannel=defaultOrderingChannel;
	SendUnified(bs, priority,reliability,orderingChannel,recipient,false);
}
void ReplicaManager2::Clear(void)
{
	fullReplicaUnorderedList.Clear();
	fullReplicaOrderedList.Clear();
	alwaysDoConstructReplicaOrderedList.Clear();
	alwaysDoSerializeReplicaOrderedList.Clear();
	variableConstructReplicaOrderedList.Clear();
	variableSerializeReplicaOrderedList.Clear();
	unsigned i;
	for (i=0; i < connectionList.Size(); i++)
		connectionFactoryInterface->DeallocConnection(connectionList[i]);
	connectionList.Clear();
}
void ReplicaManager2::DownloadToNewConnection(Connection_RM2* connection, RakNetTime timestamp, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	unsigned int i;
	RakNet::BitStream bs, bs2;
	BooleanQueryResult bqr;
	SystemAddress systemAddress = connection->GetSystemAddress();
	SerializationContext serializationContext;
	serializationContext.recipientAddress=systemAddress;
	serializationContext.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
	serializationContext.serializationType=SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM;
	serializationContext.timestamp=0;

	// bs2 is so SerializeDownloadStarted can change the timestamp
	bs2.AlignWriteToByteBoundary();
	connection->SerializeDownloadStarted(&bs2, this, &serializationContext);
	WriteHeader(&bs, ID_REPLICA_MANAGER_DOWNLOAD_STARTED, timestamp);
	bs.Write((unsigned char) SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM);
	bs.Write(&bs2);
	Send(&bs, connection->GetSystemAddress(), priority, reliability, orderingChannel);

	DataStructures::List<Replica2*> initialDownloadList;
	DataStructures::List<Replica2*> culledDownloadList;
	connection->SortInitialDownload(fullReplicaUnorderedList, initialDownloadList);

	// Construct all objects before serializing them. This way the recipient will have valid NetworkID references.
	// Send all objects that always exist
	for (i=0; i < initialDownloadList.Size(); i++)
	{
		if (initialDownloadList[i]->QueryIsConstructionAuthority())
		{
			bqr=initialDownloadList[i]->QueryConstruction(connection);
			if (bqr==BQR_ALWAYS || bqr==BQR_YES)
			{
				initialDownloadList[i]->SendConstruction(systemAddress, SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM);
				culledDownloadList.Insert(initialDownloadList[i]);
			}
			// Remember for this particular connection that we already sent this update to this system
			if (bqr==BQR_YES)
				AddConstructionReference(connection, initialDownloadList[i]);
		}
	}

	bool notVisible;

	// Send all objects that are always visible
	for (i=0; i < culledDownloadList.Size(); i++)
	{
		notVisible=false;
		if (culledDownloadList[i]->QueryIsVisibilityAuthority())
		{
			bqr=culledDownloadList[i]->QueryVisibility(connection);
			if (bqr==BQR_ALWAYS || bqr==BQR_YES)
			{
				culledDownloadList[i]->SendVisibility(systemAddress, SEND_VISIBILITY_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM);
				// Remember for this particular connection that we already sent this update to this system
				if (bqr==BQR_YES)
					AddVisibilityReference(connection, culledDownloadList[i]);
			}
			else
				notVisible=true;
		}

		if (culledDownloadList[i]->QueryIsSerializationAuthority() && notVisible==false)
			culledDownloadList[i]->SendSerialize(systemAddress, SEND_DATA_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM);
	}

	bs.Reset();
	// bs2 is so SerializeDownloadComplete can change the timestamp
	bs2.AlignWriteToByteBoundary();
	connection->SerializeDownloadComplete(&bs2, this,&serializationContext);
	WriteHeader(&bs, ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE, timestamp);
	bs.Write((unsigned char) SEND_CONSTRUCTION_SERIALIZATION_AUTO_INITIAL_DOWNLOAD_TO_SYSTEM);
	bs.Write(&bs2);
	Send(&bs, connection->GetSystemAddress(), priority, reliability, orderingChannel);
}
Replica2::Replica2()
{
	rm2=0;
	hasClientID=false;

	DataStructures::Map<SerializationType, AutoSerializeEvent*>::IMPLEMENT_DEFAULT_COMPARISON();
}
Replica2::~Replica2()
{
	DereferenceFromDestruction();
}

void Replica2::SetReplicaManager(ReplicaManager2* rm)
{
	rm2=rm;
	if (GetNetworkIDManager()==0)
		SetNetworkIDManager(rm->GetRakPeer()->GetNetworkIDManager());
}
ReplicaManager2* Replica2::GetReplicaManager(void) const
{
	return rm2;
}
bool Replica2::SerializeDestruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	(void) bitStream;
	(void) serializationContext;

	return true;
}
bool Replica2::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	(void) bitStream;
	(void) serializationContext;

	return true;
}
bool Replica2::SerializeVisibility(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	(void) bitStream;
	(void) serializationContext;

	return true;
}
void Replica2::DeserializeDestruction(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	(void) bitStream;
	(void) serializationType;
	(void) sender;
	(void) timestamp;

}
void Replica2::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	(void) bitStream;
	(void) serializationType;
	(void) sender;
	(void) timestamp;
}
void Replica2::DeserializeVisibility(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	(void) bitStream;
	(void) serializationType;
	(void) sender;
	(void) timestamp;
}
void Replica2::SendConstruction(SystemAddress recipientAddress, SerializationType serializationType)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext;

	if (serializationType==UNDEFINED_REASON)
	{
		if (QueryIsConstructionAuthority()==false)
			defaultContext.serializationType=SEND_CONSTRUCTION_REQUEST_TO_SERVER;
		else
			defaultContext.serializationType=SEND_CONSTRUCTION_GENERIC_TO_SYSTEM;
	}
	else
		defaultContext.serializationType=serializationType;

	defaultContext.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
	defaultContext.recipientAddress=recipientAddress;
	defaultContext.timestamp=0;

	unsigned char localId;
	if (QueryIsConstructionAuthority()==false)
	{
		clientPtrArray[Replica2::clientSharedID]=this; 
		localId=Replica2::clientSharedID++;
	}
	else
		localId=0;

	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	// // The prefix misaligns the data for the send, which is a problem if the user uses aligned data
	bs.AlignWriteToByteBoundary();
	if (SerializeConstruction(&bs, &defaultContext))
		rm2->SendConstruction(this,&bs,recipientAddress,defaultContext.timestamp,true,exclusionList,localId,defaultContext.serializationType);
}
void Replica2::SendDestruction(SystemAddress recipientAddress, SerializationType serializationType)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext(serializationType, UNASSIGNED_SYSTEM_ADDRESS, recipientAddress,0);

	if (serializationType==UNDEFINED_REASON)
		defaultContext.serializationType=SEND_DESTRUCTION_GENERIC_TO_SYSTEM;

	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	// // The prefix misaligns the data for the send, which is a problem if the user uses aligned data
	bs.AlignWriteToByteBoundary();
	if (SerializeDestruction(&bs, &defaultContext))
		rm2->SendDestruction(this,&bs,recipientAddress,defaultContext.timestamp,true,exclusionList,defaultContext.serializationType);
}
void Replica2::SendSerialize(SystemAddress recipientAddress, SerializationType serializationType)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext(serializationType, UNASSIGNED_SYSTEM_ADDRESS, recipientAddress,0);

	if (serializationType==UNDEFINED_REASON)
		defaultContext.serializationType=SEND_SERIALIZATION_GENERIC_TO_SYSTEM;

	// // The prefix misaligns the data for the send, which is a problem if the user uses aligned data
	bs.AlignWriteToByteBoundary();
	if (Serialize(&bs, &defaultContext))
	{
		DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
		rm2->SendSerialize(this,&bs,recipientAddress,defaultContext.timestamp,exclusionList,defaultContext.serializationType);
	}
}
void Replica2::SendVisibility(SystemAddress recipientAddress, SerializationType serializationType)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext(serializationType, UNASSIGNED_SYSTEM_ADDRESS, recipientAddress,0);

	if (serializationType==UNDEFINED_REASON)
		defaultContext.serializationType=SEND_VISIBILITY_TRUE_TO_SYSTEM;

	// // The prefix misaligns the data for the send, which is a problem if the user uses aligned data
	bs.AlignWriteToByteBoundary();
	if (SerializeVisibility(&bs, &defaultContext))
	{
		DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
		rm2->SendVisibility(this,&bs,recipientAddress,defaultContext.timestamp,exclusionList,defaultContext.serializationType);
	}
}
void Replica2::BroadcastSerialize(SerializationContext *serializationContext)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext(BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS,0);
	SerializationContext *usedContext;
	if (serializationContext)
		usedContext=serializationContext;
	else
		usedContext=&defaultContext;

	bool newReference;
	rm2->Reference(this, &newReference);

	// If this is a new object, then before sending serialization we should send construction to all systems
	if (newReference && QueryConstruction(0)==BQR_ALWAYS)
	{
		BroadcastConstruction();
	}

	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	for (unsigned i=0; i < rm2->GetConnectionCount(); i++)
	{
		usedContext->recipientAddress=rm2->GetConnectionAtIndex(i)->GetSystemAddress();
		if (usedContext->relaySourceAddress==usedContext->recipientAddress)
			continue;
		bs.Reset();
		// // The prefix misaligns the data for the send, which is a problem if the user uses aligned data
		bs.AlignWriteToByteBoundary();
		if (Serialize(&bs, usedContext)==false)
			continue;
		exclusionList.Clear();
		for (unsigned j=0; j < rm2->connectionList.Size(); j++)
		{
			if (rm2->connectionList[j]->GetSystemAddress()!=usedContext->recipientAddress)
				exclusionList.InsertAtEnd(rm2->connectionList[j]->GetSystemAddress());
		}
		rm2->SendSerialize(this,&bs,usedContext->recipientAddress,usedContext->timestamp,exclusionList,usedContext->serializationType);
	}
}

void Replica2::ReceiveSerialize(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList )
{
	// Force all autoserialize timers to go off early, so any variable changes from the Deserialize event do not themselves trigger an autoserialize event
	ForceElapseAllAutoserializeTimers(false);

	// Deserialize the new data
	Deserialize(serializedObject, serializationType, sender, timestamp);

	// Update last values for all autoserialize timers
	ForceElapseAllAutoserializeTimers(true);

	SerializationContext serializationContext;
	serializationContext.serializationType=RELAY_SERIALIZATION_TO_SYSTEMS;
	serializationContext.timestamp=timestamp;
	serializationContext.relaySourceAddress=sender;


	BooleanQueryResult bqr;
	RakNet::BitStream bs;
	unsigned exclusionListIndex=0;
	for (unsigned i=0; i < rm2->connectionList.Size(); i++)
	{
		serializationContext.recipientAddress=rm2->connectionList[i]->GetSystemAddress();
		while (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex] < serializationContext.recipientAddress)
			exclusionListIndex++;
		if (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex]==serializationContext.recipientAddress)
		{
			exclusionListIndex++;
			continue;
		}

		// Don't relay serializations if this object should not be visible
		bqr=QueryVisibility(rm2->connectionList[i]);
		if (bqr==BQR_NEVER || bqr==BQR_NO)
			continue;

		bs.Reset();
		if (Serialize(&bs, &serializationContext)==false)
			continue;
		rm2->SendSerialize(this,&bs,serializationContext.recipientAddress,serializationContext.timestamp,exclusionList,serializationContext.serializationType);
	}
}

void Replica2::BroadcastConstruction(SerializationContext *serializationContext)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext(BROADCAST_CONSTRUCTION_GENERIC_TO_SYSTEM, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS,0);
	SerializationContext *usedContext;
	if (serializationContext)
		usedContext=serializationContext;
	else
	{
		usedContext=&defaultContext;
		if (QueryIsConstructionAuthority()==false)
			defaultContext.serializationType=SEND_CONSTRUCTION_REQUEST_TO_SERVER;
	}	

	bool newReference;
	rm2->Reference(this, &newReference);

	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;

	for (unsigned i=0; i < rm2->GetConnectionCount(); i++)
	{
		usedContext->recipientAddress=rm2->GetConnectionAtIndex(i)->GetSystemAddress();
		if (usedContext->relaySourceAddress==usedContext->recipientAddress)
			continue;
		bs.Reset();
		if (SerializeConstruction(&bs, usedContext)==false)
			continue;
		unsigned char localId;
		if (QueryIsConstructionAuthority()==false)
		{
			clientPtrArray[Replica2::clientSharedID]=this;
			localId=Replica2::clientSharedID++;
		}
		else
			localId=0;
		exclusionList.Clear();
		for (unsigned j=0; j < rm2->connectionList.Size(); j++)
		{
			if (rm2->connectionList[j]->GetSystemAddress()!=usedContext->recipientAddress)
				exclusionList.InsertAtEnd(rm2->connectionList[j]->GetSystemAddress());
		}
		rm2->SendConstruction(this,&bs,usedContext->recipientAddress,usedContext->timestamp,true,exclusionList, localId, usedContext->serializationType);
	}


	bool notVisible=false;
	BooleanQueryResult bqr;
	bqr=QueryVisibility(0);

	if (bqr==BQR_ALWAYS)
		BroadcastVisibility(true);
	else if (bqr==BQR_NEVER)
		notVisible=true;

	if (notVisible==false)
		BroadcastSerialize();
}
Replica2 * Replica2::ReceiveConstructionReply(SystemAddress sender, BitStream *replicaData, bool constructionAllowed)
{
	(void) replicaData;
	(void) sender;

	if (constructionAllowed==false)
	{
		//RakNet::OP_DELETE(this, __FILE__, __LINE__);
		delete this;
		return 0;
	}

	return this;
}
void Replica2::DereferenceFromDestruction(void)
{
	if (rm2)
		rm2->Dereference(this);
	if (hasClientID)
		clientPtrArray[clientID]=0;
	ClearAutoSerializeTimers();
}
void Replica2::BroadcastDestruction(SerializationContext *serializationContext)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext(BROADCAST_DESTRUCTION_GENERIC_TO_SYSTEM, UNASSIGNED_SYSTEM_ADDRESS, UNASSIGNED_SYSTEM_ADDRESS, 0);
	SerializationContext *usedContext;
	if (serializationContext)
		usedContext=serializationContext;
	else
		usedContext=&defaultContext;

	DataStructures::OrderedList<SystemAddress, Connection_RM2*,ReplicaManager2::Connection_RM2CompBySystemAddress> culledOutput;
	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	rm2->CullByAndAddToExclusionList(rm2->connectionList, culledOutput, exclusionList);

	for (unsigned i=0; i < rm2->GetConnectionCount(); i++)
	{
		usedContext->recipientAddress=rm2->GetConnectionAtIndex(i)->GetSystemAddress();
		if (usedContext->relaySourceAddress==usedContext->recipientAddress)
			continue;
		bs.Reset();
		if (SerializeDestruction(&bs, usedContext)==false)
			continue;
		exclusionList.Clear();
		for (unsigned j=0; j < rm2->connectionList.Size(); j++)
		{
			if (rm2->connectionList[j]->GetSystemAddress()!=usedContext->recipientAddress)
				exclusionList.InsertAtEnd(rm2->connectionList[j]->GetSystemAddress());
		}
		rm2->SendDestruction(this,&bs,usedContext->recipientAddress,usedContext->timestamp,true,exclusionList,usedContext->serializationType);
	}
}
void Replica2::BroadcastVisibility(bool isVisible, SerializationContext *serializationContext)
{
	RakNet::BitStream bs;
	SerializationContext defaultContext;
	SerializationContext *usedContext;

	if (serializationContext)
	{
		usedContext=serializationContext;
	}
	else
	{
		if (isVisible)
			defaultContext.serializationType=BROADCAST_VISIBILITY_TRUE_TO_SYSTEM;
		else
			defaultContext.serializationType=BROADCAST_VISIBILITY_FALSE_TO_SYSTEM;
		defaultContext.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
		defaultContext.timestamp=0;
		usedContext=&defaultContext;
	}

	if ((QueryVisibility(0)==BQR_ALWAYS && isVisible==false) ||
		(QueryVisibility(0)==BQR_NEVER && isVisible==true))
	{
		// This doesn't make sense
		RakAssert(0);
		return;
	}

	bool newReference;
	rm2->Reference(this, &newReference);

	// If this is a new object, then before sending visibility we should send construction to all systems
	if (newReference && QueryConstruction(0)==BQR_ALWAYS)
	{
		BroadcastConstruction();
	}

	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	for (unsigned i=0; i < rm2->GetConnectionCount(); i++)
	{
		usedContext->recipientAddress=rm2->GetConnectionAtIndex(i)->GetSystemAddress();
		if (usedContext->relaySourceAddress==usedContext->recipientAddress)
			continue;
		bs.Reset();
		if (SerializeVisibility(&bs, usedContext)==false)
			continue;
		exclusionList.Clear();
		for (unsigned j=0; j < rm2->connectionList.Size(); j++)
		{
			if (rm2->connectionList[j]->GetSystemAddress()!=usedContext->recipientAddress)
				exclusionList.InsertAtEnd(rm2->connectionList[j]->GetSystemAddress());
		}
		rm2->SendVisibility(this,&bs,usedContext->recipientAddress,usedContext->timestamp,exclusionList,usedContext->serializationType);
	}

	if (newReference && QueryVisibility(0)==BQR_ALWAYS)
	{
		BroadcastSerialize();
	}
}

void Replica2::ReceiveDestruction(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList )
{
	DeserializeDestruction(serializedObject, serializationType, sender, timestamp);

	SerializationContext serializationContext;
	serializationContext.serializationType=RELAY_DESTRUCTION_TO_SYSTEMS;
	serializationContext.relaySourceAddress=sender;
	serializationContext.timestamp=0;

	RakNet::BitStream bs;
	unsigned exclusionListIndex=0;
	for (unsigned i=0; i < rm2->connectionList.Size(); i++)
	{
		serializationContext.recipientAddress=rm2->connectionList[i]->GetSystemAddress();

		while (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex] < serializationContext.recipientAddress)
			exclusionListIndex++;
		if (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex]==serializationContext.recipientAddress)
		{
			exclusionListIndex++;
			continue;
		}

		bs.Reset();
		if (SerializeDestruction(&bs, &serializationContext)==false)
			continue;
		rm2->SendDestruction(this,&bs,serializationContext.recipientAddress,serializationContext.timestamp,true,exclusionList,serializationContext.serializationType);
	}

	DeleteOnReceiveDestruction(sender, serializedObject, serializationType, timestamp, exclusionList);
}
void Replica2::DeleteOnReceiveDestruction(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList )
{
	(void) sender;
	(void) serializedObject;
	(void) serializationType;
	(void) timestamp;
	(void) exclusionList;
	//RakNet::OP_DELETE(this, __FILE__, __LINE__);
	delete this;
}
void Replica2::ReceiveVisibility(SystemAddress sender, RakNet::BitStream *serializedObject, SerializationType serializationType, RakNetTime timestamp, DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList)
{
	DeserializeVisibility(serializedObject, serializationType, sender, timestamp);

	SerializationContext serializationContext;
	if (serializationType==SEND_VISIBILITY_TRUE_TO_SYSTEM || serializationType==BROADCAST_VISIBILITY_TRUE_TO_SYSTEM)
		serializationContext.serializationType=RELAY_VISIBILITY_TRUE_TO_SYSTEMS;
	else if (serializationType==SEND_VISIBILITY_FALSE_TO_SYSTEM || serializationType==BROADCAST_VISIBILITY_FALSE_TO_SYSTEM)
		serializationContext.serializationType=RELAY_VISIBILITY_FALSE_TO_SYSTEMS;
	else
		serializationContext.serializationType=serializationType;
	serializationContext.timestamp=timestamp;
	serializationContext.relaySourceAddress=sender;

	RakNet::BitStream bs;
	unsigned exclusionListIndex=0;
	for (unsigned i=0; i < rm2->connectionList.Size(); i++)
	{
		serializationContext.recipientAddress=rm2->connectionList[i]->GetSystemAddress();

		while (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex] < serializationContext.recipientAddress)
			exclusionListIndex++;
		if (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex]==serializationContext.recipientAddress)
		{
			exclusionListIndex++;
			continue;
		}

		bs.Reset();
		if (SerializeVisibility(&bs, &serializationContext)==false)
			continue;
		rm2->SendVisibility(this,&bs,serializationContext.recipientAddress,serializationContext.timestamp,exclusionList,serializationContext.serializationType);
	}
}
BooleanQueryResult Replica2::QueryConstruction(Connection_RM2 *connection)
{
	(void) connection;

	return BQR_ALWAYS;
}
BooleanQueryResult Replica2::QueryVisibility(Connection_RM2 *connection)
{
	(void) connection;

	return BQR_ALWAYS;
}
bool Replica2::QueryIsConstructionAuthority(void) const
{
	return rm2->GetRakPeer()->GetNetworkIDManager()->IsNetworkIDAuthority();
}
bool Replica2::QueryIsDestructionAuthority(void) const
{
	return rm2->GetRakPeer()->GetNetworkIDManager()->IsNetworkIDAuthority();
}
bool Replica2::QueryIsVisibilityAuthority(void) const
{
	return rm2->GetRakPeer()->GetNetworkIDManager()->IsNetworkIDAuthority();
}
bool Replica2::QueryIsSerializationAuthority(void) const
{
	return rm2->GetRakPeer()->GetNetworkIDManager()->IsNetworkIDAuthority();
}
bool Replica2::AllowRemoteConstruction(SystemAddress sender, RakNet::BitStream *replicaData, SerializationType type, RakNetTime timestamp)
{
	(void) sender;
	(void) replicaData;
	(void) type;
	(void) timestamp;

	return true;
}
void Replica2::ForceElapseAllAutoserializeTimers(bool resynchOnly)
{
	ElapseAutoSerializeTimers(99999999, resynchOnly);
}
void Replica2::OnConstructionComplete(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)
{
	(void) replicaData;
	(void) sender;
	(void) type;
	(void) replicaManager;
	(void) timestamp;
	(void) networkId;
	(void) networkIDCollision;
}
void Replica2::ElapseAutoSerializeTimers(RakNetTime timeElapsed, bool resynchOnly)
{
	AutoSerializeEvent* ase;
	unsigned i;
	for (i=0; i < autoSerializeTimers.Size(); i++)
	{
		ase = autoSerializeTimers[i];
		if (ase->remainingCountdown>timeElapsed)
		{
			ase->remainingCountdown-=timeElapsed;
		}
		else
		{
			ase->remainingCountdown=ase->initialCountdown;

			RakNet::BitStream *lastWrite, *newWrite;
			if (ase->writeToResult1)
			{
				newWrite=&ase->lastAutoSerializeResult1;
				lastWrite=&ase->lastAutoSerializeResult2;
			}
			else
			{
				newWrite=&ase->lastAutoSerializeResult2;
				lastWrite=&ase->lastAutoSerializeResult1;
			}
			newWrite->Reset();
			OnAutoSerializeTimerElapsed(ase->serializationType,newWrite,lastWrite,ase->remainingCountdown, resynchOnly);
			ase->writeToResult1=!ase->writeToResult1;

		}
	}
}
void Replica2::OnAutoSerializeTimerElapsed(SerializationType serializationType, RakNet::BitStream *output, RakNet::BitStream *lastOutput, RakNetTime lastAutoSerializeCountdown, bool resynchOnly)
{
	(void) lastAutoSerializeCountdown;

	SerializationContext context;
	if (resynchOnly)
		context.serializationType=AUTOSERIALIZE_RESYNCH_ONLY;
	else
		context.serializationType=serializationType;
	context.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
	context.recipientAddress=UNASSIGNED_SYSTEM_ADDRESS;
	context.timestamp=0;
	if (Serialize(output, &context))
	{
		if (resynchOnly==false &&
			output->GetNumberOfBitsUsed()>0 &&
			(output->GetNumberOfBitsUsed()!=lastOutput->GetNumberOfBitsUsed() ||
			memcmp(output->GetData(), lastOutput->GetData(), (size_t) output->GetNumberOfBytesUsed())!=0))
		{
			BroadcastAutoSerialize(&context, output);
		}
	}
}
void Replica2::BroadcastAutoSerialize(SerializationContext *serializationContext, RakNet::BitStream *serializedObject)
{
	DataStructures::OrderedList<SystemAddress,SystemAddress> exclusionList;
	rm2->SendSerialize(this,serializedObject,UNASSIGNED_SYSTEM_ADDRESS, serializationContext->timestamp, exclusionList, BROADCAST_AUTO_SERIALIZE_TO_SYSTEM);
}
void Replica2::AddAutoSerializeTimer(RakNetTime interval, SerializationType serializationType, RakNetTime countdown )
{
	if (countdown==(RakNetTime)-1)
		countdown=interval;
	if (autoSerializeTimers.Has(serializationType))
	{
		AutoSerializeEvent *ase = autoSerializeTimers.Get(serializationType);
		if (interval==0)
		{
			// Elapse this timer immediately, then go back to initialCountdown
			ase->remainingCountdown=ase->initialCountdown;

			RakNet::BitStream *lastWrite, *newWrite;
			if (ase->writeToResult1)
			{
				newWrite=&ase->lastAutoSerializeResult1;
				lastWrite=&ase->lastAutoSerializeResult2;
			}
			else
			{
				newWrite=&ase->lastAutoSerializeResult2;
				lastWrite=&ase->lastAutoSerializeResult1;
			}
			newWrite->Reset();

			OnAutoSerializeTimerElapsed(serializationType,newWrite,lastWrite,ase->initialCountdown, false);

			ase->remainingCountdown=ase->initialCountdown;
		}
		else
		{
			ase->initialCountdown=interval;
			ase->remainingCountdown=countdown;
		}
	}
	else
	{		
		AutoSerializeEvent *ase = RakNet::OP_NEW<AutoSerializeEvent>( __FILE__, __LINE__ );
		ase->serializationType=serializationType;
		ase->initialCountdown=interval;
		ase->remainingCountdown=countdown;
		ase->writeToResult1=true;

		SerializationContext context;
		context.serializationType=AUTOSERIALIZE_RESYNCH_ONLY;
		context.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
		context.recipientAddress=UNASSIGNED_SYSTEM_ADDRESS;
		context.timestamp=0;
		Serialize(&ase->lastAutoSerializeResult2, &context);

		autoSerializeTimers.Set(serializationType,ase);
	}
}
RakNetTime Replica2::GetTimeToNextAutoSerialize(SerializationType serializationType)
{
	if (autoSerializeTimers.Has(serializationType))
	{
		AutoSerializeEvent *ase = autoSerializeTimers.Get(serializationType);
		return ase->remainingCountdown;
	}
	return (RakNetTime)-1;
}
void Replica2::CancelAutoSerializeTimer(SerializationType serializationType)
{
	unsigned i=0;
	while (i < autoSerializeTimers.Size())
	{
		if (autoSerializeTimers[i]->serializationType==serializationType)
		{
			RakNet::OP_DELETE(autoSerializeTimers[i], __FILE__, __LINE__);
			autoSerializeTimers.RemoveAtIndex(i);
		}
		else
			i++;
	}
}
void Replica2::ClearAutoSerializeTimers(void)
{
	unsigned i;
	for (i=0; i < autoSerializeTimers.Size(); i++)
		RakNet::OP_DELETE(autoSerializeTimers[i], __FILE__, __LINE__);
	autoSerializeTimers.Clear();
}
Connection_RM2::Connection_RM2()
{
	rakNetGuid=UNASSIGNED_RAKNET_GUID;
	systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
}
Connection_RM2::~Connection_RM2()
{
}
void Connection_RM2::SetConstructionByList(DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &currentVisibility, ReplicaManager2 *replicaManager)
{
	(void) replicaManager;

	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> exclusiveToCurrentConstructionList, exclusiveToLastConstructionList;
	CalculateListExclusivity(currentVisibility, lastConstructionList, exclusiveToCurrentConstructionList, exclusiveToLastConstructionList);

	unsigned i;
	for (i=0; i < exclusiveToCurrentConstructionList.Size(); i++)
	{
		// Construct
		if (exclusiveToCurrentConstructionList[i]->QueryIsConstructionAuthority())
		{
			exclusiveToCurrentConstructionList[i]->SendConstruction(systemAddress);
		//	lastConstructionList.Insert(exclusiveToCurrentConstructionList[i],exclusiveToCurrentConstructionList[i],true);
		}
	}

	for (i=0; i < exclusiveToLastConstructionList.Size(); i++)
	{
		// Destruction
		if (exclusiveToLastConstructionList[i]->QueryIsDestructionAuthority())
		{
			exclusiveToLastConstructionList[i]->SendDestruction(systemAddress);
			lastConstructionList.RemoveIfExists(exclusiveToLastConstructionList[i]);
			lastSerializationList.RemoveIfExists(exclusiveToLastConstructionList[i]);
		}
	}
}
void Connection_RM2::SetVisibilityByList(DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &currentVisibility, ReplicaManager2 *replicaManager)
{
	(void) replicaManager;

	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> exclusiveToCurrentSerializationList, exclusiveToLastSerializationList;
	CalculateListExclusivity(currentVisibility, lastSerializationList, exclusiveToCurrentSerializationList, exclusiveToLastSerializationList);

	unsigned i;
	for (i=0; i < exclusiveToCurrentSerializationList.Size(); i++)
	{
		// In scope
		if (exclusiveToCurrentSerializationList[i]->QueryIsVisibilityAuthority())
		{
			exclusiveToCurrentSerializationList[i]->SendVisibility(systemAddress,SEND_VISIBILITY_TRUE_TO_SYSTEM);
			exclusiveToCurrentSerializationList[i]->SendSerialize(systemAddress);
		//	lastSerializationList.Insert(exclusiveToCurrentSerializationList[i],exclusiveToCurrentSerializationList[i], true);
		}
	}

	for (i=0; i < exclusiveToLastSerializationList.Size(); i++)
	{
		// Out of scope
		if (exclusiveToLastSerializationList[i]->QueryIsVisibilityAuthority())
		{
			exclusiveToLastSerializationList[i]->SendVisibility(systemAddress,SEND_VISIBILITY_FALSE_TO_SYSTEM);
			lastSerializationList.RemoveIfExists(exclusiveToLastSerializationList[i]);
		}
	}
}
void Connection_RM2::CalculateListExclusivity(
							  const DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &listOne,
							  const DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &listTwo,
							  DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &exclusiveToListOne, 
							  DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> &exclusiveToListTwo
							  ) const
{
	int res;
	unsigned listOneIndex=0, listTwoIndex=0;

	while (listOneIndex<listOne.Size() && listTwoIndex < listTwo.Size())
	{
		res = ReplicaManager2::Replica2ObjectComp(listOne[listOneIndex],listTwo[listTwoIndex]);
		if (res<0)
		{
			exclusiveToListOne.InsertAtEnd(listOne[listOneIndex]);
			listOneIndex++;
		}
		else if (res>0)
		{
			exclusiveToListTwo.InsertAtEnd(listTwo[listTwoIndex]);
			listTwoIndex++;
		}
		else
		{
			listOneIndex++;
			listTwoIndex++;
		}
	}

	while (listOneIndex<listOne.Size())
	{
		exclusiveToListOne.InsertAtEnd(listOne[listOneIndex]);
		listOneIndex++;
	}

	while (listTwoIndex<listTwo.Size())
	{
		exclusiveToListTwo.InsertAtEnd(listTwo[listTwoIndex]);
		listTwoIndex++;
	}
}
void Connection_RM2::SetConstructionByReplicaQuery(ReplicaManager2 *replicaManager)
{
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> constructedObjects;

	unsigned i;
	BooleanQueryResult res;
	for (i=0; i < replicaManager->variableConstructReplicaOrderedList.Size(); i++)
	{
		if (replicaManager->variableConstructReplicaOrderedList[i]->QueryIsConstructionAuthority())
		{
			res = replicaManager->variableConstructReplicaOrderedList[i]->QueryConstruction(this);
			if (res==BQR_YES || res==BQR_ALWAYS) // TODO - optimize ALWAYS here
				constructedObjects.InsertAtEnd(replicaManager->variableConstructReplicaOrderedList[i]);
		}
	}

	SetConstructionByList(constructedObjects, replicaManager);
}
void Connection_RM2::SetVisibilityByReplicaQuery(ReplicaManager2 *replicaManager)
{
	DataStructures::OrderedList<Replica2*, Replica2*, ReplicaManager2::Replica2ObjectComp> currentVisibility;

	unsigned i;
	BooleanQueryResult res;
	for (i=0; i < replicaManager->variableSerializeReplicaOrderedList.Size(); i++)
	{
		if (replicaManager->variableSerializeReplicaOrderedList[i]->QueryIsVisibilityAuthority())
		{
			res = replicaManager->variableSerializeReplicaOrderedList[i]->QueryVisibility(this);
			if (res==BQR_YES || res==BQR_ALWAYS) // TODO - optimize ALWAYS here
				currentVisibility.InsertAtEnd(replicaManager->variableSerializeReplicaOrderedList[i]);
		}
	}

	SetVisibilityByList(currentVisibility, replicaManager);
}
void Connection_RM2::SortInitialDownload( const DataStructures::List<Replica2*> &orderedDownloadList, DataStructures::List<Replica2*> &initialDownloadList ) {
		initialDownloadList = orderedDownloadList;
}
void Connection_RM2::SerializeDownloadStarted(RakNet::BitStream *objectData, ReplicaManager2 *replicaManager, SerializationContext *serializationContext) {
	(void) objectData;
	(void) replicaManager;
	(void) serializationContext;
}
void Connection_RM2::SerializeDownloadComplete(RakNet::BitStream *objectData, ReplicaManager2 *replicaManager, SerializationContext *serializationContext) {
	(void) objectData;
	(void) replicaManager;
	(void) serializationContext;
}
void Connection_RM2::DeserializeDownloadStarted(RakNet::BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType){
	(void) objectData;
	(void) sender;
	(void) replicaManager;
	(void) timestamp;
	(void) replicaManager;
	(void) serializationType;
}
void Connection_RM2::DeserializeDownloadComplete(RakNet::BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType){
	(void) objectData;
	(void) sender;
	(void) replicaManager;
	(void) timestamp;
	(void) serializationType;
}
Replica2 * Connection_RM2::ReceiveConstruct(RakNet::BitStream *replicaData, NetworkID networkId, SystemAddress sender, unsigned char localClientId, SerializationType type,
					ReplicaManager2 *replicaManager, RakNetTime timestamp,
					DataStructures::OrderedList<SystemAddress,SystemAddress> &exclusionList)
{
	Replica2 *obj=0;

	bool newReference=false;
	if (type==SEND_CONSTRUCTION_REPLY_ACCEPTED_TO_CLIENT || type==SEND_CONSTRUCTION_REPLY_DENIED_TO_CLIENT)
	{
		obj = Replica2::clientPtrArray[localClientId];
		if (obj)
		{
			// The prefix misaligns the data from the send, which is a problem if the user uses aligned data
			replicaData->AlignReadToByteBoundary();
			obj = obj->ReceiveConstructionReply(sender, replicaData, type==SEND_CONSTRUCTION_REPLY_ACCEPTED_TO_CLIENT);
			obj->SetNetworkID(networkId);
			replicaManager->Reference(obj, &newReference);
			replicaManager->AddConstructionReference(this,obj);

			// The object could not be serialized before because it didn't have a NetworkID. So serialize it now.
			if (obj->QueryIsSerializationAuthority() && (obj->QueryVisibility(this)==BQR_ALWAYS || obj->QueryVisibility(this)==BQR_YES))
			{
				SerializationContext sc;
				sc.recipientAddress=UNASSIGNED_SYSTEM_ADDRESS;
				sc.timestamp=timestamp;
				sc.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
				sc.serializationType=BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM;
				obj->BroadcastSerialize(&sc);

				replicaManager->AddVisibilityReference(this,obj);
			}
		}
	}
	else
	{
		// Create locally send back reply
		bool collision = replicaManager->GetRakPeer()->GetNetworkIDManager()->GET_OBJECT_FROM_ID<NetworkIDObject*>( networkId )!=0;
		replicaData->AlignReadToByteBoundary();
		obj = Construct(replicaData, sender, type, replicaManager, timestamp, networkId, collision);
		if (obj)
		{
			replicaManager->Reference(obj, &newReference);

			if (obj->AllowRemoteConstruction(sender, replicaData, type, timestamp)==false)
			{
				if (type==SEND_CONSTRUCTION_REQUEST_TO_SERVER)
					obj->SendConstruction(sender, SEND_CONSTRUCTION_REPLY_DENIED_TO_CLIENT);
				//RakNet::OP_DELETE(obj, __FILE__, __LINE__);
				delete obj;
				obj=0;
			}
			else
			{
				if (networkId!=UNASSIGNED_NETWORK_ID)
					obj->SetNetworkID(networkId);

				RakNet::BitStream bs;
				SerializationContext serializationContext;
				serializationContext.relaySourceAddress=sender;
				serializationContext.recipientAddress=UNASSIGNED_SYSTEM_ADDRESS;

				if (type==SEND_CONSTRUCTION_REQUEST_TO_SERVER)
					serializationContext.serializationType=BROADCAST_CONSTRUCTION_REQUEST_ACCEPTED_TO_SYSTEM;
				else 
					serializationContext.serializationType=BROADCAST_CONSTRUCTION_GENERIC_TO_SYSTEM;
				exclusionList.Insert(sender,sender,false);

				unsigned exclusionListIndex=0;
				for (unsigned i=0; i < replicaManager->connectionList.Size(); i++)
				{
					serializationContext.recipientAddress=replicaManager->connectionList[i]->GetSystemAddress();

					while (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex] < serializationContext.recipientAddress)
						exclusionListIndex++;
					if (exclusionListIndex < exclusionList.Size() && exclusionList[exclusionListIndex]==serializationContext.recipientAddress)
					{
						exclusionListIndex++;
						continue;
					}

					BooleanQueryResult queryConstruction = obj->QueryConstruction(0);
					if ( (queryConstruction==BQR_ALWAYS || queryConstruction==BQR_NEVER) &&
						replicaManager->autoUpdateConstruction )
					{
						// Relay autoUpdateConstruction is on, and the construction is not variable
						bs.Reset();
						if (obj->SerializeConstruction(&bs, &serializationContext)==false)
							continue;
						unsigned char localId;
						if (obj->QueryIsConstructionAuthority()==false)
							localId=Replica2::clientSharedID++;
						else
							localId=0;
						replicaManager->SendConstruction(obj,&bs,serializationContext.recipientAddress,serializationContext.timestamp,true,exclusionList, localId, serializationContext.serializationType);
					}
				}

				if (type==SEND_CONSTRUCTION_REQUEST_TO_SERVER)
				{
					DataStructures::OrderedList<SystemAddress,SystemAddress> emptyList;
					// // The prefix misaligns the data for the send, which is a problem if the user uses aligned data
					bs.AlignWriteToByteBoundary();
					replicaManager->SendConstruction(obj, &bs, sender, timestamp, true,
						emptyList, localClientId, SEND_CONSTRUCTION_REPLY_ACCEPTED_TO_CLIENT);
				}

				replicaData->AlignReadToByteBoundary();
				obj->OnConstructionComplete(replicaData, sender, type, replicaManager, timestamp, networkId, collision);
			}
		}
	}

	if (obj && newReference && obj->QueryIsSerializationAuthority() && obj->QueryVisibility(this)==BQR_ALWAYS)
	{
		SerializationContext sc;
		sc.recipientAddress=UNASSIGNED_SYSTEM_ADDRESS;
		sc.timestamp=timestamp;
		sc.relaySourceAddress=UNASSIGNED_SYSTEM_ADDRESS;
		sc.serializationType=BROADCAST_SERIALIZATION_GENERIC_TO_SYSTEM;
		obj->BroadcastSerialize(&sc);
	}

	return obj;
}
void Connection_RM2::SetSystemAddress(SystemAddress sa)
{
	systemAddress=sa;
}
SystemAddress Connection_RM2::GetSystemAddress(void) const
{
	return systemAddress;
}
void Connection_RM2::SetGuid(RakNetGUID guid)
{
	rakNetGuid=guid;
}
RakNetGUID Connection_RM2::GetGuid(void) const
{
	return rakNetGuid;
}
void Connection_RM2::Deref(Replica2* replica)
{
	lastConstructionList.RemoveIfExists(replica);
	lastSerializationList.RemoveIfExists(replica);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
