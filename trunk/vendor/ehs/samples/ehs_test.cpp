
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
  
  Zac Hansen ( xaxxon@slackworks.com )

*/



#include <iostream>
#include <fstream>

using namespace std;

#include "../ehs.h"



class tester : public EHS
{
	

public:

	int m_nDelay;

protected:

	// override the HandleRequest method of EHS -- it's called once for each request received
	virtual ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse );
public:

	// constructor that takes a port to bind to, a server name (sent in the "Server:" header back 
	//   to the client, and a filename to read off to the client
	tester ( std::string isFilename ) : 
		EHS ( ),
		infile ( isFilename.c_str ( ) )
		{
			printf ( "loading file '%s'\n", isFilename.c_str ( ) );
			assert ( infile );
		}

	// stores the filename
	ifstream infile;

};

ResponseCode tester::HandleRequest ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
	
	pthread_mutex_t oMutex = PTHREAD_MUTEX_INITIALIZER;

	static int nRequests = 0;

	pthread_mutex_lock ( & oMutex );
	nRequests++;
	int nMyRequest = nRequests;
	pthread_mutex_unlock ( & oMutex );
	if ( nMyRequest % 1000 == 0 ) {
		fprintf ( stderr, "[%d]\n", nMyRequest );
	}

	char psBody [ 5000 ];

	sleep ( m_nDelay );

	// this no longer has to be dynamically allocated
	infile.getline ( psBody, 4999 );
	
	std::string sWordCopy = psBody;

	strcat ( psBody, "<br>previous word: " );
	strcat ( psBody, ipoHttpRequest->oCookieMap["ehs_test_cookie"].c_str ( ) );

	ipoHttpResponse->SetBody ( psBody, strlen ( psBody ) );

	// throw in a cookie here, just to show how it's done
	CookieParameters oCP;
	oCP["name"] = "ehs_test_cookie";
	oCP["value"] = sWordCopy;
	// more attributes can go here
	ipoHttpResponse->SetCookie ( oCP );
	
	return HTTPRESPONSECODE_200_OK;

}







int main ( int argc, char ** argv )
{
	
	if ( argc < 4 ) {
		fprintf ( stderr, "Usage: %s [mode] [port] [file] <delay> <threadcount> <norouterequest>)\n", argv[0] );
		fprintf ( stderr, "\tModes: 1 - Single Threaded (last parameter ignored)\n" );
		fprintf ( stderr, "\tModes: 2 - Multithreaded, fixed number of threads\n" );
		fprintf ( stderr, "\tModes: 3 - Multithreaded, one thread per request (last parameter ignored)\n" );
		fprintf ( stderr, "\tnorouterequest: if anything is specified, requests will not be routed\n" );


		exit ( 0 );
	}

	int nMode = atoi ( argv [ 1 ] );


	fprintf ( stderr, "binding to %d\n", atoi ( argv [ 2 ] ) );
	tester * srv = new tester ( argv [ 3 ] );


	srv->m_nDelay = 0;
	if ( argc >= 5 ) {
		srv->m_nDelay = atoi ( argv [ 4 ] );
	}

	int nThreadCount = 1;
	if ( argc >= 6 ) {
		nThreadCount = atoi ( argv [ 5 ] );
	}

	
	printf ( "Delay set to %d seconds\n", srv->m_nDelay );

	if ( nMode == 2 ) {
		printf ( "Starting %d threads\n", nThreadCount );
	}

	EHSServerParameters oSP;
	oSP["port"] = argv [ 2 ];

	if ( argc >= 7 ) {
		oSP [ "norouterequest" ] = 1;
	}

	if ( nMode == 1 ) {
		printf ( "Running in single threaded mode\n" );
		oSP [ "mode" ] = "singlethreaded";
	} else if ( nMode == 2 ) {
		printf ( "Running with a thread pool of %d threads\n", nThreadCount );
		oSP [ "mode" ] = "threadpool";
		oSP["threadcount"] = nThreadCount;
	} else if ( nMode == 3 ) {
		printf ( "Running with one thread per request\n" );
		oSP [ "mode" ] = "onethreadperrequest";
	} else {
		printf ( "Invalid mode specified: must be 1, 2, or 3\n" );
		exit ( 0 );
	}

	srv->StartServer ( oSP );

	tester a ( argv [ 3 ] );
	tester b ( argv [ 3 ] );

	srv->RegisterEHS ( &a, "a" );
	srv->RegisterEHS ( &b, "b" );

	tester aa ( argv [ 3 ] );
	a.RegisterEHS ( &aa, "a" );

	tester ab ( argv [ 3 ] );
	a.RegisterEHS ( &ab, "b" );
	
	while ( 1 ) {
		// normally your program would be doing useful things here...
		sleep ( 1 );
		
		// if in single threaded mode, must handle data explicitly
		if ( nMode == 1 ) {
			srv->HandleData ( );
		}

	}

	srv->StopServer ( );

	return 0;

}
