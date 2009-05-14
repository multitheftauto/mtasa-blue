/// \file
/// \brief SocketLayer class implementation
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "SocketLayer.h"
#include "RakAssert.h"
#include "MTUSize.h"

#ifdef USE_STEAM_SOCKET_FUNCTIONS
#include "RakNetTypes.h"
#include "Lobby2Client_Steam.h"
#endif

#ifdef _WIN32
#elif !defined(_PS3) && !defined(__PS3__) && !defined(SN_TARGET_PS3)
#include <string.h> // memcpy
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>  // error numbers
#include <stdio.h> // RAKNET_DEBUG_PRINTF
#endif

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#include "np/common.h"
#include <netdb.h>
#include <string.h>
// OK this is lame but you need to know the app port when sending to the external IP.
// I'm just assuming it's the same as our own because it would break the interfaces to pass it to SendTo directly.
static unsigned short HACK_APP_PORT;
#endif

#if defined(_XBOX) || defined(X360)
#include "WSAStartupSingleton.h"
#elif defined(_WIN32)
#include "WSAStartupSingleton.h"
#include <ws2tcpip.h> // 'IP_DONTFRAGMENT' 'IP_TTL'
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#define closesocket socketclose
#else
#define closesocket close
#include <unistd.h>
#endif

#include "RakSleep.h"
#include <stdio.h>

#include "ExtendedOverlappedPool.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

SocketLayer SocketLayer::I;

extern void ProcessNetworkPacket( const unsigned int binaryAddress, const unsigned short port, const char *data, const int length, RakPeer *rakPeer, unsigned connectionSocketIndex );
extern void ProcessPortUnreachable( const unsigned int binaryAddress, const unsigned short port, RakPeer *rakPeer );

#ifdef _DEBUG
#include <stdio.h>
#endif

SocketLayer::SocketLayer()
{
#ifdef _WIN32
	WSAStartupSingleton::AddRef();
#endif
}

SocketLayer::~SocketLayer()
{
#ifdef _WIN32
	WSAStartupSingleton::Deref();
#endif
}

SOCKET SocketLayer::Connect( SOCKET writeSocket, unsigned int binaryAddress, unsigned short port )
{
	RakAssert( writeSocket != (SOCKET) -1 );
	sockaddr_in connectSocketAddress;

	connectSocketAddress.sin_family = AF_INET;
	connectSocketAddress.sin_port = htons( port );
	connectSocketAddress.sin_addr.s_addr = binaryAddress;

	if ( connect( writeSocket, ( struct sockaddr * ) & connectSocketAddress, sizeof( struct sockaddr ) ) != 0 )
	{
#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG) && !defined(X360)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) &messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "WSAConnect failed:Error code - %d\n%s", dwIOError, messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
#endif
	}

	return writeSocket;
}
bool SocketLayer::IsPortInUse(unsigned short port)
{
	SOCKET listenSocket;
	sockaddr_in listenerSocketAddress;
	// Listen on our designated Port#
	listenerSocketAddress.sin_port = htons( port );
	listenSocket = socket( AF_INET, SOCK_DGRAM, 0 );
	if ( listenSocket == (SOCKET) -1 )
		return true;
	// bind our name to the socket
	// Fill in the rest of the address structure
	listenerSocketAddress.sin_family = AF_INET;
	listenerSocketAddress.sin_addr.s_addr = INADDR_ANY;
	int ret = bind( listenSocket, ( struct sockaddr * ) & listenerSocketAddress, sizeof( listenerSocketAddress ) );
	closesocket(listenSocket);
	return ret <= -1;
}
void SocketLayer::SetSocketOptions( SOCKET listenSocket)
{
	int sock_opt = 1;
	// // On Vista, can get WSAEACCESS (10013)
	/*
	if ( setsockopt( listenSocket, SOL_SOCKET, SO_REUSEADDR, ( char * ) & sock_opt, sizeof ( sock_opt ) ) == -1 )
	{
	#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG) && !defined(X360)
	DWORD dwIOError = GetLastError();
	LPVOID messageBuffer;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
	( LPTSTR ) & messageBuffer, 0, NULL );
	// something has gone wrong here...
	RAKNET_DEBUG_PRINTF( "setsockopt(SO_REUSEADDR) failed:Error code - %d\n%s", dwIOError, messageBuffer );
	//Free the buffer.
	LocalFree( messageBuffer );
	#endif
	}
	*/

	// This doubles the max throughput rate
	sock_opt=1024*256;
	setsockopt(listenSocket, SOL_SOCKET, SO_RCVBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) );

	// Immediate hard close. Don't linger the socket, or recreating the socket quickly on Vista fails.
	sock_opt=0;
	setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, ( char * ) & sock_opt, sizeof ( sock_opt ) );

#if !defined(_PS3) && !defined(__PS3__) && !defined(SN_TARGET_PS3)
	// This doesn't make much difference: 10% maybe
	// Not supported on console 2
	sock_opt=1024*16;
	setsockopt(listenSocket, SOL_SOCKET, SO_SNDBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) );
#endif

#ifdef _WIN32
	unsigned long nonblocking = 1;
	ioctlsocket( listenSocket, FIONBIO, &nonblocking );
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
	sock_opt=1;
	setsockopt(listenSocket, SOL_SOCKET, SO_NBIO, ( char * ) & sock_opt, sizeof ( sock_opt ) );
#else
	fcntl( listenSocket, F_SETFL, O_NONBLOCK );
#endif

#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG) && !defined(X360)
	// If this assert hit you improperly linked against WSock32.h
	RakAssert(IP_DONTFRAGMENT==14);
#endif

	// TODO - I need someone on dialup to test this with :(
	// Path MTU Detection

	//	if ( setsockopt( listenSocket, IPPROTO_IP, IP_DONTFRAGMENT, ( char * ) & sock_opt, sizeof ( sock_opt ) ) == -1 )
	//	{
	//#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG)
	//		DWORD dwIOError = GetLastError();
	//		LPVOID messageBuffer;
	//		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	//			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
	//			( LPTSTR ) & messageBuffer, 0, NULL );
	// something has gone wrong here...
	//		RAKNET_DEBUG_PRINTF( "setsockopt(IP_DONTFRAGMENT) failed:Error code - %d\n%s", dwIOError, messageBuffer );
	//Free the buffer.
	//		LocalFree( messageBuffer );
	//#endif
	//	}

	// Set broadcast capable
	sock_opt=1;
	if ( setsockopt( listenSocket, SOL_SOCKET, SO_BROADCAST, ( char * ) & sock_opt, sizeof( sock_opt ) ) == -1 )
		{
#if defined(_WIN32) && !defined(_XBOX) && !defined(X360) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		// On Vista, can get WSAEACCESS (10013)
		// See http://support.microsoft.com/kb/819124
		// http://blogs.msdn.com/wndp/archive/2007/03/19/winsock-so-exclusiveaddruse-on-vista.aspx
		// http://msdn.microsoft.com/en-us/library/ms740621(VS.85).aspx
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "setsockopt(SO_BROADCAST) failed:Error code - %d\n%s", dwIOError, messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
#endif

		}
}
SOCKET SocketLayer::CreateBoundSocket_PS3Lobby( unsigned short port, bool blockingSocket, const char *forceHostAddress )
{
	(void) port;
	(void) blockingSocket;
	(void) forceHostAddress;

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
	(void) blockingSocket;

	int ret;
	SOCKET listenSocket;
	sockaddr_in_p2p listenerSocketAddress;
	memset(&listenerSocketAddress, 0, sizeof(listenerSocketAddress));

	// Lobby version
	listenerSocketAddress.sin_port = htons(SCE_NP_PORT);
	HACK_APP_PORT = port; // Save the locally bound port
	listenerSocketAddress.sin_vport = htons(port);
	listenSocket = socket( AF_INET, SOCK_DGRAM_P2P, 0 );

	// Normal version as below

	if ( listenSocket == (SOCKET) -1 )
	{
		return (SOCKET) -1;
	}

	SetSocketOptions(listenSocket);

	// Fill in the rest of the address structure
	listenerSocketAddress.sin_family = AF_INET;

	if (forceHostAddress && forceHostAddress[0])
	{
		listenerSocketAddress.sin_addr.s_addr = inet_addr( forceHostAddress );
	}
	else
	{
		listenerSocketAddress.sin_addr.s_addr = INADDR_ANY;
	}

	// bind our name to the socket
	ret = bind( listenSocket, ( struct sockaddr * ) & listenerSocketAddress, sizeof( listenerSocketAddress ) );

	if ( ret <= -1 )
	{
	switch (ret)
		{
		case EBADF:
			RAKNET_DEBUG_PRINTF("bind(): sockfd is not a valid descriptor.\n"); break;
		case EINVAL:
			RAKNET_DEBUG_PRINTF("bind(): The addrlen is wrong, or the socket was not in the AF_UNIX family.\n"); break;
		case EROFS:
			RAKNET_DEBUG_PRINTF("bind(): The socket inode would reside on a read-only file system.\n"); break;
		case EFAULT:
			RAKNET_DEBUG_PRINTF("bind(): my_addr points outside the user's accessible address space.\n"); break;
		case ENAMETOOLONG:
			RAKNET_DEBUG_PRINTF("bind(): my_addr is too long.\n"); break;
		case ENOENT:
			RAKNET_DEBUG_PRINTF("bind(): The file does not exist.\n"); break;
		case ENOMEM:
			RAKNET_DEBUG_PRINTF("bind(): Insufficient kernel memory was available.\n"); break;
		case ENOTDIR:
			RAKNET_DEBUG_PRINTF("bind(): A component of the path prefix is not a directory.\n"); break;
		case EACCES:
			RAKNET_DEBUG_PRINTF("bind(): Search permission is denied on a component of the path prefix.\n"); break;
		default:
			RAKNET_DEBUG_PRINTF("Unknown bind() error %i.\n", ret); break;
		}

		return (SOCKET) -1;
	}

	return listenSocket;
#else
	return 0;
#endif
}
SOCKET SocketLayer::CreateBoundSocket( unsigned short port, bool blockingSocket, const char *forceHostAddress )
{
	(void) blockingSocket;

	int ret;
	SOCKET listenSocket;
	sockaddr_in listenerSocketAddress;
	// Listen on our designated Port#
	listenerSocketAddress.sin_port = htons( port );
	listenSocket = socket( AF_INET, SOCK_DGRAM, 0 );

	if ( listenSocket == (SOCKET) -1 )
	{
#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "socket(...) failed:Error code - %d\n%s", dwIOError, messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
#endif

		return (SOCKET) -1;
	}

	SetSocketOptions(listenSocket);

	// Fill in the rest of the address structure
	listenerSocketAddress.sin_family = AF_INET;

	if (forceHostAddress && forceHostAddress[0])
	{
		listenerSocketAddress.sin_addr.s_addr = inet_addr( forceHostAddress );
	}
	else
	{
		listenerSocketAddress.sin_addr.s_addr = INADDR_ANY;
	}

	// bind our name to the socket
	ret = bind( listenSocket, ( struct sockaddr * ) & listenerSocketAddress, sizeof( listenerSocketAddress ) );

	if ( ret <= -1 )
	{
#if defined(_WIN32) && !defined(_XBOX) && !defined(X360)
		DWORD dwIOError = GetLastError();
		if (dwIOError==10048)
		{
			// Vista has a bug where it returns WSAEADDRINUSE (10048) if you create, shutdown, then rebind the socket port unless you wait a while first.
			// Wait, then rebind
			RakSleep(100);

			closesocket(listenSocket);
			listenerSocketAddress.sin_port = htons( port );
			listenSocket = socket( AF_INET, SOCK_DGRAM, 0 );
			if ( listenSocket == (SOCKET) -1 )
				return false;
			SetSocketOptions(listenSocket);

			// Fill in the rest of the address structure
			listenerSocketAddress.sin_family = AF_INET;
			if (forceHostAddress && forceHostAddress[0])
				listenerSocketAddress.sin_addr.s_addr = inet_addr( forceHostAddress );
			else
				listenerSocketAddress.sin_addr.s_addr = INADDR_ANY;

			// bind our name to the socket
			ret = bind( listenSocket, ( struct sockaddr * ) & listenerSocketAddress, sizeof( listenerSocketAddress ) );

			if ( ret >= 0 )
				return listenSocket;
		}
		dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "bind(...) failed:Error code - %d\n%s", (unsigned int) dwIOError, (char*) messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
#elif (defined(__GNUC__)  || defined(__GCCXML__) || defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)) && !defined(__WIN32)
		switch (ret)
		{
		case EBADF:
			RAKNET_DEBUG_PRINTF("bind(): sockfd is not a valid descriptor.\n"); break;
#if !defined(_PS3) && !defined(__PS3__) && !defined(SN_TARGET_PS3)
		case ENOTSOCK:
			RAKNET_DEBUG_PRINTF("bind(): Argument is a descriptor for a file, not a socket.\n"); break;
#endif
		case EINVAL:
			RAKNET_DEBUG_PRINTF("bind(): The addrlen is wrong, or the socket was not in the AF_UNIX family.\n"); break;
		case EROFS:
			RAKNET_DEBUG_PRINTF("bind(): The socket inode would reside on a read-only file system.\n"); break;
		case EFAULT:
			RAKNET_DEBUG_PRINTF("bind(): my_addr points outside the user's accessible address space.\n"); break;
		case ENAMETOOLONG:
			RAKNET_DEBUG_PRINTF("bind(): my_addr is too long.\n"); break;
		case ENOENT:
			RAKNET_DEBUG_PRINTF("bind(): The file does not exist.\n"); break;
		case ENOMEM:
			RAKNET_DEBUG_PRINTF("bind(): Insufficient kernel memory was available.\n"); break;
		case ENOTDIR:
			RAKNET_DEBUG_PRINTF("bind(): A component of the path prefix is not a directory.\n"); break;
		case EACCES:
			RAKNET_DEBUG_PRINTF("bind(): Search permission is denied on a component of the path prefix.\n"); break;
#if !defined(_PS3) && !defined(__PS3__) && !defined(SN_TARGET_PS3)
		case ELOOP:
			RAKNET_DEBUG_PRINTF("bind(): Too many symbolic links were encountered in resolving my_addr.\n"); break;
#endif
		default:
			RAKNET_DEBUG_PRINTF("Unknown bind() error %i.\n", ret); break;
		}
#endif

		return (SOCKET) -1;
	}

	return listenSocket;
}

#if !defined(_XBOX) && !defined(X360)
const char* SocketLayer::DomainNameToIP( const char *domainName )
{
	struct hostent * phe = gethostbyname( domainName );

	if ( phe == 0 || phe->h_addr_list[ 0 ] == 0 )
	{
		//cerr << "Yow! Bad host lookup." << endl;
		return 0;
	}

	struct in_addr addr;
	if (phe->h_addr_list[ 0 ]==0)
		return 0;

	memcpy( &addr, phe->h_addr_list[ 0 ], sizeof( struct in_addr ) );
	return inet_ntoa( addr );
}
#endif


void SocketLayer::Write( const SOCKET writeSocket, const char* data, const int length )
{
#ifdef _DEBUG
	RakAssert( writeSocket != (SOCKET) -1 );
#endif

	send( writeSocket, data, length, 0 );
}
// REMOVEME
//#include "BitStream.h"
int SocketLayer::RecvFrom( const SOCKET s, RakPeer *rakPeer, int *errorCode, unsigned connectionSocketIndex, bool isPs3LobbySocket )
{
	int len=0;
	char data[ MAXIMUM_MTU_SIZE ];

	// Poseidon damn them 
#ifdef USE_STEAM_SOCKET_FUNCTIONS
	SystemAddress sender;
	if (RakNet::Lobby2Client_Steam::RecvFrom(data,&len,&sender))
	{
		ProcessNetworkPacket( sender.binaryAddress, sender.port, data, len, rakPeer, connectionSocketIndex );
		return 1;
	}
#endif

	if ( s == (SOCKET) -1 )
	{
		*errorCode = -1;
		return -1;
	}

#if defined (_WIN32) || !defined(MSG_DONTWAIT)
	const int flag=0;
#else
	const int flag=MSG_DONTWAIT;
#endif

	sockaddr_in sa;
	socklen_t len2;
	unsigned short portnum=0;
	if (isPs3LobbySocket)
	{
#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
		sockaddr_in_p2p sap2p;
		len2 = sizeof( sap2p );
		sap2p.sin_family = AF_INET;
		len = recvfrom( s, data, MAXIMUM_MTU_SIZE, flag, ( sockaddr* ) & sap2p, ( socklen_t* ) & len2 );
		portnum = ntohs( sap2p.sin_port );
		sa.sin_addr.s_addr = sap2p.sin_addr.s_addr;
#endif
	}
	else
	{
		len2 = sizeof( sa );
		sa.sin_family = AF_INET;
		len = recvfrom( s, data, MAXIMUM_MTU_SIZE, flag, ( sockaddr* ) & sa, ( socklen_t* ) & len2 );
		portnum = ntohs( sa.sin_port );
	}

	if ( len == 0 )
	{
#ifdef _DEBUG
		RAKNET_DEBUG_PRINTF( "Error: recvfrom returned 0 on a connectionless blocking call\non port %i.  This is a bug with Zone Alarm.  Please turn off Zone Alarm.\n", portnum );
		RakAssert( 0 );
#endif

		// 4/13/09 Changed from returning -1 to 0, to prevent attackers from sending 0 byte messages to shutdown the server
		*errorCode = 0;
		return 0;
	}

	if ( len > 0 )
	{

		// REMOVEME
//		char ip[256];
	//	strcpy(ip, inet_ntoa(sa.sin_addr));
	//	if (strcmp(ip, "69.129.39.163")==0)
//		if (len>=30 && len <= 36)
//		{
//			strcpy(ip, inet_ntoa(sa.sin_addr));
//			RakNet::BitStream bs((unsigned char*) data,len,false);
//			printf("%s: ", ip);
//			bs.PrintHex();
//			printf("\n");
//		}

		// strcpy(ip, "127.0.0.1");
		ProcessNetworkPacket( sa.sin_addr.s_addr, portnum, data, len, rakPeer, connectionSocketIndex );

		return 1;
	}
	else
	{
		*errorCode = 0;


#if defined(_WIN32) && !defined(_XBOX) && !defined(X360) && defined(_DEBUG)

		DWORD dwIOError = WSAGetLastError();

		if ( dwIOError == WSAEWOULDBLOCK )
		{
			return SOCKET_ERROR;
		}
		if ( dwIOError == WSAECONNRESET )
		{
#if defined(_DEBUG)
//			RAKNET_DEBUG_PRINTF( "A previous send operation resulted in an ICMP Port Unreachable message.\n" );
#endif


			unsigned short portnum=0;
			ProcessPortUnreachable(sa.sin_addr.s_addr, portnum, rakPeer);
			// *errorCode = dwIOError;
			return -1;
		}
		else
		{
#if defined(_DEBUG)
			if ( dwIOError != WSAEINTR )
			{
				LPVOID messageBuffer;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
					( LPTSTR ) & messageBuffer, 0, NULL );
				// something has gone wrong here...
				RAKNET_DEBUG_PRINTF( "recvfrom failed:Error code - %d\n%s", dwIOError, messageBuffer );

				//Free the buffer.
				LocalFree( messageBuffer );
			}
#endif
		}
#endif
	}

	return 0; // no data
}

#ifdef _MSC_VER
#pragma warning( disable : 4702 ) // warning C4702: unreachable code
#endif
int SocketLayer::SendTo( SOCKET s, const char *data, int length, unsigned int binaryAddress, unsigned short port, bool isPs3LobbySocket )
{
	// Poseidon damn them 
#ifdef USE_STEAM_SOCKET_FUNCTIONS
	RakNet::Lobby2Client_Steam::SendTo(data,length,SystemAddress(binaryAddress,port));
	return length;
#endif

	if ( s == (SOCKET) -1 )
	{
		return -1;
	}

	int len=0;

	if (isPs3LobbySocket)
	{
#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
		sockaddr_in_p2p sa;
		memset(&sa, 0, sizeof(sa));
		// LAME!!!! You have to know the behind-nat port on the recipient! Just guessing it is the same as our own
		sa.sin_vport = htons(HACK_APP_PORT);
		sa.sin_port = htons(port); // Port returned from signaling
		sa.sin_addr.s_addr = binaryAddress;
		sa.sin_family = AF_INET;
		do
		{
			len = sendto( s, data, length, 0, ( const sockaddr* ) & sa, sizeof( sa ) );
		}
		while ( len == 0 );

#endif
	}
	else
	{
		sockaddr_in sa;
		sa.sin_port = htons( port ); // User port
		sa.sin_addr.s_addr = binaryAddress;
		sa.sin_family = AF_INET;
		do
		{
			len = sendto( s, data, length, 0, ( const sockaddr* ) & sa, sizeof( sa ) );
		}
		while ( len == 0 );
	}

	if ( len != -1 )
		return 0;

#if defined(_WIN32) && !defined(_WIN32_WCE)

	DWORD dwIOError = WSAGetLastError();

	if ( dwIOError == WSAECONNRESET )
	{
#if defined(_DEBUG)
		RAKNET_DEBUG_PRINTF( "A previous send operation resulted in an ICMP Port Unreachable message.\n" );
#endif

	}
	else if ( dwIOError != WSAEWOULDBLOCK )
	{
#if defined(_WIN32) && !defined(_XBOX) && !defined(X360) && defined(_DEBUG)
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "sendto failed:Error code - %d\n%s", dwIOError, messageBuffer );

		//Free the buffer.
		LocalFree( messageBuffer );
#endif

	}

	return dwIOError;
#endif

	return 1; // error
}

int SocketLayer::SendTo( SOCKET s, const char *data, int length, const char ip[ 16 ], unsigned short port, bool isPs3LobbySocket )
{
	unsigned int binaryAddress;
	binaryAddress = inet_addr( ip );
	return SendTo( s, data, length, binaryAddress, port,isPs3LobbySocket );
}
int SocketLayer::SendToTTL( SOCKET s, const char *data, int length, const char ip[ 16 ], unsigned short port, int ttl )
{
	// Poseidon damn them 
#ifdef USE_STEAM_SOCKET_FUNCTIONS
	unsigned int binaryAddress;
	binaryAddress = inet_addr( ip );
	RakNet::Lobby2Client_Steam::SendTo(data,length,SystemAddress(binaryAddress,port));
	return length;
#endif

#if !defined(_XBOX) && !defined(X360)
	int oldTTL;
	socklen_t opLen=sizeof(oldTTL);
	// Get the current TTL
	if (getsockopt(s, IPPROTO_IP, IP_TTL, ( char * ) & oldTTL, &opLen ) == -1)
	{
#if defined(_WIN32) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "getsockopt(IPPROTO_IP,IP_TTL) failed:Error code - %d\n%s", dwIOError, messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
#endif
	}

	// Set to TTL
	int newTTL=ttl;
	if (setsockopt(s, IPPROTO_IP, IP_TTL, ( char * ) & newTTL, sizeof ( newTTL ) ) == -1)
	{

#if defined(_WIN32) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "setsockopt(IPPROTO_IP,IP_TTL) failed:Error code - %d\n%s", dwIOError, messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
#endif
	}

	// Send
	int res = SendTo(s,data,length,ip,port,false);

	// Restore the old TTL
	setsockopt(s, IPPROTO_IP, IP_TTL, ( char * ) & oldTTL, opLen );

	return res;
#else
	return 0;
#endif
}

#if !defined(_XBOX) && !defined(X360)
void SocketLayer::GetMyIP( char ipList[ MAXIMUM_NUMBER_OF_INTERNAL_IDS ][ 16 ] )
{
#if !defined(_PS3) && !defined(__PS3__) && !defined(SN_TARGET_PS3)
	char ac[ 80 ];
	if ( gethostname( ac, sizeof( ac ) ) == -1 )
	{
	#if defined(_WIN32) && !defined(_XBOX) && !defined(X360) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "gethostname failed:Error code - %d\n%s", dwIOError, messageBuffer );
		//Free the buffer.
		LocalFree( messageBuffer );
	#endif

		return ;
	}

	struct hostent *phe = gethostbyname( ac );


	if ( phe == 0 )
	{
#if defined(_WIN32) && !defined(_XBOX) && !defined(X360) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "gethostbyname failed:Error code - %d\n%s", dwIOError, messageBuffer );

		//Free the buffer.
		LocalFree( messageBuffer );
#endif

		return ;
	}

	int idx;
	for ( idx = 0; idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; ++idx )
	{
		if (phe->h_addr_list[ idx ] == 0)
			break;

		struct in_addr addr;

		memcpy( &addr, phe->h_addr_list[ idx ], sizeof( struct in_addr ) );
		//cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
		strcpy( ipList[ idx ], inet_ntoa( addr ) );
		
	}

	for ( ; idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; ++idx )
	{
		ipList[idx][0]=0;
	}
#else
	union CellNetCtlInfo info;
	int errCode;
	if((errCode=cellNetCtlGetInfo(CELL_NET_CTL_INFO_IP_ADDRESS, &info)) >= 0){
		memcpy(ipList[0], info.ip_address, sizeof(info.ip_address));
		ipList[1][0]=0;
	}
	return;
#endif

}
#endif

unsigned short SocketLayer::GetLocalPort ( SOCKET s )
{
	sockaddr_in sa;
	socklen_t len = sizeof(sa);
	if (getsockname(s, (sockaddr*)&sa, &len)!=0)
	{
#if defined(_WIN32) && !defined(_XBOX) && !defined(X360) && defined(_DEBUG)
		DWORD dwIOError = GetLastError();
		LPVOID messageBuffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
			( LPTSTR ) & messageBuffer, 0, NULL );
		// something has gone wrong here...
		RAKNET_DEBUG_PRINTF( "getsockname failed:Error code - %d\n%s", dwIOError, messageBuffer );

		//Free the buffer.
		LocalFree( messageBuffer );
#endif
		return 0;
	}
	return ntohs(sa.sin_port);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
