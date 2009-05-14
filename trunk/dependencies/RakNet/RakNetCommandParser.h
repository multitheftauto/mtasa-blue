/// \file
/// \brief Contains RakNetCommandParser , used to send commands to an instance of RakPeer
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __RAKNET_COMMAND_PARSER
#define __RAKNET_COMMAND_PARSER

#include "CommandParserInterface.h"
#include "Export.h"
class RakPeerInterface;

/// \brief This allows a console client to call most of the functions in RakPeer
class RAK_DLL_EXPORT RakNetCommandParser : public CommandParserInterface
{
public:
	RakNetCommandParser();
	~RakNetCommandParser();

	/// Given \a command with parameters \a parameterList , do whatever processing you wish.
	/// \param[in] command The command to process
	/// \param[in] numParameters How many parameters were passed along with the command
	/// \param[in] parameterList The list of parameters.  parameterList[0] is the first parameter and so on.
	/// \param[in] transport The transport interface we can use to write to
	/// \param[in] systemAddress The player that sent this command.
	/// \param[in] originalString The string that was actually sent over the network, in case you want to do your own parsing
	bool OnCommand(const char *command, unsigned numParameters, char **parameterList, TransportInterface *transport, SystemAddress systemAddress, const char *originalString);

	/// You are responsible for overriding this function and returning a static string, which will identifier your parser.
	/// This should return a static string
	/// \return The name that you return.
	const char *GetName(void) const;

	/// A callback for when you are expected to send a brief description of your parser to \a systemAddress
	/// \param[in] transport The transport interface we can use to write to
	/// \param[in] systemAddress The player that requested help.
	void SendHelp(TransportInterface *transport, SystemAddress systemAddress);

	/// Records the instance of RakPeer to perform the desired commands on
	/// \param[in] rakPeer The RakPeer instance, or a derived class (e.g. RakPeer or RakPeer)
	void SetRakPeerInterface(RakPeerInterface *rakPeer);
protected:

	/// Which instance of RakPeer we are working on.  Set from SetRakPeerInterface()
	RakPeerInterface *peer;
};

#endif
