/*****************************************************************************
 * httpd/Types.h - HTTP server type definitions
 *
 * Provides HttpRequest, HttpResponse, HttpStatusCode and related types
 * used by the built-in HTTP server (CHTTPD) and resource handling code.
 *
 * Originally these types came from EHS. After migrating to cpp-httplib,
 * the type names are kept for compatibility but the implementation is
 * standalone — CHTTPD populates these from httplib types.
 *****************************************************************************/
#pragma once

#include <map>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <utility>

// ---------------------------------------------------------------------------
// Backward compatibility: EHS headers polluted the global namespace with
// common STL names. Existing MTA code relies on map, string, list, vector
// and multimap being available without std:: qualification. These using
// declarations keep Types.h a drop-in replacement for <ehs/ehs.h>.
// ---------------------------------------------------------------------------

using std::list;
using std::make_pair;
using std::map;
using std::max;
using std::multimap;
using std::pair;
using std::string;
using std::vector;

// ---------------------------------------------------------------------------
// Common typedefs
// ---------------------------------------------------------------------------

typedef std::map<std::string, std::string> StringMap;
typedef std::map<std::string, std::string> CookieMap;
typedef std::list<std::string>             StringList;

// ---------------------------------------------------------------------------
// ContentDisposition — used by FormValue for multipart form data
// ---------------------------------------------------------------------------

class ContentDisposition
{
public:
    ContentDisposition() = default;
    ~ContentDisposition() = default;

    StringMap   oContentDispositionHeaders;
    std::string sContentDisposition;
};

// ---------------------------------------------------------------------------
// FormValue — represents a single form field from an HTTP request
// ---------------------------------------------------------------------------

class FormValue
{
public:
    FormValue() = default;
    ~FormValue() = default;

    StringMap          oFormHeaders;
    ContentDisposition oContentDisposition;
    std::string        sBody;
};

typedef std::map<std::string, FormValue> FormValueMap;

// ---------------------------------------------------------------------------
// SAllocationStats — allocation tracking (used by perf stats)
// ---------------------------------------------------------------------------

struct SAllocationStats
{
    unsigned int uiActiveNumRequests = 0;
    unsigned int uiActiveNumResponses = 0;
    unsigned int uiActiveKBAllocated = 0;
    unsigned int uiTotalNumRequests = 0;
    unsigned int uiTotalNumResponses = 0;
    unsigned int uiTotalKBAllocated = 0;
};

// ---------------------------------------------------------------------------
// HttpStatusCode — HTTP response status codes
// ---------------------------------------------------------------------------

enum HttpStatusCode
{
    HTTP_STATUS_CODE_INVALID = 0,
    HTTP_STATUS_CODE_100_CONTINUE = 100,
    HTTP_STATUS_CODE_101_SWITCHING_PROTOCOLS = 101,
    HTTP_STATUS_CODE_200_OK = 200,
    HTTP_STATUS_CODE_201_CREATED = 201,
    HTTP_STATUS_CODE_202_ACCEPTED = 202,
    HTTP_STATUS_CODE_203_NON_AUTHORITATIVE_INFO = 203,
    HTTP_STATUS_CODE_204_NO_CONTENT = 204,
    HTTP_STATUS_CODE_205_RESET_CONTENT = 205,
    HTTP_STATUS_CODE_206_PARTIAL_CONTENT = 206,
    HTTP_STATUS_CODE_300_MULTIPLE_CHOICES = 300,
    HTTP_STATUS_CODE_301_MOVED_PERMANENTLY = 301,
    HTTP_STATUS_CODE_302_FOUND = 302,
    HTTP_STATUS_CODE_303_SEE_OTHER = 303,
    HTTP_STATUS_CODE_304_NOT_MODIFIED = 304,
    HTTP_STATUS_CODE_305_USE_PROXY = 305,
    HTTP_STATUS_CODE_307_TEMPORARY_REDIRECT = 307,
    HTTP_STATUS_CODE_400_BAD_REQUEST = 400,
    HTTP_STATUS_CODE_401_UNAUTHORIZED = 401,
    HTTP_STATUS_CODE_402_PAYMENT_REQUIRED = 402,
    HTTP_STATUS_CODE_403_FORBIDDEN = 403,
    HTTP_STATUS_CODE_404_NOT_FOUND = 404,
    HTTP_STATUS_CODE_405_METHOD_NOT_ALLOWED = 405,
    HTTP_STATUS_CODE_406_NOT_ACCEPTABLE = 406,
    HTTP_STATUS_CODE_407_PROXY_AUTH_REQUIRED = 407,
    HTTP_STATUS_CODE_408_REQUEST_TIMEOUT = 408,
    HTTP_STATUS_CODE_409_CONFLICT = 409,
    HTTP_STATUS_CODE_410_GONE = 410,
    HTTP_STATUS_CODE_411_LENGTH_REQUIRED = 411,
    HTTP_STATUS_CODE_412_PRECONDITION_FAILED = 412,
    HTTP_STATUS_CODE_413_REQUEST_ENTITY_TOO_LARGE = 413,
    HTTP_STATUS_CODE_414_URI_TOO_LARGE = 414,
    HTTP_STATUS_CODE_415_UNSUPPORTED_MEDIA_TYPE = 415,
    HTTP_STATUS_CODE_416_RANGE_NOT_SATISFIABLE = 416,
    HTTP_STATUS_CODE_417_EXPECTATION_FAILED = 417,
    HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR = 500,
    HTTP_STATUS_CODE_501_NOT_IMPLEMENTED = 501,
    HTTP_STATUS_CODE_502_BAD_GATEWAY = 502,
    HTTP_STATUS_CODE_503_SERVICE_UNAVAILABLE = 503,
    HTTP_STATUS_CODE_504_GATEWAY_TIMEOUT = 504,
    HTTP_STATUS_CODE_505_VERSION_NOT_SUPPORTED = 505,
};

// ---------------------------------------------------------------------------
// RequestMethod — HTTP request method enumeration
// ---------------------------------------------------------------------------

enum RequestMethod
{
    REQUESTMETHOD_OPTIONS,
    REQUESTMETHOD_GET,
    REQUESTMETHOD_HEAD,
    REQUESTMETHOD_POST,
    REQUESTMETHOD_PUT,
    REQUESTMETHOD_DELETE,
    REQUESTMETHOD_TRACE,
    REQUESTMETHOD_CONNECT,
    REQUESTMETHOD_PATCH,
    REQUESTMETHOD_LAST,
    REQUESTMETHOD_UNKNOWN,
    REQUESTMETHOD_INVALID
};

// ---------------------------------------------------------------------------
// HttpRequest — incoming HTTP request, populated by CHTTPD from httplib
// ---------------------------------------------------------------------------

class HttpRequest
{
public:
    HttpRequest() = default;

    RequestMethod nRequestMethod = REQUESTMETHOD_UNKNOWN;

    std::string sUri;          // Request URI (may be shortened by routing)
    std::string sOriginalUri;  // Original URI before any routing
    std::string sBody;         // Request body (POST data, etc.)
    std::string sHttpVersionNumber;

    StringMap    oRequestHeaders;  // Request headers (name → value)
    FormValueMap oFormValueMap;    // Parsed form fields
    FormValueMap oQueryValueMap;   // Parsed query string fields
    CookieMap    oCookieMap;       // Parsed cookies

    int  m_nRequestId = 0;         // Request sequence number
    int  nSecure = 0;              // Whether request came over HTTPS
    bool m_bDisconnected = false;  // Client has disconnected

    std::string GetAddress() const { return m_strAddress; }
    int         GetPort() const { return m_nPort; }
    int         ClientDisconnected() const { return m_bDisconnected ? 1 : 0; }

    // Set by CHTTPD when populating from httplib::Request
    std::string m_strAddress;
    int         m_nPort = 0;
};

// ---------------------------------------------------------------------------
// Datum — simple variant type used for cookie values.
// Originally from EHS. For MTA's purposes, only string assignment is needed.
// ---------------------------------------------------------------------------

class Datum
{
public:
    Datum() = default;
    Datum(const char* s) : m_strValue(s ? s : "") {}
    Datum(const std::string& s) : m_strValue(s) {}

    Datum& operator=(const char* s)
    {
        m_strValue = s ? s : "";
        return *this;
    }
    Datum& operator=(const std::string& s)
    {
        m_strValue = s;
        return *this;
    }
    Datum& operator=(int n)
    {
        m_strValue = std::to_string(n);
        return *this;
    }

    operator const char*() const { return m_strValue.c_str(); }

private:
    std::string m_strValue;
};

typedef std::map<std::string, Datum> CookieParameters;

// ---------------------------------------------------------------------------
// HttpResponse — outgoing HTTP response, populated by resource handlers
// ---------------------------------------------------------------------------

class HttpResponse
{
public:
    HttpResponse() = default;

    /// Set the response body.
    /// Takes ownership of the data: copies the provided buffer into internal storage.
    void SetBody(const char* ipsBody, int inBodyLength)
    {
        if (ipsBody && inBodyLength > 0)
            m_strBody.assign(ipsBody, inBodyLength);
        else
            m_strBody.clear();
    }

    /// Get the response body (for CHTTPD to read back).
    const std::string& GetBody() const { return m_strBody; }

    /// HTTP status code to send back.
    HttpStatusCode m_nResponseCode = HTTP_STATUS_CODE_200_OK;

    /// Response headers (name → value). Things like content-type, content-length.
    StringMap oResponseHeaders;

    /// Cookies to send back. Store the structured params then CHTTPD flattens them.
    void SetCookie(CookieParameters& iroCookieParameters)
    {
        for (const auto& pair : iroCookieParameters)
            oCookieList.push_back(pair.first + "=" + static_cast<const char*>(pair.second));
    }

    /// Cookies to send back (raw list of "name=value" strings).
    StringList oCookieList;

    /// Response sequence number (for ordering).
    int m_nResponseId = 0;

private:
    std::string m_strBody;  // Internal storage for SetBody
};
