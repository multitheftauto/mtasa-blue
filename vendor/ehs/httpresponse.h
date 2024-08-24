
#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#ifdef _WIN32
#include <time.h>
#pragma warning(disable : 4786)
#endif

#include <map>
#include <list>
#include <string>
#include <sstream>
#include <string.h>

#include "ehstypes.h"
#include "datum.h"

enum HttpStatusCode
{
	HTTP_STATUS_CODE_INVALID = 0,
	HTTP_STATUS_CODE_100_CONTINUE = 100,                            ///< Continue
	HTTP_STATUS_CODE_101_SWITCHING_PROTOCOLS = 101,                 ///< Switching Protocols
	HTTP_STATUS_CODE_200_OK = 200,                                  ///< OK
	HTTP_STATUS_CODE_201_CREATED = 201,                             ///< Created
	HTTP_STATUS_CODE_202_ACCEPTED = 202,                            ///< Accepted
	HTTP_STATUS_CODE_203_NON_AUTHORITATIVE_INFO = 203,              ///< Non-Authoritative Information
	HTTP_STATUS_CODE_204_NO_CONTENT = 204,                          ///< No Content
	HTTP_STATUS_CODE_205_RESET_CONTENT = 205,                       ///< Reset Content
	HTTP_STATUS_CODE_206_PARTIAL_CONTENT = 206,                     ///< Partial Content
	HTTP_STATUS_CODE_300_MULTIPLE_CHOICES = 300,                    ///< Multiple Choices
	HTTP_STATUS_CODE_301_MOVED_PERMANENTLY = 301,                   ///< Moved Permanently
	HTTP_STATUS_CODE_302_FOUND = 302,                               ///< Found
	HTTP_STATUS_CODE_303_SEE_OTHER = 303,                           ///< See Other
	HTTP_STATUS_CODE_304_NOT_MODIFIED = 304,                        ///< Not Modified
	HTTP_STATUS_CODE_305_USE_PROXY = 305,                           ///< Use Proxy
	HTTP_STATUS_CODE_307_TEMPORARY_REDIRECT = 307,                  ///< Temporary Redirect
	HTTP_STATUS_CODE_400_BAD_REQUEST = 400,                         ///< Bad Request
	HTTP_STATUS_CODE_401_UNAUTHORIZED = 401,                        ///< Unauthorized
	HTTP_STATUS_CODE_402_PAYMENT_REQUIRED = 402,                    ///< Payment Required
	HTTP_STATUS_CODE_403_FORBIDDEN = 403,                           ///< Forbidden
	HTTP_STATUS_CODE_404_NOT_FOUND = 404,                           ///< Not Found
	HTTP_STATUS_CODE_405_METHOD_NOT_ALLOWED = 405,                  ///< Method Not Allowed
	HTTP_STATUS_CODE_406_NOT_ACCEPTABLE = 406,                      ///< Not Acceptable
	HTTP_STATUS_CODE_407_PROXY_AUTH_REQUIRED = 407,                 ///< Proxy Authentication Required
	HTTP_STATUS_CODE_408_REQUEST_TIMEOUT = 408,                     ///< Request Time-out
	HTTP_STATUS_CODE_409_CONFLICT = 409,                            ///< Conflict
	HTTP_STATUS_CODE_410_GONE = 410,                                ///< Gone
	HTTP_STATUS_CODE_411_LENGTH_REQUIRED = 411,                     ///< Length Required
	HTTP_STATUS_CODE_412_PRECONDITION_FAILED = 412,                 ///< Precondition Failed
	HTTP_STATUS_CODE_413_REQUEST_ENTITY_TOO_LARGE = 413,            ///< Request Entity Too Large
	HTTP_STATUS_CODE_414_URI_TOO_LARGE = 414,                       ///< Request-URI Too Large
	HTTP_STATUS_CODE_415_UNSUPPORTED_MEDIA_TYPE = 415,              ///< Unsupported Media Type
	HTTP_STATUS_CODE_416_RANGE_NOT_SATISFIABLE = 416,               ///< Requested range not satisfiable
	HTTP_STATUS_CODE_417_EXPECTATION_FAILED = 417,                  ///< Expectation Failed
	HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR = 500,               ///< Internal Server Error
	HTTP_STATUS_CODE_501_NOT_IMPLEMENTED = 501,                     ///< Not Implemented
	HTTP_STATUS_CODE_502_BAD_GATEWAY = 502,                         ///< Bad Gateway
	HTTP_STATUS_CODE_503_SERVICE_UNAVAILABLE = 503,                 ///< Service Unavailable
	HTTP_STATUS_CODE_504_GATEWAY_TIMEOUT = 504,                     ///< Gateway Time-out
	HTTP_STATUS_CODE_505_VERSION_NOT_SUPPORTED = 505,               ///< HTTP Version not supported
};

extern const char* GetHttpStatusReasonPhrase(int statusCode);

/// This defines what is sent back to the client
/**
 * This defines what is sent back to the client.  It contains the actual body, any
 *   headers specified, and the response code.
 */
class HttpResponse {

  public:

	/// constructor
	HttpResponse ( int inResponseId, EHSConnection * ipoEHSConnection );

	/// destructor
	~HttpResponse ( );

	/// sets information about the body of the response being sent back to the client.
	void SetBody ( const char * ipsBody, ///< body to be sent to client
				   int inBodyLength ///< length of body to be sent to client
		);

	/// sets cookies for the response
	void SetCookie ( CookieParameters & iroCookieParameters );


	/// Returns the body of the response
	char * GetBody ( ) { return psBody; };

	/// the response code to be sent back
	HttpStatusCode m_nResponseCode;

	/// these are the headers sent back to the client in the http response.  Things like content-type and content-length
	StringMap oResponseHeaders;

	/// cookies waiting to be sent
	StringList oCookieList;

	/// ehs connection object this response goes back on
	EHSConnection * m_poEHSConnection;

	/// response id for making sure we send responses in the right order
	int m_nResponseId;


  protected:

	/// request id for this request's connection object
	int m_nRequestId;

	/// the actual body to be sent back -- set by SetBody
	char * psBody;

	/// the size of the body to be sent back -- set by SetBody
	int nBodyLength;


};


#endif // HTTPRESPONSE_H
