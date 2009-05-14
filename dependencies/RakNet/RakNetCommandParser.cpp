#include "RakNetCommandParser.h"
#include "TransportInterface.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "RakAssert.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#pragma warning( push )
#endif

RakNetCommandParser::RakNetCommandParser()
{
	RegisterCommand(4, "Startup","( unsigned short maxConnections, int _threadSleepTimer, unsigned short localPort, const char *forceHostAddress );");
	RegisterCommand(0,"InitializeSecurity","();");
	RegisterCommand(0,"DisableSecurity","( void );");
	RegisterCommand(1,"AddToSecurityExceptionList","( const char *ip );");
	RegisterCommand(1,"RemoveFromSecurityExceptionList","( const char *ip );");
	RegisterCommand(1,"IsInSecurityExceptionList","( const char *ip );");
	RegisterCommand(1,"SetMaximumIncomingConnections","( unsigned short numberAllowed );");
	RegisterCommand(0,"GetMaximumIncomingConnections","( void ) const;");
	RegisterCommand(4,"Connect","( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength );");
	RegisterCommand(2,"Disconnect","( unsigned int blockDuration, unsigned char orderingChannel=0 );");
	RegisterCommand(0,"IsActive","( void ) const;");
	RegisterCommand(0,"GetConnectionList","() const;");
	RegisterCommand(4,"CloseConnection","( const SystemAddress target, bool sendDisconnectionNotification, unsigned char orderingChannel=0 );");
	RegisterCommand(2,"IsConnected","( );");
	RegisterCommand(2,"GetIndexFromSystemAddress","( const SystemAddress systemAddress );");
	RegisterCommand(1,"GetSystemAddressFromIndex","( int index );");
	RegisterCommand(2,"AddToBanList","( const char *IP, RakNetTime milliseconds=0 );");
	RegisterCommand(1,"RemoveFromBanList","( const char *IP );");
	RegisterCommand(0,"ClearBanList","( void );");
	RegisterCommand(1,"IsBanned","( const char *IP );");
	RegisterCommand(2,"Ping1","( const SystemAddress target );");
	RegisterCommand(3,"Ping2","( const char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections );");
	RegisterCommand(2,"GetAveragePing","( const SystemAddress systemAddress );");
	RegisterCommand(2,"GetLastPing","( const SystemAddress systemAddress ) const;");
	RegisterCommand(2,"GetLowestPing","( const SystemAddress systemAddress ) const;");
	RegisterCommand(1,"SetOccasionalPing","( bool doPing );");
	RegisterCommand(2,"SetOfflinePingResponse","( const char *data, const unsigned int length );");
	RegisterCommand(0,"GetInternalID","( void ) const;");
	RegisterCommand(2,"GetExternalID","( const SystemAddress target ) const;");
	RegisterCommand(3,"SetTimeoutTime","( RakNetTime timeMS, const SystemAddress target );");
	RegisterCommand(1,"SetMTUSize","( int size );");
	RegisterCommand(0,"GetMTUSize","( void ) const;");
	RegisterCommand(0,"GetNumberOfAddresses","( void );");
	RegisterCommand(1,"GetLocalIP","( unsigned int index );");
	RegisterCommand(1,"AllowConnectionResponseIPMigration","( bool allow );");
	RegisterCommand(4,"AdvertiseSystem","( const char *host, unsigned short remotePort, const char *data, int dataLength );");
	RegisterCommand(2,"SetIncomingPassword","( const char* passwordData, int passwordDataLength );");
	RegisterCommand(0,"GetIncomingPassword","( void );");
	RegisterCommand(3,"ApplyNetworkSimulator","( double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance);");
	RegisterCommand(0,"IsNetworkSimulatorActive","( void );");
}
RakNetCommandParser::~RakNetCommandParser()
{
}
void RakNetCommandParser::SetRakPeerInterface(RakPeerInterface *rakPeer)
{
	peer=rakPeer;
}
bool RakNetCommandParser::OnCommand(const char *command, unsigned numParameters, char **parameterList, TransportInterface *transport, SystemAddress systemAddress, const char *originalString)
{
	(void) originalString;
	(void) numParameters;

	if (peer==0)
		return false;

	if (strcmp(command, "Startup")==0)
	{
		SocketDescriptor socketDescriptor((unsigned short)atoi(parameterList[1]), parameterList[3]);
		ReturnResult(peer->Startup((unsigned short)atoi(parameterList[0]), atoi(parameterList[2]), &socketDescriptor, 1), command, transport, systemAddress);
	}
	else if (strcmp(command, "InitializeSecurity")==0)
	{
		peer->InitializeSecurity(0,0,0,0);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "DisableSecurity")==0)
	{
		peer->DisableSecurity();
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "AddToSecurityExceptionList")==0)
	{
		peer->AddToSecurityExceptionList(parameterList[1]);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "RemoveFromSecurityExceptionList")==0)
	{
		peer->RemoveFromSecurityExceptionList(parameterList[1]);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "IsInSecurityExceptionList")==0)
	{
		ReturnResult(peer->IsInSecurityExceptionList(parameterList[1]),command, transport, systemAddress);
	}
	else if (strcmp(command, "SetMaximumIncomingConnections")==0)
	{
		peer->SetMaximumIncomingConnections((unsigned short)atoi(parameterList[0]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "GetMaximumIncomingConnections")==0)
	{
		ReturnResult(peer->GetMaximumIncomingConnections(), command, transport, systemAddress);
	}
	else if (strcmp(command, "Connect")==0)
	{
		ReturnResult(peer->Connect(parameterList[0], (unsigned short)atoi(parameterList[1]),parameterList[2],atoi(parameterList[3])), command, transport, systemAddress);
	}
	else if (strcmp(command, "Disconnect")==0)
	{
		peer->Shutdown(atoi(parameterList[0]), (unsigned char)atoi(parameterList[1]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "IsActive")==0)
	{
		ReturnResult(peer->IsActive(), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetConnectionList")==0)
	{
		SystemAddress remoteSystems[32];
		unsigned short count=32;
		unsigned i;
		if (peer->GetConnectionList(remoteSystems, &count))
		{
			if (count==0)
			{
				transport->Send(systemAddress, "GetConnectionList() returned no systems connected.\r\n");
			}
			else
			{
				transport->Send(systemAddress, "GetConnectionList() returned:\r\n");
				for (i=0; i < count; i++)
				{
					char str1[64];
					remoteSystems[i].ToString(false, str1);
					transport->Send(systemAddress, "%i %s %i:%i\r\n", i, str1, remoteSystems[i].binaryAddress, remoteSystems[i].port);
				}
			}
		}
		else
			transport->Send(systemAddress, "GetConnectionList() returned false.\r\n");
	}
	else if (strcmp(command, "CloseConnection")==0)
	{
		peer->CloseConnection(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1])),atoi(parameterList[2])!=0,(unsigned char)atoi(parameterList[3]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "IsConnected")==0)
	{
		ReturnResult(peer->IsConnected(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1]))), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetIndexFromSystemAddress")==0)
	{
		ReturnResult(peer->GetIndexFromSystemAddress(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1]))), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetSystemAddressFromIndex")==0)
	{
		ReturnResult(peer->GetSystemAddressFromIndex(atoi(parameterList[0])), command, transport, systemAddress);
	}
	else if (strcmp(command, "AddToBanList")==0)
	{
		peer->AddToBanList(parameterList[0], atoi(parameterList[1]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "RemoveFromBanList")==0)
	{
		peer->RemoveFromBanList(parameterList[0]);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "ClearBanList")==0)
	{
		peer->ClearBanList();
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "IsBanned")==0)
	{
		ReturnResult(peer->IsBanned(parameterList[0]), command, transport, systemAddress);
	}
	else if (strcmp(command, "Ping1")==0)
	{
		peer->Ping(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1])));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "Ping2")==0)
	{
		peer->Ping(parameterList[0], (unsigned short) atoi(parameterList[1]), atoi(parameterList[2])!=0);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "GetAveragePing")==0)
	{
		ReturnResult(peer->GetAveragePing(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1]))), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetLastPing")==0)
	{
		ReturnResult(peer->GetLastPing(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1]))), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetLowestPing")==0)
	{
		ReturnResult(peer->GetLowestPing(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1]))), command, transport, systemAddress);
	}
	else if (strcmp(command, "SetOccasionalPing")==0)
	{
		peer->SetOccasionalPing(atoi(parameterList[0])!=0);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "SetOfflinePingResponse")==0)
	{
		peer->SetOfflinePingResponse(parameterList[0], atoi(parameterList[1]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "GetInternalID")==0)
	{
		ReturnResult(peer->GetInternalID(), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetExternalID")==0)
	{
		ReturnResult(peer->GetExternalID(IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1]))), command, transport, systemAddress);
	}
	else if (strcmp(command, "SetTimeoutTime")==0)
	{
		peer->SetTimeoutTime(atoi(parameterList[0]), IntegersToSystemAddress(atoi(parameterList[0]), atoi(parameterList[1])));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "SetMTUSize")==0)
	{
		ReturnResult(peer->SetMTUSize(atoi(parameterList[0]), UNASSIGNED_SYSTEM_ADDRESS), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetMTUSize")==0)
	{
		ReturnResult(peer->GetMTUSize(UNASSIGNED_SYSTEM_ADDRESS), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetNumberOfAddresses")==0)
	{
		ReturnResult((int)peer->GetNumberOfAddresses(), command, transport, systemAddress);
	}
	else if (strcmp(command, "GetLocalIP")==0)
	{
		ReturnResult((char*) peer->GetLocalIP(atoi(parameterList[0])), command, transport, systemAddress);
	}
	else if (strcmp(command, "AllowConnectionResponseIPMigration")==0)
	{
		peer->AllowConnectionResponseIPMigration(atoi(parameterList[0])!=0);
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "AdvertiseSystem")==0)
	{
		peer->AdvertiseSystem(parameterList[0], (unsigned short) atoi(parameterList[1]),parameterList[2],atoi(parameterList[3]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "ApplyNetworkSimulator")==0)
	{
		peer->ApplyNetworkSimulator(atof(parameterList[0]), (unsigned short) atoi(parameterList[1]),(unsigned short) atoi(parameterList[2]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "IsNetworkSimulatorActive")==0)
	{
		ReturnResult(peer->IsNetworkSimulatorActive(), command, transport, systemAddress);
	}
	else if (strcmp(command, "SetIncomingPassword")==0)
	{
		peer->SetIncomingPassword(parameterList[0], atoi(parameterList[1]));
		ReturnResult(command, transport, systemAddress);
	}
	else if (strcmp(command, "GetIncomingPassword")==0)
	{
		char password[256];
		int passwordLength;
		peer->GetIncomingPassword(password, &passwordLength);
		if (passwordLength)
			ReturnResult((char*)password, command, transport, systemAddress);
		else
			ReturnResult(0, command, transport, systemAddress);
	}

	return true;
}
const char *RakNetCommandParser::GetName(void) const
{
	return "RakNet";
}
void RakNetCommandParser::SendHelp(TransportInterface *transport, SystemAddress systemAddress)
{
	if (peer)
	{
		transport->Send(systemAddress, "The RakNet parser provides mirror functions to RakPeer\r\n");
		transport->Send(systemAddress, "SystemAddresss take two parameters: send <BinaryAddress> <Port>.\r\n");
		transport->Send(systemAddress, "For bool, send 1 or 0.\r\n");
	}
	else
	{
		transport->Send(systemAddress, "Parser not active.  Call SetRakPeerInterface.\r\n");
	}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
