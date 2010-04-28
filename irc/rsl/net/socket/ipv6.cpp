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
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include "ipv6.h"

using Rsl::Net::Socket::IPAddr;
using Rsl::Net::Socket::IPV6Addr;

IPV6Addr::IPV6Addr()
  : m_errno(0)
{ }

IPV6Addr::IPV6Addr(const char* addr, const char* service)
  : m_errno(0)
{
  Create(addr, service);
}

IPV6Addr::IPV6Addr(const IPV6Addr& Right)
  : IPAddr(Right), m_errno(Right.m_errno)
{
}

void IPV6Addr::Create(const char* addr, const char* service)
{
  addrinfo  hint;
  int       res;

  memset(&hint, 0, sizeof(addrinfo));
  hint.ai_family = AF_INET6;

  res = getaddrinfo(addr, service, &hint, &m_info);
  if (res != 0)
  {
    m_errno = errno;
  }
}

IPAddr* IPV6Addr::Clone() const
{
  IPV6Addr* newAddr = new IPV6Addr(*this);
  return newAddr;
}

bool IPV6Addr::Ok() const
{
  return !m_errno;
}

const char* IPV6Addr::Error() const
{
  return strerror(m_errno);
}

int IPV6Addr::Errno() const
{
  return m_errno;
}
