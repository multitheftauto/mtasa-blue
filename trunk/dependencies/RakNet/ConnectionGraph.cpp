/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "ConnectionGraph.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "GetTime.h"
#include <string.h>
#include "RakAssert.h"
#include "SHA1.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

static const int connectionGraphChannel=31;

ConnectionGraph::SystemAddressAndGroupId::SystemAddressAndGroupId()
{

}
ConnectionGraph::SystemAddressAndGroupId::~SystemAddressAndGroupId()
{

}
ConnectionGraph::SystemAddressAndGroupId::SystemAddressAndGroupId(SystemAddress _systemAddress, ConnectionGraphGroupID _groupID, RakNetGUID _guid)
{
	systemAddress=_systemAddress;
	groupId=_groupID;
	guid=_guid;
}
bool ConnectionGraph::SystemAddressAndGroupId::operator==( const ConnectionGraph::SystemAddressAndGroupId& right ) const
{
// Use system address, because the router plugin takes system addresses in the send list, rather than guids
	return systemAddress==right.systemAddress;
}
bool ConnectionGraph::SystemAddressAndGroupId::operator!=( const ConnectionGraph::SystemAddressAndGroupId& right ) const
{
	return systemAddress!=right.systemAddress;
}
bool ConnectionGraph::SystemAddressAndGroupId::operator > ( const ConnectionGraph::SystemAddressAndGroupId& right ) const
{
	return systemAddress>right.systemAddress;
}
bool ConnectionGraph::SystemAddressAndGroupId::operator < ( const ConnectionGraph::SystemAddressAndGroupId& right ) const
{
	return systemAddress<right.systemAddress;
}
ConnectionGraph::ConnectionGraph()
{
    pw=0;
	myGroupId=0;
	autoAddNewConnections=true;
//	forceBroadcastTime=0;
	DataStructures::WeightedGraph<ConnectionGraph::SystemAddressAndGroupId, unsigned short, false>::IMPLEMENT_DEFAULT_COMPARISON();
	DataStructures::OrderedList<SystemAddress, SystemAddress>::IMPLEMENT_DEFAULT_COMPARISON();
	DataStructures::OrderedList<ConnectionGraph::SystemAddressAndGroupId, ConnectionGraph::SystemAddressAndGroupId>::IMPLEMENT_DEFAULT_COMPARISON();
	DataStructures::OrderedList<ConnectionGraphGroupID, ConnectionGraphGroupID>::IMPLEMENT_DEFAULT_COMPARISON();

	subscribedGroups.Insert(0,0, true);
}

ConnectionGraph::~ConnectionGraph()
{
	if (pw)
		RakNet::OP_DELETE_ARRAY(pw, __FILE__, __LINE__);
}

void ConnectionGraph::SetPassword(const char *password)
{
	if (pw)
	{
		RakNet::OP_DELETE_ARRAY(pw, __FILE__, __LINE__);
		pw=0;
	}

	if (password && password[0])
	{
		RakAssert(strlen(password)<256);
		pw=(char*) rakMalloc_Ex( strlen(password)+1, __FILE__, __LINE__ );
		strcpy(pw, password);
	}
}
DataStructures::WeightedGraph<ConnectionGraph::SystemAddressAndGroupId, unsigned short, false> *ConnectionGraph::GetGraph(void)
{
	return &graph;
}
void ConnectionGraph::SetAutoAddNewConnections(bool autoAdd)
{
	autoAddNewConnections=autoAdd;
}
void ConnectionGraph::OnShutdown(void)
{
	graph.Clear();
	participantList.Clear();
//	forceBroadcastTime=0;
}
void ConnectionGraph::Update(void)
{

//	RakNetTime time = RakNet::GetTime();

	// If the time is past the next weight update time, then refresh all pings of all connected participants and send these out if substantially different.
//	if (forceBroadcastTime && time > forceBroadcastTime)
//	{
//		DataStructures::OrderedList<SystemAddress,SystemAddress> none;
	//	BroadcastGraphUpdate(none, peer);
//		forceBroadcastTime=0;
//	}
}
void ConnectionGraph::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	if (isIncoming==false)
	{
		if (autoAddNewConnections==false)
			return;

		RequestConnectionGraph(systemAddress);

		// 0 is the default groupId
		AddNewConnection(rakPeerInterface, systemAddress, rakNetGUID, 0);
	}
	else
	{
		if (autoAddNewConnections==false)
			return;

		// 0 is the default groupId
		AddNewConnection(rakPeerInterface, systemAddress, rakNetGUID, 0);
	}
}
PluginReceiveResult ConnectionGraph::OnReceive(Packet *packet)
{
	switch (packet->data[0]) 
	{
	case ID_CONNECTION_GRAPH_REQUEST:
		OnConnectionGraphRequest(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_CONNECTION_GRAPH_REPLY:
		OnConnectionGraphReply(packet);
		return RR_CONTINUE_PROCESSING;
	case ID_CONNECTION_GRAPH_UPDATE:
		OnConnectionGraphUpdate(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_CONNECTION_GRAPH_NEW_CONNECTION:
		OnNewConnectionInternal(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
		// Remove connection lost
	case ID_CONNECTION_GRAPH_CONNECTION_LOST:
	case ID_CONNECTION_GRAPH_DISCONNECTION_NOTIFICATION:
		if (OnConnectionLostInternal(packet, packet->data[0]))
		{
			if (packet->data[0]==ID_CONNECTION_GRAPH_CONNECTION_LOST)
				packet->data[0]=ID_REMOTE_CONNECTION_LOST;
			else
				packet->data[0]=ID_REMOTE_DISCONNECTION_NOTIFICATION;
			return RR_CONTINUE_PROCESSING; // Return this packet to the user
		}		
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}

	return RR_CONTINUE_PROCESSING;
}
void ConnectionGraph::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) rakNetGUID;

	if (lostConnectionReason==LCR_CLOSED_BY_USER || lostConnectionReason==LCR_DISCONNECTION_NOTIFICATION)
		HandleDroppedConnection(rakPeerInterface, systemAddress, ID_CONNECTION_GRAPH_DISCONNECTION_NOTIFICATION);
	else
		HandleDroppedConnection(rakPeerInterface, systemAddress, ID_CONNECTION_GRAPH_CONNECTION_LOST);
}

void ConnectionGraph::HandleDroppedConnection(RakPeerInterface *peer, SystemAddress systemAddress, unsigned char packetId)
{
	RakAssert(peer);
	RemoveParticipant(systemAddress);
	DataStructures::OrderedList<SystemAddress,SystemAddress> ignoreList;
	RemoveAndRelayConnection(ignoreList, packetId, systemAddress, peer->GetExternalID(systemAddress), peer);
}
void ConnectionGraph::SetGroupId(ConnectionGraphGroupID groupId)
{
	myGroupId=groupId;
}
void ConnectionGraph::AddNewConnection(RakPeerInterface *peer, SystemAddress systemAddress, RakNetGUID guid, ConnectionGraphGroupID groupId)
{
	if (autoAddNewConnections==false && subscribedGroups.HasData(groupId)==false)
		return;

	DataStructures::OrderedList<SystemAddress,SystemAddress> ignoreList;
	
	SystemAddressAndGroupId first, second;
	first.systemAddress=systemAddress;
	first.groupId=groupId;
	first.guid=guid;
	second.systemAddress=peer->GetExternalID(systemAddress);
	second.groupId=myGroupId;
	second.guid=peer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);

	AddAndRelayConnection(ignoreList, first, second, (unsigned short)peer->GetAveragePing(systemAddress), peer);
}
void ConnectionGraph::SubscribeToGroup(ConnectionGraphGroupID groupId)
{
	subscribedGroups.Insert(groupId, groupId, true);
}
void ConnectionGraph::UnsubscribeFromGroup(ConnectionGraphGroupID groupId)
{
	subscribedGroups.Remove(groupId);
}
void ConnectionGraph::RequestConnectionGraph(SystemAddress systemAddress)
{
	RakNet::BitStream outBitstream;
	outBitstream.Write((MessageID)ID_CONNECTION_GRAPH_REQUEST);
	stringCompressor->EncodeString(pw,256,&outBitstream);
	rakPeerInterface->Send(&outBitstream, LOW_PRIORITY, RELIABLE_ORDERED, connectionGraphChannel, systemAddress, false);

#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
	RAKNET_DEBUG_PRINTF("ID_CONNECTION_GRAPH_REQUEST from %i to %i\n", peer->GetInternalID().port, systemAddress.port);
#endif
}
void ConnectionGraph::OnConnectionGraphRequest(Packet *packet)
{
	char password[256];
	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	inBitstream.IgnoreBits(8);
	stringCompressor->DecodeString(password,256,&inBitstream);
	if (pw && pw[0] && strcmp(pw, password)!=0)
		return;

#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
	RAKNET_DEBUG_PRINTF("ID_CONNECTION_GRAPH_REPLY ");
#endif

	RakNet::BitStream outBitstream;
	outBitstream.Write((MessageID)ID_CONNECTION_GRAPH_REPLY);
	stringCompressor->EncodeString(pw,256,&outBitstream);
	SerializeWeightedGraph(&outBitstream, graph);
	SendUnified(&outBitstream, LOW_PRIORITY, RELIABLE_ORDERED, connectionGraphChannel, packet->systemAddress, false);

#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
	RAKNET_DEBUG_PRINTF("from %i to %i\n", peer->GetInternalID().port, packet->systemAddress.port);
#endif

	// Add packet->systemAddress to the participant list if it is not already there
	AddParticipant(packet->systemAddress);
}
void ConnectionGraph::OnConnectionGraphReply(Packet *packet)
{
	unsigned char password[256];
	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	inBitstream.IgnoreBits(8);
	stringCompressor->DecodeString((char*)password,256,&inBitstream);
	if (pw && pw[0] && strcmp(pw, (const char*)password)!=0)
		return;

	// Serialize the weighted graph and send it to them
	RakNet::BitStream outBitstream;
	outBitstream.Write((MessageID)ID_CONNECTION_GRAPH_UPDATE);

#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
	RAKNET_DEBUG_PRINTF("ID_CONNECTION_GRAPH_UPDATE ");
#endif

	// Send our current graph to the sender
	SerializeWeightedGraph(&outBitstream, graph);


	// Write the systems that have processed this graph so we don't resend to these systems
	outBitstream.Write((unsigned short) 1);
	outBitstream.Write(rakPeerInterface->GetExternalID(packet->systemAddress));

#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
	RAKNET_DEBUG_PRINTF("from %i to %i\n", peer->GetInternalID().port, packet->systemAddress.port);
#endif

	SendUnified(&outBitstream, LOW_PRIORITY, RELIABLE_ORDERED, connectionGraphChannel, packet->systemAddress, false);

	// Add packet->systemAddress to the participant list if it is not already there
	AddParticipant(packet->systemAddress);

	if (DeserializeWeightedGraph(&inBitstream, rakPeerInterface)==false)
		return;

	// Forward the updated graph to all current participants
	DataStructures::OrderedList<SystemAddress,SystemAddress> ignoreList;
	ignoreList.Insert(packet->systemAddress,packet->systemAddress, true);
	BroadcastGraphUpdate(ignoreList, rakPeerInterface);
}
void ConnectionGraph::OnConnectionGraphUpdate(Packet *packet)
{
	// Only accept from participants
	if (participantList.HasData(packet->systemAddress)==false)
		return;

	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	inBitstream.IgnoreBits(8);
	
	if (DeserializeWeightedGraph(&inBitstream, rakPeerInterface)==false)
		return;

	DataStructures::OrderedList<SystemAddress,SystemAddress> ignoreList;
	DeserializeIgnoreList(ignoreList, &inBitstream);

	// Forward the updated graph to all participants.
	ignoreList.Insert(packet->systemAddress,packet->systemAddress, false);
	BroadcastGraphUpdate(ignoreList, rakPeerInterface);
}
void ConnectionGraph::OnNewConnectionInternal(Packet *packet)
{
	// Only accept from participants
	if (participantList.HasData(packet->systemAddress)==false)
		return;

	SystemAddressAndGroupId node1, node2;
	unsigned short ping;
	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	inBitstream.IgnoreBits(8);
	inBitstream.Read(node1.systemAddress);
	inBitstream.Read(node1.groupId);
	inBitstream.Read(node1.guid);
	inBitstream.Read(node2.systemAddress);
	inBitstream.Read(node2.groupId);
	inBitstream.Read(node2.guid);
	if (inBitstream.Read(ping)==false)
		return;
	DataStructures::OrderedList<SystemAddress,SystemAddress> ignoreList;
	DeserializeIgnoreList(ignoreList, &inBitstream);
	ignoreList.Insert(packet->systemAddress,packet->systemAddress, false);
	AddAndRelayConnection(ignoreList, node1, node2, ping, rakPeerInterface);	
}
bool ConnectionGraph::OnConnectionLostInternal(Packet *packet, unsigned char packetId)
{
	// Only accept from participants
	if (participantList.HasData(packet->systemAddress)==false)
		return false;

	SystemAddress node1, node2;
	RakNet::BitStream inBitstream(packet->data, packet->length, false);
	inBitstream.IgnoreBits(8);
	// This is correct - group IDs are not written for removal, only addition.
	inBitstream.Read(node1);
	if (inBitstream.Read(node2)==false)
		return false;
	DataStructures::OrderedList<SystemAddress,SystemAddress> ignoreList;
	DeserializeIgnoreList(ignoreList, &inBitstream);
	ignoreList.Insert(packet->systemAddress, packet->systemAddress, false);
	
	return RemoveAndRelayConnection(ignoreList, packetId, node1, node2, rakPeerInterface);
}
bool ConnectionGraph::DeserializeIgnoreList(DataStructures::OrderedList<SystemAddress,SystemAddress> &ignoreList, RakNet::BitStream *inBitstream )
{
	unsigned short count;
	SystemAddress temp;
	unsigned i;
	inBitstream->Read(count);
	for (i=0; i < count; i++)
	{
		if (inBitstream->Read(temp)==false)
		{
			RakAssert(0);
			return false;
		}
		ignoreList.Insert(temp,temp, false);
	}
	return true;
}
void ConnectionGraph::SerializeWeightedGraph(RakNet::BitStream *out, const DataStructures::WeightedGraph<ConnectionGraph::SystemAddressAndGroupId, unsigned short, false> &g) const
{
	unsigned nodeIndex, connectionIndex;
	BitSize_t countOffset, oldOffset;
	unsigned short count;
	SystemAddressAndGroupId node1, node2;
	unsigned short weight;
	out->WriteCompressed(g.GetNodeCount());
	for (nodeIndex=0; nodeIndex < g.GetNodeCount(); nodeIndex++)
	{
		// Write the node
		node1=g.GetNodeAtIndex(nodeIndex);
#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
		RAKNET_DEBUG_PRINTF("[%i] ", node1.systemAddress.port);
#endif
		out->Write(node1.systemAddress);
		out->Write(node1.groupId);
		out->Write(node1.guid);

		// Write the adjacency list count
		count=(unsigned short)g.GetConnectionCount(nodeIndex);
		out->AlignWriteToByteBoundary();
		countOffset=out->GetWriteOffset();
		out->Write(count);
		count=0;
		for (connectionIndex=0; connectionIndex < g.GetConnectionCount(nodeIndex); connectionIndex++)
		{
			g.GetConnectionAtIndex(nodeIndex, connectionIndex, node2, weight);
			// For efficiencies' sake, only serialize the upper half of the connection pairs
			if (node2 > node1)
			{
				count++;
				out->Write(node2.systemAddress);
				out->Write(node2.groupId);
				out->Write(node2.guid);
				out->Write(weight);

#ifdef _CONNECTION_GRAPH_DEBUG_PRINT
				RAKNET_DEBUG_PRINTF("(%i) ", node2.systemAddress.port);
#endif
			}
		}

		// Go back and change how many elements were written
		oldOffset=out->GetWriteOffset();
		out->SetWriteOffset(countOffset);
		out->Write(count);
		out->SetWriteOffset(oldOffset);
	}
}
bool ConnectionGraph::DeserializeWeightedGraph(RakNet::BitStream *inBitstream, RakPeerInterface *peer)
{
	unsigned nodeCount, nodeIndex, connectionIndex;
	unsigned short connectionCount;
	SystemAddressAndGroupId node, connection;
	bool anyConnectionsNew=false;
	unsigned short weight;
	inBitstream->ReadCompressed(nodeCount);
	for (nodeIndex=0; nodeIndex < nodeCount; nodeIndex++)
	{
		inBitstream->Read(node.systemAddress);
		inBitstream->Read(node.groupId);
		inBitstream->Read(node.guid);

		inBitstream->AlignReadToByteBoundary();
		if (inBitstream->Read(connectionCount)==false)
		{
			RakAssert(0);
			return false;
		}
		for (connectionIndex=0; connectionIndex < connectionCount; connectionIndex++)
		{
			inBitstream->Read(connection.systemAddress);
			inBitstream->Read(connection.groupId);
			inBitstream->Read(connection.guid);
			if (inBitstream->Read(weight)==false)
			{
				RakAssert(0);
				return false;
			}
			if (subscribedGroups.HasData(connection.groupId)==false ||
				subscribedGroups.HasData(node.groupId)==false)
				continue;
			RakAssert(node.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
			RakAssert(connection.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
			if (IsNewRemoteConnection(node,connection,peer))
				NotifyUserOfRemoteConnection(node,connection,weight,peer);

			if (graph.HasConnection(node,connection)==false)
				anyConnectionsNew=true;

			graph.AddConnection(node,connection,weight);
		}
	}
	return anyConnectionsNew;
}
void ConnectionGraph::RemoveParticipant(SystemAddress systemAddress)
{
	unsigned index;
	bool objectExists;
	index=participantList.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists)
		participantList.RemoveAtIndex(index);
}

void ConnectionGraph::AddParticipant(SystemAddress systemAddress)
{
	participantList.Insert(systemAddress,systemAddress, false);
}

void ConnectionGraph::AddAndRelayConnection(DataStructures::OrderedList<SystemAddress,SystemAddress> &ignoreList, const SystemAddressAndGroupId &conn1, const SystemAddressAndGroupId &conn2, unsigned short ping, RakPeerInterface *peer)
{
	if (graph.HasConnection(conn1,conn2))
		return;

	if (ping==65535)
		ping=0;
	RakAssert(conn1.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
	RakAssert(conn2.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);

	if (IsNewRemoteConnection(conn1,conn2,peer))
	{
		NotifyUserOfRemoteConnection(conn1,conn2,ping,peer);

		// What was this return here for?
	//	return;
	}

	graph.AddConnection(conn1,conn2,ping);

	RakNet::BitStream outBitstream;
	outBitstream.Write((MessageID)ID_CONNECTION_GRAPH_NEW_CONNECTION);
	outBitstream.Write(conn1.systemAddress);
	outBitstream.Write(conn1.groupId);
	outBitstream.Write(conn1.guid);
	outBitstream.Write(conn2.systemAddress);
	outBitstream.Write(conn2.groupId);
	outBitstream.Write(conn2.guid);
	outBitstream.Write(ping);
	ignoreList.Insert(conn2.systemAddress,conn2.systemAddress, false);
	ignoreList.Insert(conn1.systemAddress,conn1.systemAddress, false);
	SerializeIgnoreListAndBroadcast(&outBitstream, ignoreList, peer);
}
bool ConnectionGraph::RemoveAndRelayConnection(DataStructures::OrderedList<SystemAddress,SystemAddress> &ignoreList, unsigned char packetId, const SystemAddress node1, const SystemAddress node2, RakPeerInterface *peer)
{
	SystemAddressAndGroupId n1, n2;
	n1.systemAddress=node1;
	n2.systemAddress=node2;
	if (graph.HasConnection(n1,n2)==false)
		return false;
	graph.RemoveConnection(n1,n2);

	// TODO - clear islands

	RakNet::BitStream outBitstream;
	outBitstream.Write(packetId);
	outBitstream.Write(node1);
	outBitstream.Write(node2);

	ignoreList.Insert(node1,node1, false);
	ignoreList.Insert(node2,node2, false);
	SerializeIgnoreListAndBroadcast(&outBitstream, ignoreList, peer);

	return true;
}

void ConnectionGraph::BroadcastGraphUpdate(DataStructures::OrderedList<SystemAddress,SystemAddress> &ignoreList, RakPeerInterface *peer)
{
	RakNet::BitStream outBitstream;
	outBitstream.Write((MessageID)ID_CONNECTION_GRAPH_UPDATE);
	SerializeWeightedGraph(&outBitstream, graph);
	SerializeIgnoreListAndBroadcast(&outBitstream, ignoreList, peer);
}
void ConnectionGraph::SerializeIgnoreListAndBroadcast(RakNet::BitStream *outBitstream, DataStructures::OrderedList<SystemAddress,SystemAddress> &ignoreList, RakPeerInterface *peer)
{
	DataStructures::List<SystemAddress> sendList;
	unsigned i;
	for (i=0; i < participantList.Size(); i++)
	{
		if (ignoreList.HasData(participantList[i])==false)
			sendList.Insert(participantList[i], __FILE__, __LINE__);
	}
	if (sendList.Size()==0)
		return;

	SystemAddress self = peer->GetExternalID(sendList[0]);
	ignoreList.Insert(self,self, false);
	outBitstream->Write((unsigned short) (ignoreList.Size()+sendList.Size()));
	for (i=0; i < ignoreList.Size(); i++)
		outBitstream->Write(ignoreList[i]);
	for (i=0; i < sendList.Size(); i++)
		outBitstream->Write(sendList[i]);

	for (i=0; i < sendList.Size(); i++)
	{
		peer->Send(outBitstream, LOW_PRIORITY, RELIABLE_ORDERED, connectionGraphChannel, sendList[i], false);
	}
}
bool ConnectionGraph::IsNewRemoteConnection(const SystemAddressAndGroupId &conn1, const SystemAddressAndGroupId &conn2,RakPeerInterface *peer)
{
	if (graph.HasConnection(conn1,conn2)==false &&
		subscribedGroups.HasData(conn1.groupId) &&
		subscribedGroups.HasData(conn2.groupId) &&
		(peer->IsConnected(conn1.systemAddress)==false || peer->IsConnected(conn2.systemAddress)==false))
	{
		SystemAddress externalId1, externalId2;
		externalId1=peer->GetExternalID(conn1.systemAddress);
		externalId2=peer->GetExternalID(conn2.systemAddress);
		return (externalId1!=conn1.systemAddress && externalId1!=conn2.systemAddress &&
			externalId2!=conn1.systemAddress && externalId2!=conn2.systemAddress);
	}
	return false;
}
void ConnectionGraph::NotifyUserOfRemoteConnection(const SystemAddressAndGroupId &conn1, const SystemAddressAndGroupId &conn2,unsigned short ping, RakPeerInterface *peer)
{
	// Create a packet to tell the user of this event
	static const int length=sizeof(MessageID) + (sizeof(SystemAddress) + sizeof(ConnectionGraphGroupID) + sizeof(RakNetGUID)) * 2 + sizeof(unsigned short);
	Packet *p = peer->AllocatePacket(length);
	RakNet::BitStream b(p->data, length, false);
	p->bitSize=p->length*8;
	b.SetWriteOffset(0);
	b.Write((MessageID)ID_REMOTE_NEW_INCOMING_CONNECTION);
	b.Write(conn1.systemAddress);
	b.Write(conn1.groupId);
	b.Write(conn1.guid);
	b.Write(conn2.systemAddress);
	b.Write(conn2.groupId);
	b.Write(conn2.guid);
	b.Write(ping);
	if (peer->IsConnected(conn2.systemAddress)==false)
	{
		p->systemAddress=conn2.systemAddress;
		p->guid=conn2.guid;
	}
	else
	{
		p->systemAddress=conn1.systemAddress;
		p->guid=conn1.guid;
	}
	peer->PushBackPacket(p, false);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
