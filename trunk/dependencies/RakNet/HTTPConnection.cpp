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

#include "TCPInterface.h"
#include "HTTPConnection.h"
#include "RakSleep.h"
#include "RakString.h"
#include "RakAssert.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

using namespace RakNet;

HTTPConnection::HTTPConnection(TCPInterface& _tcp, const char *_host, unsigned short _port)
    : tcp(_tcp), host(_host), port(_port), state(RAK_HTTP_INITIAL) {}

void HTTPConnection::Post(const char *remote_path, const char *data, const char *_contentType)
{
    if(state == RAK_HTTP_IDLE)
        state = RAK_HTTP_ESTABLISHED;
    else if(state == RAK_HTTP_INITIAL)
        state = RAK_HTTP_STARTING;
    else
        return;

    outgoing = data;
    path = remote_path; 
	contentType=_contentType;

    incoming.Clear();
}

bool HTTPConnection::HasBadResponse(int *code, RakNet::RakString *data)
{
    if(badResponses.IsEmpty())
        return false;

	if (code)
		*code = badResponses.Peek().code;
	if (data)
		*data = badResponses.Pop().data;
   return true;
}


bool HTTPConnection::InList(StatusCheckFunction func)
{
    SystemAddress address = (tcp.*func)();

    if(address == UNASSIGNED_SYSTEM_ADDRESS)
        return false;

    server = address;
    return true;
}

void HTTPConnection::Update(void)
{
	if(InList(&TCPInterface::HasCompletedConnectionAttempt))
		state = RAK_HTTP_ESTABLISHED;

	if(InList(&TCPInterface::HasFailedConnectionAttempt))
		state = RAK_HTTP_STARTING; // retry

	// normally, HTTP servers close the stream after sending data
	if(InList(&TCPInterface::HasLostConnection))
		state = RAK_HTTP_INITIAL;

	if(state == RAK_HTTP_STARTING)
	{
		server = tcp.Connect(host, port, false);
		state = RAK_HTTP_CONNECTING;
	}

	if(state == RAK_HTTP_ESTABLISHED)
	{
		RakString request("POST %s HTTP/1.0\r\n"
			"Host: %s\r\n"
			"Content-Type: %s\r\n"
			"Content-Length: %u\r\n"
			"\r\n"
			"%s",
			path.C_String(),
			host.C_String(),
			contentType.C_String(),
			(unsigned) outgoing.GetLength(),
			outgoing.C_String());
		tcp.Send(request, (unsigned int) strlen(request), server,false);

		state = RAK_HTTP_REQUEST_SENT;
	}
}
RakString HTTPConnection::Read(void)
{
    const char *start_of_body = strstr(incoming, "\r\n\r\n");
    
    if(! start_of_body)
        {
        badResponses.Push(BadResponse(incoming, HTTPConnection::NoBody));
        return RakString();
        }

    return RakString(start_of_body + 4);
}
SystemAddress HTTPConnection::GetServerAddress(void) const
{
	return server;
}
bool HTTPConnection::ProcessFinalTCPPacket(Packet *packet)
{
	RakAssert(packet);

	// read all the packets possible
	if(packet->systemAddress == server)
	{
		if(incoming.GetLength() == 0)
		{
			int response_code = atoi((char *)packet->data + strlen("HTTP/1.0 "));

			if(response_code > 299)
				badResponses.Push(BadResponse(packet->data, response_code));
		}
		incoming += (char *)packet->data; // safe because TCPInterface Null-terminates

		RakAssert(strlen((char *)packet->data) == packet->length); // otherwise it contains Null bytes


		const char *start_of_body = strstr(incoming, "\r\n\r\n");

		// besides having the server close the connection, they may
		// provide a length header and supply that many bytes
		if(start_of_body && state == RAK_HTTP_REQUEST_SENT)
		{
			if (strstr((const char*) packet->data, "\r\nConnection: close\r\n"))
			{
				state = RAK_HTTP_IDLE;
			}
			else
			{
				long length_of_headers = (long)(start_of_body + 4 - incoming.C_String());

				const char *length_header = strstr(incoming, "\r\nLength: ");
				if(length_header)
				{
					long length = atol(length_header + 10) + length_of_headers;

					if((long) incoming.GetLength() >= length)
						state = RAK_HTTP_IDLE;
				}
			}
		}
	}

	return IsBusy()==false;
}

bool HTTPConnection::IsBusy(void) const
{
	return state != RAK_HTTP_IDLE && state != RAK_HTTP_INITIAL;
}

int HTTPConnection::GetState(void) const
{
	return state;
}


HTTPConnection::~HTTPConnection(void)
{
    tcp.CloseConnection(server);
}


