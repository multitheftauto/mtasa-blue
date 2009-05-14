/// \file
/// \brief Fully connected mesh plugin.  This will connect RakPeer to all connecting peers, and all peers the connecting peer knows about.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __FULLY_CONNECTED_MESH_H
#define __FULLY_CONNECTED_MESH_H

class RakPeerInterface;
#include "PluginInterface2.h"
#include "RakMemoryOverride.h"

/// \defgroup FULLY_CONNECTED_MESH_GROUP FullyConnectedMesh
/// \ingroup PLUGINS_GROUP

/// Fully connected mesh plugin.  This will connect RakPeer to all connecting peers, and all peers the connecting peer knows about.
/// \depreciated Use FullyConnectedMesh2
/// \pre You must also install the ConnectionGraph plugin.  If you want a password, set it there.
/// \ingroup FULLY_CONNECTED_MESH_GROUP
class RAK_DLL_EXPORT FullyConnectedMesh : public PluginInterface2
{
public:
	FullyConnectedMesh();
	virtual ~FullyConnectedMesh();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------
	/// Set the password to use to connect to the other systems
 	void Startup(const char *password, int _passwordLength);

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual PluginReceiveResult OnReceive(Packet *packet);
	

protected:
	char *pw;
	int passwordLength;

	SystemAddress facilitator;
};

#endif
