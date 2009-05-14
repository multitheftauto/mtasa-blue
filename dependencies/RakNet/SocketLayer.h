/// \file
/// \brief SocketLayer class implementation
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.



#ifndef __SOCKET_LAYER_H
#define __SOCKET_LAYER_H

#include "RakMemoryOverride.h"
#include "SocketIncludes.h"

//#include "ClientContextStruct.h"

class RakPeer;

// A platform independent implementation of Berkeley sockets, with settings used by RakNet
class SocketLayer
{

public:
	
	/// Default Constructor
	SocketLayer();
	
	/// Destructor	
	~SocketLayer();
	
	// Get the singleton instance of the Socket Layer.
	/// \return unique instance 
	static inline SocketLayer* Instance()
	{
		return & I;
	}

	// Connect a socket to a remote host.
	/// \param[in] writeSocket The local socket.
	/// \param[in] binaryAddress The address of the remote host.
	/// \param[in] port the remote port.
	/// \return A new socket used for communication.
	SOCKET Connect( SOCKET writeSocket, unsigned int binaryAddress, unsigned short port );
	
	/// Creates a bound socket to listen for incoming connections on the specified port
	/// \param[in] port the port number 
	/// \param[in] blockingSocket 
	/// \return A new socket used for accepting clients 
	SOCKET CreateBoundSocket( unsigned short port, bool blockingSocket, const char *forceHostAddress );
	SOCKET CreateBoundSocket_PS3Lobby( unsigned short port, bool blockingSocket, const char *forceHostAddress );

	/// Returns if this specified port is in use, for UDP
	/// \param[in] port the port number 
	/// \return If this port is already in use
	static bool IsPortInUse(unsigned short port);

	#if !defined(_XBOX) && !defined(X360)
	const char* DomainNameToIP( const char *domainName );
	#endif
	
	/// Start an asynchronous read using the specified socket.  The callback will use the specified SystemAddress (associated with this socket) and call either the client or the server callback (one or
	/// the other should be 0)
	/// \note Was used for depreciated IO completion ports.	
	bool AssociateSocketWithCompletionPortAndRead( SOCKET readSocket, unsigned int binaryAddress, unsigned short port, RakPeer* rakPeer );
	
	/// Write \a data of length \a length to \a writeSocket
	/// \param[in] writeSocket The socket to write to
	/// \param[in] data The data to write
	/// \param[in] length The length of \a data	
	void Write( const SOCKET writeSocket, const char* data, const int length );
	
	/// Read data from a socket 
	/// \param[in] s the socket 
	/// \param[in] rakPeer The instance of rakPeer containing the recvFrom C callback
	/// \param[in] errorCode An error code if an error occured .
	/// \param[in] connectionSocketIndex Which of the sockets in RakPeer we are using
	/// \return Returns true if you successfully read data, false on error.
	int RecvFrom( const SOCKET s, RakPeer *rakPeer, int *errorCode, unsigned connectionSocketIndex, bool isPs3LobbySocket );
	
#if !defined(_XBOX) && !defined(_X360)
	/// Retrieve all local IP address in a string format.
	/// \param[in] s The socket whose port we are referring to
	/// \param[in] ipList An array of ip address in dotted notation.
	void GetMyIP( char ipList[ MAXIMUM_NUMBER_OF_INTERNAL_IDS ][ 16 ] );
#endif
	
	/// Call sendto (UDP obviously)
	/// \param[in] s the socket
	/// \param[in] data The byte buffer to send 
	/// \param[in] length The length of the \a data in bytes
	/// \param[in] ip The address of the remote host in dotted notation.
	/// \param[in] port The port number to send to.
	/// \return 0 on success, nonzero on failure.
	int SendTo( SOCKET s, const char *data, int length, const char ip[ 16 ], unsigned short port, bool isPs3LobbySocket );

	/// Call sendto (UDP obviously)
	/// It won't reach the recipient, except on a LAN
	/// However, this is good for opening routers / firewalls
	/// \param[in] s the socket
	/// \param[in] data The byte buffer to send 
	/// \param[in] length The length of the \a data in bytes
	/// \param[in] ip The address of the remote host in dotted notation.
	/// \param[in] port The port number to send to.
	/// \param[in] ttl Max hops of datagram
	/// \return 0 on success, nonzero on failure.
	int SendToTTL( SOCKET s, const char *data, int length, const char ip[ 16 ], unsigned short port, int ttl );

	/// Call sendto (UDP obviously)
	/// \param[in] s the socket
	/// \param[in] data The byte buffer to send 
	/// \param[in] length The length of the \a data in bytes
	/// \param[in] binaryAddress The address of the remote host in binary format.
	/// \param[in] port The port number to send to.
	/// \return 0 on success, nonzero on failure.
	int SendTo( SOCKET s, const char *data, int length, unsigned int binaryAddress, unsigned short port, bool isPs3LobbySocket );

	/// Returns the local port, useful when passing 0 as the startup port.
	/// \param[in] s The socket whose port we are referring to
	/// \return The local port
	static unsigned short GetLocalPort ( SOCKET s );

private:


	static SocketLayer I;
	void SetSocketOptions( SOCKET listenSocket);
};

#endif

