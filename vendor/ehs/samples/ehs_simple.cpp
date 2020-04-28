
#include <stdio.h>
#include <stdlib.h>

#include "../ehs.h"

int main ( int argc, char ** argv )
{

	if ( argc != 2 &&
		 argc != 3 ) {
		printf ( "Usage: %s <port> <threaded:0/1)>\n", argv [ 0 ] );
		exit ( 0 );
	}

	EHS * poEHS = new EHS;

	EHSServerParameters oSP;
	oSP [ "port" ] = argv [ 1 ];

	int nThreaded = 0;
	if ( argc == 3 ) {
		nThreaded = atoi ( argv [ 2 ] );
	}


	// strt in thread pool mode
	if ( nThreaded ) {
		printf ( "Starting in threaded mode\n" );
		oSP [ "mode" ] = "threadpool";
		
		// unnecessary because 1 is the default
		oSP [ "threadcount" ] = 2;
		
		poEHS->StartServer ( oSP );
		
		while ( 1 ) {
			sleep ( 1 );
		}
		
	} 
	// start in single threaded mode
	else {
		printf ( "Starting in single threaded mode\n" );
		oSP [ "mode" ] = "singlethreaded";
		
		// not threaded
		poEHS->StartServer ( oSP );
		
		
		while ( 1 ) {
			poEHS->HandleData ( 1000 ); // waits for 1 second
		}
		
	}

}
