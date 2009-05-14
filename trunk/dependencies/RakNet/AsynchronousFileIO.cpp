/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


// No longer used as I no longer support IO Completion ports
/*

#ifdef __USE_IO_COMPLETION_PORTS

#include "AsynchronousFileIO.h"
#include "ClientContextStruct.h"
#include <process.h>
#include "ExtendedOverlappedPool.h"
#include <stdio.h>
#include "RakAssert.h"

// All these are used for the Read callback.  For general Asynch file IO you would change these
#include "RakNetTypes.h"

class RakPeer;

#ifdef _WIN32
extern void __stdcall ProcessNetworkPacket( unsigned int binaryAddress, unsigned short port, const char *data, int length, RakPeer *rakPeer );
#else
extern void ProcessNetworkPacket( unsigned int binaryAddress, unsigned short port, const char *data, int length, RakPeer *rakPeer );
#endif

AsynchronousFileIO AsynchronousFileIO::I;

AsynchronousFileIO::AsynchronousFileIO()
{
	userCount = 0;
	threadCount = 0;
	completionPort = NULL;

	// Determine how many processors are on the system.
	GetSystemInfo( &systemInfo );
}

void AsynchronousFileIO::IncreaseUserCount()
{
	userCountMutex.Lock();
	++userCount;

	if ( userCount == 1 )
	{

		// Create the completion port that will be used by all the worker
		// threads.
		completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, systemInfo.dwNumberOfProcessors * 2 );

		if ( completionPort == NULL )
		{
			userCount = 0;
			userCountMutex.Unlock();
			return ;
		}

		UINT nThreadID;
		HANDLE workerHandle;

		// Create worker threads

		// One worker thread per processor

		for ( DWORD i = 0; i < systemInfo.dwNumberOfProcessors * 2; i++ )
			// In debug just make one worker thread so it's easier to trace
			//for ( i = 0; i < systemInfo.dwNumberOfProcessors * 1; i++ )
		{
			workerHandle = ( HANDLE ) _beginthreadex( NULL,      // Security
				0,      // Stack size - use default
				ThreadPoolFunc,     // Thread fn entry point
				( void* ) completionPort,     // Param for thread
				0,      // Init flag
				&nThreadID );   // Thread address


			// Feel free to comment this out for regular thread priority
			SetThreadPriority( workerHandle, THREAD_PRIORITY_HIGHEST );

			CloseHandle( workerHandle );
		}


		// Wait for the threads to start
		while ( threadCount < systemInfo.dwNumberOfProcessors * 2 )
			Sleep( 0 );
	}

	userCountMutex.Unlock();
}

void AsynchronousFileIO::DecreaseUserCount()
{
	userCountMutex.Lock();

	assert( userCount > 0 );

	if ( userCount == 0 )
		return ;

	userCount--;

	if ( userCount == 0 )
		Shutdown();

	userCountMutex.Unlock();
}

void AsynchronousFileIO::Shutdown( void )
{
	killThreads = true;

	if ( completionPort != NULL )
		for ( DWORD i = 0; i < systemInfo.dwNumberOfProcessors * 2; i++ )
			PostQueuedCompletionStatus( completionPort, 0, 0 , 0 );

	// Kill worker threads
	while ( threadCount > 0 )
		Sleep( 0 );

	if ( completionPort != NULL )
		CloseHandle( completionPort );
}

int AsynchronousFileIO::GetUserCount( void )
{
	return userCount;
}

AsynchronousFileIO::~AsynchronousFileIO()
{
	if ( threadCount > 0 )
		Shutdown();
}

bool AsynchronousFileIO::AssociateSocketWithCompletionPort( SOCKET socket, DWORD dwCompletionKey )
{
	HANDLE h = CreateIoCompletionPort( ( HANDLE ) socket, completionPort, dwCompletionKey, 0 );
	return h == completionPort;
}

BOOL ReadAsynch( HANDLE handle, ExtendedOverlappedStruct *extended )
{
	BOOL success;
	extended->read = true;
	success = ReadFile( handle, extended->data, extended->length, 0, ( LPOVERLAPPED ) extended );

	if ( !success )
	{
		DWORD dwErrCode = GetLastError();

		if ( dwErrCode != ERROR_IO_PENDING )
		{
#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG)
			LPVOID messageBuffer;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, dwErrCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
				( LPTSTR ) & messageBuffer, 0, NULL );
			// something has gone wrong here...
			RAKNET_DEBUG_PRINTF( "ReadFile failed:Error code - %d\n%s", dwErrCode, messageBuffer );
			//Free the buffer.
			LocalFree( messageBuffer );
#endif

			return FALSE;
		}
	}

	return TRUE;
}

void WriteAsynch( HANDLE handle, ExtendedOverlappedStruct *extended )
{
	//RAKNET_DEBUG_PRINTF("Beginning asynch write of %i bytes.\n",extended->length);
	//for (int i=0; i < extended->length && i < 10; i++)
	// RAKNET_DEBUG_PRINTF("%i ", extended->data[i]);
	//RAKNET_DEBUG_PRINTF("\n\n");
	BOOL success;
	extended->read = false;
	success = WriteFile( handle, extended->data, extended->length, 0, ( LPOVERLAPPED ) extended );

	if ( !success )
	{
		DWORD dwErrCode = GetLastError();

		if ( dwErrCode != ERROR_IO_PENDING )
		{
#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG)
			LPVOID messageBuffer;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, dwErrCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
				( LPTSTR ) & messageBuffer, 0, NULL );
			// something has gone wrong here...
			RAKNET_DEBUG_PRINTF( "WriteFile failed:Error code - %d\n%s", dwErrCode, messageBuffer );

			//Free the buffer.
			LocalFree( messageBuffer );
#endif

		}
	}
}

unsigned __stdcall ThreadPoolFunc( LPVOID arguments )
{
	DWORD dwIoSize;
	ClientContextStruct* lpClientContext;
	ExtendedOverlappedStruct* lpOverlapped;
	LPOVERLAPPED temp;
	BOOL bError;

	HANDLE *completionPort = ( HANDLE * ) arguments;
	AsynchronousFileIO::Instance()->threadCount++;

	while ( 1 )
	{
		// Get a completed IO request.
		BOOL returnValue = GetQueuedCompletionStatus(
			completionPort,
			&dwIoSize,
			( LPDWORD ) & lpClientContext,
			&temp, INFINITE );

		lpOverlapped = ( ExtendedOverlappedStruct* ) temp;

		DWORD dwIOError = GetLastError();

		if ( lpOverlapped == 0 )
			break; // Cancelled thread

		if ( !returnValue && dwIOError != WAIT_TIMEOUT )
		{
			if ( dwIOError != ERROR_OPERATION_ABORTED )
			{
				// Print all but this very common error message
#if defined(_WIN32) && !defined(_XBOX) && defined(_DEBUG)
				LPVOID messageBuffer;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, dwIOError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),  // Default language
					( LPTSTR ) & messageBuffer, 0, NULL );
				// something has gone wrong here...
				RAKNET_DEBUG_PRINTF( "GetQueuedCompletionStatus failed:Error code - %d\n%s", dwIOError, messageBuffer );

				//Free the buffer.
				LocalFree( messageBuffer );
#endif

			}

HANDLE_ERROR:
			// Some kind of error. Erase the data for this call
			bError = true;

			// This socket is no longer used

			if ( lpOverlapped )
				RakNet::OP_DELETE(lpOverlapped, __FILE__, __LINE__);

			if ( lpClientContext )
				RakNet::OP_DELETE(lpClientContext, __FILE__, __LINE__);

			// If we are killing the threads, then we keep posting fake completion statuses until we get a fake one through the queue (i.e. lpOverlapped==0 as above)
			// This way we delete all the data from the real calls before exiting the thread
			if ( AsynchronousFileIO::Instance()->killThreads )
			{
				PostQueuedCompletionStatus( completionPort, 0, 0, 0 );
			}

		}

		else
			bError = false;

		if ( !bError )
		{
			if ( returnValue && NULL != lpOverlapped && NULL != lpClientContext )
			{
				if ( lpOverlapped->read == true )
				{
					assert( dwIoSize > 0 );

					ProcessNetworkPacket( lpOverlapped->binaryAddress, lpOverlapped->port, lpOverlapped->data, dwIoSize, lpOverlapped->rakPeer );

					// Issue a new read so we always have one outstanding read per socket
					// Finished a read.  Reuse the overlapped pointer
					bError = ReadAsynch( lpClientContext->handle, lpOverlapped );

					if ( !bError )
						goto HANDLE_ERROR; // Windows is super unreliable!
				}

				else
				{
					// AsynchronousFileIO::Instance()->Write(lpClientContext);
					// Finished a write
					ExtendedOverlappedPool::Instance()->ReleasePointer( lpOverlapped );
				}
			}

			else
				assert( 0 );
		}
	}

	AsynchronousFileIO::Instance()->threadCount--;
	return 0;
}

#endif
*/
