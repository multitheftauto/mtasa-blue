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

#include <string>
#include <errno.h>
#include "udpstream.h"

using Rsl::Net::Socket::SocketStreamUDP;

SocketStreamUDP::SocketStreamUDP(int handle, const addrinfo* addr)
  : SocketStream(handle), m_addr(addr)
{
}

void SocketStreamUDP::SetHandle(int handle)
{
  SocketStream::SetHandle(handle);
}

void SocketStreamUDP::SetAddr(const addrinfo* addr)
{
  m_addr = addr;
}

static inline bool IsValidPeer(const addrinfo* addr, const sockaddr& fromAddr)
{
  if (fromAddr.sa_family == addr->ai_family)
  {
    switch (fromAddr.sa_family)
    {
      case AF_INET:
      {
        sockaddr_in* from = (sockaddr_in *)&fromAddr;
        sockaddr_in* check = (sockaddr_in *)addr->ai_addr;

        if (from->sin_addr.s_addr == check->sin_addr.s_addr)
        {
          return true;
        }
        break;
      }
      case AF_INET6:
      {
        sockaddr_in6* from = (sockaddr_in6 *)&fromAddr;
        sockaddr_in6* check = (sockaddr_in6 *)addr->ai_addr;

        if (from->sin6_addr.s6_addr == check->sin6_addr.s6_addr)
        {
          return true;
        }
        break;
      }
    }
  }

  return false;
}

bool SocketStreamUDP::Ok() const
{
  if (!SocketStream::Ok() || !m_addr)
    return false;
  return true;
}

ssize_t SocketStreamUDP::QueueLength()
{
  ssize_t len = -1;
  sockaddr fromAddr;
  socklen_t fromLength;
  char tmp;

  if (Ok())
  {
    fromLength = sizeof(fromAddr);
    len = recvfrom(m_handle, (void *)&tmp, sizeof(char), MSG_TRUNC | MSG_PEEK, (sockaddr *)&fromAddr, &fromLength);

    if (IsValidPeer(m_addr, fromAddr))
    {
      if (len < 0)
      {
        m_errno = errno;
      }
    }
    else
    {
      len = 0;
    }
  }

  return len;
}

ssize_t SocketStreamUDP::Read(char* buffer, size_t maxLength)
{
  ssize_t len = -1;
  sockaddr fromAddr;
  socklen_t fromLength;

  if (Ok())
  {
    fromLength = sizeof(fromAddr);
    len = recvfrom(m_handle, buffer, maxLength, 0, (sockaddr *)&fromAddr, &fromLength);

    if (IsValidPeer(m_addr, fromAddr))
    {
      if (len < 0)
      {
        m_errno = errno;
      }
      else
      {
        m_inbytes += len;
      }
    }
    else
    {
      len = 0;
    }
  }

  return len;
}

ssize_t SocketStreamUDP::Write(const char* buffer, size_t length)
{
  ssize_t len = -1;

  if (Ok())
  {
    len = sendto(m_handle, buffer, length, 0, m_addr->ai_addr, m_addr->ai_addrlen);
    if (len == -1)
    {
      m_errno = errno;
    }
    else
    {
      m_outbytes += len;
    }
  }

  return len;
}

ssize_t SocketStreamUDP::WriteString(const std::string& str)
{
  return Write(str.c_str(), str.length());
}

