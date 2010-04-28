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

#ifndef _RSL_NET_HTTP_STREAM_H
#define _RSL_NET_HTTP_STREAM_H

#include "../../stream/stream.h"
#include "../socket/tcpclient.h"

namespace Rsl { namespace Net { namespace HTTP {

  class HTTPStream : public Rsl::Stream
  {
    friend class HTTPClient;

  private:
    void SetSocket(Rsl::Net::Socket::SocketClientTCP* socket,
                   const std::string& bufferedData,
                   ssize_t contentLength);
    Rsl::Net::Socket::SocketClientTCP* m_socket;
    std::string m_buffer;
    ssize_t m_contentLength;

  public:
    HTTPStream();
    virtual ~HTTPStream();

    virtual ssize_t Write(const char* buffer, size_t length) { }
    virtual ssize_t WriteString(const std::string& str) { }
    virtual ssize_t Read(char* buffer, size_t maxLength);

    virtual int Errno() const;
    virtual const char* Error() const;
    virtual bool Ok() const;
    virtual bool Eof() const;

    virtual size_t InBytes() const;
    virtual size_t OutBytes() const;

  private:
    int m_errno;
    std::string m_error;
  };

}; }; };

#endif /* #ifndef _RSL_NET_HTTP_STREAM_H */
