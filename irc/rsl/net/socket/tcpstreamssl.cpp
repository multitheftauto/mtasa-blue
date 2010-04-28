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

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "tcpstreamssl.h"

using Rsl::Net::Socket::SocketStreamTCPSSL;
using Rsl::Net::Socket::SocketStream;

static bool SSL_Initialized = false;

SocketStreamTCPSSL::SocketStreamTCPSSL(int handle)
  : SocketStream(handle), m_ssl(0), m_certFile(0), m_verifyCbk(0)
{
  if (!SSL_Initialized)
  {
    SSL_Initialized = true;
    SSL_library_init();
    SSL_load_error_strings();
  }
}

SocketStreamTCPSSL::~SocketStreamTCPSSL()
{
  if (m_ssl)
  {
    SSL_shutdown(m_ssl);
    m_ssl = 0;
  }
}

void SocketStreamTCPSSL::SetCertFile(const char* certFile)
{
  m_certFile = certFile;
}

void SocketStreamTCPSSL::SetVerifyCallback(Rsl_SSL_Verify_CBK verifyCbk)
{
  m_verifyCbk = verifyCbk;
}

void SocketStreamTCPSSL::SetHandle(int handle)
{
  SocketStream::SetHandle(handle);

  if (m_ssl)
  {
    SSL_shutdown(m_ssl);
    m_ssl = 0;
  }

  if (!m_ssl && handle != -1)
  {
    SSL_METHOD* method = SSLv23_method();
    SSL_CTX* ctx = SSL_CTX_new(method);

    if (m_certFile)
    {
      if (!SSL_CTX_load_verify_locations(ctx, m_certFile, 0))
      {
        m_errno = ERR_get_error();
        return;
      }
    }
    else
    {
      if (!SSL_CTX_load_verify_locations(ctx, 0, "/etc/ssl/certs"))
      {
        m_errno = ERR_get_error();
        return;
      }
    }

    if (m_verifyCbk)
      SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, m_verifyCbk);
    else
      SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
    SSL_CTX_set_verify_depth(ctx, 1);

    m_ssl = SSL_new(ctx);
    SSL_set_fd(m_ssl, m_handle);

    if (SSL_connect(m_ssl) < 1)
    {
      m_errno = ERR_get_error();
      return;
    }
  }
}

const char* SocketStreamTCPSSL::Error() const
{
  static char buffer[2048];
  return ERR_error_string(m_errno, buffer);
}

ssize_t SocketStreamTCPSSL::QueueLength()
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

ssize_t SocketStreamTCPSSL::Read(char* buffer, size_t maxLength)
{
  ssize_t len = -1;

  if (Ok())
  {
    len = SSL_read(m_ssl, buffer, maxLength);
    if (len == 0)
    {
      m_eof = true;
    }
    else if (len < 0)
    {
      m_errno = ERR_get_error();
    }
    else
    {
      m_inbytes += len;
    }
  }

  return len;
}

ssize_t SocketStreamTCPSSL::Write(const char* buffer, size_t length)
{
  ssize_t len = -1;

  if (Ok())
  {
    len = SSL_write(m_ssl, buffer, length);
    if (len == -1)
    {
      m_errno = ERR_get_error();
    }
    else
    {
      m_outbytes += len;
    }
  }

  return len;
}

ssize_t SocketStreamTCPSSL::WriteString(const std::string& str)
{
  return Write(str.c_str(), str.length());
}

