
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

#ifndef STATIC_SSL_LOCKING_H
#define STATIC_SSL_LOCKING_H

#ifdef COMPILE_WITH_SSL


#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <assert.h>

#include "threadabstractionlayer.h"

/// static locking mechanism for OpenSSL
class StaticSslLocking
{

  public:

	/// constructor
	StaticSslLocking ( );

	/// destructor
	~StaticSslLocking ( );

	/// static mutex array
	static MUTEX_TYPE * poMutexes;

  protected:

	/// callback for locking/unlocking a mutex in the array
	static void SslStaticLockCallback ( int inMode, 
										int inMutex,
										const char * ipsFile,
										int inLine );

	/// returns the id of the thread from which it is called
	static unsigned long SslThreadIdCallback ( );



};

#endif // COMPILE_WITH_SSL

#endif // STATIC_SSL_LOCKING_H
