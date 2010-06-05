
#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H


#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <map>

#include <pme.h>

#include "ehs.h"
#include "ehstypes.h"
#include "formvalue.h"


/// UNKNOWN must be the last one, as these must match up with RequestMethodStrings *exactly*
enum RequestMethod { REQUESTMETHOD_OPTIONS, /* not implemented */
					 REQUESTMETHOD_GET,
					 REQUESTMETHOD_HEAD,
					 REQUESTMETHOD_POST,
					 REQUESTMETHOD_PUT, ///< not implemented
					 REQUESTMETHOD_DELETE, ///< not implemented
					 REQUESTMETHOD_TRACE, ///< not implemented
					 REQUESTMETHOD_CONNECT, ///< not implemented
					 REQUESTMETHOD_LAST, ///< must be the last valid entry
					 REQUESTMETHOD_UNKNOWN, ///< used until we find the method
					 REQUESTMETHOD_INVALID ///< must be the last entry
};

/// this holds a list of strings corresponding to the order of the RequestMethod enumeration
extern char * RequestMethodStrings [];

/// This is what the client sent in a more organized form
/**
 * This is what the client sent in a more organized form.  It has any form
 * data,  header information, the request type
 */
class HttpRequest {

  public:

	/// constructor
	HttpRequest ( int inRequestId,
				  EHSConnection * ipoSourceEHSConnection );

	/// destructor
	virtual ~HttpRequest ( );

	/// Enumeration for the state of the current HTTP parsing
	enum HttpParseStates { HTTPPARSESTATE_INVALID = 0,
						   HTTPPARSESTATE_REQUEST,
						   HTTPPARSESTATE_HEADERS,
						   HTTPPARSESTATE_BODY,
						   HTTPPARSESTATE_COMPLETEREQUEST,
						   HTTPPARSESTATE_INVALIDREQUEST,
};

	/// Enumeration of error codes for ParseMultipartFormDataResult
	enum ParseMultipartFormDataResult { 
		PARSEMULTIPARTFORMDATA_INVALID = 0,
		PARSEMULTIPARTFORMDATA_SUCCESS,
		PARSEMULTIPARTFORMDATA_FAILED 
	};
	
	/// treats the body as if it's a multipart form data as specified in RFC not sure what the number is and I'll probably forget to look it up
	ParseMultipartFormDataResult ParseMultipartFormData ( );

	/// Enumeration of error codes for ParseSubbody
	enum ParseSubbodyResult {
		PARSESUBBODY_INVALID = 0,
		PARSESUBBODY_SUCCESS,
		PARSESUBBODY_INVALIDSUBBODY, // no blank line?
		PARSESUBBODY_FAILED // other reason

	};

	/// goes through a subbody and parses out elements
	ParseSubbodyResult ParseSubbody ( std::string sSubBody );


	/// this function is given data that is read from the client and it deals with it
	HttpParseStates ParseData ( std::string & irsData );

	/// takes the cookie header and breaks it down into usable chunks -- returns number of name/value pairs found
	int ParseCookieData ( std::string & irsData );

	/// interprets the given string as if it's name=value pairs and puts them into oFormElements
	void GetFormDataFromString ( const std::string & irsString );

	/// the current parse state -- where we are in looking at the data from the client
	HttpParseStates nCurrentHttpParseState;

	/// this is the request method from the client
	RequestMethod nRequestMethod;
	
	/// the clients requested URI
	std::string sUri;

	/// holds the original requested URI, not changed by any path routing
	std::string sOriginalUri;


	/// the HTTP version of the request
	std::string sHttpVersionNumber;

	/// Binary data, not NULL terminated
	std::string sBody; 

	/// whether or not this came over secure channels
	int nSecure;

	/// headers from the client request
	StringMap oRequestHeaders;

	/// Data specified by the client.  The 'name' field is mapped to a FormValue object which has the value and any metadata
	FormValueMap oFormValueMap;

	/// cookies that come in from the client
	CookieMap oCookieMap;

	/// request id for this connection
	int m_nRequestId;

	/// connection object from which this request came
	EHSConnection * m_poSourceEHSConnection;

	/// returns the address this request came from
	std::string GetAddress ( );
	
	/// returns the port this request came from
	int GetPort ( );

	/// returns true if the client is disconnected
	int ClientDisconnected ( );

};


// HELPER FUNCTIONS

/// removes the next line from irsBuffer and returns it in irsLine
int GetNextLine ( std::string & irsLine, std::string & irsBuffer );

/// gets the RequestMethod enumeration based on isRequestMethod
RequestMethod GetRequestMethodFromString ( const std::string & isRequestMethod );

/// makes the string lowercase
std::string & mytolower (std::string & s );


#endif // HTTPREQUEST_H
