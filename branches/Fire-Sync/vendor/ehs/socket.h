
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



#ifndef SOCKET_H
#define SOCKET_H

///////////////////////////////////
#ifdef _WIN32 // windows headers //
///////////////////////////////////

// Pragma'ing away nasty MS 255-char-name problem.  Otherwise
// you will get warnings on many template names that
//	"identifier was truncated to '255' characters in the debug information".
#pragma warning(disable : 4786)

// to use winsock2.h instead of winsock.h
#define _WIN32_WINNT 0x0400
#include <winsock.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

// make stricmp sound like strcasecmp
#define strcasecmp stricmp

// make windows sleep act like UNIX sleep
#define sleep(seconds) (Sleep(seconds * 1000))

///////////////////////////////////
#else // unix headers go here    //
///////////////////////////////////

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

///////////////////////////////////
#endif // end platform headers   //
///////////////////////////////////

#include "networkabstraction.h"

#ifdef _WIN32
    #define E_WOULDBLOCK WSAEWOULDBLOCK
    #define GetLastSocketError() WSAGetLastError()
#else
    #define E_WOULDBLOCK EWOULDBLOCK
    #define GetLastSocketError() errno
    #define SOCKET_ERROR (-1)
    #define INVALID_SOCKET (0)
    #ifndef Sleep
        #define Sleep(x) usleep((x)*1000)
    #endif
#endif

/// plain socket implementation of NetworkAbstraction
class Socket : public NetworkAbstraction {

  public:
	
	/// sets up socket stuff (mostly for win32) and then listens on specified port
	virtual InitResult Init ( int iIP, int inPort );

	/// default constructor
	Socket ( ) : nAcceptSocket ( INVALID_SOCKET ) { }

	/// client socket constructor
	Socket ( int inAcceptSocket, sockaddr_in * );

	/// destructor
	virtual ~Socket ( );

	/// returns the FD associated with this socket
	virtual int GetFd ( ) { return nAcceptSocket; };

	/// implements standard FD read
	virtual int Read ( void * ipBuffer, int ipBufferLength );

	/// implements standard FD send
	virtual int Send ( const void * ipMessage, size_t inLength, int inFlags = 0 );

	/// implements standard FD close
	virtual void Close ( );

	/// implements standard FD accept
	virtual NetworkAbstraction * Accept ( );
	
	/// Returns false, plain sockets are not secure
	virtual int IsSecure ( ) { return 0; }

    // Check status
    virtual bool IsReadable( int inTimeoutMilliseconds );
    virtual bool IsWritable( int inTimeoutMilliseconds );
    virtual bool IsAtError( int inTimeoutMilliseconds );
  protected:

	/// Socket on which this connection came in
	int nAcceptSocket;


	/// stores the address of the current connection
	sockaddr_in oInternetSocketAddress;

	/// returns the address of the incoming connection
	std::string GetAddress ( );

	/// returns the port of the incoming connection
	int GetPort ( );

    // Set non blocking mode
    void SetNonBlocking( bool bOn );

    // Set re-use address option
    void SetReuseAddress( bool bOn );
};

#endif // SOCKET_H
