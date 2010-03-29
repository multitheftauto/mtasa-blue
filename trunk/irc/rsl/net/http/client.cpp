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

#include <errno.h>
#include "client.h"
#include "../socket/tcpclient.h"
#include "../socket/tcpclientssl.h"
#include "../socket/tcpstream.h"

using namespace Rsl::Net::HTTP;
using Rsl::Net::Socket::IPAddr;
using Rsl::Net::Socket::SocketClientTCP;
using Rsl::Net::Socket::SocketClientTCPSSL;
using Rsl::Net::Socket::SocketStream;

HTTPClient::HTTPClient()
  : m_socket(0), m_errno(0), m_error(""), m_responseStatus(-1)
{
}

HTTPClient::HTTPClient(const std::string& URI, HTTPMethod method)
  : m_socket(0), m_errno(0), m_error(""), m_responseStatus(-1),
    m_statusText("")
{
  /* TODO */
}

HTTPClient::HTTPClient(const std::string& hostname,
                       const IPAddr& addr,
                       const IPAddr& bindAddr,
                       bool useSSL,
                       const char* certFile)
  : m_socket(0), m_errno(0), m_error(""), m_responseStatus(-1),
    m_statusText("")
{
  Create(hostname, addr, bindAddr, useSSL, certFile);
}


HTTPClient::~HTTPClient()
{
  if (m_socket)
  {
    delete m_socket;
    m_socket = 0;
  }
  m_errno = 0;
  m_error = "";
}


void HTTPClient::Create(const std::string& hostname,
                        const IPAddr& addr,
                        const IPAddr& bindAddr,
                        bool useSSL,
                        const char* certFile)
{
  if (useSSL)
  {
    m_socket = new SocketClientTCPSSL(addr, bindAddr, certFile);
  }
  else
  {
    m_socket = new SocketClientTCP(addr, bindAddr);
  }

  HTTPHeader hostHeader = { "Host", hostname };
  m_sendHeaders.push_back(hostHeader);
}

bool HTTPClient::Ok() const
{
  return !m_errno && m_socket && m_socket->Ok();
}

int HTTPClient::Errno() const
{
  return m_errno;
}

const char* HTTPClient::Error() const
{
  return m_error.c_str();
}

bool HTTPClient::Connect(const std::string& path, HTTPMethod method)
{
  if (!m_socket->Ok())
  {
    m_errno = m_socket->Errno();
    m_error = m_socket->Error();
    return false;
  }

  if (!m_socket->Connect())
  {
    m_errno = m_socket->Errno();
    m_error = m_socket->Error();
    return false;
  }

  SocketStream& stream = m_socket->GetStream();
  if (method == HTTP_METHOD_GET)
    stream.WriteString("GET " + path + " HTTP/1.0\r\n");
  else if (method == HTTP_METHOD_POST)
    stream.WriteString("POST " + path + " HTTP/1.0\r\n");

  for (std::vector<HTTPHeader>::iterator i = m_sendHeaders.begin();
       i != m_sendHeaders.end();
       i++)
  {
    stream.WriteString((*i).name + ": " + (*i).value + "\r\n");
  }

  return true;
}

void HTTPClient::SendHeader(const std::string& headerName, const std::string& headerValue)
{
  HTTPHeader header = { headerName, headerValue };
  m_sendHeaders.push_back(header);
}

void HTTPClient::Send(const std::string& vars)
{
  SocketStream& stream = m_socket->GetStream();

  if (vars.length() > 0)
  {
    char tmp[32];
    snprintf(tmp, 32, "%d", vars.length());
    stream.WriteString(std::string("Content-length: ") + tmp + "\r\n\r\n");
    stream.WriteString(vars);
  }
  else
  {
    stream.WriteString("\r\n");
  }

  char tmp[512];
  std::string buffer("");
  ssize_t contentLength = -1;
  int p;
  bool headersRead = false;

  do
  {
    ssize_t len = stream.Read(tmp, sizeof(tmp)-1);
    if (len > 0)
    {
      buffer.append(tmp, len);

      while ((p = buffer.find("\r\n")) > -1)
      {
        std::string line(buffer.substr(0, p));
        buffer.erase(0, p + 2);

        if (line.length() == 0)
        {
          /* End of headers */
          headersRead = true;
          break;
        }

        if (!strncmp(line.c_str(), "HTTP/1.0 ", 9))
        {
          /* Response status */
          if (m_responseStatus != -1)
          {
            /* Multiple response status? */
            m_responseStatus = -1;
            break;
          }

          p = line.find(" ", 9);
          if (p == -1)
          {
            /* Wrong response status */
            break;
          }
          m_responseStatus = atoi(line.substr(9, p - 9).c_str());
          m_statusText = line.substr(p + 1);
        }

        else
        {
          /* Parse headers */
          p = line.find(": ");
          if (p == -1)
          {
            /* Invalid header, ignore it */
            break;
          }
          std::string headerName(line, 0, p);
          std::string headerValue(line, p + 2);
      HTTPHeader header = { headerName, headerValue };
          m_receivedHeaders.push_back(header);

          if (!strcasecmp(headerName.c_str(), "Content-Length"))
            contentLength = atoi(headerValue.c_str());
        }
      }
    }
  } while (!stream.Eof() && !headersRead);

  if (stream.Eof() || m_responseStatus == 0)
    return;

  m_stream.SetSocket(m_socket, buffer, contentLength);
}

const std::vector<HTTPHeader>& HTTPClient::GetResponseHeaders() const
{
  return m_receivedHeaders;
}

int HTTPClient::ResponseStatus()
{
  return m_responseStatus;
}

const char* HTTPClient::StatusText()
{
  return m_statusText.c_str();
}

HTTPStream& HTTPClient::GetStream()
{
  return m_stream;
}
