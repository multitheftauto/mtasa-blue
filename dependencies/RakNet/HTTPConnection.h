/// \file
/// \brief Contains HTTPConnection, used to communicate with web servers
///
/// This file is part of RakNet Copyright 2008 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __HTTP_CONNECTION
#define __HTTP_CONNECTION

#include "Export.h"
#include "RakString.h"
#include "RakMemoryOverride.h"
#include "RakNetTypes.h"
#include "DS_Queue.h"

class TCPInterface;
struct SystemAddress;

/// \brief Use HTTPConnection to communicate with a web server.
/// Start an instance of TCPInterface via the Start() command.
/// Instantiate a new instance of HTTPConnection, and associate TCPInterface with the class in the constructor.
/// Use Post() to send commands to the web server, and ProcessDataPacket() to update the connection with packets returned from TCPInterface that have the system address of the web server
/// This class will handle connecting and reconnecting as necessary.
///
/// Note that only one Post() can be handled at a time. 
class RAK_DLL_EXPORT HTTPConnection
{
public:
    /// Returns a HTTP object associated with this tcp connection
    /// \pre tcp should already be started
    HTTPConnection(TCPInterface& tcp, const char *host, unsigned short port=80);
    virtual ~HTTPConnection();

    /// Submit data to the HTTP server
    /// HTTP only allows one request at a time per connection
    ///
	/// \pre IsBusy()==false
    /// \param path the path on the remote server you want to POST to. For example "mywebpage/index.html"
    /// \param data A NULL terminated string to submit to the server
	/// \param contentType "Content-Type:" passed to post.
    void Post(const char *path, const char *data, const char *_contentType="application/x-www-form-urlencoded");
    
    /// Get data returned by the HTTP server
    /// If IsFinished()==false then this may be empty or a partial
    /// response.
    RakNet::RakString Read(void);

	/// Call periodically to do time-based updates
	void Update(void);

	/// Returns the address of the server we are connected to
	SystemAddress GetServerAddress(void) const;

	/// Process an HTTP data packet returned from TCPInterface
	/// Returns true when we have gotten all the data from the HTTP server.
    /// If this returns true then it's safe to Post() another request
	/// Deallocate the packet as usual via TCPInterface
    /// \param packet NULL or a packet associated with our host and port
	/// \return true when all data from one Post() has been read.
    bool ProcessFinalTCPPacket(Packet *packet);

    /// Results of HTTP requests.  Standard response codes are < 999
    /// ( define HTTP codes and our internal codes as needed )
    enum ResponseCodes { NoBody=1001, OK=200, Deleted=1002 };

	HTTPConnection& operator=(const HTTPConnection& rhs){(void) rhs; return *this;}
   
    /// Encapsulates a raw HTTP response and response code
    struct BadResponse
    {
    public:
		BadResponse() {code=0;}
        
        BadResponse(const unsigned char *_data, int _code)
            : data((const char *)_data), code(_code) {}
        
        BadResponse(const char *_data, int _code)
            : data(_data), code(_code) {}

		operator int () const { return code; }

		RakNet::RakString data;
		int code;  // ResponseCodes
    };

    /// Queued events of failed exchanges with the HTTP server
    bool HasBadResponse(int *code, RakNet::RakString *data);

	/// Returns false if the connection is not doing anything else
	bool IsBusy(void) const;

	/// \internal
	int GetState(void) const;

private:
    SystemAddress server;
    TCPInterface& tcp;
	RakNet::RakString host;
	unsigned short port;
	enum { RAK_HTTP_INITIAL,
		RAK_HTTP_STARTING,
		RAK_HTTP_CONNECTING,
		RAK_HTTP_ESTABLISHED,
		RAK_HTTP_REQUEST_SENT,
		RAK_HTTP_IDLE } state;
    RakNet::RakString outgoing, incoming, path, contentType;
    DataStructures::Queue<BadResponse> badResponses;
    void Process(Packet *packet); // the workhorse
    
    // this helps check the various status lists in TCPInterface
    typedef SystemAddress (TCPInterface::*StatusCheckFunction)(void);
    bool InList(StatusCheckFunction func);
};

#endif

