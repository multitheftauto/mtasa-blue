/*
 * Copyright (c) 2007, Alberto Alonso Pinto
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions
 *       and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *       and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Alberto Alonso Pinto nor the names of its contributors may be used to endorse or
 *       promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RSL_NET_HTTP_CLIENT_H
#define _RSL_NET_HTTP_CLIENT_H

#include <string>
#include <vector>
#include "stream.h"
#include "../socket/ip.h"
#include "../socket/ipv4.h"
#include "../socket/tcpclient.h"

namespace Rsl { namespace Net { namespace HTTP {

  enum HTTPMethod
  {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST
  };

  struct HTTPHeader
  {
    std::string name;
    std::string value;
  };

  class HTTPClient
  {
  public:
    HTTPClient();
    HTTPClient(const std::string& hostname,
               const Rsl::Net::Socket::IPAddr& addr,
               const Rsl::Net::Socket::IPAddr& bindAddr = Rsl::Net::Socket::IPV4Addr("0.0.0.0", "0"),
               bool useSSL = false,
               const char* certFile = 0);
    HTTPClient(const std::string& URI, HTTPMethod method = HTTP_METHOD_GET); /* TODO: Implement URIs */
    ~HTTPClient();

    void Create(const std::string& hostname,
                const Rsl::Net::Socket::IPAddr& addr,
                const Rsl::Net::Socket::IPAddr& bindAddr = Rsl::Net::Socket::IPV4Addr("0.0.0.0", "0"),
                bool useSSL = false,
                const char* certFile = 0);

    bool Ok() const;
    int Errno() const;
    const char* Error() const;

    void SendHeader(const std::string& headerName, const std::string& headerValue);
    bool Connect(const std::string& path, HTTPMethod method = HTTP_METHOD_GET);
    void Send(const std::string& vars = "");

    int ResponseStatus();
    const char* StatusText();
    HTTPStream& GetStream();
    const std::vector<HTTPHeader>& GetResponseHeaders() const;

  private:
    std::string m_error;
    std::vector<HTTPHeader> m_sendHeaders;
    std::vector<HTTPHeader> m_receivedHeaders;
    int m_errno;
    int m_responseStatus;
    std::string m_statusText;
    Rsl::Net::Socket::SocketClientTCP* m_socket;
    HTTPStream m_stream;
  };

}; }; };

#endif /* #ifndef _RSL_NET_HTTP_CLIENT_H */
