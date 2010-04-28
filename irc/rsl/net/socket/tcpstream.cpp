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
#include <sys/types.h>
#include <sys/socket.h>
#include "tcpstream.h"

using Rsl::Net::Socket::SocketStreamTCP;

SocketStreamTCP::SocketStreamTCP(int handle)
  : SocketStream(handle)
{
}

void SocketStreamTCP::SetHandle(int handle)
{
  SocketStream::SetHandle(handle);
}

ssize_t SocketStreamTCP::QueueLength()
{
  char tmp;
  ssize_t len = -1;

  if (Ok())
  {
    len = recv(m_handle, (void *)&tmp, sizeof(char), MSG_TRUNC | MSG_PEEK);
    if (len < 0)
    {
      m_errno = errno;
    }
  }

  return len;
}

ssize_t SocketStreamTCP::Read(char* buffer, size_t maxLength)
{
  ssize_t len = -1;

  if (Ok())
  {
    len = recv(m_handle, buffer, maxLength, 0);
    /* TODO: Make non blocking sockets support, so they can return 0 */
    if (len == 0)
    {
      /* EOF */
      m_eof = true;
    }
    else if (len < 1)
    {
      m_errno = errno;
    }
    else
    {
      m_inbytes += len;
    }
  }

  return len;
}

ssize_t SocketStreamTCP::Write(const char* buffer, size_t length)
{
  ssize_t len = -1;

  if (Ok())
  {
    len = send(m_handle, buffer, length, 0);
    /* TODO: Make non blocking sockets support here too */
    if (len == 0)
    {
      m_eof = true;
    }
    else if (len == -1)
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

ssize_t SocketStreamTCP::WriteString(const std::string& str)
{
  return Write(str.c_str(), str.length());
}
