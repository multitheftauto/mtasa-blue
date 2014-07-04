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

#ifndef _RSL_NET_SOCKET_UDPSTREAM_H
#define _RSL_NET_SOCKET_UDPSTREAM_H

#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include "socketstream.h"

namespace Rsl { namespace Net { namespace Socket {

  class SocketStreamUDP : public SocketStream
  {
    friend class SocketClientUDP;

    private:
      SocketStreamUDP(int handle = -1, const addrinfo* addr = 0);
      void SetHandle(int handle);
      const addrinfo* m_addr;
      void SetAddr(const addrinfo* addr);

    public:
      ssize_t Read(char* buffer, size_t maxLength);
      ssize_t Write(const char* buffer, size_t length);
      ssize_t WriteString(const std::string& str);
      ssize_t QueueLength();

      bool Ok() const;
  };

}; }; };

#endif /* #ifndef _RSL_NET_SOCKET_UDPSTREAM_H */
