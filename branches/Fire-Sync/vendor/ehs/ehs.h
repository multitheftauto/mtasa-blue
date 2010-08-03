
/** \mainpage EHS 1.1.3


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

#ifndef EHS_H
#define EHS_H

///////////////////////
#ifdef HAVE_CONFIG_H //
///////////////////////

// load up the autoheader-generated config file if it exists (UNIX only)
#include <config.h>

/////////////////////
#endif             //
/////////////////////



///////////////////////////////////
#ifdef _WIN32 // windows headers //
///////////////////////////////////


// Pragma'ing away nasty MS 255-char-name problem.  Otherwise
// you will get warnings on many template names that
//	"identifier was truncated to '255' characters in the debug information".
#pragma warning(disable : 4786)

// to use winsock2.h instead of winsock.h
#define _WIN32_WINNT 0x0400
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

#include <dlfcn.h>
#include <sys/time.h>
#include <string.h>
#include <sys/fcntl.h>

///////////////////////////////////
#endif // end platform headers   //
///////////////////////////////////


// STL headers
#include <algorithm>
#include <cctype>
#include <deque>
#include <functional>
#include <list>
#include <map>
#include <string>
#include <typeinfo>

// C headers
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


// other library headers
#include <pcre.h>
#include <pme.h>

// EHS headers
#include "ehstypes.h"
#include "datum.h"
#include "networkabstraction.h"
#include "socket.h"
#include "securesocket.h"
#include "httpresponse.h"
#include "httprequest.h"
#include "threadabstractionlayer.h"

class EHSServer;

/// EHSConnection abstracts the concept of a connection to an EHS application
/**
 * EHSConnection abstracts the concept of a connection to an EHS application.  
 * It stores file descriptor information, unhandled data, and the current 
 * state of the request
 */
class EHSConnection {

  protected:

	int m_nDoneReading; ///< we're never reading from this again
	int m_nDisconnected; ///< client has closed connection on us
	
	HttpRequest * m_poCurrentHttpRequest; ///< request we're currently parsing

	EHSServer * m_poEHSServer; ///< server with which this is associated

	time_t m_nLastActivity; ///< time at which the last activity occured

	int m_nRequests; ///< holds id of last request received

	int m_nResponses; ///< holds id of last response sent



	/// file descriptor associated with this client
	NetworkAbstraction * m_poNetworkAbstraction;	

	/// raw data received from client that doesn't comprise a full request
	std::string m_sBuffer;
	
	/// holds out-of-order httpresponses that aren't ready to go out yet
	HttpResponseMap m_oHttpResponseMap;
	
	/// holds pending requests
	HttpRequestList m_oHttpRequestList;

	/// address from which the connection originated
	std::string m_sAddress;

	/// remote port from which the connection originated
	int m_nPort;

  public:

	/// Constructor
	EHSConnection ( NetworkAbstraction * ipoNetworkAbstraction,
					EHSServer * ipoEHSServer );

	/// destructor
	~EHSConnection ( );

	MUTEX_TYPE m_oConnectionMutex; ///< mutex protecting entire object

    long long m_UnusedSyncId;       // Value of SyncId when the connection was first unused

	/// updates the last activity to the current time
	void UpdateLastActivity ( ) { m_nLastActivity = time ( NULL ); }

	/// returns the time of last activity
	time_t LastActivity ( ) { return m_nLastActivity; }

	/// returns whether we're still reading from this socket -- mutex must be locked
	int StillReading ( ) { return !m_nDoneReading; }

	/// returns whether the client has disconnected from us -- mutex must be locked
	int Disconnected ( ) { return m_nDisconnected; }

	/// call when no more reads will be performed on this object.  inDisconnected is true when client has disconnected
	void DoneReading ( int inDisconnected );

	/// gets the next request object
	HttpRequest * GetNextRequest ( );

	/// returns true if object should be deleted
	int CheckDone ( );

	/// Enumeration result for AddBuffer
	enum AddBufferResult { ADDBUFFER_INVALID = 0,
						   ADDBUFFER_OK,
						   ADDBUFFER_INVALIDREQUEST,
						   ADDBUFFER_TOOBIG };

    /// this is to protect from people being malicious or really stupid
    #define MAX_BUFFER_SIZE_BEFORE_BOOT 102400

	/// adds new data to psBuffer
	AddBufferResult AddBuffer ( char * ipsData, int inSize );

	/// sends the actual data back to the client
	void SendHttpResponse ( HttpResponse * ipoHttpResponse);

	/// adds a response to the response list and sends as many responses as are ready -- takes over the memory in ipoHttpResponse
	void AddResponse ( HttpResponse * ipoHttpResponse );

	/// returns address of connection
	std::string GetAddress ( ) { return m_sAddress; }

	/// returns client port of connection
	int GetPort ( ) { return m_nPort; }

	/// returns true of httprequestlist is not empty
	int RequestsPending ( ) { return !m_oHttpRequestList.empty ( ); }

	/// returns underlying network abstraction
	NetworkAbstraction * GetNetworkAbstraction ( );
	
    // Send with retries
    int TrySend ( const char * ipMessage, size_t inLength, int inFlags = 0 );

};



// predeclare because of circular reference between EHSServer and EHS
class EHSServer;


/// EHS provides HTTP server functionality to a child class
/**
 * EHS provides HTTP server functionality to a child class.  The child class 
 * must inherit from it and then override HandleRequest ( ).
 */
class EHS {

  protected:
	
	/// stores path => EHSConnection pairs for path/tree traversal
	EHSMap oEHSMap;

	/// points to the EHS object this object was registered with, NULL if top level
	EHS * poParent; 

	/// the string that this EHS object is regestered as
	std::string sRegisteredAs;

	/// EHSServer object associated with this EHS object
	EHSServer * poEHSServer;
	
	/// source EHS object to route requests to for data instead of processing it ourselves
	EHS * m_poSourceEHS;


  public:

	/// default constructor that can set a parrent and a path name
	EHS ( EHS * ipoParent = NULL, std::string isRegisteredAs = "" );

	/// destructor
	virtual ~EHS ( );

	/// set the certificate file for use in HTTPS transactions
	void SetCertificateFile ( std::string & irsCertificateFile );

	/// set certificate passphrase
	void SetCertificatePassphrase ( std::string & irsCertificatePassphrase );

	/// sets a new passphrase callback function
	void SetPassphraseCallback ( int ( * m_ipfOverridePassphraseCallback ) ( char *, int, int, void * ) );

	/// sets the poParent member of this class to the specified EHS.  This is how an EHS object can unregister itself on destruction
	void SetParent ( EHS * ipoParent, std::string isRegisteredAs );


	/// Enumeration for error results for RegisterEHSResult
	enum RegisterEHSResult { REGISTEREHSINTERFACE_INVALID = 0,
							 REGISTEREHSINTERFACE_ALREADYEXISTS,
							 REGISTEREHSINTERFACE_SUCCESS };

	/// this associates an EHS object with this EHS object under the path ipsRegisterPath
	RegisterEHSResult
		RegisterEHS ( EHS * ipoEHS,
					  const char * ipsRegisterPath );

	/// Enumeration for error results for UnregisterEHSResult
	enum UnregisterEHSResult { UNREGISTEREHSINTERFACE_INVALID = 0,
							   UNREGISTEREHSINTERFACE_NOTREGISTERED,
							   UNREGISTEREHSINTERFACE_SUCCESS };

	/// Unregister an EHS object from the specified path
	UnregisterEHSResult
		UnregisterEHS ( const char * ipsRegisterPath );

	/// this is responsible for routing a request through the EHS tree and sending the request to the final destination.  It returns the HttpResponse object to be sent back to the client
	HttpResponse * RouteRequest ( HttpRequest * ipoHttpRequest );

	/// This function should be defined by the subclass
	virtual ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest,
										 HttpResponse * ipoHttpResponse );

	/// This function should be defined by the subclass
	virtual void HttpPulse ( void ) {};

	/// makes this EHS object get its data from another EHS -- useful for having secure and normal connections share same data
	void SetSourceEHS ( EHS & iroSourceEHS );


	/// result codes for StartServer and StartSErver_Threaded
	enum StartServerResult { STARTSERVER_INVALID = 0,
							 STARTSERVER_SUCCESS,
							 STARTSERVER_NODATASPECIFIED,
							 STARTSERVER_ALREADYRUNNING,
							 STARTSERVER_SOCKETSNOTINITIALIZED,
							 STARTSERVER_THREADCREATIONFAILED,
							 STARTSERVER_FAILED };



	/// stores a map with server parameters
	EHSServerParameters m_oEHSServerParameters;


	/// do everything needed to start server
	StartServerResult StartServer ( EHSServerParameters & iroEHSServerParameters );


	/// brings down socket stuff.  If StartServer_Threaded() was called it also stops that by setting the nServerStopped variable
	void StopServer ( );

	/// This looks for incoming connections in EHSServer.
	void HandleData ( int inTimeoutMilliseconds = 0 );

};


class CThreadsSyncPoint
{
    int                 m_iHighestThreadIndex;
    long long           m_SyncId;
    std::map<int,int>   m_SyncList;
public:

    CThreadsSyncPoint ()
    {
        m_iHighestThreadIndex = -1;
        m_SyncId = 0;
    }

    long long GetSyncId () const
    {
        return m_SyncId;
    }

    int GetNewThreadIndex ()
    {
        return ++m_iHighestThreadIndex;
    }

    void SyncThreadIndex( int iThreadIndex )
    {
        m_SyncList[ iThreadIndex ] = 1;
        if ( (int)m_SyncList.size() == m_iHighestThreadIndex )
        {
            m_SyncId++;
            m_SyncList.clear ();
        }
    }
};

/// EHSServer contains all the network functionality for EHS
/**
 * EHSServer has all the network related services for EHS.  It is responsible
 *   for accepting new connections and getting EHSConnection objects set up
 */
class EHSServer {

  public:

	/// consturctor for an EHSServer -- takes parameters out of ipoTopLevelEHS->m_oEHSServerParameters;
	EHSServer ( EHS * ipoTopLevelEHS );
	virtual ~EHSServer ( );

	/// removes the specified EHSConnection object, returns true if it actually found something to remove
	int RemoveEHSConnection ( EHSConnection * ipoEHSConnection );

	/// disconnects idle connections
	int ClearIdleConnections ( );

	/// removes all connections from the server that are no longer active
	int RemoveFinishedConnections ( );

	/// sets default http response headers, such as date, and content type
	void InitHttpResponse ( HttpResponse * ipoHttpResponse );

	/// stops the server
	void EndServerThread ( char * ipsReason );
	
	/// main function that deals with client connections and getting data
	void HandleData ( int inTimeoutMilliseconds, pthread_t inThreadId  ); 

	/// check clients that are already connected
	void CheckClientSockets ( );

	/// check the listen socket for a new connection
	void CheckAcceptSocket ( );

	/// Enumeration on the current running status of the EHSServer
	enum ServerRunningStatus { SERVERRUNNING_INVALID = 0,
							   SERVERRUNNING_NOTRUNNING,
							   SERVERRUNNING_SINGLETHREADED,
							   SERVERRUNNING_THREADPOOL,
							   SERVERRUNNING_ONETHREADPERREQUEST
	};

	/// Current running status of the EHSServer
	ServerRunningStatus m_nServerRunningStatus;

	/// reason for shutting down
	std::string m_sShutdownReason;
	
	/// this runs in a loop until told to stop by StopServer() -- runs off it's own thread created by StartServer_Threaded
	void HandleData_Threaded ( );

	/// pointer back up to top-most level EHS object
	EHS * m_poTopLevelEHS;

	/// gets a pending request
	HttpRequest * GetNextRequest ( );

	/// whether we accepted a new connection last time through
	int m_nAcceptedNewConnection;

	/// returns number of requests pending
	int RequestsPending ( ) { return m_nRequestsPending; }

	/// increments the number of pending requests
	void IncrementRequestsPending ( ) { m_nRequestsPending++; }

	/// mutex for controlling who is accepting or processing jobs
	pthread_mutex_t m_oMutex;

	/// condition for when a thread is done accepting and there may be more jobs to process
	pthread_cond_t m_oDoneAccepting;

    // Used to ensure all threads are past a certain point
    CThreadsSyncPoint m_ThreadsSyncPoint;

    // Count of active threads
    int m_iActiveThreadCount;

	/// static method for starting threaded mode -- pthreads can't start a thread on a normal member function, only static
	static void * PthreadHandleData_ThreadedStub ( void * ipData );



  protected:

	/// number of requests waiting to be processed
	int m_nRequestsPending;
	

	/// number of threads currently accepting (0 or 1)
	int m_nAccepting;

	/// this is the server name sent out in the response headers
	std::string sServerName;

	/// creates the poll array with the accept socket and any connections present
	int CreateFdSet ( );

	/// this is the read set for sending to select(2)
	fd_set m_oReadFds;

	/// List of all connections currently attached to the server
	EHSConnectionList m_oEHSConnectionList;

    // List of all connections that are no longer used
	EHSConnectionList m_oEHSConnectionUnusedList;

	/// the network abstraction this server is listening on
	NetworkAbstraction * m_poNetworkAbstraction;

	/// pthread identifier for the accept thread -- only used when started in threaded mode
	pthread_t m_nAcceptThreadId;

	/// number of seconds a connection can be idle before disconnect
	int m_nIdleTimeout;

};



// EHS_ASSERT()
// EHS_VERIFY()
// EHS_TRACE()

// looks for needle in haystack
char * EHSMemMem ( const char * ipsHaystack, int inHaystackLength,
				   const char * ipsNeedle,   int inNeedleLength );


#ifdef _WIN32
#define strcasecmp stricmp
#endif

#define EHS_ASSERT assert

#ifdef EHS_DEBUG
#define EHS_VERIFY(test) EHS_ASSERT(test)
#else
#define EHS_VERIFY(test)	test
#endif

#undef EHS_DEBUG
//#define EHS_DEBUG
inline void EHS_TRACE( const char* szFormat ... )
{
#ifdef EHS_DEBUG

	const int bufsize = 100000; 
	char buf [ bufsize ] ; 
	va_list VarList;
	va_start( VarList, szFormat );
	int len = vsprintf(buf, szFormat, VarList ) ;
	assert( len > 0 && len < bufsize ) ;
	va_end ( VarList ) ;
	//fprintf ( stderr, buf ) ;
#ifdef WIN32
	//OutputDebugString(buf) ;
//   This would also work but requires more includes
//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG );
//	_CrtDbgReport( _CRT_WARN, NULL, NULL, NULL, "%s", buf );
#endif
#endif
}



#define _STR(x) #x
#define EHS_TODO			(_message) message ("  *** TODO: " ##_message "\t\t\t\t" __FILE__ ":" _STR(__LINE__) )	
#define EHS_FUTURE		(_message) message ("  *** FUTURE: " ##_message "\t\t\t\t" __FILE__ ":" _STR(__LINE__) )	
#define EHS_TODOCUMENT	(_message) message ("  *** TODOCUMENT: " ##_message "\t\t\t\t" __FILE__ ":" _STR(__LINE__) )	
#define EHS_DEBUGCODE	(_message) message ("  *** DEBUG CODE (REMOVE!): " ##_message "\t\t\t\t" __FILE__ ":" _STR(__LINE__) )	




#endif // EHS_H


