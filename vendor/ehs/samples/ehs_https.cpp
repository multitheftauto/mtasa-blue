
#include <stdio.h>
#include <stdlib.h>

#include "../ehs.h"

int main ( int argc, char ** argv )
{

	if ( argc != 4 ) {
		printf ( "Usage: %s <port> <certificate file> <passphrase>\n", 
				 argv [ 0 ] );
		exit ( 0 );
	}

	EHS * poEHS = new EHS;

	EHSServerParameters oSP;
	oSP [ "port" ] = argv [ 1 ];
	oSP [ "https" ] = 1;
	oSP [ "certificate" ] = argv [ 2 ];
	oSP [ "passphrase" ] = argv [ 3 ];
	oSP [ "mode" ] = "threadpool";

	// unnecessary because 1 is the default
	oSP [ "threadcount" ] = 1;

	poEHS->StartServer ( oSP );

	while ( 1 ) {
		sleep ( 1 );
	}

}
