
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

#include "staticssllocking.h"


// deal with static variables
MUTEX_TYPE * StaticSslLocking::poMutexes;


void StaticSslLocking::SslStaticLockCallback ( int inMode,
												int inMutex,
												const char * ipsFile,
												int inLine )
{

	if ( inMode & CRYPTO_LOCK ) {
		MUTEX_LOCK ( StaticSslLocking::poMutexes [ inMutex ] );
	} else {
		MUTEX_UNLOCK ( StaticSslLocking::poMutexes [ inMutex ] );
	}

}

unsigned long StaticSslLocking::SslThreadIdCallback ( )
{

	return ( (unsigned long) THREAD_ID );

}


StaticSslLocking::StaticSslLocking ( )
{

	// allocate the needed number of locks
	StaticSslLocking::poMutexes = new MUTEX_TYPE [ CRYPTO_num_locks ( ) ];

	assert ( StaticSslLocking::poMutexes != NULL );

	// initialize the mutexes
	for ( int i = 0; i < CRYPTO_num_locks ( ); i++ ) {

		MUTEX_SETUP ( StaticSslLocking::poMutexes [ i ] );

	}

	// set callbacks
	CRYPTO_set_id_callback ( StaticSslLocking::SslThreadIdCallback );
	CRYPTO_set_locking_callback ( StaticSslLocking::SslStaticLockCallback );


}

StaticSslLocking::~StaticSslLocking ( )
{

	assert ( StaticSslLocking::poMutexes != NULL );

	CRYPTO_set_id_callback ( NULL );
	CRYPTO_set_locking_callback ( NULL );

	for ( int i = 0; i < CRYPTO_num_locks ( ); i++ ) {
		
		MUTEX_CLEANUP ( StaticSslLocking::poMutexes [ i ] );

	}

	delete [] StaticSslLocking::poMutexes;
	StaticSslLocking::poMutexes = NULL;

}

#endif // COMPILE_WITH_SSL
