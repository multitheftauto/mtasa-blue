/// \file
/// \brief \b RakNet's plugin functionality system.  You can derive from this to create your own plugins.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


/*
#ifndef __PLUGIN_INTERFACE_H
#define __PLUGIN_INTERFACE_H

class RakPeerInterface;
struct Packet;
struct InternalPacket;

enum PluginReceiveResult
{
	// The plugin used this message and it shouldn't be given to the user.
	RR_STOP_PROCESSING_AND_DEALLOCATE=0,

	// This message will be processed by other plugins, and at last by the user.
	RR_CONTINUE_PROCESSING,

	// The plugin is going to hold on to this message.  Do not deallocate it but do not pass it to other plugins either.
	RR_STOP_PROCESSING,
};

#include "RakNetTypes.h"
#include "Export.h"
#include "RakMemoryOverride.h"

/// \defgroup PLUGINS_GROUP PluginInterface

/// \brief PluginInterface provides a mechanism to add functionality in a modular way.
/// MessageHandlers should derive from PluginInterface and be attached to RakPeer using the function AttachPlugin
/// On a user call to Receive, OnReceive is called for every PluginInterface, which can then take action based on the message
/// passed to it.  This is used to transparently add game-independent functional modules, similar to browser plugins
///
/// \sa ReplicaManager
/// \sa FullyConnectedMesh
/// \sa PacketLogger
/// \ingroup PLUGINS_GROUP
class RAK_DLL_EXPORT PluginInterface
{
public:
	PluginInterface();
	virtual ~PluginInterface();

	/// Called when the interface is attached
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnAttach(RakPeerInterface *peer);

	/// Called when the interface is detached
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnDetach(RakPeerInterface *peer);

	/// Called when RakPeer is initialized
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnStartup(RakPeerInterface *peer);

	/// Update is called every time a packet is checked for .
	/// \param[in] peer - the instance of RakPeer that is calling Receive
	virtual void Update(RakPeerInterface *peer);

	/// OnReceive is called for every packet.
	/// \param[in] peer the instance of RakPeer that is calling Receive
	/// \param[in] packet the packet that is being returned to the user
	/// \return True to allow the game and other plugins to get this message, false to absorb it
	virtual PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet);

	/// Called when RakPeer is shutdown
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnShutdown(RakPeerInterface *peer);

	/// Called when a connection is dropped because the user called RakPeer::CloseConnection() for a particular system
	/// \param[in] peer the instance of RakPeer that is calling Receive
	/// \param[in] systemAddress The system whose connection was closed
	virtual void OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress);

	/// Called on a send to the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress Which system this message is being sent to
	virtual void OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
	
	/// Called on a receive from the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress Which system this message is being sent to
	virtual void OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
	
	/// Called on a send or recieve within the reliability layer
	/// \param[in] internalPacket The user message, along with all send data.
	/// \param[in] frameNumber The number of frames sent or received so far for this player depending on \a isSend .  Indicates the frame of this user message.
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	/// \param[in] time The current time as returned by RakNet::GetTime()
	/// \param[in] isSend Is this callback representing a send event or receive event?
	virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, RakNetTime time, bool isSend);
};

#endif

*/