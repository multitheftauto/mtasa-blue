/// \file
/// \brief A simple TCP based server allowing sends and receives.  Can be connected by any TCP client, including telnet.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __PACKETIZED_TCP
#define __PACKETIZED_TCP

#include "TCPInterface.h"
#include "DS_ByteQueue.h"
#include "PluginInterface2.h"
#include "DS_Map.h"

class RAK_DLL_EXPORT PacketizedTCP : public TCPInterface
{
public:
	PacketizedTCP();
	virtual ~PacketizedTCP();

	/// Starts the TCP server on the indicated port
	bool Start(unsigned short port, unsigned short maxIncomingConnections);

	/// Stops the TCP server
	void Stop(void);

	/// Sends a byte stream
	void Send( const char *data, unsigned length, SystemAddress systemAddress, bool broadcast );

	// Sends a concatenated list of byte streams
	bool SendList( char **data, const int *lengths, const int numParameters, SystemAddress systemAddress, bool broadcast );

	/// Returns data received
	Packet* Receive( void );

	/// Disconnects a player/address
	void CloseConnection( SystemAddress systemAddress );

	/// Has a previous call to connect succeeded?
	/// \return UNASSIGNED_SYSTEM_ADDRESS = no. Anything else means yes.
	SystemAddress HasCompletedConnectionAttempt(void);

	/// Has a previous call to connect failed?
	/// \return UNASSIGNED_SYSTEM_ADDRESS = no. Anything else means yes.
	SystemAddress HasFailedConnectionAttempt(void);

	/// Queued events of new incoming connections
	SystemAddress HasNewIncomingConnection(void);

	/// Queued events of lost connections
	SystemAddress HasLostConnection(void);

	// Only currently tested with FileListTransfer!
	void AttachPlugin( PluginInterface2 *plugin );
	void DetachPlugin( PluginInterface2 *plugin );

protected:
	void ClearAllConnections(void);
	void RemoveFromConnectionList(SystemAddress sa);
	void AddToConnectionList(SystemAddress sa);
	void PushNotificationsToQueues(void);

	// Plugins
	DataStructures::List<PluginInterface2*> messageHandlerList;
	DataStructures::Map<SystemAddress, DataStructures::ByteQueue *> connections;

	// Mirrors single producer / consumer, but processes them in Receive() before returning to user
	DataStructures::Queue<SystemAddress> _newIncomingConnections, _lostConnections, _failedConnectionAttempts, _completedConnectionAttempts;
};

#endif
