#include "MessageFilter.h"
#include "RakAssert.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

int MessageFilterStrComp( char *const &key,char *const &data )
{
	return strcmp(key,data);
}

int FilteredSystemComp( const SystemAddress &key, const FilteredSystem &data )
{
	if (key < data.systemAddress)
		return -1;
	else if (key==data.systemAddress)
		return 0;
	else
		return 1;
}

int FilterSetComp( const int &key, FilterSet * const &data )
{
	if (key < data->filterSetID)
		return -1;
	else if (key==data->filterSetID)
		return 0;
	else
		return 1;
}
MessageFilter::MessageFilter()
{

}
MessageFilter::~MessageFilter()
{
	Clear();
}
void MessageFilter::SetAutoAddNewConnectionsToFilter(int filterSetID)
{
	autoAddNewConnectionsToFilter=filterSetID;
}
void MessageFilter::SetAllowMessageID(bool allow, int messageIDStart, int messageIDEnd,int filterSetID)
{
	RakAssert(messageIDStart <= messageIDEnd);
	FilterSet *filterSet = GetFilterSetByID(filterSetID);
	int i;
	for (i=messageIDStart; i <= messageIDEnd; ++i)
		filterSet->allowedIDs[i]=allow;
}
void MessageFilter::SetAllowRPC(bool allow, const char *functionName, int filterSetID)
{
	(void) allow;
	FilterSet *filterSet = GetFilterSetByID(filterSetID);
	bool objectExists;
	unsigned index = filterSet->allowedRPCs.GetIndexFromKey((char *const) functionName, &objectExists);
	if (objectExists==false)
	{
		char *str = (char*) rakMalloc_Ex( strlen(functionName)+1, __FILE__, __LINE__ );
		strcpy(str, functionName);
		filterSet->allowedRPCs.InsertAtIndex(str, index);
	}
}
void MessageFilter::SetActionOnDisallowedMessage(bool kickOnDisallowed, bool banOnDisallowed, RakNetTime banTimeMS, int filterSetID)
{
	FilterSet *filterSet = GetFilterSetByID(filterSetID);
	filterSet->kickOnDisallowedMessage=kickOnDisallowed;
	filterSet->disallowedMessageBanTimeMS=banTimeMS;
	filterSet->banOnDisallowedMessage=banOnDisallowed;
}
void MessageFilter::SetDisallowedMessageCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(RakPeerInterface *peer, SystemAddress systemAddress, int filterSetID, void *userData, unsigned char messageID))
{
	FilterSet *filterSet = GetFilterSetByID(filterSetID);
	filterSet->invalidMessageCallback=invalidMessageCallback;
	filterSet->disallowedCallbackUserData=userData;
}
void MessageFilter::SetTimeoutCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(RakPeerInterface *peer, SystemAddress systemAddress, int filterSetID, void *userData))
{
	FilterSet *filterSet = GetFilterSetByID(filterSetID);
	filterSet->timeoutCallback=invalidMessageCallback;
	filterSet->timeoutUserData=userData;
}
void MessageFilter::SetFilterMaxTime(int allowedTimeMS, bool banOnExceed, RakNetTime banTimeMS, int filterSetID)
{
	FilterSet *filterSet = GetFilterSetByID(filterSetID);
	filterSet->maxMemberTimeMS=allowedTimeMS;
	filterSet->banOnFilterTimeExceed=banOnExceed;
	filterSet->timeExceedBanTimeMS=banTimeMS;
}
int MessageFilter::GetSystemFilterSet(SystemAddress systemAddress)
{
	bool objectExists;
	unsigned index = systemList.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists==false)
		return -1;
	else
		return systemList[index].filter->filterSetID;
}
void MessageFilter::SetSystemFilterSet(SystemAddress systemAddress, int filterSetID)
{
	// Allocate this filter set if it doesn't exist.
	RakAssert(systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
	bool objectExists;
	unsigned index = systemList.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists==false)
	{
		if (filterSetID<0)
			return;

		FilteredSystem filteredSystem;
		filteredSystem.filter = GetFilterSetByID(filterSetID);
		filteredSystem.systemAddress=systemAddress;
		filteredSystem.timeEnteredThisSet=RakNet::GetTime();
		systemList.Insert(systemAddress, filteredSystem, true);
	}
	else
	{
		if (filterSetID>=0)
		{
			FilterSet *filterSet = GetFilterSetByID(filterSetID);
			systemList[index].timeEnteredThisSet=RakNet::GetTime();
			systemList[index].filter=filterSet;
		}
		else
		{
			systemList.RemoveAtIndex(index);
		}
	}	
}
unsigned MessageFilter::GetSystemCount(int filterSetID) const
{
	if (filterSetID==-1)
		return systemList.Size();
	else
	{
		unsigned i;
		unsigned count=0;
		for (i=0; i < systemList.Size(); i++)
			if (systemList[i].filter->filterSetID==filterSetID)
				++count;
		return count;
	}
}
SystemAddress MessageFilter::GetSystemByIndex(int filterSetID, unsigned index)
{
	if (filterSetID==-1)
		return systemList[index].systemAddress;
	else
	{
		unsigned i;
		unsigned count=0;
		for (i=0; i < systemList.Size(); i++)
		{
			if (systemList[i].filter->filterSetID==filterSetID)
			{
				if (index==count)
					return systemList[i].systemAddress;
				count++;
			}
		}
	}
	return UNASSIGNED_SYSTEM_ADDRESS;
}
unsigned MessageFilter::GetFilterSetCount(void) const
{
	return filterList.Size();
}
int MessageFilter::GetFilterSetIDByIndex(unsigned index)
{
	return filterList[index]->filterSetID;
}
void MessageFilter::DeleteFilterSet(int filterSetID)
{
	FilterSet *filterSet;
	bool objectExists;
	unsigned i,index;
	index = filterList.GetIndexFromKey(filterSetID, &objectExists);
	if (objectExists)
	{
		filterSet=filterList[index];
		DeallocateFilterSet(filterSet);
		filterList.RemoveAtIndex(index);

		// Don't reference this pointer any longer
		i=0;
		while (i < systemList.Size())
		{
			if (systemList[i].filter==filterSet)
				systemList.RemoveAtIndex(i);
			else
				++i;
		}
	}
}
void MessageFilter::Clear(void)
{
	unsigned i;
	systemList.Clear();
	for (i=0; i < filterList.Size(); i++)
		DeallocateFilterSet(filterList[i]);
	filterList.Clear();
}
void MessageFilter::DeallocateFilterSet(FilterSet* filterSet)
{
	unsigned i;
	for (i=0; i < filterSet->allowedRPCs.Size(); i++)
		rakFree_Ex(filterSet->allowedRPCs[i], __FILE__, __LINE__ );
	RakNet::OP_DELETE(filterSet, __FILE__, __LINE__);
}
FilterSet* MessageFilter::GetFilterSetByID(int filterSetID)
{
	RakAssert(filterSetID>=0);
	bool objectExists;
	unsigned index;
	index = filterList.GetIndexFromKey(filterSetID, &objectExists);
	if (objectExists)
		return filterList[index];
	else
	{
		FilterSet *newFilterSet = RakNet::OP_NEW<FilterSet>( __FILE__, __LINE__ );
		memset(newFilterSet->allowedIDs, 0, MESSAGE_FILTER_MAX_MESSAGE_ID * sizeof(bool));
		newFilterSet->banOnFilterTimeExceed=false;
		newFilterSet->kickOnDisallowedMessage=false;
		newFilterSet->banOnDisallowedMessage=false;
		newFilterSet->disallowedMessageBanTimeMS=0;
		newFilterSet->timeExceedBanTimeMS=0;
		newFilterSet->maxMemberTimeMS=0;
		newFilterSet->filterSetID=filterSetID;
		newFilterSet->invalidMessageCallback=0;
		newFilterSet->timeoutCallback=0;
		newFilterSet->timeoutUserData=0;
		filterList.Insert(filterSetID, newFilterSet, true);
		return newFilterSet;
	}
}
void MessageFilter::OnInvalidMessage(FilterSet *filterSet, SystemAddress systemAddress, unsigned char messageID)
{
	if (filterSet->invalidMessageCallback)
		filterSet->invalidMessageCallback(rakPeerInterface, systemAddress, filterSet->filterSetID, filterSet->disallowedCallbackUserData, messageID);
	if (filterSet->banOnDisallowedMessage)
	{
		char str1[64];
		systemAddress.ToString(false, str1);
		rakPeerInterface->AddToBanList(str1, filterSet->disallowedMessageBanTimeMS);
	}
	if (filterSet->kickOnDisallowedMessage)
		rakPeerInterface->CloseConnection(systemAddress, true, 0);
}
void MessageFilter::Update(void)
{
	// Update all timers for all systems.  If those systems' filter sets are expired, take the appropriate action.
	RakNetTime time = RakNet::GetTime();
	unsigned index;
	index=0;
	while (index < systemList.Size())
	{
		if (systemList[index].filter &&
			systemList[index].filter->maxMemberTimeMS>0 &&
			time-systemList[index].timeEnteredThisSet >= systemList[index].filter->maxMemberTimeMS)
		{
			if (systemList[index].filter->timeoutCallback)
				systemList[index].filter->timeoutCallback(rakPeerInterface, systemList[index].systemAddress, systemList[index].filter->filterSetID, systemList[index].filter->timeoutUserData);

			if (systemList[index].filter->banOnFilterTimeExceed)
			{
				char str1[64];
				systemList[index].systemAddress.ToString(false, str1);
				rakPeerInterface->AddToBanList(str1, systemList[index].filter->timeExceedBanTimeMS);
			}
			rakPeerInterface->CloseConnection(systemList[index].systemAddress, true, 0);
			systemList.RemoveAtIndex(index);
		}
		else
			++index;
	}
}
void MessageFilter::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) systemAddress;
	(void) rakNetGUID;
	(void) isIncoming;

	// New system, automatically assign to filter set if appropriate
	if (autoAddNewConnectionsToFilter>=0 && systemList.HasData(systemAddress)==false)
		SetSystemFilterSet(systemAddress, autoAddNewConnectionsToFilter);
}
void MessageFilter::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) rakNetGUID;
	(void) lostConnectionReason;

	// Lost system, remove from the list
	systemList.RemoveIfExists(systemAddress);
}
 PluginReceiveResult MessageFilter::OnReceive(Packet *packet)
{
	bool objectExists;
	unsigned index;
	unsigned char messageId;

	switch (packet->data[0]) 
	{
	case ID_NEW_INCOMING_CONNECTION:
	case ID_CONNECTION_REQUEST_ACCEPTED:
	case ID_CONNECTION_LOST:
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_ATTEMPT_FAILED:
	case ID_NO_FREE_INCOMING_CONNECTIONS:
	case ID_RSA_PUBLIC_KEY_MISMATCH:
	case ID_CONNECTION_BANNED:
	case ID_INVALID_PASSWORD:
	case ID_MODIFIED_PACKET:
	case ID_PONG:
	case ID_ALREADY_CONNECTED:
	case ID_ADVERTISE_SYSTEM:
	case ID_REMOTE_DISCONNECTION_NOTIFICATION:
	case ID_REMOTE_CONNECTION_LOST:
	case ID_REMOTE_NEW_INCOMING_CONNECTION:
	case ID_DOWNLOAD_PROGRESS:
		break;
	default:
		if (packet->data[0]==ID_TIMESTAMP)
		{
			if (packet->length<sizeof(MessageID) + sizeof(RakNetTime))
				return RR_STOP_PROCESSING_AND_DEALLOCATE; // Invalid message
			messageId=packet->data[sizeof(MessageID) + sizeof(RakNetTime)];
		}
		else
			messageId=packet->data[0];
		// If this system is filtered, check if this message is allowed.  If not allowed, return RR_STOP_PROCESSING_AND_DEALLOCATE
		index = systemList.GetIndexFromKey(packet->systemAddress, &objectExists);
		if (objectExists==false)
			break;
		if (messageId==ID_RPC)
		{
			const char *uniqueIdentifier = rakPeerInterface->GetRPCString((const char*) packet->data, packet->bitSize, packet->systemAddress);
			if (systemList[index].filter->allowedRPCs.HasData((char *const)uniqueIdentifier)==false)
			{
				OnInvalidMessage(systemList[index].filter, packet->systemAddress, packet->data[0]);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
		}
		else
		{
			if (systemList[index].filter->allowedIDs[messageId]==false)
			{
				OnInvalidMessage(systemList[index].filter, packet->systemAddress, packet->data[0]);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
		}		
		
		break;
	}
	
	return RR_CONTINUE_PROCESSING;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
