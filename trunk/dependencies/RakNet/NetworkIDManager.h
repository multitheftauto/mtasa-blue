/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __NETWORK_ID_MANAGER_H
#define __NETWORK_ID_MANAGER_H

#include "DS_BinarySearchTree.h"
#include "RakNetTypes.h"
#include "Export.h"
#include "RakMemoryOverride.h"
#include "NetworkIDObject.h"

/// \internal
/// \brief A node in the AVL tree that holds the mapping between NetworkID and pointers.
struct RAK_DLL_EXPORT NetworkIDNode
{
	NetworkID networkID;
	NetworkIDObject *object;
	NetworkIDNode();
	NetworkIDNode( NetworkID _networkID, NetworkIDObject *_object );
	bool operator==( const NetworkIDNode& right ) const;
	bool operator > ( const NetworkIDNode& right ) const;
	bool operator < ( const NetworkIDNode& right ) const;
};

/// This class is simply used to generate a unique number for a group of instances of NetworkIDObject
/// An instance of this class is required to use the ObjectID to pointer lookup system
/// You should have one instance of this class per game instance.
/// Call SetIsNetworkIDAuthority before using any functions of this class, or of NetworkIDObject
class RAK_DLL_EXPORT NetworkIDManager
{
public:
	NetworkIDManager(void);
	virtual ~NetworkIDManager(void);

	/// For every group of systems, one system needs to be responsible for creating unique IDs for all objects created on all systems.
	/// This way, systems can send that id in packets to refer to objects (you can't send pointers because the memory allocations may be different).
	/// In a client/server environment, the system that creates unique IDs would be the server.
	/// If you are using peer to peer or other situations where you don't have a single system to assign ids, 
	/// set this to true, and be sure NETWORK_ID_SUPPORTS_PEER_TO_PEER is defined in RakNetDefines.h
	void SetIsNetworkIDAuthority(bool isAuthority);

	/// \return Returns what was passed to SetIsNetworkIDAuthority()
	bool IsNetworkIDAuthority(void) const;

	/// \Depreciated. Use SetGuid and GetGuid instead
	/// Necessary for peer to peer, as NetworkIDs are then composed of your external player Id (doesn't matter which, as long as unique)
	/// plus the usual object ID number.
	/// Get this from RakPeer::GetExternalSystemAddress) one time, the first time you make a connection.
	/// \pre You must first call SetNetworkIDManager before using this function
	/// \param[in] systemAddress Your external systemAddress
	void SetExternalSystemAddress(SystemAddress systemAddress);
	SystemAddress GetExternalSystemAddress(void);

	/// Necessary for peer to peer, as NetworkIDs are then composed of your external player Id (doesn't matter which, as long as unique)
	/// plus the usual object ID number.
	/// Get this from RakPeer::GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS) one time, the first time you make a connection.
	/// \pre You must first call SetNetworkIDManager before using this function
	void SetGuid(RakNetGUID g);
	RakNetGUID GetGuid(void);	

	/// These function is only meant to be used when saving games as you
	/// should save the HIGHEST value staticItemID has achieved upon save
	/// and reload it upon load.  Save AFTER you've created all the items
	/// derived from this class you are going to create.  
	/// \return the HIGHEST Object Id currently used 
	unsigned short GetSharedNetworkID( void );

	/// These function is only meant to be used when loading games. Load
	/// BEFORE you create any new objects that are not SetIDed based on
	/// the save data. 
	/// \param[in] i the highest number of NetworkIDObject reached. 
	void SetSharedNetworkID( unsigned short i );

	/// If you use a parent, returns this instance rather than the parent object.
	/// \pre You must first call SetNetworkIDManager before using this function
	NetworkIDObject* GET_BASE_OBJECT_FROM_ID( NetworkID x );

	/// Returns the parent object, or this instance if you don't use a parent.
	/// \depreciated, use the template form. This form requires that NetworkIDObject is the basemost derived class
	/// \pre You must first call SetNetworkIDManager before using this function
	void* GET_OBJECT_FROM_ID( NetworkID x );

	/// Returns the parent object, or this instance if you don't use a parent.
	/// Supports NetworkIDObject anywhere in the inheritance hierarchy
	/// \pre You must first call SetNetworkIDManager before using this function
	template <class returnType>
	returnType GET_OBJECT_FROM_ID(NetworkID x) {
		NetworkIDObject *nio = GET_BASE_OBJECT_FROM_ID(x);
		if (nio==0)
			return 0;
		if (nio->GetParent())
			return (returnType) nio->GetParent();
		return (returnType) nio;
	}

protected:
	RakNetGUID guid;
	// Depreciated - use guid instead. This has the problem that it can be different between the LAN and the internet, or duplicated on different LANs
	SystemAddress externalSystemAddress;
	unsigned short sharedNetworkID;
	bool isNetworkIDAuthority;
	bool calledSetIsNetworkIDAuthority;
	friend class NetworkIDObject;


#if ! defined(NETWORK_ID_USE_PTR_TABLE) || defined(NETWORK_ID_USE_HASH)
	/// This AVL tree holds the pointer to NetworkID mappings	
	DataStructures::AVLBalancedBinarySearchTree<NetworkIDNode> IDTree;
#else
	NetworkIDObject **IDArray;
#endif
};

#endif
