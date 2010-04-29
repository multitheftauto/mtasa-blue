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

#include <arpa/inet.h>
#include <malloc.h>
#include <sys/socket.h>
#include "ip.h"

using Rsl::Net::Socket::IPAddr;

static inline addrinfo* duplicateAddrinfo(const addrinfo* from)
{
  addrinfo* newInfo;
  sockaddr* newAddress;

  newInfo = (addrinfo *)malloc(sizeof(addrinfo));
  if (!newInfo)
    return 0;
  newAddress = (sockaddr*)malloc(sizeof(sockaddr));
  if (!newAddress)
  {
    free(newInfo);
    return 0;
  }

  *newAddress = *(from->ai_addr);
  *newInfo = *from;
  newInfo->ai_addr = newAddress;
  newInfo->ai_next = 0;

  return newInfo;
}

IPAddr::IPAddr() : m_info(0)
{
}

IPAddr::IPAddr(const IPAddr& copyFrom)
{
  CloneAddrinfo(copyFrom.m_info);
}

const IPAddr& IPAddr::operator=(const IPAddr& copyFrom)
{
  if (m_info)
  {
    freeaddrinfo(m_info);
    m_info = 0;
  }
  CloneAddrinfo(copyFrom.m_info);

  return *this;
}

IPAddr* IPAddr::Clone() const
{
  IPAddr* newAddr = new IPAddr(*this);
  return newAddr;
}

void IPAddr::CloneAddrinfo(const addrinfo* from)
{
  if (from == 0)
  {
    m_info = 0;
    return;
  }

  addrinfo* last = 0;
  for (const addrinfo* p = from; p != 0; p = p->ai_next)
  {
    if (!last)
    {
      this->m_info = duplicateAddrinfo(p);
      last = this->m_info;
    }
    else
    {
      last->ai_next = duplicateAddrinfo(p);
      last = last->ai_next;
    }
  }
}

IPAddr::~IPAddr()
{
  if (m_info)
  {
    freeaddrinfo(m_info);
  }
}

const addrinfo* IPAddr::GetAddr() const
{
  return m_info;
}

const char* IPAddr::GetAddrStr(char* buffer, size_t length) const
{
  void* ptr;
  
  switch (m_info->ai_family)
  {
    case AF_INET:
      ptr = &((struct sockaddr_in *)m_info->ai_addr)->sin_addr;
      break;
    case AF_INET6:
      ptr = &((struct sockaddr_in6 *)m_info->ai_addr)->sin6_addr;
      break;
  }

  return inet_ntop(m_info->ai_family, ptr, buffer, length);
}

unsigned short IPAddr::GetPort() const
{
  return ntohs(((sockaddr_in *)m_info->ai_addr)->sin_port);
}
