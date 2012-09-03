
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

#ifndef THREAD_ABSTRACTION_LAYER
#define THREAD_ABSTRACTION_LAYER

#define VERBOSE_LOCKING 0


/** 
 * defines a set of common thread macros between windows and posix
 */

#include <pthread.h>
#define MUTEX_TYPE          pthread_mutex_t
#define MUTEX_SETUP(x)      pthread_mutex_init ( &(x), NULL )
#define MUTEX_CLEANUP(x)    pthread_mutex_destroy ( &(x) )

#if VERBOSE_LOCKING

#define MUTEX_LOCK(x)       fprintf ( stderr, "[%d] trying to lock mutex %x %s:%d\n", pthread_self ( ), &x, __FILE__, __LINE__ ); pthread_mutex_lock ( &(x) ); fprintf ( stderr, "[%d] successfully locked mutex %x %s:%d\n", pthread_self ( ), &x, __FILE__, __LINE__ );
#define MUTEX_UNLOCK(x)     fprintf ( stderr, "[%d] unlocking mutex %x %s:%d\n", pthread_self ( ), &x,__FILE__, __LINE__ ); pthread_mutex_unlock ( &(x) )

#else

#define MUTEX_LOCK(x)       pthread_mutex_lock ( &(x) )
#define MUTEX_UNLOCK(x)     pthread_mutex_unlock ( &(x) )

#endif // VERBOSE_LOCKING

#define THREAD_ID           pthread_self ( )
#define THREAD_CC           __cdecl
#define THREAD_TYPE         pthread_t
#define THREAD_create(id,function,parameter) pthread_create ( &(id), NULL, (function), ((void *)(parameter)) )


#endif // THREAD_ABSTRACTION_LAYER
