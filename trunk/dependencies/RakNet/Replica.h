/// \file
/// \brief Contains interface Replica used by the ReplicaManager.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __REPLICA_H
#define __REPLICA_H

#include "NetworkIDObject.h"
#include "PacketPriority.h"
#include "ReplicaEnums.h"

/// This is an interface of a replicated object for use in the framework of ReplicaManager
/// You should derive from this class, implementing the functions to provide the behavior you want.
/// If your architecture doesn't allow you to derive from this class, you can store an instance of a derived instance of this class in your base game object.
/// In that case, use GetParent() and SetParent() and propagate the function calls up to your real classes. For an example where I do this, see Monster.h in the ReplicaManagerCS sample.
/// \note All send functions are called one for every target recipient, so you can customize the data sent per-user.
/// \brief The interface to derive your game's networked classes from
/// \ingroup REPLICA_MANAGER_GROUP
class Replica : public NetworkIDObject
{
public:
	/// This function is called in the first update tick after this object is first passed to ReplicaManager::Replicate for each player, and also when a new participant joins
	/// The intent of this function is to tell another system to create an instance of this class.
	/// You can do this however you want - I recommend having each class having a string identifier which is registered with StringTable, and then using EncodeString to write the name of the class.  In the callback passed to SetReceiveConstructionCB create an instance of the object based on that string.
	/// \note If you return true from IsNetworkIDAuthority, which you should do for a server or peer, I recommend encoding the value returned by GetNetworkID() into your bitstream and calling SetNetworkID with that value in your SetReceiveConstructionCB callback.
	/// \note Dereplicate, SetScope, and SignalSerializeNeeded all rely on being able to call GET_OBJECT_FROM_ID which requires that SetNetworkID be called on that object.
	/// \note SendConstruction is called once for every new player that connects and every existing player when an object is passed to Replicate.
	/// \param[in] currentTime The current time that would be returned by RakNet::GetTime().  That's a slow call I do already, so you can use the parameter instead of having to call it yourself.
	/// \param[in] systemAddress The participant to send to.
	/// \param[in,out] flags Per-object per-system serialization flags modified by this function, ReplicaManager::SignalSerializationFlags, and ReplicaManager::AccessSerializationFlags.  Useful for simple customization of what you serialize based on application events.  This value is not automatically reset.
	/// \param[out] outBitStream The data you want to write in the message. If you do not write to outBitStream and return true, then no send call will occur and the system will consider this object as not created on that remote system.
	/// \param[out] includeTimestamp Set to true to include a timestamp with the message.  This will be reflected in the timestamp parameter of the callback.  Defaults to false.
	/// \return See ReplicaReturnResult
	virtual ReplicaReturnResult SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp )=0;

	/// The purpose of the function is to send a packet containing the data in \a outBitStream to \a systemAddress telling that system that Dereplicate was called.
	/// In the code, this is called in the update cycle after you call ReplicaManager::Destruct().  Then, if you write to outBitStream, a message is sent to that participant.
	/// This is the one send you cannot delay because objects may be deleted and we can't read into them past this.
	/// \param[out] outBitStream The data to send.  If you do not write to outBitStream, then no send call will occur
	/// \param[in] systemAddress The participant to send to.
	/// \param[out] includeTimestamp Set to true to include a timestamp with the message.  This will be reflected in the timestamp parameter of the callback.  Defaults to false.
	/// \return See ReplicaReturnResult
	virtual ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp )=0;

	/// This function is called when SendDestruction is sent from another system.  Delete your object if you want.
	/// \param[in] inBitStream What was sent in SendDestruction::outBitStream
	/// \param[in] systemAddress The participant that sent this message to us.
	/// \param[in] timestamp if Serialize::SendDestruction was set to true, the time the packet was sent.
	/// \return See ReplicaReturnResult.  Only REPLICA_PROCESSING_DONE is valid, and will send the destruction message.  Anything else will not send any messages.
	virtual ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)=0;

	/// Called when ReplicaManager::SetScope is called with a different value than what it currently has.
	/// It is up to you to write \a inScope to \a outBitStream.  Not doing so, and returning true, means you want to cancel the scope change call.
	/// If \a inScope is true, you return true, and data is written to outBitStream, then Serialize will be called automatically
	/// This is a convenience feature, since there's almost no case where an object would go in scope but not be serialized
	/// \param[in] inScope The new scope that will be sent to ReceiveScopeChange that originally came from SetScope
	/// \param[out] outBitStream The data to send.  If you do not write to outBitStream and return true, then no send will occur and the object will keep its existing scope
	/// \param[in] currentTime The current time that would be returned by RakNet::GetTime().  That's a slow call I do already, so you can use the parameter instead of having to call it yourself.
	/// \param[in] systemAddress The participant to send to.
	/// \param[out] includeTimestamp Set to true to include a timestamp with the message.  This will be reflected in the timestamp parameter of the callback.  Defaults to false.
	/// \return See ReplicaReturnResult
	virtual ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp )=0;

	/// Called when when we get the SendScopeChange message.  The new scope should have been encoded (by you) into \a inBitStream
	/// \param[in] inBitStream What was sent in SendScopeChange::outBitStream
	/// \param[in] systemAddress The participant that sent this message to us.
	/// \param[in] timestamp if Serialize::SendScopeChange was set to true, the time the packet was sent.
	/// \return See ReplicaReturnResult
	virtual ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream,SystemAddress systemAddress, RakNetTime timestamp)=0;

	/// Called when ReplicaManager::SignalSerializeNeeded is called with this object as the parameter.
	/// The system will ensure that Serialize only occurs for participants that have this object constructed and in scope
	/// The intent of this function is to serialize all your class member variables for remote transmission.
	/// \param[out] sendTimestamp Set to true to include a timestamp with the message.  This will be reflected in the timestamp parameter Deserialize.  Defaults to false.
	/// \param[out] outBitStream The data you want to write in the message. If you do not write to outBitStream and return true, then no send will occur for this participant.
	/// \param[in] lastSendTime The last time Serialize returned true and outBitStream was written to.  0 if this is the first time the function has ever been called for this \a systemAddress
	/// \param[in] priority Passed to RakPeer::Send for the send call.
	/// \param[in] reliability Passed to RakPeer::Send for the send call.
	/// \param[in] currentTime The current time that would be returned by RakNet::GetTime().  That's a slow call I do already, so you can use the parameter instead of having to call it yourself.
	/// \param[in] systemAddress The participant we are sending to.
	/// \param[in,out] flags Per-object per-system serialization flags modified by this function, ReplicaManager::SignalSerializationFlags, and ReplicaManager::AccessSerializationFlags.  Useful for simple customization of what you serialize based on application events.  This value is not automatically reset.
	/// \return See ReplicaReturnResult
	virtual ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)=0;

	/// Called when another participant called Serialize with our system as the target
	/// \param[in] inBitStream What was written to Serialize::outBitStream
	/// \param[in] timestamp if Serialize::SendTimestamp was set to true, the time the packet was sent.
	/// \param[in] lastDeserializeTime Last time you returned true from this function for this object, or 0 if never, regardless of \a systemAddress.
	/// \param[in] systemAddress The participant that sent this message to us.
	virtual ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )=0;

	/// Used to sort the order that commands (construct, serialize) take place in.
	/// Lower sort priority commands happen before higher sort priority commands.
	/// Same sort priority commands take place in random order.
	/// For example, if both players and player lists are replicas, you would want to create the players before the player lists if the player lists refer to the players.
	/// So you could specify the players as priority 0, and the lists as priority 1, and the players would be created and serialized first
	/// \return A higher value to process later, a lower value to process sooner, the same value to process in random order.
	virtual int GetSortPriority(void) const=0;
};

#endif
