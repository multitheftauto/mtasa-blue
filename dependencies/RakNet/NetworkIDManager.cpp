/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "NetworkIDManager.h"
#include "NetworkIDObject.h"
#include "RakAssert.h"
#include <string.h> // For memset

bool NetworkIDNode::operator==( const NetworkIDNode& right ) const
{
	if ( networkID == right.networkID )
		return !0;

	return 0;
}

bool NetworkIDNode::operator > ( const NetworkIDNode& right ) const
{
	if ( networkID > right.networkID )
		return !0;

	return 0;
}

bool NetworkIDNode::operator < ( const NetworkIDNode& right ) const
{
	if ( networkID < right.networkID )
		return !0;

	return 0;
}

NetworkIDNode::NetworkIDNode()
{
	object = 0;
}

NetworkIDNode::NetworkIDNode( NetworkID _networkID, NetworkIDObject *_object )
{
	networkID = _networkID;
	object = _object;
}


//-------------------------------------------------------------------------------------
NetworkIDObject* NetworkIDManager::GET_BASE_OBJECT_FROM_ID( NetworkID x )
{
	if ( x == UNASSIGNED_NETWORK_ID )
		return 0;

#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
	// You can't use this technique in peer to peer mode.  Undefine NETWORK_ID_USE_PTR_TABLE in NetworkIDManager.h
	RakAssert(NetworkID::IsPeerToPeerMode()==false);
	return IDArray[x.localSystemAddress];
#else

	NetworkIDNode *n = IDTree.GetPointerToNode( NetworkIDNode( ( x ), 0 ) );

	if ( n )
	{
		return n->object;
	}

	return 0;

#endif

}
//-------------------------------------------------------------------------------------
void* NetworkIDManager::GET_OBJECT_FROM_ID( NetworkID x )
{
#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
	if (x.localSystemAddress==65535)
		return 0;

	// You can't use this technique in peer to peer mode.  Undefine NETWORK_ID_USE_PTR_TABLE in NetworkIDManager.h
	RakAssert(NetworkID::IsPeerToPeerMode()==false);
	if (IDArray[x.localSystemAddress])
	{
		if (IDArray[x.localSystemAddress]->GetParent())
		{
			return IDArray[x.localSystemAddress]->GetParent();
		}
		else
		{
#ifdef _DEBUG
			// If this assert hit then this object requires a call to SetParent and it never got one.
			RakAssert(IDArray[x.localSystemAddress]->RequiresSetParent()==false);
#endif
			return IDArray[x.localSystemAddress];
		}
	}
#else
	NetworkIDObject *object = (NetworkIDObject *) GET_BASE_OBJECT_FROM_ID( x );
	if (object)
	{
		if (object->GetParent())
		{
			return object->GetParent();
		}
		else
		{
#ifdef _DEBUG
			// If this assert hit then this object requires a call to SetParent and it never got one.
			RakAssert(object->RequiresSetParent()==false);
#endif
			return object;
		}
	}
#endif

	return 0;
}
//-------------------------------------------------------------------------------------
NetworkIDManager::NetworkIDManager(void)
{
	calledSetIsNetworkIDAuthority=false;
	sharedNetworkID=0;
	externalSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	guid=UNASSIGNED_RAKNET_GUID;

#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
	// Last element is reserved for UNASSIGNED_NETWORK_ID
	IDArray = (NetworkIDObject**) rakMalloc_Ex(sizeof(NetworkIDObject*) * 65534, __FILE__, __LINE__);
	memset(IDArray,0,sizeof(NetworkIDObject*)*65534);
	// You can't use this technique in peer to peer mode.  Undefine NETWORK_ID_USE_PTR_TABLE in NetworkIDManager.h
	RakAssert(NetworkID::IsPeerToPeerMode()==false);
#endif
}
//-------------------------------------------------------------------------------------
NetworkIDManager::~NetworkIDManager(void)
{
#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
	rakFree_Ex(IDArray, __FILE__, __LINE__ );
#endif
}
//-------------------------------------------------------------------------------------

void NetworkIDManager::SetIsNetworkIDAuthority(bool isAuthority)
{
	isNetworkIDAuthority=isAuthority;
	calledSetIsNetworkIDAuthority=true;
}

//-------------------------------------------------------------------------------------

bool NetworkIDManager::IsNetworkIDAuthority(void) const
{
	RakAssert(calledSetIsNetworkIDAuthority);
	return isNetworkIDAuthority;
}

//-------------------------------------------------------------------------------------

unsigned short NetworkIDManager::GetSharedNetworkID( void )
{
	RakAssert(calledSetIsNetworkIDAuthority);
	return sharedNetworkID;
}

//-------------------------------------------------------------------------------------

void NetworkIDManager::SetSharedNetworkID( unsigned short i )
{
	RakAssert(calledSetIsNetworkIDAuthority);
	sharedNetworkID = i;
}

//-------------------------------------------------------------------------------------
void NetworkIDManager::SetExternalSystemAddress(SystemAddress systemAddress)
{
	RakAssert(calledSetIsNetworkIDAuthority);
	RakAssert(systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
	externalSystemAddress=systemAddress;
}
//-------------------------------------------------------------------------------------
SystemAddress NetworkIDManager::GetExternalSystemAddress(void)
{
	RakAssert(calledSetIsNetworkIDAuthority);
	return externalSystemAddress;
}
//-------------------------------------------------------------------------------------
void NetworkIDManager::SetGuid(RakNetGUID g)
{
	guid=g;
}
RakNetGUID NetworkIDManager::GetGuid(void)
{
	RakAssert(guid!=UNASSIGNED_RAKNET_GUID);
	return guid;
}
