
#include <stdio.h>
#include <stdlib.h>

#include "../ehs.h"


class MyEHS : public EHS { 
	
	ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest,
								 HttpResponse * ipoHttpResponse ) {

		char psBuffer [ 100 ];

		sprintf ( psBuffer, 
				  "ehs_mirror: Secure - %s %s:%d",
				  ipoHttpRequest->nSecure ? "yes" : "no",
				  ipoHttpRequest->GetAddress().c_str ( ),
				  ipoHttpRequest->GetPort()
				  );

		ipoHttpResponse->SetBody ( psBuffer, strlen ( psBuffer ) );
		return HTTPRESPONSECODE_200_OK;
	}

};

int main ( int argc, char ** argv )
{

	if ( argc != 2 ) {
		printf ( "Usage: %s <port>\n", argv [ 0 ] );
		exit ( 0 );
	}

	EHS * poMyEHS = new MyEHS;

	EHSServerParameters oSP;
	oSP [ "port" ] = argv [ 1 ];
	oSP [ "mode" ] = "threadpool";

	// unnecessary because 1 is the default
	oSP [ "threaded" ] = 1;

	poMyEHS->StartServer ( oSP );



	// create a default EHS object, but set poMyEHS as its source EHS object
	EHS * poEHS = new EHS;
	oSP [ "port" ] = atoi ( argv [ 1 ] ) + 1;
	oSP [ "https" ] = 1;
	oSP [ "mode" ] = "threadpool";
	oSP [ "threadcount" ] = 1;
	oSP [ "passphrase" ] = "comoesta";
	oSP [ "certificate" ] = "/home/xaxxon/certificates/server.pem";

	poEHS->SetSourceEHS ( *poMyEHS );
	poEHS->StartServer ( oSP );


	while ( 1 ) {
		sleep ( 1 );
	}

}
