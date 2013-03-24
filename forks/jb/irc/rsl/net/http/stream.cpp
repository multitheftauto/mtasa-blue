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

#include <string.h>
#include "stream.h"
#include "../socket/tcpclient.h"
#include "../socket/tcpstream.h"

using Rsl::Net::HTTP::HTTPStream;
using Rsl::Net::Socket::SocketClientTCP;
using Rsl::Net::Socket::SocketStream;

HTTPStream::HTTPStream()
  : m_socket(0), m_errno(0), m_error(""), m_buffer("")
{
}

HTTPStream::~HTTPStream()
{
}

void HTTPStream::SetSocket(SocketClientTCP* socket, const std::string& bufferData, ssize_t contentLength)
{
  m_socket = socket;
  m_buffer = bufferData;
  m_contentLength = contentLength;
}

ssize_t HTTPStream::Read(char* buffer, size_t maxLength)
{
  char* p = buffer;
  size_t copySize = 0;

  if (maxLength < 1 || m_contentLength == 0)
    return 0;

  if (m_buffer.length() > 0)
  {
    if (m_buffer.length() < maxLength)
      copySize = m_buffer.length();
    else
      copySize = maxLength;

    if (m_contentLength > -1)
    {
      if (copySize > m_contentLength)
        copySize = m_contentLength;
      m_contentLength -= copySize;
    }

    memcpy(p, m_buffer.c_str(), copySize * sizeof(char));
    m_buffer.erase(0, copySize);
    p += copySize;

    if (maxLength == copySize || m_contentLength == 0)
      return copySize;
  }

  SocketStream& stream = m_socket->GetStream();

  size_t readSize = maxLength - copySize;
  if (m_contentLength > -1 && readSize > m_contentLength)
    readSize = m_contentLength;

  size_t numBytes = stream.Read(p, readSize);
  if (numBytes == -1)
  {
    m_errno = stream.Errno();
    m_error = stream.Error();
    return (copySize == 0 ? -1 : copySize);
  }

  if (m_contentLength > -1)
    m_contentLength -= numBytes;

  return copySize + numBytes;
}

int HTTPStream::Errno() const
{
  return m_errno;
}

const char* HTTPStream::Error() const
{
  return m_error.c_str();
}

bool HTTPStream::Ok() const
{
  return !m_errno && m_socket && m_socket->Ok();
}

bool HTTPStream::Eof() const
{
  return !m_socket || m_errno || m_socket->GetStream().Eof() || m_contentLength == 0;
}

size_t HTTPStream::InBytes() const
{
  if (!m_socket)
    return 0;
  return m_socket->GetStream().InBytes() - m_buffer.length();
}

size_t HTTPStream::OutBytes() const
{
  if (!m_socket)
    return 0;
  return m_socket->GetStream().OutBytes();
}

