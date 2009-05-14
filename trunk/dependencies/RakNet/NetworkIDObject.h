/// \file
/// \brief A class you can derive from to make it easier to represent every networked object with an integer.  This way you can refer to objects over the network.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#if !defined(__NETWORK_ID_GENERATOR)
#define      __NETWORK_ID_GENERATOR

#include "RakNetTypes.h"
#include "RakMemoryOverride.h"
#include "Export.h"

class NetworkIDManager;

/// \brief Unique shared ids for each object instance
///
/// A class you can derive from to make it easier to represent every networked object with an integer.  This way you can refer to objects over the network.
/// One system should return true for IsNetworkIDAuthority() and the rest should return false.  When an object needs to be created, have the the one system create the object.
/// Then have that system send a message to all other systems, and include the value returned from GetNetworkID() in that packet.  All other systems should then create the same
/// class of object, and call SetNetworkID() on that class with the NetworkID in the packet.
/// \see the manual for more information on this.
class RAK_DLL_EXPORT NetworkIDObject
{
public:
	/// Constructor.  NetworkIDs, if IsNetworkIDAuthority() is true, are created here.
	NetworkIDObject();

	/// Destructor.  Used NetworkIDs, if any, are freed here.
	virtual ~NetworkIDObject();

	/// Sets the manager class from which to request unique network IDs
	/// Unlike previous versions, the NetworkIDObject relies on a manager class to provide IDs, rather than using statics,
	/// So you can have more than one set of IDs on the same system.
	virtual void SetNetworkIDManager( NetworkIDManager *manager);

	/// Returns what was passed to SetNetworkIDManager
	virtual NetworkIDManager * GetNetworkIDManager( void );
	
	/// Returns the NetworkID that you can use to refer to this object over the network.
	/// \pre You must first call SetNetworkIDManager before using this function
	/// \retval UNASSIGNED_NETWORK_ID UNASSIGNED_NETWORK_ID is returned IsNetworkIDAuthority() is false and SetNetworkID() was not previously called.  This is also returned if you call this function in the constructor.
	/// \retval 0-65534 Any other value is a valid NetworkID.  NetworkIDs start at 0 and go to 65534, wrapping at that point.
	virtual NetworkID GetNetworkID( void );
	
	/// Sets the NetworkID for this instance.  Usually this is called by the clients and determined from the servers.  However, if you save multiplayer games you would likely use
	/// This on load as well.	
	virtual void SetNetworkID( NetworkID id );
	
	/// Your class does not have to derive from NetworkIDObject, although that is the easiest way to implement this.
	/// If you want this to be a member object of another class, rather than inherit, then call SetParent() with a pointer to the parent class instance.
	/// GET_OBJECT_FROM_ID will then return the parent rather than this instance.
	virtual void SetParent( void *_parent );
	
	/// Return what was passed to SetParent
	/// \return The value passed to SetParent, or 0 if it was never called.
	virtual void* GetParent( void ) const;
	
	/// Overload this function and return true if you require that SetParent is called before this object is used.
	/// This is a safety check you should do this if you want this to be
	/// a member object of another class rather than derive from this class.
	virtual bool RequiresSetParent(void) const;

	/// Used so I can compare pointers in the ReplicaManager
	uint32_t GetAllocationNumber(void) const;

protected:
	/// The  network ID of this object
	NetworkID networkID;

	/// The parent set by SetParent()
	void *parent;

	/// Used so I can compare pointers in the ReplicaManager
	static uint32_t nextAllocationNumber;
	uint32_t allocationNumber;
	
	/// Internal function to generate an ID when needed.  This is deferred until needed and is not called from the constructor.
	void GenerateID(void);
	
	/// This is crap but is necessary because virtual functions don't work in the constructor
	bool callGenerationCode; 
	
	NetworkIDManager *networkIDManager;
};

#endif
