
#include "httprequest.h"
#include "stats.h"

#include <assert.h>

void HttpRequest::ParseRequestURI(std::string_view uri)
{
	if (uri.empty())
		return;

	// See: https://www.rfc-editor.org/rfc/rfc3986#section-3.4
	if (size_t queryDelimiter = uri.find_first_of("?&"); queryDelimiter != std::string_view::npos)
	{
		// Skip repeating '?' (question mark) and '&' (ampersand) characters after the first one.
		if (queryDelimiter = uri.find_first_not_of("?&", queryDelimiter + 1); queryDelimiter == std::string_view::npos)
			return;

		std::string_view parameters = uri.substr(queryDelimiter);

		// Discard any trailing fragment.
		if (size_t fragmentDelimiter = parameters.find('#'); fragmentDelimiter != std::string_view::npos)
		{
			parameters = parameters.substr(0, fragmentDelimiter);
		}

		// Limit request uri parameters to 4096 bytes.
		if (parameters.length() > 4096)
		{
			parameters = parameters.substr(0, 4096);
		}

		// According to the RFC, query can be anything, but this web server implementation only supports "key=value" pairs,
		// which are parsable as form data.
		ParseFormData(parameters, true);
	}
}

void HttpRequest::ParseFormData(std::string_view formData, bool isQueryData)
{
	size_t counter = 0;

	// Limit the maximum acceptable form data fields to 256.
	while (!formData.empty() && counter < 256)
	{
		std::string_view parameter;

		if (size_t delimiter = formData.find('&'); delimiter != std::string_view::npos)
		{
			parameter = formData.substr(0, delimiter);

			// Skip repeating '&' (ampersand) characters after the first one.
			if (delimiter = formData.find_first_not_of('&', delimiter + 1); delimiter != std::string_view::npos)
			{
				formData = formData.substr(delimiter);
			}
			else
			{
				formData = {};
			}
		}
		else
		{
			parameter = formData;
			formData = {};
		}

		if (!parameter.empty())
		{
			std::string_view key, value;

			if (size_t delimiter = parameter.find('='); delimiter != std::string_view::npos)
			{
				key = parameter.substr(0, delimiter);

				// Skip repeating '=' (equals) characters after the first one.
				if (delimiter = parameter.find_first_not_of('=', delimiter + 1); delimiter != std::string_view::npos)
				{
					value = parameter.substr(delimiter);
				}
			}
			else
			{
				key = parameter;
				// NOTE: value is empty.
			}

			if (!key.empty())
			{
				oFormValueMap[std::string{key}] = FormValue{value};

				if (isQueryData)
				{
					oQueryValueMap[std::string{key}] = FormValue{value};
				}
			}
		}

		counter += 1;
	}
}

// this parses a single piece of a multipart form body
// here are two examples -- first is an uploaded file, second is a
//   standard text box html form
/*
  -----------------------------7d2248207500d4
  Content-Disposition: form-data; name="DFWFilename2"; filename="C:\Documents and Settings\administrator\Desktop\contacts.dat"
  Content-Type: application/octet-stream

  Default Screen Name
  -----------------------------7d2248207500d4


  -----------------------------7d2248207500d4
  Content-Disposition: form-data; name="foo"

  asdfasdf
  -----------------------------7d2248207500d4
*/


HttpRequest::ParseSubbodyResult HttpRequest::ParseSubbody ( std::string isSubbody ///< string in which to look for subbody stuff
	)
{

	// find the spot after the headers in the body
	std::string::size_type nBlankLinePosition = isSubbody.find ( "\r\n\r\n" );
	
	// if there's no double blank line, then this isn't a valid subbody
	if ( nBlankLinePosition == std::string::npos ) {

#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEUBG] Invalix subbody, couldn't find double blank line\n" );
#endif
		return PARSESUBBODY_INVALIDSUBBODY;

	}

	// create a string from the beginning to the blank line -- OFF BY ONE?
	std::string sHeaders ( isSubbody, 0, nBlankLinePosition - 1 );


	// first line MUST be the content-disposition header line, so that
	//   we know what the name of the field is.. otherwise, we're in trouble



	int nMatchResult = 0;
	PME oContentDispositionRegex ( "Content-Disposition:[ ]?([^;]+);[ ]?(.*)" );

	nMatchResult = oContentDispositionRegex.match ( sHeaders );


	if ( nMatchResult == 3 ) {
				

		std::string sContentDisposition ( oContentDispositionRegex [ 1 ] );
		std::string sNameValuePairs ( oContentDispositionRegex [ 2 ] );

/*		fprintf ( stderr, "cont. disp. '%s', name/value pairs '%s'\n",
				  sContentDisposition.c_str ( ),
				  sNameValuePairs.c_str ( ) );
*/
		StringMap oStringMap;
		


		PME oContentDispositionNameValueRegex ( "[ ]?([^= ]+)=\"([^\"]+)\"[;]?", "g" );
			

		// go through the sNameValuePairs string and grab out the pieces
		nMatchResult = 3;

		while ( nMatchResult == 3 ) {

			nMatchResult = oContentDispositionNameValueRegex.match ( sNameValuePairs );
			if ( nMatchResult == 3 ) {

				std::string sName = oContentDispositionNameValueRegex [ 1 ];
				std::string sValue = oContentDispositionNameValueRegex [ 2 ];

#ifdef EHS_DEBUG
				fprintf ( stderr, "[EHS_DEBUG] Info: Subbody header found: '%s' => '%s' with %d matches\n",
						  sName.c_str ( ),
						  sValue.c_str ( ),
						  nMatchResult );
#endif				
				
				oStringMap [ sName ] = sValue;
				
			} else if ( nMatchResult == 0 ) {
				; // this is okay -- just done with name value pairs
			} else {
				//fprintf ( stderr, "Unexpected number of matches, '%d' != 3\n", nMatchResult );
				assert ( 0 );				
			}

			//fprintf ( stderr, "nMatchResult = %d\n", nMatchResult );
			
			
		}

//		fprintf ( stderr, "done looking for headers\n" );
		
		// take oStringMap and actually fill the right object with its data
		FormValue & roFormValue = oFormValueMap [ oStringMap [ "name" ] ];
		
		// copy the headers in now that we know the name
		roFormValue.oContentDisposition.oContentDispositionHeaders = oStringMap;
		
		// grab out the body
		roFormValue.sBody = isSubbody.substr ( nBlankLinePosition + 4);

#ifdef EHS_DEBUG		
		fprintf ( stderr, "[EHS_DEBUG] Info: Subbody body (in binary):\n---\n" );
		fwrite ( roFormValue.sBody.c_str ( ), 1, roFormValue.sBody.length ( ), stderr );
		fprintf ( stderr, "\n---\n" );
#endif
		
	}
	// couldn't find content-disposition line -- FATAL ERROR
	else {
		
#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Error: Couldn't find content-disposition line\n" );
#endif
		return PARSESUBBODY_INVALIDSUBBODY;
		
	}
	
	
	
	
	return PARSESUBBODY_SUCCESS;
	
}





HttpRequest::ParseMultipartFormDataResult
HttpRequest::ParseMultipartFormData ( )
{
	
	assert ( !oRequestHeaders [ "content-type" ].empty ( ) );
	

	// find the boundary string
	int nMatchResult = 0;

	PME oMultipartFormDataContentTypeValueRegex ( "multipart/[^;]+;[ ]*boundary=([^\"]+)$" );

#ifdef EHS_DEBUG
	fprintf ( stderr, "looking for boundary in '%s'\n",oRequestHeaders [ "content-type" ].c_str ( ) );
#endif	

	if ( ( nMatchResult = oMultipartFormDataContentTypeValueRegex.match ( oRequestHeaders [ "content-type" ] ) ) ) {

		
		assert ( nMatchResult == 2 );

		std::string sBoundaryString = oMultipartFormDataContentTypeValueRegex [ 1 ];


		// the actual boundary has two dashes prepended to it
		std::string sActualBoundary = std::string ("--") + sBoundaryString;

#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Info: Found boundary of '%s'\n", sBoundaryString.c_str ( ) );
		fprintf ( stderr, "[EHS_DEBUG] Info: Looking for boundary to match (%d) '%s'\n", sBody.length ( ), 
				  sBody.substr ( 0, sActualBoundary.length ( ) ).c_str ( ) );
#endif


		// check to make sure we started at the boundary
		if ( sBody.substr ( 0, sActualBoundary.length ( ) ) != sActualBoundary ) {
#ifdef EHS_DEBUG
			fprintf ( stderr, "[EHS_DEBUG] Error: Misparsed multi-part form data for unknown reason - first bytes weren't the boundary string\n" );
#endif			
			return PARSEMULTIPARTFORMDATA_FAILED;

		}


		// go past the initial boundary
		std::string sRemainingBody = sBody.substr ( sActualBoundary.length ( ) );

		// while we're at a boundary after we grab a part, keep going
		std::string::size_type nNextPartPosition;


		while ( ( nNextPartPosition = sRemainingBody.find ( std::string ( "\r\n" ) + sActualBoundary ) ) != 
				std::string::npos ) {


#ifdef EHS_DEBUG
			fprintf ( stderr, "[EHS_DEBUG] Info: Found subbody from pos %d to %d\n",
					  0, nNextPartPosition );
#endif

			assert ( (unsigned int) sRemainingBody.length ( ) >= sActualBoundary.length ( ) );
			
			ParseSubbody ( sRemainingBody.substr ( 0, nNextPartPosition ) );
			
			// skip past the boundary at the end and look for the next one
			nNextPartPosition += sActualBoundary.length ( );

			sRemainingBody = sRemainingBody.substr ( nNextPartPosition );

		}
			
	} else {

#ifdef EHS_DEBUG
		fprintf ( stderr, "[EHS_DEBUG] Error: Couldn't find boundary specification in content-type header\n" );
#endif
		return PARSEMULTIPARTFORMDATA_FAILED;
	}

	return PARSEMULTIPARTFORMDATA_SUCCESS;
	
}

// A cookie header looks like: Cookie: username=xaxxon; password=comoesta
//   everything after the : gets passed in in irsData
int HttpRequest::ParseCookieData ( std::string & irsData )
{

#ifdef EHS_DEBUG
	fprintf ( stderr, "looking for cookies in '%s'\n", irsData.c_str ( ) );
#endif
	
	PME oCookieRegex ( "\\s*([^=]+)=([^;]+)(;|$)*", "g" );

	int nNameValuePairsFound = 0;

	while ( oCookieRegex.match ( irsData ) ) {

/*		fprintf ( stderr, "mapping cookie data: %s => %s\n",
		oCookieRegex [ 1 ].c_str ( ),
		oCookieRegex [ 2 ].c_str ( ) );
*/
		oCookieMap [ oCookieRegex [ 1 ] ] = oCookieRegex [ 2 ];
		nNameValuePairsFound++;

	}

	return nNameValuePairsFound;

}



// takes data and tries to figure out what it is.  it will loop through 
//   irsData as many times as it can as long as it gets a full line each time.
//   Depending on how much data it's gotten already, it will handle a line 
//   differently.. 
HttpRequest::HttpParseStates HttpRequest::ParseData ( std::string & irsData ///< buffer to look in for more data
	)
{


	std::string sLine;

	int nDoneWithCurrentData = 0;

	PME oHeaderRegex ( "^([^:]*):\\s+(.*)\\r\\n$" );

	while ( ! nDoneWithCurrentData && 
			nCurrentHttpParseState != HTTPPARSESTATE_INVALIDREQUEST &&
			nCurrentHttpParseState != HTTPPARSESTATE_COMPLETEREQUEST &&
			nCurrentHttpParseState != HTTPPARSESTATE_INVALID ) {

		switch ( nCurrentHttpParseState ) {
			
		case HTTPPARSESTATE_REQUEST:
			
			// get the request line
			GetNextLine ( sLine, irsData );
			
			// if we got a line, parse out the data..
			if ( sLine.length ( ) == 0 ) {

				nDoneWithCurrentData = 1;

			} 
			// if we got a line, look for a request line
			else {
				
				// everything must be uppercase according to rfc2616
				PME oRequestLineRegex ( "^(OPTIONS|GET|HEAD|POST|PUT|DELETE|TRACE|CONNECT|PATCH) ([^ ]*) HTTP/([^ ]+)\\r\\n$" );

				if ( oRequestLineRegex.match ( sLine ) ) {

					// get the info from the request line
					nRequestMethod = GetRequestMethodFromString ( oRequestLineRegex [ 1 ] );
					sUri = oRequestLineRegex [ 2 ];
					sOriginalUri = oRequestLineRegex [ 2 ];
					sHttpVersionNumber = oRequestLineRegex [ 3 ];
					
					
					// check to see if the uri appeared to have form data in it
					ParseRequestURI(sUri);

					// on to the headers
					nCurrentHttpParseState = HTTPPARSESTATE_HEADERS;
					
				} 
				// if the regex failed
				else {

					nCurrentHttpParseState = HTTPPARSESTATE_INVALIDREQUEST;

				} // end match on request line
			
			} // end whether we got a line
			
			break;
			
		case HTTPPARSESTATE_HEADERS:
			
			// get the next line
			GetNextLine ( sLine, irsData );

			// if we didn't get a full line of data
			if ( sLine.length ( ) == 0 ) {

				nDoneWithCurrentData = 1;

			}
			// check to see if we're done with headers
			else if ( sLine == "\r\n" ) {

				// if content length is found
				if ( oRequestHeaders.find ( "content-length" ) !=
					 oRequestHeaders.end ( ) ) {
					
					nCurrentHttpParseState = HTTPPARSESTATE_BODY;
				
				} else {

					
					nCurrentHttpParseState = HTTPPARSESTATE_COMPLETEREQUEST;

				}

				// if this is an HTTP/1.1 request, then it had better have a Host: header
				if ( sHttpVersionNumber == "1.1" && 
					 oRequestHeaders [ "host" ].length ( ) == 0 ) {

					nCurrentHttpParseState = HTTPPARSESTATE_INVALIDREQUEST;

				}

			}
			// else if there is still data
			else if ( oHeaderRegex.match ( sLine ) ) {

				std::string sName = oHeaderRegex [ 1 ];
				std::string sValue = oHeaderRegex [ 2 ];

				if ( sName == "Transfer-Encoding" &&
					 sValue == "chunked" ) {
					
#ifdef EHS_DEBUG
					fprintf ( stderr, "EHS DOES NOT SUPPORT CHUNKED ENCODING.  Send an email to xaxxon@slackworks.com and tell him you want chunked encoding (or send a patch)\n" );
#endif
					nCurrentHttpParseState = HTTPPARSESTATE_INVALIDREQUEST;

				}

				sName = mytolower ( sName );

				if ( sName == "cookie" ) {

					ParseCookieData ( sValue );

				}

				oRequestHeaders [ sName ] = sValue;

				
				
			}
			// else we had some sort of error -- bail out
			else {

#ifdef EHS_DEBUG
				fprintf ( stderr, "[EHS_DEBUG] Error: Invalid header line: '%s'\n",
						  sLine.c_str ( ) );
#endif

				nCurrentHttpParseState = HTTPPARSESTATE_INVALIDREQUEST;
				nDoneWithCurrentData = 1;

			}

			break;

			
		case HTTPPARSESTATE_BODY:
		{

			// if a content length wasn't specified, we can't be here (we 
			//   don't know chunked encoding)
			if ( oRequestHeaders.find ( "content-length" ) == 
				 oRequestHeaders.end ( ) ) {

				nCurrentHttpParseState = HTTPPARSESTATE_INVALIDREQUEST;
				continue;

			}

			// get the content length
			unsigned int nContentLength = 
				atoi ( oRequestHeaders [ "content-length" ].c_str ( ) );

			// else if we haven't gotten all the data we're looking for,
			//   just hold off and try again when we get more
			if ( irsData.length ( ) < nContentLength ) {

#ifdef EHS_DEBUG
				fprintf ( stderr, "[EHS_DEBUG] Info: Not enough data yet -- %d < %d\n",
						  irsData.length ( ), nContentLength );
#endif
				nDoneWithCurrentData = 1;

			}
			// otherwise, we've gotten enough data from the client, handle it now
			else {

				// grab out the actual body from the request and leave the rest
				sBody = irsData.substr ( 0, nContentLength );

				irsData = irsData.substr ( nContentLength );

				
				// if we're dealing with multi-part form attachments
				if ( oRequestHeaders [ "content-type" ].substr ( 0, 9 ) == 
					 "multipart" ) {

					// handle the body as if it's multipart form data
					if ( ParseMultipartFormData ( ) == 
						 PARSEMULTIPARTFORMDATA_SUCCESS ) {

						nCurrentHttpParseState = HTTPPARSESTATE_COMPLETEREQUEST;

					} else {

#ifdef EHS_DEBUG						
						fprintf ( stderr, "[EHS_DEBUG] Error: Mishandled multi-part form data for unknown reason\n" );
#endif
						nCurrentHttpParseState = HTTPPARSESTATE_INVALIDREQUEST;

					}
	

				}
				// else the body is just one piece
				else {
					// check for any form data
					ParseFormData(sBody, false);
					
#ifdef EHS_DEBUG
					fprintf ( stderr, "Done with body, done with entire request\n" );
#endif
					
					nCurrentHttpParseState = HTTPPARSESTATE_COMPLETEREQUEST;
				}
				
			}
			
			nDoneWithCurrentData = 1;

		}

		break;


		case HTTPPARSESTATE_INVALID:
		default:
#ifdef EHS_DEBUG
			fprintf ( stderr, "[EHS_DEBUG] Critical error: Invalid internal state: %d.  Aborting\n", nCurrentHttpParseState );
#endif
			assert ( 0 );
			break;
		}
	}

	return nCurrentHttpParseState;
	
}





////////////////////////////////////////////////////////////////////
//  HTTP REQUEST
//
////////////////////////////////////////////////////////////////////


HttpRequest::HttpRequest ( int inRequestId,
						   EHSConnection * ipoSourceEHSConnection ) :
	nCurrentHttpParseState  ( HTTPPARSESTATE_REQUEST ),
	nRequestMethod ( REQUESTMETHOD_UNKNOWN ),
	sUri ( "" ),
	sHttpVersionNumber ( "" ),
	m_nRequestId ( inRequestId ),
	m_poSourceEHSConnection ( ipoSourceEHSConnection )
{
    StatsNumRequestsInc();

#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Allocated: HttpRequest\n" );
#endif
	if ( m_poSourceEHSConnection == NULL ) {
#ifdef EHS_DEBUG
		fprintf ( stderr, "Not allowed to have null source connection\n" );
#endif
		exit ( 2 );
	}
}

HttpRequest::~HttpRequest ( )
{
#ifdef EHS_MEMORY
	fprintf ( stderr, "[EHS_MEMORY] Deallocated: HttpRequest\n" );
#endif
    StatsNumRequestsDec();
}


// HELPER FUNCTIONS

// Takes a char* buffer and grabs a line off it, puts the new line in irsLine
//   and shrinks the buffer by the size of the line and sets ipnBufferLength to 
//   the new size

// returns 1 if it got a line, 0 otherwise
int GetNextLine ( std::string & irsLine, ///< line removed from *ippsBuffer
				  std::string & irsBuffer ///< buffer from which to remove a line
	)
{

	int nResult = 0;

	// can't use split, because we lose our \r\n
	PME oLineRegex ( "^([^\\r]*\\r\\n)(.*)$", "sm" );

	if ( oLineRegex.match ( irsBuffer ) == 3 ) {

		irsLine = oLineRegex [ 1 ];
		irsBuffer = oLineRegex [ 2 ];
		
	} else {

		irsLine = "";

	}

	return nResult;

}


/// List of possible HTTP request methods
const char * RequestMethodStrings [] = { "OPTIONS", "GET", "HEAD", "POST", 
								   "PUT", "DELETE", "TRACE", "CONNECT", "PATCH", "*"};


RequestMethod GetRequestMethodFromString ( const std::string & isRequestMethod  ///< determine the request type enumeration from the request string
	)
{

	int i = 0;

	for ( i = 0; i < REQUESTMETHOD_LAST; i++ ) {

		if ( isRequestMethod == RequestMethodStrings [ i ] ) {

			break;

		}

	}

	return ( RequestMethod ) i;

}

std::string & mytolower (std::string & s ///< string to make lowercase
	) 
{ 
	std::transform ( s.begin(), s.end(), s.begin(), [](char c) { return std::tolower(c); });
	return s;

}


std::string HttpRequest::GetAddress ( )
{

	return m_poSourceEHSConnection->GetAddress ( );

}
	
int HttpRequest::GetPort ( )
{

	return m_poSourceEHSConnection->GetPort ( );

}


int HttpRequest::ClientDisconnected ( )
{ 
	return m_poSourceEHSConnection->Disconnected ( ); 
}
