
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
#include <assert.h>

long long ms_HttpTotalBytesSent = 0;
SAllocationStats ms_AllocationStats = { 0 };
CCriticalSection ms_StatsCS;

// Returns true if lock succeeded
static bool MUTEX_TRY_LOCK( MUTEX_TYPE& x )
{
    return ( pthread_mutex_trylock( &x ) != EBUSY );
}

int EHSServer::CreateFdSet ( )
{
    MUTEX_LOCK ( m_oMutex );

	m_oReadFds.Reset();

	// add the accepting FD	
	m_oReadFds.Add( m_poNetworkAbstraction->GetFd(), POLLIN );

	int nHighestFd = m_poNetworkAbstraction->GetFd ( );

	for ( EHSConnectionList::iterator oCurrentConnection = m_oEHSConnectionList.begin ( );
		  !( oCurrentConnection == m_oEHSConnectionList.end ( ) );
		  oCurrentConnection++ ) {
		
		/// skip this one if it's already been used
		if ( (*oCurrentConnection)->StillReading ( ) ) {

			int nCurrentFd = 
				( * oCurrentConnection )->GetNetworkAbstraction ( )->GetFd ( );
			
			EHS_TRACE ( "Adding %d to FD SET\n", nCurrentFd );

			m_oReadFds.Add( nCurrentFd, POLLIN );

			// store the highest FD in the set to return it
			if ( nCurrentFd > nHighestFd ) {

				nHighestFd = nCurrentFd;

			}

		} else {

			EHS_TRACE ( "FD %d isn't reading anymore\n", 
						( * oCurrentConnection )->GetNetworkAbstraction ( )->GetFd ( ) );

		}
	}

    MUTEX_UNLOCK ( m_oMutex );

	return nHighestFd;

}



int EHSServer::ClearIdleConnections ( )
{

	// don't lock mutex, as it is only called rom within locked sections

	int nIdleConnections = 0;

	for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		  i != m_oEHSConnectionList.end ( );
		  i++ ) {

        if ( MUTEX_TRY_LOCK ( (*i)->m_oConnectionMutex ) == false )
            continue;

		// if it's been more than N seconds since a response has been
		//   sent and there are no pending requests
		if ( (*i)->StillReading ( ) &&
			 ( time ( NULL ) - (*i)->LastActivity ( ) > m_nIdleTimeout || (*i)->m_iStopASAP ) &&
			 !(*i)->RequestsPending ( )
			  ) {

			EHS_TRACE ( "Done reading because of idle timeout\n" );
    
			(*i)->DoneReading ( false );

			nIdleConnections++;

		}
		
		MUTEX_UNLOCK ( (*i)->m_oConnectionMutex );

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
	m_poEHSServer ( ipoEHSServer ),
    m_iStopASAP ( 0 )
{
    m_UnusedSyncId = 0;
	UpdateLastActivity ( );

	// initialize mutex for this object
	MUTEX_SETUP ( m_oConnectionMutex );

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
    MUTEX_CLEANUP ( m_oConnectionMutex );
    if ( m_poCurrentHttpRequest )
        delete m_poCurrentHttpRequest;
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
    // Mutex is locked

	// make sure we actually got some data
	if ( inSize <= 0 ) {
		return ADDBUFFER_INVALID;
	}

	// this is binary safe -- only the single argument char* constructor looks for NULL
	m_sBuffer += std::string ( ipsData, inSize );

	// make sure the buffer hasn't grown too big
	if ( m_sBuffer.length ( ) > MAX_BUFFER_SIZE_BEFORE_BOOT ) {
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

					MUTEX_UNLOCK ( m_oConnectionMutex );
					pthread_create ( & oThread,
									 NULL,
									 EHSServer::PthreadHandleData_ThreadedStub,
									 (void *) m_poEHSServer );
					pthread_detach ( oThread );
					MUTEX_LOCK ( m_oConnectionMutex );


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

	return nReturnValue;

}

/// call when no more reads will be performed on this object.  inDisconnected is true when client has disconnected
void EHSConnection::DoneReading ( int inDisconnected )
{
    // Mutex is locked
	m_nDoneReading = 1;
	m_nDisconnected = inDisconnected;
}


HttpRequest * EHSConnection::GetNextRequest ( )
{

	HttpRequest * poHttpRequest = NULL;

    if ( MUTEX_TRY_LOCK ( m_oConnectionMutex ) == false )
        return NULL;

	if ( m_oHttpRequestList.empty ( ) ) {

		poHttpRequest = NULL;

	} else {

		poHttpRequest = m_oHttpRequestList.front ( );
		
		m_oHttpRequestList.pop_front ( );
		
	}

	MUTEX_UNLOCK ( m_oConnectionMutex );

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
    m_iActiveThreadCount = 0;

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
    if ( m_poNetworkAbstraction )
    {
        m_poNetworkAbstraction->Close ();
        delete m_poNetworkAbstraction;
        m_poNetworkAbstraction = NULL;
    }
    MUTEX_CLEANUP ( m_oMutex );

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
#if STOP_MEMORY_LEAKS   // Needs testing
            delete poEHSConnection;
#endif

			// start back over at the beginning of the list
			m_oEHSConnectionList.erase ( i );
			i = m_oEHSConnectionList.begin ( );

            // Mark as unused
            poEHSConnection->m_UnusedSyncId = m_ThreadsSyncPoint.GetSyncId ();
            m_oEHSConnectionUnusedList.push_back ( poEHSConnection );
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

        // Allow 10 seconds for threads to stop
        for( int i = 0; i < 100 && poEHSServer->m_iActiveThreadCount; i++ )
        {
            pthread_cond_broadcast ( & poEHSServer->m_oDoneAccepting );
            Sleep(100);
        }
	}

}




void EHSServer::HandleData_Threaded ( )
{

#define HANDLEDATA_THREADEDSELECTTIMEOUTMILLISECONDS 1000


	pthread_t nMyThreadId = pthread_self ( );

	MUTEX_LOCK ( m_oMutex );
    int ThreadIndex = m_ThreadsSyncPoint.GetNewThreadIndex ();
    m_iActiveThreadCount++;
	MUTEX_UNLOCK ( m_oMutex );

	do {

        //fprintf ( stderr, "##### [%d] Thread HandleData\n", nMyThreadId );
		HandleData ( HANDLEDATA_THREADEDSELECTTIMEOUTMILLISECONDS,
					 nMyThreadId );

	    MUTEX_LOCK ( m_oMutex );
        m_ThreadsSyncPoint.SyncThreadIndex ( ThreadIndex );
	    MUTEX_UNLOCK ( m_oMutex );

	} while ( m_nServerRunningStatus != SERVERRUNNING_NOTRUNNING &&
              ( m_nServerRunningStatus == SERVERRUNNING_THREADPOOL ||
			    pthread_equal(nMyThreadId,m_nAcceptThreadId) ) );

	//fprintf ( stderr, "##### [%d] Thread exiting\n", nMyThreadId );
	MUTEX_LOCK ( m_oMutex );
    m_iActiveThreadCount--;
	MUTEX_UNLOCK ( m_oMutex );


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

			// create the FD set for poll
			CreateFdSet();
			int nSocketCount = poll( m_oReadFds.GetFdArray(), m_oReadFds.GetFdCount(), inTimeoutMilliseconds );

			// handle select error
#ifdef _WIN32
			if ( nSocketCount == SOCKET_ERROR )
#else // NOT _WIN32
			if ( nSocketCount == -1 )
#endif // _WIN32
			{

				EHS_TRACE ( "[%d] Critical Error: select() failed.  Aborting\n", inThreadId );

				// Idea! Remove stupid idea
				//exit ( 0 );
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

            // Occasional pulse for updating of things
            m_poTopLevelEHS->HttpPulse ();

		} // END ACCEPTING
		
	} // END NO REQUESTS PENDING


    MUTEX_LOCK ( m_oMutex );

    // Delete unused connections after all threads have been synced
    EHSConnectionList :: iterator iter = m_oEHSConnectionUnusedList.begin ();
    while ( iter != m_oEHSConnectionUnusedList.end () )
    {
        EHSConnection* pConnection = *iter;
        // Delete it after all threads are past syncing point
        if ( pConnection->m_UnusedSyncId < m_ThreadsSyncPoint.GetSyncId () - 1 )
        {
            iter = m_oEHSConnectionUnusedList.erase ( iter );
            delete pConnection;
        }
        else
            iter++;
    }

    MUTEX_UNLOCK ( m_oMutex );

}

void EHSServer::CheckAcceptSocket ( )
{

	// see if we got data on this socket
	if ( m_oReadFds.IsSet( m_poNetworkAbstraction->GetFd(), POLLIN ) ) {
		

        //printf ( "Accept new connection\n");
		// THIS SHOULD BE NON-BLOCKING OR ELSE A HANG CAN OCCUR IF THEY DISCONNECT BETWEEN WHEN
		//   POLL SEES THE CONNECTION AND WHEN WE ACTUALLY CALL ACCEPT
		NetworkAbstraction * poNewClient = 
			m_poNetworkAbstraction->Accept ( );
		
        

		// not sure what would cause this
		if ( poNewClient == NULL ) {
			return;
		}

        // Flood detection		
        if ( !m_poTopLevelEHS->ShouldAllowConnection ( poNewClient->GetAddress ().c_str () ) )
        {

		    MUTEX_LOCK ( m_oMutex );
            // Stop all other connections using this address
	        for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		          i != m_oEHSConnectionList.end ( );
		          i++ ) {

		        if ( (*i)->GetNetworkAbstraction ( )->GetAddress ( ) == poNewClient->GetAddress () )
                {
                    (*i)->m_iStopASAP = true;
                }
            }
		    MUTEX_UNLOCK ( m_oMutex );

            // Reject connection
            delete poNewClient;
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
    MUTEX_LOCK ( m_oMutex );

	// go through all the sockets from which we're still reading
	for ( EHSConnectionList::iterator i = m_oEHSConnectionList.begin ( );
		  i != m_oEHSConnectionList.end ( );
		  i++ ) {

		if ( m_oReadFds.IsSet( (*i)->GetNetworkAbstraction()->GetFd(), POLLIN ) ) {

            if ( MUTEX_TRY_LOCK ( (*i)->m_oConnectionMutex ) == false )
                continue;

			EHS_TRACE ( "$$$$$ Got data on client connection\n" );


			// prepare a buffer for the read
			static const int BYTES_TO_READ_AT_A_TIME = 10240;
			char psReadBuffer [ BYTES_TO_READ_AT_A_TIME + 1 ];
			memset ( psReadBuffer, 0, BYTES_TO_READ_AT_A_TIME + 1 );

			// do the actual read
			int nBytesReceived =
				(*i)->GetNetworkAbstraction ( )->Read ( psReadBuffer,
														BYTES_TO_READ_AT_A_TIME );

            if ( nBytesReceived == SOCKET_ERROR )
            {
                int err = GetLastSocketError ();
                if ( err == E_WOULDBLOCK )
                {
                    MUTEX_UNLOCK ( (*i)->m_oConnectionMutex );
                    continue;
                }
            }
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

            MUTEX_UNLOCK ( (*i)->m_oConnectionMutex );
		} // FD_ISSET
		
	} // for loop through connections
    MUTEX_UNLOCK ( m_oMutex );

} 


const char* GetHttpStatusReasonPhrase(int statusCode)
{
	if (statusCode >= 100 && statusCode <= 199)
	{
		switch (statusCode)
		{
			case HTTP_STATUS_CODE_100_CONTINUE:
				return "Continue";
			case HTTP_STATUS_CODE_101_SWITCHING_PROTOCOLS:
				return "Switching Protocols";
			default:
				return "Informational";
		}
	}

	if (statusCode >= 200 && statusCode <= 299)
	{
		switch (statusCode)
		{
			case HTTP_STATUS_CODE_200_OK:
				return "OK";
			case HTTP_STATUS_CODE_201_CREATED:
				return "Created";
			case HTTP_STATUS_CODE_202_ACCEPTED:
				return "Accepted";
			case HTTP_STATUS_CODE_203_NON_AUTHORITATIVE_INFO:
				return "Non-Authoritative Information";
			case HTTP_STATUS_CODE_204_NO_CONTENT:
				return "No Content";
			case HTTP_STATUS_CODE_205_RESET_CONTENT:
				return "Reset Content";
			case HTTP_STATUS_CODE_206_PARTIAL_CONTENT:
				return "Partial Content";
			default:
				return "Success";
		}
	}

	if (statusCode >= 300 && statusCode <= 399)
	{
		switch (statusCode)
		{
			case HTTP_STATUS_CODE_300_MULTIPLE_CHOICES:
				return "Multiple Choices";
			case HTTP_STATUS_CODE_301_MOVED_PERMANENTLY:
				return "Moved Permanently";
			case HTTP_STATUS_CODE_302_FOUND:
				return "Found";
			case HTTP_STATUS_CODE_303_SEE_OTHER:
				return "See Other";
			case HTTP_STATUS_CODE_304_NOT_MODIFIED:
				return "Not Modified";
			case HTTP_STATUS_CODE_305_USE_PROXY:
				return "Use Proxy";
			case HTTP_STATUS_CODE_307_TEMPORARY_REDIRECT:
				return "Temporary Redirect";
			default:
				return "Redirection";
		}
	}

	if (statusCode >= 400 && statusCode <= 499)
	{
		switch (statusCode)
		{
			case HTTP_STATUS_CODE_400_BAD_REQUEST:
				return "Bad Request";
			case HTTP_STATUS_CODE_401_UNAUTHORIZED:
				return "Unauthorized";
			case HTTP_STATUS_CODE_402_PAYMENT_REQUIRED:
				return "Payment Required";
			case HTTP_STATUS_CODE_403_FORBIDDEN:
				return "Forbidden";
			case HTTP_STATUS_CODE_404_NOT_FOUND:
				return "Not Found";
			case HTTP_STATUS_CODE_405_METHOD_NOT_ALLOWED:
				return "Method Not Allowed";
			case HTTP_STATUS_CODE_406_NOT_ACCEPTABLE:
				return "Not Acceptable";
			case HTTP_STATUS_CODE_407_PROXY_AUTH_REQUIRED:
				return "Proxy Authentication Required";
			case HTTP_STATUS_CODE_408_REQUEST_TIMEOUT:
				return "Request Time-out";
			case HTTP_STATUS_CODE_409_CONFLICT:
				return "Conflict";
			case HTTP_STATUS_CODE_410_GONE:
				return "Gone";
			case HTTP_STATUS_CODE_411_LENGTH_REQUIRED:
				return "Length Required";
			case HTTP_STATUS_CODE_412_PRECONDITION_FAILED:
				return "Precondition Failed";
			case HTTP_STATUS_CODE_413_REQUEST_ENTITY_TOO_LARGE:
				return "Request Entity Too Large";
			case HTTP_STATUS_CODE_414_URI_TOO_LARGE:
				return "Request-URI Too Large";
			case HTTP_STATUS_CODE_415_UNSUPPORTED_MEDIA_TYPE:
				return "Unsupported Media Type";
			case HTTP_STATUS_CODE_416_RANGE_NOT_SATISFIABLE:
				return "Requested range not satisfiable";
			case HTTP_STATUS_CODE_417_EXPECTATION_FAILED:
				return "Expectation Failed";
			default:
				return "Client Error";
		}
	}

	if (statusCode >= 500 && statusCode <= 599)
	{
		switch (statusCode)
		{
			case HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR:
				return "Internal Server Error";
			case HTTP_STATUS_CODE_501_NOT_IMPLEMENTED:
				return "Not Implemented";
			case HTTP_STATUS_CODE_502_BAD_GATEWAY:
				return "Bad Gateway";
			case HTTP_STATUS_CODE_503_SERVICE_UNAVAILABLE:
				return "Service Unavailable";
			case HTTP_STATUS_CODE_504_GATEWAY_TIMEOUT:
				return "Gateway Time-out";
			case HTTP_STATUS_CODE_505_VERSION_NOT_SUPPORTED:
				return "HTTP Version not supported";
			default:
				return "Server Error";
		}
	}

	return "Unknown";
}


void EHSConnection::AddResponse ( HttpResponse * ipoHttpResponse )
{

	MUTEX_LOCK ( m_oConnectionMutex );

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
				MUTEX_UNLOCK ( m_oConnectionMutex );
				MUTEX_LOCK ( m_poEHSServer->m_oMutex );
				m_poEHSServer->RemoveEHSConnection ( this );
				MUTEX_UNLOCK ( m_poEHSServer->m_oMutex );
                return;

			}


			EHS_TRACE ( "Sending response %d to %x\n", m_nResponses, this );


		}

	} while ( nFoundNextResponse == 1 );

	MUTEX_UNLOCK ( m_oConnectionMutex );

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
	sOutput +=  GetHttpStatusReasonPhrase ( ipoHttpResponse->m_nResponseCode );
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

	int ret = TrySend ( sOutput.c_str (), sOutput.length() );

    if ( ret == -1 )
        return;

	// now send the body
	TrySend ( ipoHttpResponse->GetBody ( ), 
								   atoi ( ipoHttpResponse->oResponseHeaders [ "content-length" ].c_str ( ) ) );

}

int EHSConnection::TrySend ( const char * ipMessage, size_t inLength, int inFlags )
{

    int iWouldBlockCount = 0;
lp1:
    {
        int iCount = 0;
        UpdateLastActivity ( );
        while( !m_poNetworkAbstraction->IsWritable(1000) && !m_poNetworkAbstraction->IsAtError(0) )
        {
            iCount++;
            if ( iCount == 10 )
            {
                break;
            }
        } 
    }

	int ret = m_poNetworkAbstraction->Send ( ipMessage, inLength, inFlags );

    if ( ret == SOCKET_ERROR )
    {
        int err = GetLastSocketError ();
        if ( err == E_WOULDBLOCK )
        {
            Sleep(20);
            iWouldBlockCount++;
            if ( iWouldBlockCount < 2 )
                goto lp1;
            m_poNetworkAbstraction->Close();
        }
    }
    else
    if ( ret != inLength )
    {
        ipMessage += ret;
        inLength -= ret;
        Sleep(1);
        goto lp1;
    }

    return ret;
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

		poHttpResponse->m_nResponseCode = HTTP_STATUS_CODE_404_NOT_FOUND;
		poHttpResponse->SetBody ( "404 - Not Found", strlen ( "404 - Not Found" ) );

		return poHttpResponse;
		
	}

}

// default handle request does nothing
HttpStatusCode EHS::HandleRequest ( HttpRequest * ipoHttpRequest,
								    HttpResponse * ipoHttpResponse)
{

	// if we have a source EHS specified, use it
	if ( m_poSourceEHS != NULL ) {
		return m_poSourceEHS->HandleRequest ( ipoHttpRequest, ipoHttpResponse );
	}

	// otherwise, just send back the current time
	char psTime [ 20 ];
	sprintf ( psTime, "%lld", time ( NULL ) );
	ipoHttpResponse->SetBody ( psTime, strlen ( psTime ) );
	return HTTP_STATUS_CODE_200_OK;

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

long long EHS::StaticGetTotalBytesSent ( void )
{
    ms_StatsCS.Lock();
    long long llResult = ms_HttpTotalBytesSent;
    ms_StatsCS.Unlock();
    return llResult;
}

void EHS::StaticGetAllocationStats ( SAllocationStats& outAllocationStats )
{
    ms_StatsCS.Lock();
    outAllocationStats = ms_AllocationStats;
    ms_StatsCS.Unlock();
}

void StatsAddTotalBytesSent( size_t inLength )
{
    ms_StatsCS.Lock();
    ms_HttpTotalBytesSent += inLength;
    ms_StatsCS.Unlock();
}

void StatsNumRequestsInc( void )
{
    ms_StatsCS.Lock();
    ms_AllocationStats.uiTotalNumRequests++;
    ms_AllocationStats.uiActiveNumRequests++;
    ms_StatsCS.Unlock();
}

void StatsNumRequestsDec( void )
{
    ms_StatsCS.Lock();
    ms_AllocationStats.uiActiveNumRequests--;
    ms_StatsCS.Unlock();
}

void StatsNumResponsesInc( void )
{
    ms_StatsCS.Lock();
    ms_AllocationStats.uiTotalNumResponses++;
    ms_AllocationStats.uiActiveNumResponses++;
    ms_StatsCS.Unlock();
}

void StatsNumResponsesDec( void )
{
    ms_StatsCS.Lock();
    ms_AllocationStats.uiActiveNumResponses--;
    ms_StatsCS.Unlock();
}

void StatsBytesAllocated( int nBodyLength )
{
    ms_StatsCS.Lock();
    ms_AllocationStats.uiTotalKBAllocated += nBodyLength / 1024;
    ms_AllocationStats.uiActiveKBAllocated += nBodyLength / 1024;
    ms_StatsCS.Unlock();
}

void StatsBytesDeallocated( int nBodyLength )
{
    ms_StatsCS.Lock();
    ms_AllocationStats.uiActiveKBAllocated -= nBodyLength / 1024;
    ms_StatsCS.Unlock();
}
