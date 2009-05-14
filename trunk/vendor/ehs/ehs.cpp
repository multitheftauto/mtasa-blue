
/*

EHS is a library for embedding HTTP(S) support into a C++ application
Copyright (C) 2004 Zachary J. Hansen

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License version 2.1 as published by the Free Software Foundation; 

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

This can be found in the 'COPYING' file.

*/



#include "ehs.h"
#include <fstream>

int EHSServer::CreateFdSet ( )
{

	// don't lock mutex, as it is only called from within a locked section

	FD_ZERO( &m_oReadFds );

	// add the accepting FD	
	FD_SET( m_poNetworkAbstraction->GetFd ( ), &m_oReadFds );

	int nHighestFd = m_poNetworkAbstraction->GetFd ( );

	for ( EHSConnectionList::iterator oCurrentConnection = m_oEHSConnectionList.begin ( );
		  !( oCurrentConnection == m_oEHSConnectionList.end ( ) );
		  oCurrentConnection++ ) {
		
		/// skip this one if it's already been used
		if ( (*oCurrentConnection)->StillReading ( ) ) {

			int nCurrentFd = 
				( * oCurrentConnection )->GetNetworkAbstraction ( )->GetFd ( );
			
			EHS_TRACE ( "Adding %d to FD SET\n", nCurrentFd );

			FD_SET ( nCurrentFd, &m_oReadFds );

			// store the highest FD in the set to return it
			if ( nCurrentFd > nHighestFd ) {

				nHighestFd = nCurrentFd;

			}

		} else {

			EHS_TRACE ( "FD %d isn't reading anymore\n", 
						( * oCurrentConnection )->GetNetworkAbstraction ( )->GetFd ( ) );

		}
	}

	return nHighestFd;

}



int EHSServer::ClearIdleConnections ( )
{

	// don't lock mutex, as it is only called rom within locked sections

	int nIdleConnections = 0;

	for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		  i != m_oEHSConnectionList.end ( );
		  i++ ) {

		MUTEX_LOCK ( (*i)->m_oMutex );

		// if it's been more than N seconds since a response has been
		//   sent and there are no pending requests
		if ( (*i)->StillReading ( ) &&
			 time ( NULL ) - (*i)->LastActivity ( ) > m_nIdleTimeout &&
			 (*i)->RequestsPending ( )
			  ) {

			EHS_TRACE ( "Done reading because of idle timeout\n" );
    
			(*i)->DoneReading ( false );

			nIdleConnections++;

		}
		
		MUTEX_UNLOCK ( (*i)->m_oMutex );

	}


	if ( nIdleConnections > 0 ) {
		EHS_TRACE ( "Cleared %d connections\n", nIdleConnections );
	}


	RemoveFinishedConnections ( );

	return nIdleConnections;

}


int EHSServer::RemoveFinishedConnections ( ) {

	// don't lock mutex, as it is only called rom within locked sections

	for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		  i != m_oEHSConnectionList.end ( );
		  /* no third part */ ) {
		
		if ( (*i)->CheckDone ( ) ) {

			RemoveEHSConnection ( *i );
			i = m_oEHSConnectionList.begin ( );

		} else {

			i++;

		}

	}

	return 0;

}


EHSConnection::EHSConnection ( NetworkAbstraction * ipoNetworkAbstraction,
							   EHSServer * ipoEHSServer ) :
	m_nDoneReading ( 0 ),
	m_nDisconnected ( 0 ),
	m_nRequests ( 0 ),
	m_nResponses ( 0 ),
	m_poNetworkAbstraction ( ipoNetworkAbstraction ),
	m_poCurrentHttpRequest ( NULL ),
	m_poEHSServer ( ipoEHSServer )
{

	UpdateLastActivity ( );

	// initialize mutex for this object
	MUTEX_SETUP ( m_oMutex );

	// get the address and port of the new connection
	m_sAddress = ipoNetworkAbstraction->GetAddress ( );
	m_nPort = ipoNetworkAbstraction->GetPort ( );
	
	// make sure the buffer is clear
	m_sBuffer = "";


#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Allocated: EHSConnection\n" );
#endif
}


EHSConnection::~EHSConnection ( )
{
#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Deallocated: EHSConnection\n" );
#endif

	delete m_poNetworkAbstraction;

}


NetworkAbstraction * EHSConnection::GetNetworkAbstraction ( )
{

	return m_poNetworkAbstraction;

}


// adds data to the current buffer for this connection
EHSConnection::AddBufferResult
EHSConnection::AddBuffer ( char * ipsData, ///< new data to be added
						   int inSize ///< size of new data
	)
{

	MUTEX_LOCK ( m_oMutex );

	// make sure we actually got some data
	if ( inSize <= 0 ) {
		MUTEX_UNLOCK ( m_oMutex );
		return ADDBUFFER_INVALID;
	}

	// this is binary safe -- only the single argument char* constructor looks for NULL
	m_sBuffer += std::string ( ipsData, inSize );

	// make sure the buffer hasn't grown too big
	if ( m_sBuffer.length ( ) > MAX_BUFFER_SIZE_BEFORE_BOOT ) {
		MUTEX_UNLOCK ( m_oMutex );
		return ADDBUFFER_TOOBIG;
	}

	// need to run through our buffer until we don't get a full result out
	do {
//		fprintf ( stderr, "##### Top of do loop\n" );
		// if we need to make a new request object, do that now
		if ( m_poCurrentHttpRequest == NULL ||
			 m_poCurrentHttpRequest->nCurrentHttpParseState == HttpRequest::HTTPPARSESTATE_COMPLETEREQUEST ) {
//			fprintf ( stderr, "##### Making new http request object\n" );
			// if we have one already, toss it on the list
			if ( m_poCurrentHttpRequest != NULL ) {

				m_oHttpRequestList.push_back ( m_poCurrentHttpRequest );
				m_poEHSServer->IncrementRequestsPending ( );

				// wake up everyone
				pthread_cond_broadcast ( & m_poEHSServer->m_oDoneAccepting );


				if ( m_poEHSServer->m_nServerRunningStatus == EHSServer::SERVERRUNNING_ONETHREADPERREQUEST ) {


					static int nThreadsCreated = 0;

					// create a thread if necessary
					pthread_t oThread;

					MUTEX_UNLOCK ( m_oMutex );
					pthread_create ( & oThread,
									 NULL,
									 EHSServer::PthreadHandleData_ThreadedStub,
									 (void *) m_poEHSServer );
					pthread_detach ( oThread );
					MUTEX_LOCK ( m_oMutex );


//					fprintf ( stderr, "##### Created new thread %d - %d\n", ++nThreadsCreated, oThread );
				}

			} else {
				//fprintf ( stderr, "not moving old request\n" );
			}


			
			
			// create the initial request
			m_poCurrentHttpRequest = new HttpRequest ( ++m_nRequests, this );
			m_poCurrentHttpRequest->nSecure = m_poNetworkAbstraction->IsSecure ( );

		}

		// parse through the current data
//		fprintf ( stderr, "##### Calling ParseData()\n" );
		m_poCurrentHttpRequest->ParseData ( m_sBuffer );
//		fprintf ( stderr, "##### Done calling ParseData()\n" );

	} while ( m_poCurrentHttpRequest->nCurrentHttpParseState ==
			  HttpRequest::HTTPPARSESTATE_COMPLETEREQUEST );
	

	AddBufferResult nReturnValue;

	// return either invalid request or ok
	if ( m_poCurrentHttpRequest->nCurrentHttpParseState == HttpRequest::HTTPPARSESTATE_INVALIDREQUEST ) {

		nReturnValue = ADDBUFFER_INVALIDREQUEST;

	} else {

		nReturnValue = ADDBUFFER_OK;

	}

	MUTEX_UNLOCK ( m_oMutex );

	return nReturnValue;

}

/// call when no more reads will be performed on this object.  inDisconnected is true when client has disconnected
void EHSConnection::DoneReading ( int inDisconnected )
{
    // REMOVE LOCKS (eAi), already locked when this function is called (at least sometimes it is)
    // May need real fix
//	MUTEX_LOCK ( m_oMutex );

	m_nDoneReading = 1;
	m_nDisconnected = inDisconnected;

	//MUTEX_UNLOCK ( m_oMutex );

}


HttpRequest * EHSConnection::GetNextRequest ( )
{

	HttpRequest * poHttpRequest = NULL;

	MUTEX_LOCK ( m_oMutex );

	if ( m_oHttpRequestList.empty ( ) ) {

		poHttpRequest = NULL;

	} else {

		poHttpRequest = m_oHttpRequestList.front ( );
		
		m_oHttpRequestList.pop_front ( );
		
	}

	MUTEX_UNLOCK ( m_oMutex );

	return poHttpRequest;

}


int EHSConnection::CheckDone ( )
{

	// if we're not still reading, we may want to drop this connection
	if ( !StillReading ( ) ) {
		
		// if we're done with all our responses (-1 because the next (unused) request is already created)
		if ( m_nRequests - 1 <= m_nResponses ) {
			
			// if we haven't disconnected, do that now
			if ( !m_nDisconnected) {
			
				m_poNetworkAbstraction->Close ( );

			}

			return 1;

		}

	}	

	return 0;

}


////////////////////////////////////////////////////////////////////
// EHS SERVER
////////////////////////////////////////////////////////////////////

EHSServer::EHSServer ( EHS * ipoTopLevelEHS ///< pointer to top-level EHS for request routing
	) :
	m_nServerRunningStatus ( SERVERRUNNING_NOTRUNNING ),
	m_poTopLevelEHS ( ipoTopLevelEHS ),
	m_nRequestsPending ( 0 ),
	m_nIdleTimeout ( 15 )
{
	// you HAVE to specify a top-level EHS object
	//  compare against NULL for 64-bit systems
	assert ( m_poTopLevelEHS != NULL );

	
	MUTEX_SETUP ( m_oMutex );
	pthread_cond_init ( & m_oDoneAccepting, NULL );

	m_nAccepting = 0;

	// grab out the parameters for less typing later on
	EHSServerParameters & roEHSServerParameters = 
		ipoTopLevelEHS->m_oEHSServerParameters;

	// whether to run with https support
	int nHttps = roEHSServerParameters [ "https" ];

#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Allocated: EHSServer\n" );
#endif		


	if ( nHttps ) {
		EHS_TRACE ( "EHSServer running in HTTPS mode\n");
	} else {
		EHS_TRACE ( "EHSServer running in plain-text mode (no HTTPS)\n");
	}

	

	// are we using secure sockets?
	if ( !nHttps ) {
		m_poNetworkAbstraction = new Socket ( );
	} else {

#ifdef COMPILE_WITH_SSL		


		EHS_TRACE ( "Trying to create secure socket with certificate='%s' and passphrase='%s'\n",
					(const char*)roEHSServerParameters [ "certificate" ],
					(const char*)roEHSServerParameters [ "passphrase" ] );

		
		SecureSocket * poSecureSocket = 
			new SecureSocket ( roEHSServerParameters [ "certificate" ],
							   roEHSServerParameters [ "passphrase" ] );
		
		// HIGHLY EXPERIMENTAL
		// scary shit
		EHS_TRACE ( "Thinking about loading callback - '%s'\n", 
					roEHSServerParameters [ "passphrasecallback" ].
					GetCharString ( ) );
		

		if ( roEHSServerParameters [ "passphrasecallback" ] != "" &&
			 dlsym ( RTLD_DEFAULT, 
					 roEHSServerParameters [ "passphrasecallback" ] ) == 
			 NULL ) {

			EHS_TRACE ( "Couldn't load symbol for '%s' -- make sure you extern \"C\" the function\n",
						roEHSServerParameters [ "passphrasecallback" ].
						GetCharString ( ) );
		}



		EHS_TRACE ( "done thinking about loading callback\n" );



		
		poSecureSocket->SetPassphraseCallback (  (int ( * ) ( char *, int, int, void * ))
												 dlsym ( RTLD_DEFAULT,
														 roEHSServerParameters [ "passphrasecallback" ] ) );
		// END EXPERIMENTAL
		
		m_poNetworkAbstraction = poSecureSocket;
#else // COMPILE_WITH_SSL
		fprintf ( stderr, "EHS not compiled with SSL support.  Cannot create HTTPS server.  Aborting\n" );
		exit ( 1 );
#endif // COMPILE_WITH_SSL



	}
	

	// initialize the socket
	assert ( m_poNetworkAbstraction != NULL );
	int nResult = m_poNetworkAbstraction->Init ( roEHSServerParameters [ "bindip" ], roEHSServerParameters [ "port" ] ); // initialize socket stuff

	

	
	if ( nResult != NetworkAbstraction::INITSOCKET_SUCCESS ) {

		EHS_TRACE ( "Error: Failed to initialize sockets\n" );

		return;
	}


	if ( roEHSServerParameters [ "mode" ] == "threadpool" ) {

		// need to set this here because the thread will check this to make
		//   sure it's supposed to keep running
		m_nServerRunningStatus = SERVERRUNNING_THREADPOOL;

		// create a pthread 
		int nResult = -1;

		EHS_TRACE ( "Starting %d threads\n", roEHSServerParameters [ "threadcount" ].GetInt ( ) );


		int nThreadsToStart = roEHSServerParameters [ "threadcount" ].GetInt ( );
		if ( nThreadsToStart == 0 ) {
			nThreadsToStart = 1;
		}

		for ( int i = 0; i < nThreadsToStart; i++ ) {

			EHS_TRACE ( "creating thread with %x, NULL, %x, %x\n",
					  &m_nAcceptThreadId,
					  EHSServer::PthreadHandleData_ThreadedStub,
					  (void *) this );

			// create new thread and detach so we don't have to join on it
			nResult = 
				pthread_create ( &m_nAcceptThreadId,
								 NULL,
								 EHSServer::PthreadHandleData_ThreadedStub,
								 (void *) this );
			pthread_detach ( m_nAcceptThreadId );

		}

		if ( nResult != 0 ) {
			m_nServerRunningStatus = SERVERRUNNING_NOTRUNNING;
		}


	} else if ( roEHSServerParameters [ "mode" ] == "onethreadperrequest" ) {

		m_nServerRunningStatus = SERVERRUNNING_ONETHREADPERREQUEST;

		// spawn off one thread just to deal with basic stuff
		nResult = pthread_create ( &m_nAcceptThreadId,
								   NULL,
								   EHSServer::PthreadHandleData_ThreadedStub,
								   (void *) this );
		pthread_detach ( m_nAcceptThreadId );
	
		// check to make sure the thread was created properly
		if ( nResult != 0 ) {
			m_nServerRunningStatus = SERVERRUNNING_NOTRUNNING;
		}

	} else if ( roEHSServerParameters [ "mode" ] == "singlethreaded" ) {

		// we're single threaded
		m_nServerRunningStatus = SERVERRUNNING_SINGLETHREADED;

	} else {

		EHS_TRACE ( "INVALID 'mode' SPECIFIED.\ntMust be 'singlethreaded', 'threadpool', or 'onethreadperrequest'\n" );

		assert ( 0 );
	}


	if ( m_nServerRunningStatus == SERVERRUNNING_THREADPOOL ) {
		EHS_TRACE ( "Info: EHS Server running in dedicated thread mode with '%s' threads\n",
				  roEHSServerParameters [ "threadcount" ] == "" ? 
				  roEHSServerParameters [ "threadcount" ].GetCharString ( ) : "1" );
	} else if ( m_nServerRunningStatus == SERVERRUNNING_ONETHREADPERREQUEST ) {
		EHS_TRACE ( "Info: EHS Server running with one thread per request\n" );
	} else if ( m_nServerRunningStatus == SERVERRUNNING_SINGLETHREADED ) {
		EHS_TRACE ( "Info: EHS Server running in non-dedicated thread mode\n" );
	} else {
		EHS_TRACE ( "Error: EHS Server not running.  Server initialization failed\n" );
	}

	return;


}


EHSServer::~EHSServer ( )
{

#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Deallocated: EHSServer\n" );
#endif		

}

HttpRequest * EHSServer::GetNextRequest ( )
{

	// don't lock because it's only called from within locked sections

	HttpRequest * poNextRequest = NULL;

	// pick a random connection if the list isn't empty
	if ( ! m_oEHSConnectionList.empty ( ) ) {
		
		// pick a random connection, so no one takes too much time
		int nWhich = (int) ( ( (double) m_oEHSConnectionList.size ( ) ) * rand ( ) / ( RAND_MAX + 1.0 ) );
		
		// go to that element
		EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		int nCounter = 0;
		
		for ( nCounter = 0; nCounter < nWhich; nCounter++ ) {
			i++;
		}
		
		// now get the next available request treating the list as circular
		
		EHSConnectionList::iterator iStartPoint = i;
		int nFirstTime = 1;
		while ( poNextRequest == NULL &&
				!( iStartPoint == i && nFirstTime == 0 ) ) {
			
			// check this one to see if it has anything
			poNextRequest = (*i)->GetNextRequest ( );

			
			i++;
			
			if ( i == m_oEHSConnectionList.end ( ) ) {
				i = m_oEHSConnectionList.begin ( );
			}
			
			nFirstTime = 0;
			
			// decrement the number of pending requests
			if ( poNextRequest != NULL ) {

				m_nRequestsPending--;

			}

		}

	}


	if ( poNextRequest == NULL ) {
//		EHS_TRACE ( "No request found\n" );
	} else {
		EHS_TRACE ( "Found request\n" );
	}


	return poNextRequest;

}

int EHSServer::RemoveEHSConnection ( EHSConnection * ipoEHSConnection )
{

	// don't lock as it's only called from within locked sections
	assert ( ipoEHSConnection != NULL );
	int nDeletedOneAlready = 0;


	EHS_TRACE ( "%d elements to look for somethign to delete\n",
				m_oEHSConnectionList.size ( ) );


	// go through the list and find all occurances of ipoEHSConnection
	for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		  i != m_oEHSConnectionList.end ( );
		  /* no third part */ ) {

		if ( *i == ipoEHSConnection ) {
			if ( nDeletedOneAlready ) {
				EHS_TRACE ( "FATAL ERROR: Deleting a second element in RemoveEHSConnection - EXITING\n" );
				exit ( 2 );
			}
			nDeletedOneAlready = 1;


			// close the FD
			EHSConnection * poEHSConnection = *i;

			NetworkAbstraction * poNetworkAbstraction = poEHSConnection->GetNetworkAbstraction ( );
			
			poNetworkAbstraction->Close ( );

			// start back over at the beginning of the list
			m_oEHSConnectionList.erase ( i );
			i = m_oEHSConnectionList.begin ( );

		} else {
			i++;
		}

	}

	return nDeletedOneAlready;

}



EHS::StartServerResult 
EHS::StartServer ( EHSServerParameters & iroEHSServerParameters )
{
	StartServerResult nResult = STARTSERVER_INVALID;

	m_oEHSServerParameters = iroEHSServerParameters;

	if ( poEHSServer != NULL ) {

		EHS_TRACE ( "Warning: Tried to start server that was already running\n" );


		nResult = STARTSERVER_ALREADYRUNNING;

	} else {

		// associate a EHSServer object to this EHS object
		poEHSServer = new EHSServer ( this  );

		if ( poEHSServer->m_nServerRunningStatus == EHSServer::SERVERRUNNING_NOTRUNNING ) {
	

			EHS_TRACE ( "Error: Failed to start server\n" );



			return STARTSERVER_FAILED;

		}
	}

	return STARTSERVER_SUCCESS;

}



// this is the function specified to pthread_create under UNIX
//   because you can't start a thread directly into a class method
void * EHSServer::PthreadHandleData_ThreadedStub ( void * ipParam ///< EHSServer object cast to a void pointer
	)
{

	EHSServer * poThis = (EHSServer *) ipParam;
		
	poThis->HandleData_Threaded ( );

	return NULL;

}



void EHS::StopServer ( )
{
	
	// make sure we're in a sane state
	assert ( ( poParent == NULL && poEHSServer != NULL ) ||
			 ( poParent != NULL && poEHSServer == NULL ) );
	
	if ( poParent ) { 
		poParent->StopServer ( );
	} else {
		poEHSServer->m_nServerRunningStatus = EHSServer::SERVERRUNNING_NOTRUNNING;
	}

}




void EHSServer::HandleData_Threaded ( )
{

#define HANDLEDATA_THREADEDSELECTTIMEOUTMILLISECONDS 1000


	pthread_t nMyThreadId = pthread_self ( );

	do {

        //fprintf ( stderr, "##### [%d] Thread HandleData\n", nMyThreadId );
		HandleData ( HANDLEDATA_THREADEDSELECTTIMEOUTMILLISECONDS,
					 nMyThreadId );


	} while ( m_nServerRunningStatus == SERVERRUNNING_THREADPOOL ||
			  pthread_equal(nMyThreadId,m_nAcceptThreadId) );

	//fprintf ( stderr, "##### [%d] Thread exiting\n", nMyThreadId );


}




void EHSServer::HandleData ( int inTimeoutMilliseconds, ///< milliseconds for timeout on select
							 pthread_t inThreadId ///< numeric ID for this thread to help debug
	)
{

	//fprintf ( stderr, "##### [%d] Trying to lock server mutex\n", inThreadId );

	MUTEX_LOCK ( m_oMutex );

	//fprintf ( stderr, "##### [%d] Got lock on server mutex\n", inThreadId );

	// determine if there are any jobs waiting if this thread should --
	//   if we're running one-thread-per-request and this is the accept thread
	//   we don't look for requests
	HttpRequest * poHttpRequest = NULL;
	if ( m_nServerRunningStatus != SERVERRUNNING_ONETHREADPERREQUEST ||
		 !pthread_equal(inThreadId,m_nAcceptThreadId) ) {

		poHttpRequest = GetNextRequest ( );

	}

	// if we got a request to handle
	if ( poHttpRequest != NULL ) {

		//fprintf ( stderr, "##### [%d] Got a request to handle\n", inThreadId );

		// handle the request and post it back to the connection object
		MUTEX_UNLOCK ( m_oMutex );

		// route the request
		HttpResponse * poHttpResponse = 
			m_poTopLevelEHS->RouteRequest ( poHttpRequest );

		// add the response to the appropriate connection's response list
		poHttpResponse->m_poEHSConnection->AddResponse ( poHttpResponse );

		delete poHttpRequest;

	} 
	// otherwise, no requests are pending
	else {

		// if something is already accepting, sleep
		if ( m_nAccepting ) {
			
			// wait until something happens
			// it's ok to not recheck our condition here, as we'll come back in the same way and recheck then
			//fprintf ( stderr, "##### [%d] Sleeping because no requests and someone else is accepting\n", inThreadId );

			pthread_cond_wait ( & m_oDoneAccepting,
								& m_oMutex );			

			MUTEX_UNLOCK ( m_oMutex );

		} 
		// if no one is accepting, we accept
		else {

			m_nAcceptedNewConnection = 0;
	
            
			
			//fprintf ( stderr, "Accepting\n" );

			// we're now accepting
			m_nAccepting = 1;

			MUTEX_UNLOCK ( m_oMutex );

			// set up the timeout and normalize
			timeval tv = { 0, inTimeoutMilliseconds * 1000 }; 
			tv.tv_sec = tv.tv_usec / 1000000;
			tv.tv_usec %= 1000000;

			// create the FD set for select
			int nHighestFd = CreateFdSet ( );			
			
			// call select
			//fprintf ( stderr, "##### [%d] Calling select\n", inThreadId );
			int nSocketCount = select ( nHighestFd + 1,
										&m_oReadFds,
										NULL,
										NULL,
										&tv );
			//fprintf ( stderr, "##### [%d] Done calling select\n", inThreadId );

			// handle select error
#ifdef _WIN32
			if ( nSocketCount == SOCKET_ERROR )
#else // NOT _WIN32
			if ( nSocketCount == -1 )
#endif // _WIN32
			{

				EHS_TRACE ( "[%d] Critical Error: select() failed.  Aborting\n", inThreadId );

				exit ( 0 );
			}
			
			
			
			// if no sockets have data to read, clear accepting flag and return
			if ( nSocketCount > 0 ) {

				// Check the accept socket for a new connection
				CheckAcceptSocket ( );

				// check client sockets for data
				CheckClientSockets ( );

			}

			MUTEX_LOCK ( m_oMutex );
			ClearIdleConnections ( );
			m_nAccepting = 0;
			MUTEX_UNLOCK ( m_oMutex );


		} // END ACCEPTING
		
	} // END NO REQUESTS PENDING

}

void EHSServer::CheckAcceptSocket ( )
{

	// see if we got data on this socket
	if ( FD_ISSET ( m_poNetworkAbstraction->GetFd ( ), &m_oReadFds ) ) {
		

        //printf ( "Accept new connection\n");
		// THIS SHOULD BE NON-BLOCKING OR ELSE A HANG CAN OCCUR IF THEY DISCONNECT BETWEEN WHEN
		//   POLL SEES THE CONNECTION AND WHEN WE ACTUALLY CALL ACCEPT
		NetworkAbstraction * poNewClient = 
			m_poNetworkAbstraction->Accept ( );
		
        

		// not sure what would cause this
		if ( poNewClient == NULL ) {
			return;
		}
		
		// create a new EHSConnection object and initialize it
		EHSConnection * poEHSConnection = 
			new EHSConnection ( poNewClient, this );
		
		MUTEX_LOCK ( m_oMutex );
		m_oEHSConnectionList.push_back ( poEHSConnection );
		m_nAcceptedNewConnection = 1;
		MUTEX_UNLOCK ( m_oMutex );

	} // end FD_ISSET ( )

}

void EHSServer::CheckClientSockets ( )
{

	// go through all the sockets from which we're still reading
	for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		  i != m_oEHSConnectionList.end ( );
		  i++ ) {

		if ( FD_ISSET ( (*i)->GetNetworkAbstraction ( )->GetFd ( ), &m_oReadFds ) ) {


			EHS_TRACE ( "$$$$$ Got data on client connection\n" );


			// prepare a buffer for the read
			static const int BYTES_TO_READ_AT_A_TIME = 10240;
			char psReadBuffer [ BYTES_TO_READ_AT_A_TIME + 1 ];
			memset ( psReadBuffer, 0, BYTES_TO_READ_AT_A_TIME + 1 );

			// do the actual read
			int nBytesReceived =
				(*i)->GetNetworkAbstraction ( )->Read ( psReadBuffer,
														BYTES_TO_READ_AT_A_TIME );

			// if we received a disconnect
			if ( nBytesReceived <= 0 ) {

				// we're done reading and we received a disconnect
				(*i)->DoneReading ( true );

			}
			// otherwise we got data
			else {

				// take the data we got and append to the connection's buffer
				EHSConnection::AddBufferResult nAddBufferResult = 
					(*i)->AddBuffer ( psReadBuffer, nBytesReceived );

				// if add buffer failed, don't read from this connection anymore
				if ( nAddBufferResult == EHSConnection::ADDBUFFER_INVALIDREQUEST ||
					 nAddBufferResult == EHSConnection::ADDBUFFER_TOOBIG ) {

					// done reading but did not receieve disconnect
					EHS_TRACE ( "Done reading because we got a bad request\n" );
					(*i)->DoneReading ( false );

				} // end error with AddBuffer

			} // end nBytesReceived

		} // FD_ISSET
		
	} // for loop through connections

} 


const char * ResponsePhrase [] = { "INVALID", "OK", "Moved Permanently", "Found", "Unauthorized", "Forbidden", "Not Found", "Internal Server Error" };

const char * GetResponsePhrase ( int inResponseCode ///< HTTP response code to get text version of
)
{
	const char * psReturn = NULL;
	
	switch ( inResponseCode ) {
		
	case HTTPRESPONSECODE_200_OK:
		psReturn = ResponsePhrase [ 1 ];
		break;
		
	case HTTPRESPONSECODE_301_MOVEDPERMANENTLY:
		psReturn = ResponsePhrase [ 2 ];
		break;
		
	case HTTPRESPONSECODE_302_FOUND:
		psReturn = ResponsePhrase [ 3 ];
		break;

	case HTTPRESPONSECODE_401_UNAUTHORIZED:
		psReturn = ResponsePhrase [ 4 ];
		break;

	case HTTPRESPONSECODE_403_FORBIDDEN:
		psReturn = ResponsePhrase [ 5 ];
		break;

	case HTTPRESPONSECODE_404_NOTFOUND:
		psReturn = ResponsePhrase [ 6 ];
		break;

	case HTTPRESPONSECODE_500_INTERNALSERVERERROR:
		psReturn = ResponsePhrase [ 7 ];
		break;
		
	default:
		assert ( 0 );
		psReturn = ResponsePhrase [ 0 ];
		break;

	}
	
	return psReturn;

}



void EHSConnection::AddResponse ( HttpResponse * ipoHttpResponse )
{

	MUTEX_LOCK ( m_oMutex );

	// push the object on to the list
	m_oHttpResponseMap [ ipoHttpResponse->m_nResponseId ] = ipoHttpResponse;

	// go through the list until we can't find the next response to send
	int nFoundNextResponse = 0;
	do {

		nFoundNextResponse = 0;

		if ( m_oHttpResponseMap.find ( m_nResponses + 1 ) != m_oHttpResponseMap.end ( ) ) {

			nFoundNextResponse = 1;
			
			HttpResponseMap::iterator i = m_oHttpResponseMap.find ( m_nResponses + 1 );
			
			SendHttpResponse ( i->second );

			delete i->second;

			m_oHttpResponseMap.erase ( i );

			m_nResponses++;
			
			// set last activity to the current time for idle purposes
			UpdateLastActivity ( );
			
			// if we're done with this connection, get rid of it
			if ( CheckDone ( ) ) {
				EHS_TRACE( "add response found something to delete\n" );
				
				// careful with mutexes around here.. Don't want to hold both
				MUTEX_UNLOCK ( m_oMutex );
				MUTEX_LOCK ( m_poEHSServer->m_oMutex );
				m_poEHSServer->RemoveEHSConnection ( this );
				MUTEX_UNLOCK ( m_poEHSServer->m_oMutex );
				MUTEX_LOCK ( m_oMutex );

			}


			EHS_TRACE ( "Sending response %d to %x\n", m_nResponses, this );


		}

	} while ( nFoundNextResponse == 1 );

	MUTEX_UNLOCK ( m_oMutex );

}

void EHSConnection::SendHttpResponse ( HttpResponse * ipoHttpResponse )
{

	// only send it if the client isn't disconnected
	if ( Disconnected ( ) ) {

		return;

	}

	std::string sOutput;

	char psSmallBuffer [ 20 ];

	sOutput = "HTTP/1.1 ";

	// add in the response code
	sprintf ( psSmallBuffer, "%d", ipoHttpResponse->m_nResponseCode );
	sOutput += psSmallBuffer;

	sOutput += " ";
	sOutput +=  GetResponsePhrase ( ipoHttpResponse->m_nResponseCode );
	sOutput += "\r\n";


	// now go through all the entries in the responseheaders string map
	for ( StringMap::iterator oCurrentHeader = ipoHttpResponse->oResponseHeaders.begin ( );
		  oCurrentHeader != ipoHttpResponse->oResponseHeaders.end ( );
		  oCurrentHeader++ ) {

		sOutput += (*oCurrentHeader).first;
		sOutput += ": ";
		sOutput += (*oCurrentHeader).second;
		sOutput += "\r\n";
		
	}

	// now push out all the cookies
	for ( StringList::iterator i = ipoHttpResponse->oCookieList.begin ( );
		  i != ipoHttpResponse->oCookieList.end ( );
		  i++ ) {
		sOutput += "Set-Cookie: ";
		sOutput += *i;
		sOutput += "\r\n";
	}

	// extra line break signalling end of headers
	sOutput += "\r\n";

	const char * psBuffer = sOutput.c_str ( );
#ifdef _WIN32
	m_poNetworkAbstraction->Send ( (const char*) psBuffer, sOutput.length() );
#else
	m_poNetworkAbstraction->Send ( (void *) psBuffer, sOutput.length() );
#endif


	// now send the body
	m_poNetworkAbstraction->Send ( ipoHttpResponse->GetBody ( ), 
								   atoi ( ipoHttpResponse->oResponseHeaders [ "content-length" ].c_str ( ) ) );

}


void EHSServer::EndServerThread ( char * ipsReason ///< reason for ending the thread
	)
{

	MUTEX_LOCK ( m_oMutex );

	m_sShutdownReason = ipsReason;
	m_nServerRunningStatus = SERVERRUNNING_NOTRUNNING;

	MUTEX_UNLOCK ( m_oMutex );

}

EHS::EHS ( EHS * ipoParent, ///< parent EHS object for routing purposes
		   std::string isRegisteredAs ///< path string for routing purposes
	) :
	poParent ( NULL ),
	poEHSServer ( NULL ),
	m_poSourceEHS ( NULL )
{

	if ( ipoParent != NULL ) {
		SetParent ( ipoParent, isRegisteredAs );
	}

#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Allocated: EHS\n" );
#endif		

}

EHS::~EHS ( )
{
	// needs to clean up all its registered interfaces
	if ( poParent ) {
		poParent->UnregisterEHS ( sRegisteredAs.c_str ( ) );
	}

	delete poEHSServer;

#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Deallocated: EHS\n" );
#endif		

}

void EHS::SetParent ( EHS * ipoParent, ///< this is the new parent
					  std::string isRegisteredAs ///< path for routing
	)
{
	poParent = ipoParent;
	sRegisteredAs = isRegisteredAs;
}

EHS::RegisterEHSResult
EHS::RegisterEHS ( EHS * ipoEHS, ///< new sibling
				   const char * ipsRegisterPath ///< path for routing
	)
{

	ipoEHS->SetParent ( this, ipsRegisterPath );

	if ( oEHSMap [ ipsRegisterPath ] ) {
		return REGISTEREHSINTERFACE_ALREADYEXISTS;
	}

	oEHSMap [ ipsRegisterPath ] = ipoEHS;

	return REGISTEREHSINTERFACE_SUCCESS;

}


EHS::UnregisterEHSResult
EHS::UnregisterEHS ( const char * ipsRegisterPath ///< remove object at this path
	)
{

	if ( !oEHSMap [ ipsRegisterPath ] ) {
		return UNREGISTEREHSINTERFACE_NOTREGISTERED;
	}

	oEHSMap.erase ( ipsRegisterPath );

	return UNREGISTEREHSINTERFACE_SUCCESS;

}


void EHS::HandleData ( int inTimeoutMilliseconds ///< milliseconds for select timeout
	)
{
	// make sure we're in a sane state
	assert ( ( poParent == NULL && poEHSServer != NULL ) ||
			 ( poParent != NULL && poEHSServer == NULL ) );
	
	if ( poParent ) {

		poParent->HandleData( inTimeoutMilliseconds );

	} else {

		// if we're in single threaded mode, handle data until there are no more jobs left
		if ( poEHSServer->m_nServerRunningStatus ==
			 EHSServer::SERVERRUNNING_SINGLETHREADED ) {

			int nChecksInARow=0;

			do {

/*				fprintf ( stderr, "Doing %d requests in a row - %d - %d\n", ++nChecksInARow,
				poEHSServer->RequestsPending ( ),
				poEHSServer->m_nAcceptedNewConnection );
*/
			    pthread_t dummy;
memset(&dummy,0,sizeof(pthread_t));
//                dummy.p = NULL;
				poEHSServer->HandleData( inTimeoutMilliseconds, dummy );

			} while ( poEHSServer->RequestsPending ( ) ||
					  poEHSServer->m_nAcceptedNewConnection );

		}

	}

//	fprintf ( stderr, "##### Done checking this time through\n" );

}



std::string GetNextPathPart ( std::string & irsUri ///< URI to look for next path part in
	)
{


	PME oNextPathPartRegex ( "^[/]{0,1}([^/]+)/(.*)$" );
	
	if ( oNextPathPartRegex.match ( irsUri ) ) {
					 
		std::string sReturnValue = oNextPathPartRegex [ 1 ];
		std::string sNewUri = oNextPathPartRegex [ 2 ];

		irsUri = sNewUri;
		return sReturnValue;
		
	} else {

		return "";

	}

}

HttpResponse * 
EHS::RouteRequest ( HttpRequest * ipoHttpRequest ///< request info for service
	)
{

	// get the next path from the URI
	std::string sNextPathPart = GetNextPathPart ( ipoHttpRequest->sUri );


	EHS_TRACE ( "Info: Trying to route: '%s'\n", sNextPathPart.c_str ( ) );


	// if there is no more path, call HandleRequest on this EHS object with
	//   whatever's left - or if we're not routing
	if ( sNextPathPart.empty ( ) ||
		 m_oEHSServerParameters.find ( "norouterequest" ) !=
		 m_oEHSServerParameters.end ( ) ) {

		// create an HttpRespose object for the client
		HttpResponse * poHttpResponse = 
			new HttpResponse ( ipoHttpRequest->m_nRequestId,
							   ipoHttpRequest->m_poSourceEHSConnection );
		
		// get the actual response and return code
		poHttpResponse->m_nResponseCode = 
			HandleRequest ( ipoHttpRequest, poHttpResponse );

		return poHttpResponse;

	}

	// if the path exists, check it against the map of EHSs
	if ( oEHSMap [ sNextPathPart ] ) {

		// if it exists, call RouteRequest with that EHS and the
		//   new shortened path

		return oEHSMap [ sNextPathPart ]->RouteRequest ( ipoHttpRequest );

	}
	// if it doesn't exist, send an error back up saying resource doesn't exist
	else {

		
		EHS_TRACE ( "Info: Routing failed.  Most likely caused by an invalid URL, not internal error\n" );


		// send back a 404 response
		HttpResponse * poHttpResponse = new HttpResponse ( ipoHttpRequest->m_nRequestId,
														   ipoHttpRequest->m_poSourceEHSConnection );

		poHttpResponse->m_nResponseCode = HTTPRESPONSECODE_404_NOTFOUND;
		poHttpResponse->SetBody ( "404 - Not Found", strlen ( "404 - Not Found" ) );

		return poHttpResponse;
		
	}

}

// default handle request does nothing
ResponseCode EHS::HandleRequest ( HttpRequest * ipoHttpRequest,
								  HttpResponse * ipoHttpResponse)
{

	// if we have a source EHS specified, use it
	if ( m_poSourceEHS != NULL ) {
		return m_poSourceEHS->HandleRequest ( ipoHttpRequest, ipoHttpResponse );
	}

	// otherwise, just send back the current time
	char psTime [ 20 ];
	sprintf ( psTime, "%d", time ( NULL ) );
	ipoHttpResponse->SetBody ( psTime, strlen ( psTime ) );
	return HTTPRESPONSECODE_200_OK;

}

void EHS::SetSourceEHS ( EHS & iroSourceEHS )
{

	m_poSourceEHS = &iroSourceEHS;

}


void EHS::SetCertificateFile ( std::string & irsCertificateFile ) 
{

	assert ( 0 );

}

/// set certificate passphrase
void EHS::SetCertificatePassphrase ( std::string & irsCertificatePassphrase )
{

	assert ( 0 );

}

/// sets a new passphrase callback function
void EHS::SetPassphraseCallback ( int ( * m_ipfOverridePassphraseCallback ) ( char *, int, int, void * ) )
{

	assert ( 0 );

}


#ifdef COMPILE_WITH_SSL
// secure socket static class variables
DynamicSslLocking * SecureSocket::poDynamicSslLocking = NULL;
StaticSslLocking * SecureSocket::poStaticSslLocking = NULL;
SslError * SecureSocket::poSslError = NULL;

SSL_CTX * SecureSocket::poCtx;

#endif // COMPILE_WITH_SSL
