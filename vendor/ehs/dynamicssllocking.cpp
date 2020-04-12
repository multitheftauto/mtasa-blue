
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

// must be outside COMPILE_WITH_SSL check
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef COMPILE_WITH_SSL

#include "dynamicssllocking.h"



CRYPTO_dynlock_value * DynamicSslLocking::DynamicLockCreateCallback ( const char * ipsFile,
																   int inLine )
{

	CRYPTO_dynlock_value * poDynlock = new CRYPTO_dynlock_value;

	assert ( poDynlock != NULL );

	MUTEX_SETUP ( poDynlock->oMutex );
	return poDynlock;

}

void DynamicSslLocking::DynamicLockCallback ( int inMode,
											  CRYPTO_dynlock_value * ipoDynlock,
											  const char * ipsFile,
											  int inLine )
{

	if ( inMode & CRYPTO_LOCK ) {
		MUTEX_LOCK ( ipoDynlock->oMutex );
	} else {
		MUTEX_UNLOCK ( ipoDynlock->oMutex );
	}

}

void DynamicSslLocking::DynamicLockCleanupCallback ( struct CRYPTO_dynlock_value * ipoDynlock,
													 const char * ipsFile,
													 int inLine )
{

	MUTEX_CLEANUP ( ipoDynlock->oMutex );
	delete ipoDynlock;

}


DynamicSslLocking::DynamicSslLocking ( )
{

	CRYPTO_set_dynlock_create_callback ( DynamicLockCreateCallback );
	CRYPTO_set_dynlock_lock_callback ( DynamicLockCallback );
	CRYPTO_set_dynlock_destroy_callback ( DynamicLockCleanupCallback );

}

DynamicSslLocking::~DynamicSslLocking ( )
{

	CRYPTO_set_dynlock_create_callback ( NULL );
	CRYPTO_set_dynlock_lock_callback ( NULL );
	CRYPTO_set_dynlock_destroy_callback ( NULL );

}

#endif // COMPILE WITH SSL
