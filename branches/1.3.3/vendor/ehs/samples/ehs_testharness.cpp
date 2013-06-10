
#include "../ehs.h"



// subclass of EHS that defines a custom HTTP response.
class TestHarness : public EHS
{

	virtual ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest,
										 HttpResponse * ipoHttpResponse );

};




// generates a page for each http request
ResponseCode TestHarness::HandleRequest ( HttpRequest * ipoHttpRequest,
										  HttpResponse * ipoHttpResponse )
{

	char psBody [ 1000000 ];

	sprintf ( psBody,
			  "request-method: %d\n"
			  "uri: %s\n"
			  "http-version: %s\n"
			  "body-length: %d\n"
			  "number-request-headers: %d\n"
			  "number-form-value-maps: %d\n"
			  "client-address: %s\n"
			  "client-port: %d\n",
			  ipoHttpRequest->nRequestMethod,
			  ipoHttpRequest->sUri.c_str ( ),
			  ipoHttpRequest->sHttpVersionNumber.c_str ( ),
			  ipoHttpRequest->sBody.length ( ),
			  ipoHttpRequest->oRequestHeaders.size ( ),
			  ipoHttpRequest->oFormValueMap.size ( ),
			  ipoHttpRequest->GetAddress ( ).c_str ( ),
			  ipoHttpRequest->GetPort ( )
		);

	char psBuffer [ 1000 ];
	for ( StringMap::iterator i = ipoHttpRequest->oRequestHeaders.begin ( );
		  i != ipoHttpRequest->oRequestHeaders.end ( );
		  i++ ) {

		sprintf ( psBuffer, "Request Header: %s => %s\n",
				  (*i).first.c_str ( ),
				  (*i).second.c_str ( ) );
		strcat ( psBody, psBuffer );

	}

	for ( CookieMap::iterator i = ipoHttpRequest->oCookieMap.begin ( );
		  i != ipoHttpRequest->oCookieMap.end ( );
		  i++ ) {
		sprintf ( psBuffer, "Cookie: %s => %s\n", 
				  (*i).first.c_str ( ),
				  (*i).second.c_str ( ) );
		strcat ( psBody, psBuffer );
	}
			  
	ipoHttpResponse->SetBody ( psBody, strlen ( psBody ) );

	return HTTPRESPONSECODE_200_OK;

}


// basic main that creates a threaded EHS object and then
//   sleeps forever and lets the EHS thread do its job.
int main ( int argc, char ** argv )
{

	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s [port]\n", argv[0] );
		exit ( 0 );
	}

	fprintf ( stderr, "binding to %d\n", atoi ( argv [ 1 ] ) );
	TestHarness * srv = new TestHarness;

	EHSServerParameters oSP;
	oSP [ "port" ] = argv [ 1 ];
	oSP [ "mode" ] = "threadpool";

	// unnecessary because 1 is the default
	oSP [ "threadcount" ] = 1;

	srv->StartServer ( oSP );

	while ( 1 ) {
		// normally your program would be doing useful things here...
		sleep ( 1 );
	}

	srv->StopServer ( );

	return 0;

}
