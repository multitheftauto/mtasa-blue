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

#include "user.h"

using Rsl::Net::IRC::IRCUser;

IRCUser::IRCUser()
{
}

IRCUser::IRCUser(const IRCUser& copy)
  : m_name(copy.m_name), m_username(copy.m_username),
    m_host(copy.m_host), m_type(copy.m_type)
{
}

IRCUser::IRCUser(const std::string& name)
{
  SetName(name);
}

IRCUser::IRCUser(const char* name)
{
  SetName(name);
}

const std::string& IRCUser::GetName() const
{
  return m_name;
}

const std::string& IRCUser::GetUsername() const
{
  return m_username;
}

const std::string& IRCUser::GetHost() const
{
  return m_host;
}

void IRCUser::SetName(const char* name)
{
  SetName(std::string(name));
}

void IRCUser::SetName(const std::string& name)
{
  m_name = name;
  int i;
  
  if (m_name[0] == '#')
  {
    m_type = IRCUSER_CHANNEL;
  }
  else if ((i = m_name.find("!")) != -1)
  {
    int x;
    if ((x = name.find("@")) != -1)
    {
      m_host = m_name.substr(x + 1);
      m_username = m_name.substr(i + 1, x - i - 1);
      m_name.erase(i);
      m_type = IRCUSER_USER;
    }
  }
  else if ((i = m_name.find(".")) != -1)
  {
    m_type = IRCUSER_SERVER;
  }
  else
  {
    m_type = IRCUSER_USER;
  }
}

IRCUserType IRCUser::GetType() const
{
  return m_type;
}

