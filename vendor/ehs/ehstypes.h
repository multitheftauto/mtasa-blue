
#ifndef EHSTYPES_H
#define EHSTYPES_H

#include <string>
#include <map>
#include <list>

class EHSConnection;
class EHS;
class Datum;
class FormValue;
class HttpResponse;
class HttpRequest;

/// generic std::string => std::string map used by many things
typedef std::map < std::string, std::string > StringMap;

/// generic list of std::strings
typedef std::list < std::string > StringList;

/// Define a list of EHSConnection objects to handle all current connections
typedef std::list < EHSConnection * > EHSConnectionList;

// map for registered EHS objects on a path
typedef std::map < std::string, EHS * > EHSMap;

/// map type for storing EHSServer parameters
typedef std::map < std::string, Datum > EHSServerParameters;

/// cookies that come in from the client, mapped by name
typedef std::map < std::string, std::string > CookieMap;

/// describes a form value that came in from a client
typedef std::map < std::string, FormValue > FormValueMap;

/// describes a cookie to be sent back to the client
typedef std::map < std::string, Datum > CookieParameters;

/// holds respose objects not yet ready to send
typedef std::map < int, HttpResponse * > HttpResponseMap;

/// holds a list of pending requests
typedef std::list < HttpRequest * > HttpRequestList;



#endif
