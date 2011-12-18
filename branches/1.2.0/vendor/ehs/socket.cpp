
/*

EHS is a library for adding web server support to a C++ application
Copyright (C) 2001, 2002 Zac Hansen
  
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License only.
  
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
or see http://www.gnu.org/licenses/gpl.html

Zac Hansen ( xaxxon@slackworks.com )

*/

/*
 * Solaris 2.6 troubles
 */

#ifdef sun
#ifndef BSD_COMP 
#define BSD_COMP 1
#endif
#ifndef _XPG4_2
#define _XPG4_2 1
#endif
#ifndef __EXTENSIONS__
#define __EXTENSIONS__ 1
#endif
#ifndef __PRAGMA_REDEFINE_EXTNAME
#define __PRAGMA_REDEFINE_EXTNAME 1
#endif
#endif // sun

#include "socket.h"

#ifndef _WIN32
#include <assert.h>
#include <stdio.h>
#include <sys/ioctl.h>
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0 // no support
#endif // MSG_NOSIGNAL

extern long long ms_HttpTotalBytesSent;

Socket::Socket ( int inAcceptSocket,
				 sockaddr_in * ipoInternetSocketAddress )
{
	
	nAcceptSocket = inAcceptSocket;

	memcpy ( &oInternetSocketAddress, 
			 ipoInternetSocketAddress,
			 sizeof ( oInternetSocketAddress ) );

}

Socket::~Socket ( )
{
    Close();
}




NetworkAbstraction::InitResult
Socket::Init (  int iIP,   ///< ip address to bind to
		        int inPort ///< port on which to listen
	) 
{

#ifdef _WIN32

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	
	// Found a usable winsock dll?
	assert( err == 0 );

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( 
		LOBYTE( wsaData.wVersion ) != 2 
		||	HIBYTE( wsaData.wVersion ) != 2
		) {

		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );

#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Critical Error: Couldn't find useable Winsock DLL.  Must be at least 2.2.  Aborting\n" );
#endif

		assert( false ); 
	}

	/* The WinSock DLL is acceptable. Proceed. */

#endif // End WIN32-specific network initialization code

	// need to create a socket for listening for new connections
#ifdef EHS_DEBUG
	if ( nAcceptSocket != 0 ) {
		fprintf ( stderr, "[EHS_DEBUG] nAcceptSocket = '%d'\n", nAcceptSocket );
		assert ( nAcceptSocket == 0 );
	}
#endif
	
	nAcceptSocket = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( nAcceptSocket == -1 ) {

#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Error: socket() failed\n" );
#endif

		return INITSOCKET_SOCKETFAILED;
	}

    SetNonBlocking( true );
    SetReuseAddress( true );

	// bind the socket to the appropriate port
	struct sockaddr_in oSocketInfo;
	memset ( &oSocketInfo, 0, sizeof ( oSocketInfo ) );
	int nResult = -1;



	// IP address is 32 bits, int is 32 bits..
	// binds to all interfaces.  This maybe should be customizable
	int nSinAddr = iIP;

	oSocketInfo.sin_family = AF_INET;
	memcpy ( & ( oSocketInfo.sin_addr ), &nSinAddr, sizeof ( oSocketInfo.sin_addr ) );
	oSocketInfo.sin_port = htons ( inPort );

	nResult = bind ( nAcceptSocket, (sockaddr *)&oSocketInfo, sizeof ( sockaddr_in ) );

	if ( nResult == -1 ) {
#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Error: bind() failed\n" );
#endif

		return INITSOCKET_BINDFAILED;
		
	}

	// listen 
	nResult = listen ( nAcceptSocket, 20 );
	if ( nResult != 0 ) {

#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Error: listen() failed\n" );
#endif


		return INITSOCKET_LISTENFAILED;
	}
	

	return INITSOCKET_SUCCESS;


}


int Socket::Read ( void * ipBuffer, int ipBufferLength )
{

	return recv ( nAcceptSocket, 
#ifdef _WIN32
				  (char *) ipBuffer,
#else
				  ipBuffer, 
#endif
				  ipBufferLength, 0 );

}

int Socket::Send ( const void * ipMessage, size_t inLength, int inFlags )
{
    ms_HttpTotalBytesSent += inLength;

	return send ( nAcceptSocket, 
#ifdef _WIN32
		(const char *) ipMessage,
#else
		ipMessage, 
#endif	
		inLength, inFlags | MSG_NOSIGNAL );

}

void Socket::Close ( )
{
    if ( nAcceptSocket == INVALID_SOCKET )
        return;

#ifdef _WIN32
	closesocket ( nAcceptSocket );
#else
	close ( nAcceptSocket );
#endif

    nAcceptSocket = INVALID_SOCKET;
}

NetworkAbstraction * Socket::Accept ( )
{
#ifdef _WIN32
	int oInternetSocketAddressLength = sizeof ( oInternetSocketAddress );
#else
	socklen_t oInternetSocketAddressLength = sizeof ( oInternetSocketAddress );
#endif
	int nNewFd = accept ( nAcceptSocket, 
						  (sockaddr *) &oInternetSocketAddress,
#ifdef _WIN32
						  &oInternetSocketAddressLength 
#else
						  &oInternetSocketAddressLength 
#endif
						  );


	char psBuffer [ 25 ];
	sprintf ( psBuffer,
			  "%d.%d.%d.%d",
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 24 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 16 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 8 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 0 ) & 0xff
		);
#ifdef EHS_DEBUG
	printf ( "Got a connection from %s:%d\n", psBuffer,
			 ntohs(oInternetSocketAddress.sin_port)
 );
#endif

	if ( nNewFd == -1 ) {
		return NULL;
	}
	
	Socket * poSocket = new Socket ( nNewFd, &oInternetSocketAddress );

    poSocket->SetNonBlocking( true );

#ifdef EHS_DEBUG
	fprintf ( stderr, "[EHS_DEBUG] Allocated new socket object with fd='%d' and socket addr='%x'\n",
			  nNewFd,
			  poSocket );
#endif

	return poSocket;

}

std::string Socket::GetAddress ( )
{
	
	// buffer long enough to hold 255.255.255.255
	char psBuffer [ 25 ];

	sprintf ( psBuffer,
			  "%d.%d.%d.%d",
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 24 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 16 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 8 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 0 ) & 0xff
		);

	return psBuffer;

}


int Socket::GetPort ( )
{

	return ntohs ( oInternetSocketAddress.sin_port );

}


void Socket::SetNonBlocking( bool bOn )
{
#ifdef _WIN32
    u_long MyTrueVar = bOn ? 1 : 0; 
    ioctlsocket ( nAcceptSocket, FIONBIO, &MyTrueVar );
#else
	int MyTrueVar =  bOn ? 1 : 0; ;
	ioctl ( nAcceptSocket, FIONBIO, &MyTrueVar );
#endif
}

void Socket::SetReuseAddress( bool bOn )
{
	int MyTrueVar = bOn ? 1 : 0;
	setsockopt ( nAcceptSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) &MyTrueVar, sizeof ( int ) );
}

bool Socket::IsReadable( int inTimeoutMilliseconds )
{
	timeval tv = { 0, inTimeoutMilliseconds * 1000 }; 
	tv.tv_sec = tv.tv_usec / 1000000;
	tv.tv_usec %= 1000000;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(nAcceptSocket, &rfds);
    // See if socket it writable
    int ret = select(nAcceptSocket+1, &rfds, NULL, NULL, &tv);
    if (ret == 0)
        return false;     // Not readable yet
    if (ret == -1)
        return false;    // select error

    return true;
}

bool Socket::IsWritable( int inTimeoutMilliseconds )
{
	timeval tv = { 0, inTimeoutMilliseconds * 1000 }; 
	tv.tv_sec = tv.tv_usec / 1000000;
	tv.tv_usec %= 1000000;
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(nAcceptSocket, &wfds);
    // See if socket it writable
    int ret = select(nAcceptSocket+1, NULL, &wfds, NULL, &tv);
    if (ret == 0)
        return false;     // Not writable yet
    if (ret == -1)
        return false;    // select error

    return true;
}

bool Socket::IsAtError( int inTimeoutMilliseconds )
{
	timeval tv = { 0, inTimeoutMilliseconds * 1000 }; 
	tv.tv_sec = tv.tv_usec / 1000000;
	tv.tv_usec %= 1000000;
    fd_set efds;
    FD_ZERO(&efds);
    FD_SET(nAcceptSocket, &efds);
    // See if socket it writable
    int ret = select(nAcceptSocket+1, NULL, NULL, &efds, &tv);
    if (ret == 0)
        return false;     // Not error
    if (ret == -1)
        return true;    // select error

    return true;
}
