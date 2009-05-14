#include "AutoRPC.h"
#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include "StringCompressor.h"
#include "BitStream.h"
//#include "Types.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include <stdlib.h>

using namespace RakNet;

#ifdef _MSC_VER
#pragma warning( push )
#endif

int AutoRPC::RemoteRPCFunctionComp( const RPCIdentifier &key, const RemoteRPCFunction &data )
{
	if (key.isObjectMember==false && data.identifier.isObjectMember==true)
		return -1;
	if (key.isObjectMember==true && data.identifier.isObjectMember==false)
		return 1;
	return strcmp(key.uniqueIdentifier, data.identifier.uniqueIdentifier);
}

AutoRPC::AutoRPC()
{
	currentExecution[0]=0;
	networkIdManager=0;
	outgoingTimestamp=0;
	outgoingPriority=HIGH_PRIORITY;
	outgoingReliability=RELIABLE_ORDERED;
	outgoingOrderingChannel=0;
	outgoingBroadcast=true;
	incomingTimeStamp=0;
	DataStructures::Map<SystemAddress, DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *>::IMPLEMENT_DEFAULT_COMPARISON();
}

AutoRPC::~AutoRPC()
{
	Clear();
}
void AutoRPC::SetNetworkIDManager(NetworkIDManager *idMan)
{
	networkIdManager=idMan;
}
bool AutoRPC::RegisterFunction(const char *uniqueIdentifier, void *functionPtr, bool isObjectMember, char parameterCount)
{
	if (uniqueIdentifier==0 || functionPtr==0)
	{
		RakAssert(0);
		return false;
	}

	RPCIdentifier identifier;
	identifier.isObjectMember=isObjectMember;
	identifier.uniqueIdentifier=(char*) uniqueIdentifier;
	unsigned localIndex = GetLocalFunctionIndex(identifier);
	// Already registered?
	if (localIndex!=(unsigned)-1 && localFunctions[localIndex].functionPtr!=0)
		return false;
	if (localIndex!=(unsigned)-1)
	{
		// Reenable existing
		localFunctions[localIndex].functionPtr=functionPtr;
		localFunctions[localIndex].parameterCount=parameterCount;
	}
	else
	{
		// Add new
		LocalRPCFunction func;
		func.functionPtr=functionPtr;
		func.identifier.isObjectMember=isObjectMember;
		func.identifier.uniqueIdentifier = (char*) rakMalloc_Ex(strlen(uniqueIdentifier)+1, __FILE__, __LINE__);
		func.parameterCount=parameterCount;
		strcpy(func.identifier.uniqueIdentifier, uniqueIdentifier);
		localFunctions.Insert(func, __FILE__, __LINE__);
	}
	return true;
}
bool AutoRPC::UnregisterFunction(const char *uniqueIdentifier, bool isObjectMember)
{
	if (uniqueIdentifier==0)
	{
		RakAssert(0);
		return false;
	}

	RPCIdentifier identifier;
	identifier.isObjectMember=isObjectMember;
	identifier.uniqueIdentifier=(char*) uniqueIdentifier;
	unsigned localIndex = GetLocalFunctionIndex(identifier);
	// Not registered?
	if (localIndex==(unsigned)-1)
		return false;
	// Leave the id in, in case the function is set again later. That way we keep the same remote index
	localFunctions[localIndex].functionPtr=0;
	return true;
}
void AutoRPC::SetTimestamp(RakNetTime timeStamp)
{
	outgoingTimestamp=timeStamp;
}
void AutoRPC::SetSendParams(PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	outgoingPriority=priority;
	outgoingReliability=reliability;
	outgoingOrderingChannel=orderingChannel;
}
void AutoRPC::SetRecipientAddress(SystemAddress systemAddress, bool broadcast)
{
	outgoingSystemAddress=systemAddress;
	outgoingBroadcast=broadcast;
}
void AutoRPC::SetRecipientObject(NetworkID networkID)
{
	outgoingNetworkID=networkID;
}
RakNet::BitStream *AutoRPC::SetOutgoingExtraData(void)
{
	return &outgoingExtraData;
}
RakNetTime AutoRPC::GetLastSenderTimestamp(void) const
{
	return incomingTimeStamp;
}
SystemAddress AutoRPC::GetLastSenderAddress(void) const
{
	return incomingSystemAddress;
}
RakPeerInterface *AutoRPC::GetRakPeer(void) const
{
	return rakPeerInterface;
}
const char *AutoRPC::GetCurrentExecution(void) const
{
	return (const char *) currentExecution;
}
RakNet::BitStream *AutoRPC::GetIncomingExtraData(void)
{
	return &incomingExtraData;
}
bool AutoRPC::SendCall(const char *uniqueIdentifier, const char *stack, unsigned int bytesOnStack, char parameterCount)
{
	SystemAddress systemAddr;
	RPCIdentifier identifier;
	unsigned int outerIndex;
	unsigned int innerIndex;

	if (uniqueIdentifier==0)
		return false;

	identifier.uniqueIdentifier=(char*) uniqueIdentifier;
	identifier.isObjectMember=(outgoingNetworkID!=UNASSIGNED_NETWORK_ID);

	RakNet::BitStream bs;
	if (outgoingTimestamp!=0)
	{
		bs.Write((MessageID)ID_TIMESTAMP);
		bs.Write(outgoingTimestamp);
	}
	bs.Write((MessageID)ID_AUTO_RPC_CALL);
	if (parameterCount>=0)
	{
		bs.Write(true);
		bs.Write(parameterCount);
	}
	else
	{
		bs.Write(false);
	}
	bs.WriteCompressed(outgoingExtraData.GetNumberOfBitsUsed());
	bs.Write(&outgoingExtraData);
	if (outgoingNetworkID!=UNASSIGNED_NETWORK_ID)
	{
		bs.Write(true);
		bs.Write(outgoingNetworkID);
	}
	else
	{
		bs.Write(false);
	}
	// This is so the call SetWriteOffset works
	bs.AlignWriteToByteBoundary();
	BitSize_t writeOffset = bs.GetWriteOffset();
	if (outgoingBroadcast)
	{
		unsigned systemIndex;
		for (systemIndex=0; systemIndex < rakPeerInterface->GetMaximumNumberOfPeers(); systemIndex++)
		{
			systemAddr=rakPeerInterface->GetSystemAddressFromIndex(systemIndex);
			if (systemAddr!=UNASSIGNED_SYSTEM_ADDRESS && systemAddr!=outgoingSystemAddress)
			{
				if (GetRemoteFunctionIndex(systemAddr, identifier, &outerIndex, &innerIndex))
				{
					// Write a number to identify the function if possible, for faster lookup and less bandwidth
					bs.Write(true);
					bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
				}
				else
				{
					bs.Write(false);
					stringCompressor->EncodeString(uniqueIdentifier, 512, &bs, 0);
				}

				bs.WriteCompressed(bytesOnStack);
				bs.WriteAlignedBytes((const unsigned char*) stack, bytesOnStack);
				SendUnified(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);

				// Start writing again after ID_AUTO_RPC_CALL
				bs.SetWriteOffset(writeOffset);
			}
		}
	}
	else
	{
		systemAddr = outgoingSystemAddress;
		if (systemAddr!=UNASSIGNED_SYSTEM_ADDRESS)
		{
			if (GetRemoteFunctionIndex(systemAddr, identifier, &outerIndex, &innerIndex))
			{
				// Write a number to identify the function if possible, for faster lookup and less bandwidth
				bs.Write(true);
				bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
			}
			else
			{
				bs.Write(false);
				stringCompressor->EncodeString(uniqueIdentifier, 512, &bs, 0);
			}

			bs.WriteCompressed(bytesOnStack);
			bs.WriteAlignedBytes((const unsigned char*) stack, bytesOnStack);
			SendUnified(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);
		}
		else
			return false;
	}
	return true;
}
void AutoRPC::OnAttach(void)
{
	outgoingSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	outgoingNetworkID=UNASSIGNED_NETWORK_ID;
	incomingSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;

}
PluginReceiveResult AutoRPC::OnReceive(Packet *packet)
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
	case ID_AUTO_RPC_CALL:
		incomingTimeStamp=timestamp;
		incomingSystemAddress=packet->systemAddress;
		OnAutoRPCCall(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTO_RPC_REMOTE_INDEX:
		OnRPCRemoteIndex(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTO_RPC_UNKNOWN_REMOTE_INDEX:
		OnRPCUnknownRemoteIndex(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset, timestamp);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;		
	}

	return RR_CONTINUE_PROCESSING;
}
void AutoRPC::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) rakNetGUID;
	(void) lostConnectionReason;

	if (remoteFunctions.Has(systemAddress))
	{
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions.Get(systemAddress);
		unsigned i;
		for (i=0; i < theList->Size(); i++)
		{
			if (theList->operator [](i).identifier.uniqueIdentifier)
				rakFree_Ex(theList->operator [](i).identifier.uniqueIdentifier, __FILE__, __LINE__ );
		}
		RakNet::OP_DELETE(theList, __FILE__, __LINE__);
		remoteFunctions.Delete(systemAddress);
	}
}
void AutoRPC::OnAutoRPCCall(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	RakNet::BitStream bs(data,lengthInBytes,false);

	bool hasParameterCount=false;
	char parameterCount;
	char inputStack[ARPC_MAX_STACK_SIZE];
	NetworkIDObject *networkIdObject;
	NetworkID networkId;
	bool hasNetworkId=false;
	bool hasFunctionIndex=false;
	unsigned int functionIndex;
	unsigned int bytesOnStack;
	char strIdentifier[512];
	int numberOfBitsUsed;
	incomingExtraData.Reset();
	bs.Read(hasParameterCount);
	if (hasParameterCount)
		bs.Read(parameterCount);
	else
		parameterCount=-1;
	bs.ReadCompressed(numberOfBitsUsed);
	if (numberOfBitsUsed > (int) incomingExtraData.GetNumberOfBitsAllocated())
		incomingExtraData.AddBitsAndReallocate(numberOfBitsUsed-(int) incomingExtraData.GetNumberOfBitsAllocated());
	bs.ReadBits(incomingExtraData.GetData(), numberOfBitsUsed, false);
	incomingExtraData.SetWriteOffset(numberOfBitsUsed);


//	const unsigned int outputStackSize = ARPC_MAX_STACK_SIZE+128*4; // Enough padding to round up to 4 for each parameter, max 128 parameters
//	char outputStack[outputStackSize];

	bs.Read(hasNetworkId);
	if (hasNetworkId)
	{
		bs.Read(networkId);
		if (networkIdManager==0 && (networkIdManager=rakPeerInterface->GetNetworkIDManager())==0)
		{
			// Failed - Tried to call object member, however, networkIDManager system was never registered
			SendError(systemAddress, RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE, "");
			return;
		}
		networkIdObject = networkIdManager->GET_OBJECT_FROM_ID<NetworkIDObject*>(networkId);
		if (networkIdObject==0)
		{
			// Failed - Tried to call object member, object does not exist (deleted?)
			SendError(systemAddress, RPC_ERROR_OBJECT_DOES_NOT_EXIST, "");
			return;
		}
	}
	else
	{
		networkIdObject=0;
	}
	bs.AlignReadToByteBoundary();
	bs.Read(hasFunctionIndex);
	if (hasFunctionIndex)
		bs.ReadCompressed(functionIndex);
	else
		stringCompressor->DecodeString(strIdentifier,512,&bs,0);
	bs.ReadCompressed(bytesOnStack);
	bs.ReadAlignedBytes((unsigned char *) inputStack,bytesOnStack);
	if (hasFunctionIndex)
	{
		if (functionIndex>localFunctions.Size())
		{
			// Failed - other system specified a totally invalid index
			// Possible causes: Bugs, attempts to crash the system, requested function not registered
			SendError(systemAddress, RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE, "");
			return;
		}
		// it was actually a mistake to implement ID_AUTO_RPC_UNKNOWN_REMOTE_INDEX. This hides the more relevant return code RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED and more importantly can result in the calls being out of order since it takes extra communication steps.
		/*
		if (localFunctions[functionIndex].functionPtr==0)
		{
			// Failed - Function index lookup failure. Try passing back what was sent to us, and requesting the string
			RakNet::BitStream out;
			if (incomingTimeStamp!=0)
			{
				out.Write((MessageID)ID_TIMESTAMP);
				out.Write(incomingTimeStamp);
			}
			out.Write((MessageID)ID_AUTO_RPC_UNKNOWN_REMOTE_INDEX);
			if (parameterCount>=0)
			{
				out.Write(true);
				out.Write(parameterCount);
			}
			else
			{
				out.Write(false);
			}
			out.WriteCompressed(functionIndex);
			out.WriteCompressed(numberOfBitsUsed);
			out.Write(&incomingExtraData);
			out.Write(hasNetworkId);
			if (hasNetworkId)
				out.Write(networkId);
			out.WriteCompressed(bytesOnStack);
			out.WriteAlignedBytes((const unsigned char*) inputStack, bytesOnStack);
			SendUnified(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
			
			return;
		}
		*/
	}
	else
	{
		// Find the registered function with this str
		for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
		{
			if (localFunctions[functionIndex].identifier.isObjectMember == (networkIdObject!=0) &&
				strcmp(localFunctions[functionIndex].identifier.uniqueIdentifier, strIdentifier)==0)
			{
				// SEND RPC MAPPING
				RakNet::BitStream outgoingBitstream;
				outgoingBitstream.Write((MessageID)ID_AUTO_RPC_REMOTE_INDEX);
				outgoingBitstream.Write(hasNetworkId);
				outgoingBitstream.WriteCompressed(functionIndex);
				stringCompressor->EncodeString(strIdentifier,512,&outgoingBitstream,0);
				SendUnified(&outgoingBitstream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
				break;
			}
		}

		if (functionIndex==localFunctions.Size())
		{
			for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
			{
				if (strcmp(localFunctions[functionIndex].identifier.uniqueIdentifier, strIdentifier)==0)
				{
					if (localFunctions[functionIndex].identifier.isObjectMember==true && networkIdObject==0)
					{
						// Failed - Calling C++ function as C function
						SendError(systemAddress, RPC_ERROR_CALLING_CPP_AS_C, strIdentifier);
						return;
					}

					if (localFunctions[functionIndex].identifier.isObjectMember==false && networkIdObject!=0)
					{
						// Failed - Calling C++ function as C function
						SendError(systemAddress, RPC_ERROR_CALLING_C_AS_CPP, strIdentifier);
						return;
					}
				}
			}

			SendError(systemAddress, RPC_ERROR_FUNCTION_NOT_REGISTERED, strIdentifier);
			return;
		}
	}

	if (localFunctions[functionIndex].functionPtr==0)
	{
		// Failed - Function was previously registered, but isn't registered any longer
		SendError(systemAddress, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED, localFunctions[functionIndex].identifier.uniqueIdentifier);
		return;
	}

	if (bytesOnStack > ARPC_MAX_STACK_SIZE)
	{
		// Failed - Not enough bytes on predetermined stack. Shouldn't hit this since the sender also uses this value
		SendError(systemAddress, RPC_ERROR_STACK_TOO_SMALL, localFunctions[functionIndex].identifier.uniqueIdentifier);
		return;
	}

	if (localFunctions[functionIndex].parameterCount>=0 && parameterCount>=0 && parameterCount!=localFunctions[functionIndex].parameterCount)
	{
		// Failed - The number of parameters that this function has was explicitly specified, and does not match up.
		SendError(systemAddress, RPC_ERROR_INCORRECT_NUMBER_OF_PARAMETERS, localFunctions[functionIndex].identifier.uniqueIdentifier);
		return;
	}

	
//	unsigned int bytesWritten;
//	unsigned char numParameters;
//	unsigned int parameterLengths[64]; // 64 is arbitrary, just needs to be more than whatever might be serialized


	GenRPC::CallParams call;

	if (DeserializeParametersAndBuildCall(call, inputStack, bytesOnStack, this, networkIdObject)==false)
	{
		// Failed - Couldn't deserialize
		SendError(systemAddress, RPC_ERROR_STACK_DESERIALIZATION_FAILED, localFunctions[functionIndex].identifier.uniqueIdentifier);
		return;
	}

	strncpy(currentExecution, localFunctions[functionIndex].identifier.uniqueIdentifier, sizeof(currentExecution)-1);

	if (!CallWithStack( call, localFunctions[functionIndex].functionPtr)){
		// Failed - Couldn't deserialize
		SendError(systemAddress, RPC_ERROR_STACK_DESERIALIZATION_FAILED, currentExecution);
		return;
	}


	currentExecution[0]=0;
}
void AutoRPC::OnRPCRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	// A remote system has given us their internal index for a particular function.
	// Store it and use it from now on, to save bandwidth and search time
	bool objectExists;
	char strIdentifier[512];
	unsigned int insertionIndex;
	unsigned int remoteIndex;
	RemoteRPCFunction newRemoteFunction;
	RakNet::BitStream bs(data,lengthInBytes,false);
	RPCIdentifier identifier;
	bs.Read(identifier.isObjectMember);
	bs.ReadCompressed(remoteIndex);
	stringCompressor->DecodeString(strIdentifier,512,&bs,0);
	identifier.uniqueIdentifier=strIdentifier;

	if (strIdentifier[0]==0)
		return;

	DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList;
	if (remoteFunctions.Has(systemAddress))
	{
		theList = remoteFunctions.Get(systemAddress);
		insertionIndex=theList->GetIndexFromKey(identifier, &objectExists);
		if (objectExists==false)
		{
			newRemoteFunction.functionIndex=remoteIndex;
			newRemoteFunction.identifier.isObjectMember=identifier.isObjectMember;
			newRemoteFunction.identifier.uniqueIdentifier = (char*) rakMalloc_Ex(strlen(strIdentifier)+1, __FILE__, __LINE__);
			strcpy(newRemoteFunction.identifier.uniqueIdentifier, strIdentifier);
			theList->InsertAtIndex(newRemoteFunction, insertionIndex);
		}
	}
	else
	{
		theList = RakNet::OP_NEW<DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> >( __FILE__, __LINE__ );

		newRemoteFunction.functionIndex=remoteIndex;
		newRemoteFunction.identifier.isObjectMember=identifier.isObjectMember;
		newRemoteFunction.identifier.uniqueIdentifier = (char*) rakMalloc_Ex(strlen(strIdentifier)+1, __FILE__, __LINE__);
		strcpy(newRemoteFunction.identifier.uniqueIdentifier, strIdentifier);
		theList->InsertAtEnd(newRemoteFunction);

		remoteFunctions.SetNew(systemAddress,theList);
	}
}
void AutoRPC::OnRPCUnknownRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes, RakNetTime timestamp)
{
	char inputStack[ARPC_MAX_STACK_SIZE];
	NetworkID networkId;
	bool hasNetworkId=false;
	unsigned int functionIndex;
	unsigned int bytesOnStack;
	int numberOfBitsUsed;
	char parameterCount;
	bool hasParameterCount=false;

	RakNet::BitStream extraData;
	RakNet::BitStream bs(data,lengthInBytes,false);
	bs.Read(hasParameterCount);
	if (hasParameterCount)
		bs.Read(parameterCount);
	bs.ReadCompressed(functionIndex);
	bs.ReadCompressed(numberOfBitsUsed);
	extraData.AddBitsAndReallocate(numberOfBitsUsed);
	bs.ReadBits(extraData.GetData(), numberOfBitsUsed, false);
	extraData.SetWriteOffset(numberOfBitsUsed);
	bs.Read(hasNetworkId);
	if (hasNetworkId)
		bs.Read(networkId);
	bs.ReadCompressed(bytesOnStack);
	bs.ReadAlignedBytes((unsigned char*) inputStack, bytesOnStack);

	unsigned outerIndex;
	if (remoteFunctions.Has(systemAddress))
	{
		outerIndex = remoteFunctions.GetIndexAtKey(systemAddress);
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions[outerIndex];
		unsigned i;
		for (i=0; i < theList->Size(); i++)
		{
			if (theList->operator [](i).functionIndex==functionIndex)
			{
				RakNet::BitStream out;
				// Recover by resending the RPC with the function identifier string this time
				if (timestamp!=0)
				{
					out.Write((MessageID)ID_TIMESTAMP);
					out.Write(timestamp);
				}
				out.Write((MessageID)ID_AUTO_RPC_CALL);
				if (parameterCount>=0)
				{
					out.Write(true);
					out.Write(parameterCount);
				}
				else
				{
					out.Write(false);
				}				
				out.WriteCompressed(numberOfBitsUsed);
				out.Write(&extraData);
				out.Write(hasNetworkId);
				if (hasNetworkId)
					out.Write(networkId);
				out.AlignWriteToByteBoundary();
				out.Write(false);
				stringCompressor->EncodeString(theList->operator [](i).identifier.uniqueIdentifier, 512, &out, 0);
				out.WriteCompressed(bytesOnStack);
				out.WriteAlignedBytes((const unsigned char*) inputStack, bytesOnStack);
				SendUnified(&out, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddress, false);
				return;
			}
		}
	}

	// Failed to recover, inform the user
	Packet *p = rakPeerInterface->AllocatePacket(sizeof(MessageID)+sizeof(unsigned char));
	RakNet::BitStream bs2(p->data, sizeof(MessageID)+sizeof(unsigned char), false);
	bs2.SetWriteOffset(0);
	bs2.Write((MessageID)ID_RPC_REMOTE_ERROR);
	bs2.Write((unsigned char)RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED);
	stringCompressor->EncodeString("",256,&bs,0);
	p->systemAddress=systemAddress;
	rakPeerInterface->PushBackPacket(p, false);

}
void AutoRPC::SendError(SystemAddress target, unsigned char errorCode, const char *functionName)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_RPC_REMOTE_ERROR);
	bs.Write(errorCode);
	stringCompressor->EncodeString(functionName,256,&bs,0);
	SendUnified(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, target, false);
}
void AutoRPC::OnShutdown(void)
{
	Clear();
}
void AutoRPC::Clear(void)
{
	unsigned i,j;
	for (j=0; j < remoteFunctions.Size(); j++)
	{
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions[j];
		for (i=0; i < theList->Size(); i++)
		{
			if (theList->operator [](i).identifier.uniqueIdentifier)
				rakFree_Ex(theList->operator [](i).identifier.uniqueIdentifier, __FILE__, __LINE__ );
		}
		RakNet::OP_DELETE(theList, __FILE__, __LINE__);
	}
	for (i=0; i < localFunctions.Size(); i++)
	{
		if (localFunctions[i].identifier.uniqueIdentifier)
			rakFree_Ex(localFunctions[i].identifier.uniqueIdentifier, __FILE__, __LINE__ );
	}
	localFunctions.Clear();
	remoteFunctions.Clear();
	outgoingExtraData.Reset();
	incomingExtraData.Reset();
}
unsigned AutoRPC::GetLocalFunctionIndex(AutoRPC::RPCIdentifier identifier)
{
	unsigned i;
	for (i=0; i < localFunctions.Size(); i++)
	{
		if (localFunctions[i].identifier.isObjectMember==identifier.isObjectMember &&
			strcmp(localFunctions[i].identifier.uniqueIdentifier,identifier.uniqueIdentifier)==0)
			return i;
	}
	return (unsigned) -1;
}
bool AutoRPC::GetRemoteFunctionIndex(SystemAddress systemAddress, AutoRPC::RPCIdentifier identifier, unsigned int *outerIndex, unsigned int *innerIndex)
{
	bool objectExists=false;
	if (remoteFunctions.Has(systemAddress))
	{
		*outerIndex = remoteFunctions.GetIndexAtKey(systemAddress);
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions[*outerIndex];
		*innerIndex = theList->GetIndexFromKey(identifier, &objectExists);
	}
	return objectExists;
}


#ifdef _MSC_VER
#pragma warning( pop )
#endif

