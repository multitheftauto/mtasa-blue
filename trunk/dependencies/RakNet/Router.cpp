#include "Router.h"
#include "BitStream.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakAssert.h"

//#define _DO_PRINTF

#ifdef _DO_PRINTF
#include <stdio.h>
#endif

#ifdef _MSC_VER
#pragma warning( push )
#endif

Router::Router()
{
	graph=0;
	restrictByType=false;
	DataStructures::OrderedList<unsigned char,unsigned char>::IMPLEMENT_DEFAULT_COMPARISON();
}
Router::~Router()
{
}
void Router::SetRestrictRoutingByType(bool restrict__)
{
	restrictByType=restrict__;
}
void Router::AddAllowedType(unsigned char messageId)
{
	if (allowedTypes.HasData(messageId)==false)
		allowedTypes.Insert(messageId,messageId, true);
}
void Router::RemoveAllowedType(unsigned char messageId)
{
	if (allowedTypes.HasData(messageId)==true)
		allowedTypes.Remove(messageId);
}
void Router::SetConnectionGraph(DataStructures::WeightedGraph<ConnectionGraph::SystemAddressAndGroupId, unsigned short, false> *connectionGraph)
{
	graph=connectionGraph;
}
bool Router::Send( const char *data, BitSize_t bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress )
{
	if (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		RakAssert(data);
		RakAssert(bitLength);
		// Prevent recursion in case a routing call itself calls the router
		if (bitLength>=8 && data[0]==ID_ROUTE_AND_MULTICAST)
			return false;

		SystemAddressList systemAddressList;
		systemAddressList.AddSystem(systemAddress);
		return Send((char*)data, bitLength, priority, reliability, orderingChannel, &systemAddressList);
	}
	return false;	
}
bool Router::Send( char *data, BitSize_t bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddressList *recipients )
{
	RakAssert(data);
	RakAssert(bitLength);
	if (recipients->GetList()->Size()==0)
		return false;
	if (bitLength==0)
		return false;
	DataStructures::Tree<ConnectionGraph::SystemAddressAndGroupId> tree;
	SystemAddress root;
	root = rakPeerInterface->GetExternalID(rakPeerInterface->GetSystemAddressFromIndex(0));
	if (root==UNASSIGNED_SYSTEM_ADDRESS)
		return false;
	DataStructures::List<ConnectionGraph::SystemAddressAndGroupId> recipientList;
	unsigned i;
	for (i=0; i < recipients->Size(); i++)
		recipientList.Insert(ConnectionGraph::SystemAddressAndGroupId(recipients->GetList()->operator [](i),0, UNASSIGNED_RAKNET_GUID), __FILE__, __LINE__);
	if (graph->GetSpanningTree(tree, &recipientList, ConnectionGraph::SystemAddressAndGroupId(root,0,UNASSIGNED_RAKNET_GUID), 65535)==false)
		return false;

	RakNet::BitStream out;

	// Write timestamp first, if the user had a timestamp
	if (data[0]==ID_TIMESTAMP && bitLength >= BYTES_TO_BITS(sizeof(MessageID)+sizeof(RakNetTime)))
	{
		out.Write(data, sizeof(MessageID)+sizeof(RakNetTime));
		data+=sizeof(MessageID)+sizeof(RakNetTime);
		bitLength-=BYTES_TO_BITS(sizeof(MessageID)+sizeof(RakNetTime));
	}

	SendTree(priority, reliability, orderingChannel, &tree, data, bitLength, &out, recipients);
	return true;
}
void Router::SendTree(PacketPriority priority, PacketReliability reliability, char orderingChannel, DataStructures::Tree<ConnectionGraph::SystemAddressAndGroupId> *tree, const char *data, BitSize_t bitLength, RakNet::BitStream *out, SystemAddressList *recipients)
{
	BitSize_t outputOffset;

	// Write routing identifer
	out->Write((MessageID)ID_ROUTE_AND_MULTICAST);

	// Write the send parameters
	out->WriteCompressed((unsigned char)priority);
	out->WriteCompressed((unsigned char)reliability);
	out->WriteCompressed((unsigned char)orderingChannel);

	// Write the user payload length
	out->Write((unsigned int)bitLength);
//	out->PrintBits();
//	payload->PrintBits();

	out->AlignWriteToByteBoundary();
//	payload->AlignReadToByteBoundary();
//	out->Write(payload, payload->GetNumberOfUnreadBits());
//	out->PrintBits();
	if ((bitLength % 8)==0)
		out->Write(data, BITS_TO_BYTES(bitLength));
	else
		out->WriteBits((const unsigned char*)data, bitLength, false);

	// Save where to start writing per-system data
	outputOffset=out->GetWriteOffset();

	// Write every child of the root of the tree (SystemAddress, isRecipient, branch)
	unsigned i;
	for (i=0; i < tree->children.Size(); i++)
	{
		// Start writing at the per-system data byte
		out->SetWriteOffset(outputOffset);

		// Write our external IP to designate the sender
		out->Write(rakPeerInterface->GetExternalID(tree->children[i]->data.systemAddress));

		// Serialize the tree
		SerializePreorder(tree->children[i], out, recipients);

		// Send to the first hop
#ifdef _DO_PRINTF
		RAKNET_DEBUG_PRINTF("%i sending to %i\n", rakPeerInterface->GetExternalID(tree->children[i]->data.systemAddress).port, tree->children[i]->data.systemAddress.port);
#endif
		SendUnified(out, priority, reliability, orderingChannel, tree->children[i]->data.systemAddress, false);
	}
}
PluginReceiveResult Router::OnReceive(Packet *packet)
{
	if (packet->data[0]==ID_ROUTE_AND_MULTICAST ||
		(packet->length>5 && packet->data[0]==ID_TIMESTAMP && packet->data[5]==ID_ROUTE_AND_MULTICAST))
	{
#ifdef _DO_PRINTF
		RAKNET_DEBUG_PRINTF("%i got routed message from %i\n", peer->GetExternalID(packet->systemAddress).port, packet->systemAddress.port);
#endif
		RakNetTime timestamp;
		PacketPriority priority;
		PacketReliability reliability;
		unsigned char orderingChannel;
		SystemAddress originalSender;
		RakNet::BitStream out;
		BitSize_t outStartingOffset;
		unsigned int payloadBitLength;
		unsigned payloadWriteByteOffset;
		RakNet::BitStream incomingBitstream(packet->data, packet->length, false);
		incomingBitstream.IgnoreBits(8);
		
		if (packet->data[0]==ID_TIMESTAMP)
		{
			incomingBitstream.Read(timestamp);
			out.Write((MessageID)ID_TIMESTAMP);
			out.Write(timestamp);
			incomingBitstream.IgnoreBits(8);
		}

		// Read the send parameters
		unsigned char c;
		incomingBitstream.ReadCompressed(c);
		priority=(PacketPriority)c;
		incomingBitstream.ReadCompressed(c);
		reliability=(PacketReliability)c;
		incomingBitstream.ReadCompressed(orderingChannel);
		incomingBitstream.Read(payloadBitLength);
		
		out.Write((MessageID)ID_ROUTE_AND_MULTICAST);
		out.WriteCompressed((unsigned char)priority);
		out.WriteCompressed((unsigned char)reliability);
		out.WriteCompressed((unsigned char)orderingChannel);
		out.Write(payloadBitLength);
		out.AlignWriteToByteBoundary();
		incomingBitstream.AlignReadToByteBoundary();
		payloadWriteByteOffset=(unsigned int) BITS_TO_BYTES(out.GetWriteOffset());
		out.Write(&incomingBitstream, payloadBitLength); // This write also does a read on incomingBitStream

		if (restrictByType)
		{
			RakNet::BitStream t(out.GetData()+payloadWriteByteOffset, sizeof(unsigned char), false);
			MessageID messageID;
			t.Read(messageID);
			if (allowedTypes.HasData(messageID)==false)
				return RR_STOP_PROCESSING_AND_DEALLOCATE; // Don't route restricted types
		}

		incomingBitstream.Read(originalSender);
		out.Write(originalSender);
		outStartingOffset=out.GetWriteOffset();

		// Deserialize the root
		bool hasData=false;
		SystemAddress recipient;
		unsigned short numberOfChildren;
		incomingBitstream.Read(hasData);
		incomingBitstream.Read(recipient); // This should be our own address
		if (incomingBitstream.ReadCompressed(numberOfChildren)==false)
		{
#ifdef _DEBUG
			RakAssert(0);
#endif
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}

		unsigned childIndex;
		bool childHasData=false;
		SystemAddress childRecipient;
		unsigned short childNumberOfChildren;
		SystemAddress immediateRecipient;
		immediateRecipient=UNASSIGNED_SYSTEM_ADDRESS;
		int pendingNodeCount=0;

		for (childIndex=0; childIndex < numberOfChildren; childIndex++)
		{
			while (pendingNodeCount!=-1)
			{
				// Copy out the serialized subtree for this child
				incomingBitstream.Read(childHasData);
				incomingBitstream.Read(childRecipient);
				if (!incomingBitstream.ReadCompressed(childNumberOfChildren))
					return RR_STOP_PROCESSING_AND_DEALLOCATE;
				if (immediateRecipient==UNASSIGNED_SYSTEM_ADDRESS)
				{
					immediateRecipient=childRecipient;
				}

				pendingNodeCount+=childNumberOfChildren-1;

				out.Write(childHasData);
				out.Write(childRecipient);
				out.WriteCompressed(childNumberOfChildren);
			}

#ifdef _DO_PRINTF
			RAKNET_DEBUG_PRINTF("%i routing to %i\n", peer->GetExternalID(packet->systemAddress).port, immediateRecipient.port);
#endif

			// Send what we got so far
			SendUnified(&out, priority, reliability, orderingChannel, immediateRecipient, false);

			// Restart writing the per recipient data
			out.SetWriteOffset(outStartingOffset);

			// Reread the top level node
			immediateRecipient=UNASSIGNED_SYSTEM_ADDRESS;

			pendingNodeCount=0;

		}

		// Write the user payload to the packet struct if this is a destination and change the sender and return true
		if (hasData)
		{
#ifdef _DO_PRINTF
			RAKNET_DEBUG_PRINTF("%i returning payload to user\n", peer->GetExternalID(packet->systemAddress).port);
#endif

			if (packet->data[0]==ID_TIMESTAMP )
			{
				memcpy( packet->data + sizeof(RakNetTime)+sizeof(unsigned char), out.GetData()+payloadWriteByteOffset, BITS_TO_BYTES(payloadBitLength) );
				packet->bitSize=BYTES_TO_BITS(sizeof(RakNetTime)+sizeof(unsigned char))+payloadBitLength;
			}
			else
			{
				memcpy( packet->data, out.GetData()+payloadWriteByteOffset, BITS_TO_BYTES(payloadBitLength) );
				packet->bitSize=payloadBitLength;
			}
			packet->length=(unsigned int) BITS_TO_BYTES(packet->bitSize);
			packet->systemIndex=65535;
			packet->systemAddress=originalSender;

			return RR_CONTINUE_PROCESSING;
		}

		// Absorb
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}

	return RR_CONTINUE_PROCESSING;
}
void Router::OnAttach(void)
{
	rakPeerInterface->SetRouterInterface(this);
}
void Router::OnDetach(void)
{
	rakPeerInterface->RemoveRouterInterface(this);
}
void Router::SerializePreorder(DataStructures::Tree<ConnectionGraph::SystemAddressAndGroupId> *tree, RakNet::BitStream *out, SystemAddressList *recipients) const
{
	unsigned i;
	out->Write((bool) (recipients->GetList()->GetIndexOf(tree->data.systemAddress)!=MAX_UNSIGNED_LONG));
	out->Write(tree->data.systemAddress);
	out->WriteCompressed((unsigned short) tree->children.Size());
	for (i=0; i < tree->children.Size(); i++)
		SerializePreorder(tree->children[i], out, recipients);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
