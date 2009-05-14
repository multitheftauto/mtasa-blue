/// \file
/// \brief \b RakNet's plugin functionality system, version 2.  You can derive from this to create your own plugins.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __PLUGIN_INTERFACE_2_H
#define __PLUGIN_INTERFACE_2_H

class RakPeerInterface;
class PacketizedTCP;
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
#include "PacketPriority.h"

enum PI2_LostConnectionReason
{
	LCR_CLOSED_BY_USER,
	LCR_DISCONNECTION_NOTIFICATION,
	LCR_CONNECTION_LOST
};

enum PI2_FailedConnectionAttemptReason
{
	FCAR_CONNECTION_ATTEMPT_FAILED,
	FCAR_ALREADY_CONNECTED,
	FCAR_NO_FREE_INCOMING_CONNECTIONS,
	FCAR_RSA_PUBLIC_KEY_MISMATCH,
	FCAR_CONNECTION_BANNED,
	FCAR_INVALID_PASSWORD
};

/// \defgroup PLUGINS_GROUP PluginInterface2

/// \brief PluginInterface2 provides a mechanism to add functionality in a modular way.
/// MessageHandlers should derive from PluginInterface and be attached to RakPeer using the function AttachPlugin
/// On a user call to Receive, OnReceive is called for every PluginInterface, which can then take action based on the message
/// passed to it.  This is used to transparently add game-independent functional modules, similar to browser plugins
///
/// \ingroup PLUGINS_GROUP
class RAK_DLL_EXPORT PluginInterface2
{
public:
	PluginInterface2();
	virtual ~PluginInterface2();

	/// Called when the interface is attached
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnAttach(void) {}

	/// Called when the interface is detached
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnDetach(void) {}

	/// Update is called every time a packet is checked for .
	virtual void Update(void) {}

	/// OnReceive is called for every packet.
	/// \param[in] packet the packet that is being returned to the user
	/// \return True to allow the game and other plugins to get this message, false to absorb it
	virtual PluginReceiveResult OnReceive(Packet *packet) {(void) packet; return RR_CONTINUE_PROCESSING;}

	/// Called when RakPeer is initialized
	virtual void OnStartup(void) {}

	/// Called when RakPeer is shutdown
	virtual void OnShutdown(void) {}

	/// Called when a connection is dropped because the user called RakPeer::CloseConnection() for a particular system
	/// \param[in] systemAddress The system whose connection was closed
	/// \param[in] rakNetGuid The guid of the specified system
	/// \param[in] lostConnectionReason How the connection was closed: manually, connection lost, or notification of disconnection
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason ){(void) systemAddress; (void) rakNetGUID; (void) lostConnectionReason;}

	/// Called when we got a new connection
	/// \param[in] systemAddress Address of the new connection
	/// \param[in] rakNetGuid The guid of the specified system
	/// \param[in] isIncoming If true, this is ID_NEW_INCOMING_CONNECTION, or the equivalent
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming) {(void) systemAddress; (void) rakNetGUID; (void) isIncoming;}

	/// Called when a connection attempt fails
	/// \param[in] systemAddress Address of the connection
	/// \param[in] failedConnectionReason Why the connection failed
	virtual void OnFailedConnectionAttempt(SystemAddress systemAddress, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason) {(void) systemAddress; (void) failedConnectionAttemptReason;}

	/// Called on a send to the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress Which system this message is being sent to
	virtual void OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) {(void) data; (void) bitsUsed; (void) remoteSystemAddress;}
	
	/// Called on a receive from the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress Which system this message is being sent to
	virtual void OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) {(void) data; (void) bitsUsed; (void) remoteSystemAddress;}
	
	/// Called on a send or receive of a message within the reliability layer
	/// \param[in] internalPacket The user message, along with all send data.
	/// \param[in] frameNumber The number of frames sent or received so far for this player depending on \a isSend .  Indicates the frame of this user message.
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	/// \param[in] time The current time as returned by RakNet::GetTime()
	/// \param[in] isSend Is this callback representing a send event or receive event?
	virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, RakNetTime time, bool isSend) {(void) internalPacket; (void) frameNumber; (void) remoteSystemAddress; (void) time; (void) isSend;}

	/// Called when we get an ack for a message we reliabily sent
	/// \param[in] messageNumber The numerical identifier for which message this is
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	/// \param[in] time The current time as returned by RakNet::GetTime()
	virtual void OnAck(unsigned int messageNumber, SystemAddress remoteSystemAddress, RakNetTime time) {(void) messageNumber; (void) remoteSystemAddress; (void) time;}

	/// System called RakPeerInterface::PushBackPacket
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	virtual void OnPushBackPacket(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) {(void) data; (void) bitsUsed; (void) remoteSystemAddress;}

	RakPeerInterface *GetRakPeerInterface(void) const {return rakPeerInterface;}

	/// \internal
	void SetRakPeerInterface( RakPeerInterface *ptr );
	/// \internal
	void SetPacketizedTCP( PacketizedTCP *ptr );
protected:
	// Send through either rakPeerInterface or packetizedTCP, whichever is available
	void SendUnified( const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast );
	bool SendListUnified( char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast );

	Packet *AllocatePacketUnified(unsigned dataSize);
	void PushBackPacketUnified(Packet *packet, bool pushAtHead);

	// Filled automatically in when attached
	RakPeerInterface *rakPeerInterface;
	PacketizedTCP *packetizedTCP;
};

#endif

