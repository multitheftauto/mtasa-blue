
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

#ifndef NETWORK_ABSTRACTION_H
#define NETWORK_ABSTRACTION_H

#include <string>
#include <stdlib.h>

/// Abstracts different socket types
/**
 * this abstracts the differences between normal sockets and ssl sockets
 *   Socket is the standard socket class and SecureSocket is the SSL
 *   implementation
 */
class NetworkAbstraction {
	
  public:
	
	/// returns the address of the connection
	virtual std::string GetAddress ( ) = 0;

	/// returns the port of the connection
	virtual int GetPort ( ) = 0;

	/// Enumeration of error results for InitSocketsResults
	enum InitResult { INITSOCKET_INVALID,
					  INITSOCKET_SOCKETFAILED,
					  INITSOCKET_BINDFAILED,
					  INITSOCKET_LISTENFAILED,
					  INITSOCKET_FAILED,
					  INITSOCKET_CERTIFICATE,
					  INITSOCKET_SUCCESS };

	/// Iniitalize sockets
	virtual InitResult Init ( int iPort, int inPort ) = 0;

	/// destructor
	virtual ~NetworkAbstraction ( ) {};

	/// returns the FD/Socket for the socket on which we're listening
	virtual int GetFd ( ) = 0;

	/// pure virtual read function to be overloaded in child classes
	virtual int Read ( void * ipBuffer, int ipBufferLength ) = 0;

	/// pure virtual send function to be overloaded in child class
	virtual int Send ( const void * ipMessage, size_t inLength, int inFlags = 0 ) = 0;

	/// pure virtual close function to be overloaded in child class
	virtual void Close ( ) = 0;

	/// pure virtual accept function to be overloaded in child class
	virtual NetworkAbstraction * Accept ( ) = 0;

	/// returns whether the child class connection is considered secure
	virtual int IsSecure ( ) = 0;

};


#endif // NETWORK_ABSTRACTION_H
