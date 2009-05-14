/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "FullyConnectedMesh.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "ConnectionGraph.h"
#include <string.h>
#include "RakAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

FullyConnectedMesh::FullyConnectedMesh()
{
	pw=0;
}

FullyConnectedMesh::~FullyConnectedMesh()
{
	if (pw)
		rakFree_Ex(pw, __FILE__, __LINE__ );
}

void FullyConnectedMesh::Startup(const char *password, int _passwordLength)
{
	if (pw)
		rakFree_Ex(pw, __FILE__, __LINE__ );
	if (password)
	{
		pw = (char*) rakMalloc_Ex( _passwordLength, __FILE__, __LINE__ );
		memcpy(pw, password, _passwordLength);
		passwordLength=_passwordLength;
	}
	else
		pw=0;
	
}

PluginReceiveResult FullyConnectedMesh::OnReceive(Packet *packet)
{
	RakAssert(packet);

	switch (packet->data[0])
	{
	case ID_REMOTE_NEW_INCOMING_CONNECTION: // This comes from the connection graph plugin
		{
			RakNet::BitStream b(packet->data, packet->length, false);
			b.IgnoreBits(8);
			ConnectionGraphGroupID group1, group2;
			SystemAddress node1, node2;
			RakNetGUID guid1, guid2;
			b.Read(node1);
			b.Read(group1);
			b.Read(guid1);
			if (rakPeerInterface->IsConnected(node1,true)==false)
			{
				char str1[64];
				node1.ToString(false, str1);
				rakPeerInterface->Connect(str1, node1.port, pw, pw ? passwordLength : 0);
			}				
			b.Read(node2);
			b.Read(group2);
			b.Read(guid2);
			if (rakPeerInterface->IsConnected(node2,true)==false)
			{
				char str1[64];
				node2.ToString(false, str1);
				rakPeerInterface->Connect(str1, node2.port, pw, pw ? passwordLength : 0);
			}
				
			break;
		}
	}

	return RR_CONTINUE_PROCESSING;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
