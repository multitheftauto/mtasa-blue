
#include "httpresponse.h"




const char * DaysOfWeek [] = { 
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char * Months [] = {

	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"

};

char * CreateHttpTime ( )
{
	// 30 is a magic number that is the length of the http
	//   time format + 1 for NULL terminator
	
	time_t nTime = time ( NULL );
	char * psTime = new char [ 30 ];
	tm * oTm = gmtime ( &nTime ); // get the time info
	sprintf ( psTime, "%s, %02d %s %04d %02d:%02d:%02d GMT",
			  DaysOfWeek [ oTm->tm_wday ],
			  oTm->tm_mday,
			  Months [ oTm->tm_mon ],
			  oTm->tm_year + 1900,
			  oTm->tm_hour,
			  oTm->tm_min,
			  oTm->tm_sec );
			  
	return psTime;

}




////////////////////////////////////////////////////////////////////
//  HTTP RESPONSE
//
////////////////////////////////////////////////////////////////////

HttpResponse::HttpResponse ( int inResponseId,
							 EHSConnection * ipoEHSConnection ) :
	m_nResponseCode ( HTTPRESPONSECODE_INVALID ),	
	psBody ( NULL ),
	nBodyLength ( -1 ),
	m_nResponseId ( inResponseId ),
	m_poEHSConnection ( ipoEHSConnection )
	
{
#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Allocated: HttpResponse\n" );
#endif   

	char * psHttpTime = CreateHttpTime ( );

	// General Header Fields (HTTP 1.1 Section 4.5)
	oResponseHeaders [ "date" ] = psHttpTime;
	oResponseHeaders [ "cache-control" ] = "no-cache";


	oResponseHeaders [ "last-modified" ] = psHttpTime;
	oResponseHeaders [ "content-type" ] = "text/html";

	delete [] psHttpTime;

}



HttpResponse::~HttpResponse ( )
{

#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Deallocated: HttpResponse\n" );
#endif

	delete [] psBody;

}




// sets informatino regarding the body of the HTTP response
//   sent back to the client
void HttpResponse::SetBody ( const char * ipsBody, ///< body to return to user
							 int inBodyLength ///< length of the body
	)
							 
{

	assert ( psBody == NULL );
	
	psBody = new char [ inBodyLength + 1 ];
	assert ( psBody != NULL );
	memset ( psBody, 0, inBodyLength + 1 );
	memcpy ( psBody, ipsBody, inBodyLength );

	
	char psContentLength [ 100 ];
	sprintf ( psContentLength, "%d", inBodyLength );

	oResponseHeaders [ "content-length" ] = psContentLength;

}


// this will send stuff if it's not valid.. 
void HttpResponse::SetCookie ( CookieParameters & iroCookieParameters )
{

	// name and value must be set
	if ( iroCookieParameters [ "name" ] != "" &&
		 iroCookieParameters [ "value" ] != "" ) {

		std::stringstream ssBuffer;

		ssBuffer << iroCookieParameters["name"].GetCharString ( ) <<
			"=" << iroCookieParameters["value"].GetCharString ( );

		// Version should have capital V according to RFC 2109
		if ( iroCookieParameters [ "Version" ] == "" ) {
			iroCookieParameters [ "Version" ] = "1";
		}

		for ( CookieParameters::iterator i = iroCookieParameters.begin ( );
			  i != iroCookieParameters.end ( );
			  i++ ) {

			if ( (*i).first != "name" &&
				 (*i).first != "value" ) {

				ssBuffer << "; " << (*i).first.c_str ( ) << 
					"=" << (*i).second.GetCharString ( );

			}

		}

		oCookieList.push_back ( ssBuffer.str ( ) );

	} else {

#ifdef EHS_DEBUG
		fprintf ( stderr, "Cookie set with insufficient data -- requires name and value\n" );
#endif
	}

}
