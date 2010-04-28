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

#include "client.h"
#include "user.h"
#include "../socket/socketstream.h"
#include <errno.h>

using Rsl::Net::IRC::IRCClient;
using Rsl::Net::IRC::IRCUser;
using Rsl::Net::Socket::IPAddr;
using Rsl::Net::Socket::SocketClientTCP;
using Rsl::Net::Socket::SocketClientTCPSSL;
using Rsl::Net::Socket::SocketStream;

IRCClient::IRCClient()
  : m_errno(0), m_error(""), m_connected(false)
{
}

IRCClient::IRCClient(const IPAddr& addr, const IPAddr& bindAddr,
                     const std::string& nickname,
                     const std::string& username,
                     const std::string& fullname,
                     const std::string& password,
                     bool useSSL,
                     const char* certFile)
  : m_errno(0), m_error(""), m_connected(false)
{
  Create(addr, bindAddr, nickname, username, fullname, password, useSSL, certFile);
}

IRCClient::~IRCClient()
{
  std::map<IRCMessage *, IRCMessage_callback_t>::const_iterator i;
  
  for (i = m_callbacks.begin();
       i != m_callbacks.end();
       i++)
  {
    delete (*i).first;
  }

  delete m_sock;
}

void IRCClient::Create(const IPAddr& addr, const IPAddr& bindAddr,
                       const std::string& nickname,
                       const std::string& username,
                       const std::string& fullname,
                       const std::string& password,
                       bool useSSL,
                       const char* certFile)
{
  m_nickname = nickname;
  m_username = username;
  m_fullname = fullname;
  m_password = password;
  m_me.SetName(m_nickname + "!" + m_username + "@0.0.0.0");
  m_numericsCallback = 0;

  if (useSSL)
    m_sock = new SocketClientTCPSSL(addr, bindAddr, certFile);
  else
    m_sock = new SocketClientTCP(addr, bindAddr);
}

bool IRCClient::Connect()
{
  if (Ok())
  {
    if (!m_sock->Ok())
    {
      m_errno = m_sock->Errno();
      m_error = m_sock->Error();
      return false;
    }
    else if (!m_sock->Connect())
    {
      m_errno = m_sock->Errno();
      m_error = m_sock->Error();
      return false;
    }

    if (m_password.length())
    {
      Send(IRCMessagePass(m_password));
    }
    Send(IRCMessageUser(m_username, m_fullname));
    Send(IRCMessageNick(m_nickname));

    m_connected = true;
    return true;
  }
  
  return false;
}

void IRCClient::Close()
{
  if (m_sock)
  {
    m_sock->Close();
  }

  m_errno = 0;
  m_error = "";
  m_connected = false;
}

bool IRCClient::IsConnected() const
{
  return m_connected;
}

ssize_t IRCClient::Loop()
{
  char buffer[2049];
  ssize_t len;
  int i;

  SocketStream& stream = m_sock->GetStream();

  if (!m_sock->Ok() || !stream.Ok())
  {
    m_connected = false;
    return -1;
  }

  len = stream.Read(buffer, sizeof(buffer) - 1);
  if (len == -1)
  {
    m_errno = stream.Errno();
    m_error = stream.Error();
    m_connected = false;
    return -1;
  }
  buffer[len] = '\0';
  m_buffer.append(buffer);

  while ((i = m_buffer.find(RSLIRC_CRLF)) > -1)
  {
    DispatchMessage(m_buffer.substr(0, i));
    m_buffer.erase(0, i + 2);
  }

  return len;
}

static bool IsNum(const char* str)
{
  do
  {
    if (!isdigit(*str))
      return false;
  } while (*++str);
  return true;
}

void IRCClient::DispatchMessage(const std::string& msg)
{
  int i = 0;
  int p;
  int p2;
  bool isNumeric = false;
  std::string source;
  std::string cmd;
  std::string dest;
  std::string text;
  std::map<IRCMessage *, IRCMessage_callback_t>::const_iterator it;

  if (msg[0] == ':')
  {
    /* We have source */
    p = msg.find(" ");
    if (p == -1)
    {
      /* Invalid message */
      return;
    }
    source = msg.substr(1, p);
    i = p + 1;
  }

  p = msg.find(" ", i);
  if (p == -1)
  {
    /* Invalid message */
    return;
  }
  cmd = msg.substr(i, p - i);
  i = p + 1;

  if (IsNum(cmd.c_str()))
  {
    /* We have a numeric server message */
    isNumeric = true;
  }

  if (msg[i] == ':')
  {
    /* We haven't destination */
    text = msg.substr(i + 1);
  }
  else
  {
    p = msg.find(" ", i);
    if (p == -1)
    {
      dest = msg.substr(i);
    }
    else
    {
      dest = msg.substr(i, p - i);
      i = p + 1;

      if (msg[i] == ':')
      {
        i++;
      }
      text = msg.substr(i);
    }
  }

  /* Find callback */
  IRCUser p_source(source);
  IRCUser p_dest(dest);
  IRCText p_text;
  p_text.SetRawText(text);


  if (isNumeric)
  {
    if (m_numericsCallback)
    {
      IRCMessageNumeric num;
      ((IRCMessage *)&num)->IsValidCmd(cmd, p_source, p_dest, p_text);
      m_numericsCallback(this, &p_source, &num);
    }
  }
  else
  {
    for (it = m_callbacks.begin();
         it != m_callbacks.end();
         it++)
    {
      IRCMessage *current = (*it).first;
      if (current->IsValidCmd(cmd, p_source, p_dest, p_text))
      {
        IRCMessage_callback_t callback = (*it).second;
        callback(this, &p_source, current);
        break;
      }
    }
  }
}

bool IRCClient::Ok() const
{
  return !m_errno && (m_nickname.length() > 0) && (m_username.length() > 0) && (m_fullname.length() > 0);
}

SocketClientTCP& IRCClient::GetSocket()
{
  return *m_sock;
}

int IRCClient::Errno() const
{
  return m_errno;
}

const char* IRCClient::Error() const
{
  return m_error;
}

void IRCClient::RegisterCallback(IRCMessage* message, IRCMessage_callback_t callback)
{
  m_callbacks[message] = callback;
}

void IRCClient::RegisterNumericsCallback(IRCMessage_callback_t callback)
{
  m_numericsCallback = callback;
}

void IRCClient::Send(const IRCMessage& message)
{
  SocketStream& stream = m_sock->GetStream();
  if (stream.Ok())
  {
    std::string str;
    message.ConstructMessage(str);

    stream.WriteString(str);
  }
}

bool IRCClient::IsMe(const IRCUser& usr) const
{
  return (usr.GetType() == IRCUSER_USER) && (usr.GetName() == m_nickname);
}

const IRCUser& IRCClient::GetMe() const
{
  return m_me;
}

void IRCClient::ChangeNick(const IRCUser& newnick)
{
  m_nickname = newnick.GetName();
  m_me.SetName(m_nickname + "!" + m_username + "@0.0.0.0");
  Send(IRCMessageNick(m_nickname));
}
