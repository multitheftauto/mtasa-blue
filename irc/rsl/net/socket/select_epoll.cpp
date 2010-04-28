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
#include <string.h>
#include "select.h"
#include "select_epoll.h"

using Rsl::Net::Socket::SocketSelect;

SocketSelect::SocketSelect(int maxSockets)
{
  m_epoll = epoll_create(maxSockets);
  if (m_epoll < 0)
  {
    m_errno = errno;
    return;
  }

  m_timeout = -1;
  m_errno = 0;
}

SocketSelect::~SocketSelect()
{
  close(m_epoll);
}

int SocketSelect::Errno() const
{
  return m_errno;
}

const char* SocketSelect::Error() const
{
  return strerror(m_errno);
}

bool SocketSelect::Ok() const
{
  return !m_errno;
}

static inline int GetEventFlags(int events)
{
  int ret = 0;
  if (events & RSL_SELECT_EVENT_IN)
    ret |= EPOLLIN;
  if (events & RSL_SELECT_EVENT_OUT)
    ret |= EPOLLOUT;
  return ret;
}

bool SocketSelect::Add(SocketClient* socket, int events)
{
  epoll_event evt;
  evt.events = GetEventFlags(events);
  evt.data.ptr = socket;
  if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, socket->GetHandler(), &evt) < 0)
  {
    m_errno = errno;
    return false;
  }
  return true;
}

bool SocketSelect::Del(SocketClient* socket)
{
  if (epoll_ctl(m_epoll, EPOLL_CTL_DEL, socket->GetHandler(), 0) < 0)
  {
    m_errno = errno;
    return false;
  }
  return true;
}

bool SocketSelect::SetEvents(SocketClient* socket, int events)
{
  epoll_event evt;
  evt.events = GetEventFlags(events);
  evt.data.ptr = socket;
  if (epoll_ctl(m_epoll, EPOLL_CTL_MOD, socket->GetHandler(), &evt) < 0)
  {
    m_errno = errno;
    return false;
  }
  return true;
}

void SocketSelect::SetTimeout(int timeout)
{
  m_timeout = timeout;
}

int SocketSelect::Select(SocketClient* dest[], int size)
{
  epoll_event evts[size];
  int num;

  num = epoll_wait(m_epoll, evts, size, m_timeout);
  if (num < 0)
  {
    if (errno == EINTR)
      return 0;
    m_errno = errno;
    return -1;
  }

  for (int i = 0; i < num; i++)
  {
    SocketClient* socket = (SocketClient *)evts[i].data.ptr;

    if (evts[i].events & (EPOLLERR | EPOLLHUP))
    {
      this->Del(socket);
      socket->Close();
    }
    dest[i] = socket;
  }

  return num;
}

