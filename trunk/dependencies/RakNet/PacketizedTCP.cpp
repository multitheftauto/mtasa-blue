#include "PacketizedTCP.h"
#include "NativeTypes.h"
#include "BitStream.h"

typedef uint32_t PTCPHeader;

PacketizedTCP::PacketizedTCP()
{

}
PacketizedTCP::~PacketizedTCP()
{
	ClearAllConnections();
}

bool PacketizedTCP::Start(unsigned short port, unsigned short maxIncomingConnections)
{
	bool success = TCPInterface::Start(port, maxIncomingConnections);
	if (success)
	{
		unsigned int i;
		for (i=0; i < messageHandlerList.Size(); i++)
			messageHandlerList[i]->OnStartup();
	}
	return success;
}

void PacketizedTCP::Stop(void)
{
	unsigned int i;
	for (i=0; i < messageHandlerList.Size(); i++)
		messageHandlerList[i]->OnShutdown();
	TCPInterface::Stop();
}

void PacketizedTCP::Send( const char *data, unsigned length, SystemAddress systemAddress, bool broadcast )
{
	unsigned int copyLength = length + sizeof(PTCPHeader);
	PTCPHeader dataLength;
	dataLength=length;
#ifndef __BITSTREAM_NATIVE_END
	if (RakNet::BitStream::DoEndianSwap())
		RakNet::BitStream::ReverseBytes((unsigned char*) &length,(unsigned char*) &dataLength,sizeof(dataLength));
#else
		dataLength=length;
#endif
	char *dataCopy = (char*) rakMalloc_Ex(copyLength, __FILE__, __LINE__);
	if (dataCopy==0)
	{
		notifyOutOfMemory(__FILE__, __LINE__);
		return;
	}
	memcpy(dataCopy, &dataLength, sizeof(PTCPHeader));
	memcpy(dataCopy+sizeof(PTCPHeader), data, length);
	TCPInterface::Send(dataCopy, copyLength, systemAddress, broadcast);
	rakFree_Ex(dataCopy, __FILE__, __LINE__);
}
bool PacketizedTCP::SendList( char **data, const int *lengths, const int numParameters, SystemAddress systemAddress, bool broadcast )
{
	if (isStarted==false)
		return false;
	if (remoteClients.Size()==0)
		return false;
	if (data==0)
		return false;
	if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS && broadcast==false)
		return false;
	unsigned int totalLengthToTransmit=0, totalLengthOfUserData=0;
	unsigned int lengthOffset;
	int i;
	for (i=0; i < numParameters; i++)
	{
		if (lengths[i]>0)
			totalLengthOfUserData+=lengths[i];
	}
	if (totalLengthOfUserData==0)
		return false;

	PTCPHeader dataLengthHeader;
#ifndef __BITSTREAM_NATIVE_END
	if (RakNet::BitStream::DoEndianSwap())
		RakNet::BitStream::ReverseBytes((unsigned char*) &totalLengthOfUserData,(unsigned char*) &dataLengthHeader,sizeof(dataLengthHeader));
#else
	dataLengthHeader=totalLengthOfUserData;
#endif
	totalLengthToTransmit=totalLengthOfUserData+sizeof(PTCPHeader);


	OutgoingMessage *p;
	p=outgoingMessages.WriteLock();
	p->length=totalLengthToTransmit;
	p->data = (unsigned char*) rakMalloc_Ex( totalLengthToTransmit, __FILE__, __LINE__ );
	memcpy(p->data, &dataLengthHeader, sizeof(PTCPHeader));
	lengthOffset=sizeof(PTCPHeader);
	for (i=0; i < numParameters; i++)
	{
		if (lengths[i]>0)
		{
			memcpy(p->data+lengthOffset, data[i], lengths[i]);
			lengthOffset+=lengths[i];
		}
	}
	p->systemAddress=systemAddress;
	p->broadcast=broadcast;
	outgoingMessages.WriteUnlock();
	
	return true;
}
void PacketizedTCP::PushNotificationsToQueues(void)
{
	SystemAddress sa;
	sa = TCPInterface::HasNewIncomingConnection();
	if (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		_newIncomingConnections.Push(sa);
		AddToConnectionList(sa);
		unsigned int i;
		for (i=0; i < messageHandlerList.Size(); i++)
			messageHandlerList[i]->OnNewConnection(sa, UNASSIGNED_RAKNET_GUID, true);
	}

	sa = TCPInterface::HasFailedConnectionAttempt();
	if (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		_failedConnectionAttempts.Push(sa);
		unsigned int i;
		for (i=0; i < messageHandlerList.Size(); i++)
			messageHandlerList[i]->OnFailedConnectionAttempt(sa, FCAR_CONNECTION_ATTEMPT_FAILED);
	}

	sa = TCPInterface::HasLostConnection();
	if (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		_lostConnections.Push(sa);
		RemoveFromConnectionList(sa);
		unsigned int i;
		for (i=0; i < messageHandlerList.Size(); i++)
			messageHandlerList[i]->OnClosedConnection(sa, UNASSIGNED_RAKNET_GUID, LCR_DISCONNECTION_NOTIFICATION);
	}

	sa = TCPInterface::HasCompletedConnectionAttempt();
	if (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		_completedConnectionAttempts.Push(sa);
		AddToConnectionList(sa);
		unsigned int i;
		for (i=0; i < messageHandlerList.Size(); i++)
			messageHandlerList[i]->OnNewConnection(sa, UNASSIGNED_RAKNET_GUID, true);
	}
}
Packet* PacketizedTCP::Receive( void )
{
	PushNotificationsToQueues();

	unsigned int i;
	for (i=0; i < messageHandlerList.Size(); i++)
		messageHandlerList[i]->Update();

	Packet *packet = TCPInterface::Receive();
	if (packet==0)
		return 0;

	if (connections.Has(packet->systemAddress)==false)
	{
		DeallocatePacket(packet);
		return 0;
	}

	unsigned int index = connections.GetIndexAtKey(packet->systemAddress);
	//RakAssert(index!=-1);
	if (index!=(unsigned int)-1)
	{
		if (packet->deleteData==true)
		{
			// Came from network
			SystemAddress systemAddressFromPacket;
			DataStructures::ByteQueue *bq = connections[index];
			// Buffer data
			bq->WriteBytes((const char*) packet->data,packet->length);
			RakAssert(bq->GetBytesWritten()>=packet->length);
			systemAddressFromPacket=packet->systemAddress;
			DeallocatePacket(packet);
			packet=0;
			PTCPHeader dataLength;

			// Peek the header to see if a full message is waiting
			bq->ReadBytes((char*) &dataLength,sizeof(PTCPHeader),true);
#ifndef __BITSTREAM_NATIVE_END
			if (RakNet::BitStream::DoEndianSwap())
				RakNet::BitStream::ReverseBytesInPlace((unsigned char*) &dataLength,sizeof(dataLength));
#endif
			// Header indicates packet length. If enough data is available, read out and return one packet
			if (bq->GetBytesWritten()>=dataLength+sizeof(PTCPHeader))
			{
				bq->IncrementReadOffset(sizeof(PTCPHeader));
				packet = RakNet::OP_NEW<Packet>(__FILE__, __LINE__);
				packet->length=dataLength;
				packet->bitSize=BYTES_TO_BITS(packet->length);
				packet->guid=UNASSIGNED_RAKNET_GUID;
				packet->systemAddress=systemAddressFromPacket;
				packet->systemIndex=(SystemIndex)-1;
				packet->deleteData=false;
				packet->data=(unsigned char*) rakMalloc_Ex(dataLength, __FILE__, __LINE__);
				if (packet->data==0)
				{
					notifyOutOfMemory(__FILE__, __LINE__);
					RakNet::OP_DELETE(packet,__FILE__,__LINE__);
					return 0;
				}
				bq->ReadBytes((char*) packet->data,dataLength,false);
			}
			else
			{
				// Not enough data to make one message
				packet=0;
			}
		}

		if (packet)
		{
			PluginReceiveResult pluginResult;
			for (i=0; i < messageHandlerList.Size(); i++)
			{
				pluginResult=messageHandlerList[i]->OnReceive(packet);
				if (pluginResult==RR_STOP_PROCESSING_AND_DEALLOCATE)
				{
					DeallocatePacket( packet );
					packet=0; // Will do the loop again and get another packet
					break; // break out of the enclosing for
				}
				else if (pluginResult==RR_STOP_PROCESSING)
				{
					packet=0;
					break;
				}
			}
		}

		return packet;
	}
	else
	{
		DeallocatePacket(packet);
	}
	return 0;
}

void PacketizedTCP::AttachPlugin( PluginInterface2 *plugin )
{
	if (messageHandlerList.GetIndexOf(plugin)==MAX_UNSIGNED_LONG)
	{
		messageHandlerList.Insert(plugin, __FILE__, __LINE__);
		plugin->SetPacketizedTCP(this);
		plugin->OnAttach();
	}
}
void PacketizedTCP::DetachPlugin( PluginInterface2 *plugin )
{
	if (plugin==0)
		return;

	unsigned int index;
	index = messageHandlerList.GetIndexOf(plugin);
	if (index!=MAX_UNSIGNED_LONG)
	{
		messageHandlerList[index]->OnDetach();
		// Unordered list so delete from end for speed
		messageHandlerList[index]=messageHandlerList[messageHandlerList.Size()-1];
		messageHandlerList.RemoveFromEnd();
		plugin->SetPacketizedTCP(0);
	}
}
void PacketizedTCP::CloseConnection( SystemAddress systemAddress )
{
	RemoveFromConnectionList(systemAddress);
	unsigned int i;
	for (i=0; i < messageHandlerList.Size(); i++)
		messageHandlerList[i]->OnClosedConnection(systemAddress, UNASSIGNED_RAKNET_GUID, LCR_CLOSED_BY_USER);
	TCPInterface::CloseConnection(systemAddress);
}
void PacketizedTCP::RemoveFromConnectionList(SystemAddress sa)
{
	if (sa==UNASSIGNED_SYSTEM_ADDRESS)
		return;
	unsigned int index = connections.GetIndexAtKey(sa);
	if (index!=(unsigned int)-1)
	{
		RakNet::OP_DELETE(connections[index],__FILE__,__LINE__);
		connections.RemoveAtIndex(index);
	}
}
void PacketizedTCP::AddToConnectionList(SystemAddress sa)
{
	if (sa==UNASSIGNED_SYSTEM_ADDRESS)
		return;
	connections.SetNew(sa, RakNet::OP_NEW<DataStructures::ByteQueue>(__FILE__, __LINE__));
}
void PacketizedTCP::ClearAllConnections(void)
{
	unsigned int i;
	for (i=0; i < connections.Size(); i++)
		RakNet::OP_DELETE(connections[i],__FILE__,__LINE__);
	connections.Clear();
}
SystemAddress PacketizedTCP::HasCompletedConnectionAttempt(void)
{
	PushNotificationsToQueues();

	if (_completedConnectionAttempts.IsEmpty()==false)
		return _completedConnectionAttempts.Pop();
	return UNASSIGNED_SYSTEM_ADDRESS;
}
SystemAddress PacketizedTCP::HasFailedConnectionAttempt(void)
{
	PushNotificationsToQueues();

	if (_failedConnectionAttempts.IsEmpty()==false)
		return _failedConnectionAttempts.Pop();
	return UNASSIGNED_SYSTEM_ADDRESS;
}
SystemAddress PacketizedTCP::HasNewIncomingConnection(void)
{
	PushNotificationsToQueues();

	if (_newIncomingConnections.IsEmpty()==false)
		return _newIncomingConnections.Pop();
	return UNASSIGNED_SYSTEM_ADDRESS;
}
SystemAddress PacketizedTCP::HasLostConnection(void)
{
	PushNotificationsToQueues();

	if (_lostConnections.IsEmpty()==false)
		return _lostConnections.Pop();
	return UNASSIGNED_SYSTEM_ADDRESS;
}
