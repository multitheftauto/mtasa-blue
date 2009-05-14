/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "RakAssert.h"

#include "RakAlloca.h"

uint32_t NetworkIDObject::nextAllocationNumber=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkIDObject::NetworkIDObject()
{
	callGenerationCode=true;
	networkID=UNASSIGNED_NETWORK_ID;
	parent=0;
	networkIDManager=0;
	allocationNumber=nextAllocationNumber++;
}

//-------------------------------------------------------------------------------------


NetworkIDObject::~NetworkIDObject()
{
	if (networkID!=UNASSIGNED_NETWORK_ID)
	{
#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
		void *obj = networkIDManager->IDArray[networkID.localSystemAddress];
		if (obj==this)
			networkIDManager->IDArray[networkID.localSystemAddress]=0;
#else
		NetworkIDNode * object = networkIDManager->IDTree.GetPointerToNode( NetworkIDNode( ( networkID ), 0 ) );
		if ( object && object->object == this )
			networkIDManager->IDTree.Del( NetworkIDNode( object->networkID, 0 ) );
#endif
	}
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

void NetworkIDObject::SetNetworkIDManager( NetworkIDManager *manager)
{
	networkIDManager = manager;
}

//-------------------------------------------------------------------------------------

NetworkIDManager * NetworkIDObject::GetNetworkIDManager( void )
{
	return networkIDManager;
}

//-------------------------------------------------------------------------------------

NetworkID NetworkIDObject::GetNetworkID( void )
{
	RakAssert(networkIDManager);
	if (callGenerationCode && networkIDManager->IsNetworkIDAuthority())
	{
		GenerateID();
		RakAssert(networkID!=UNASSIGNED_NETWORK_ID);
		callGenerationCode=false;
	}

	return networkID;
};
//-------------------------------------------------------------------------------------

bool NetworkIDObject::RequiresSetParent(void) const
{
	return false;
}

//-------------------------------------------------------------------------------------

void NetworkIDObject::SetNetworkID( NetworkID id )
{
	callGenerationCode=false;

	if ( id == UNASSIGNED_NETWORK_ID )
	{
		// puts("Warning: NetworkIDObject passed UNASSIGNED_NETWORK_ID.  SetID ignored");
		return ;
	}

	if ( networkID == id )
	{
		// RAKNET_DEBUG_PRINTF("NetworkIDObject passed %i which already exists in the tree.  SetID ignored", id);
		return ;
	}

	RakAssert(networkIDManager);

#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
	networkID = id;
	networkIDManager->IDArray[id.localSystemAddress]=this;
#else

	NetworkIDNode* collision = networkIDManager->IDTree.GetPointerToNode( NetworkIDNode( ( id ), 0 ) );

	if ( collision )   // Tree should have only unique values.  The new value is already in use.
	{
		//RAKNET_DEBUG_PRINTF("Warning: NetworkIDObject::SetID passed %i, which has an existing node in the tree.  Old node removed, which will cause the item pointed to to be inaccessible to the network", id);
		networkIDManager->IDTree.Del( NetworkIDNode( collision->networkID, collision->object ) );
	}

	if ( networkID == UNASSIGNED_NETWORK_ID )   // Object has not had an ID assigned so does not already exist in the tree
	{
		networkID = id;
		networkIDManager->IDTree.Add( NetworkIDNode( networkID, this ) );
	}
	else // Object already exists in the tree and has an assigned ID
	{
		networkIDManager->IDTree.Del( NetworkIDNode( networkID, this ) ); // Delete the node with whatever ID the existing object is using
		networkID = id;
		networkIDManager->IDTree.Add( NetworkIDNode( networkID, this ) );
	}
#endif
}

//-------------------------------------------------------------------------------------
void NetworkIDObject::SetParent( void *_parent )
{
	parent=_parent;

#ifdef _DEBUG
	if (networkIDManager)
	{
		// Avoid duplicate parents in the tree
		unsigned i;
#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
		for (i=0; i < 65534; i++)
		{
			NetworkIDObject *nio =  networkIDManager->IDArray[i];
			RakAssert(nio==0 || nio->GetParent()!=_parent);
		}
#else
		unsigned size = networkIDManager->IDTree.Size();
		NetworkIDNode *nodeArray;

		bool usedAlloca=false;
	#if !defined(_XBOX) && !defined(X360)
		if (sizeof(NetworkIDNode) * size < MAX_ALLOCA_STACK_ALLOCATION)
		{
			nodeArray = (NetworkIDNode*) alloca(sizeof(NetworkIDNode) * size);
			usedAlloca=true;
		}
		else
	#endif
			nodeArray = RakNet::OP_NEW_ARRAY<NetworkIDNode>(size, __FILE__, __LINE__ );

		networkIDManager->IDTree.DisplayBreadthFirstSearch( nodeArray );
		for (i=0; i < size; i++)
		{
			// If this assert hits then this _parent is already in the tree.  Classes instance should never contain more than one NetworkIDObject
			RakAssert(nodeArray->object->GetParent()!=parent);
		}

		if (usedAlloca==false)
			RakNet::OP_DELETE_ARRAY(nodeArray, __FILE__, __LINE__);
#endif
	}
#endif
}
//-------------------------------------------------------------------------------------
void* NetworkIDObject::GetParent( void ) const
{
	return parent;
}
//-------------------------------------------------------------------------------------
uint32_t NetworkIDObject::GetAllocationNumber(void) const
{
	return allocationNumber;
}
//-------------------------------------------------------------------------------------
void NetworkIDObject::GenerateID(void)
{
	RakAssert(networkIDManager->IsNetworkIDAuthority());

#if defined(NETWORK_ID_USE_PTR_TABLE) || defined (NETWORK_ID_USE_HASH)
	int count = 65535;
	(void) count;
	do 
	{
		RakAssert(count-->0);
		networkID.localSystemAddress=networkIDManager->sharedNetworkID++;
	} while(networkIDManager->IDArray[networkID.localSystemAddress]!=0);
	networkIDManager->IDArray[networkID.localSystemAddress]=this;
#else
	// If you want more than 65535 network objects, change the type of networkID
	RakAssert(networkIDManager->IDTree.Size() < 65535);

	NetworkIDNode* collision;
	do
	{
		networkID.localSystemAddress=networkIDManager->sharedNetworkID++;
#ifdef NETWORK_ID_SUPPORTS_PEER_TO_PEER
		if (NetworkID::IsPeerToPeerMode())
		{
			if (networkIDManager->GetGuid()==UNASSIGNED_RAKNET_GUID)
			{
				 // If this assert hits you forgot to call SetGUID and SetExternalSystemAddress
				RakAssert(networkIDManager->externalSystemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
				networkID.systemAddress=networkIDManager->externalSystemAddress;
			}
			else
			{
				networkID.guid=networkIDManager->GetGuid();
				networkID.systemAddress=networkIDManager->externalSystemAddress;
			}
		}
#endif
		collision = networkIDManager->IDTree.GetPointerToNode( NetworkIDNode( ( networkID ), 0 ) );
	}
	while ( collision );

	networkIDManager->IDTree.Add( NetworkIDNode( networkID, this ) );
#endif
}

//////////////////////////////////////////////////////////////////////
// EOF
//////////////////////////////////////////////////////////////////////
