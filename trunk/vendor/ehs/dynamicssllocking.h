
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

#ifndef DYNAMIC_SSL_LOCKING
#define DYNAMIC_SSL_LOCKING

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <assert.h>

#include "threadabstractionlayer.h"

/// used for dynamic locking callback
struct CRYPTO_dynlock_value
{

	MUTEX_TYPE oMutex;

};


/// dynamic locking mechanism for OpenSSL.  Created as needed.
class DynamicSslLocking
{

  public:

	/// default constructor
	DynamicSslLocking ( );
   
	/// destructor
	~DynamicSslLocking ( );
	
	
  protected:

	/// callback for creating a CRYPTO_dynlock_value object for later use
	static CRYPTO_dynlock_value * DynamicLockCreateCallback ( const char * ipsFile,
														   int inLine );

	/// callback for locking/unlocking CRYPTO_dynlock_value
	static void DynamicLockCallback ( int inMode,
									  CRYPTO_dynlock_value * ipoDynlock,
									  const char * ipsFile,
									  int inLine );

	/// callback for freeing CRYPTO_dynlock_value
	static void DynamicLockCleanupCallback ( struct CRYPTO_dynlock_value * ipoDynlock,
											 const char * ipsFile,
											 int inLine );

};

#endif
