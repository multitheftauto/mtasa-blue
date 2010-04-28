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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "tcpclient.h"
#include "tcpstream.h"

using Rsl::Net::Socket::SocketClientTCP;
using Rsl::Net::Socket::SocketStreamTCP;
using Rsl::Net::Socket::SocketStream;

SocketClientTCP::SocketClientTCP() : m_errno(0)
{
}

SocketClientTCP::SocketClientTCP(const IPAddr& addr, const IPAddr& bindAddr)
{
  Create(addr, bindAddr);
}

void SocketClientTCP::Create(const IPAddr& addr, const IPAddr& bindAddr)
{
  int family;
  int connect_res = -1;

  m_errno = 0;
  SocketClient::Create(addr, bindAddr);
}

bool SocketClientTCP::Connect()
{
  if (Ok())
  {
    if (sockdesc != -1)
      Close();

    sockdesc = -1;
    for (const addrinfo* info = m_addr->GetAddr(); info != 0; info = info->ai_next)
    {
      if (info->ai_socktype == SOCK_STREAM)
      {
        sockdesc = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (sockdesc > -1)
        {
          m_currentAddr = info;
          break;
        }
      }
    }
    if (sockdesc == -1)
    {
      m_errno = errno;
      return false;
    }

    if (connect(sockdesc, m_currentAddr->ai_addr, m_currentAddr->ai_addrlen) > -1)
    {
      m_stream.SetHandle(sockdesc);
      return true;
    }

    m_errno = errno;
  }
  return false;
}

SocketClientTCP::~SocketClientTCP()
{
  Close();
  m_stream.SetHandle(-1);
}

void SocketClientTCP::Close()
{
  if (sockdesc != -1)
  {
    close(sockdesc);
    sockdesc = -1;
    m_stream.SetHandle(-1);
  }
  m_errno = 0;
}

int SocketClientTCP::Errno() const
{
  return m_errno;
}

const char * SocketClientTCP::Error() const
{
  return strerror(m_errno);
}

bool SocketClientTCP::Ok() const
{
  return m_addr->Ok() && m_bindAddr->Ok() && !m_errno;
}

SocketStream& SocketClientTCP::GetStream()
{
  return m_stream;
}

