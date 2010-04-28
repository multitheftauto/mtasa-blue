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
 *     * Neither the name of the WikiBot nor the names of its contributors may be used to endorse or
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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "configuration.h"

using namespace Rsl::File::Ini;

Config::Config()
  : m_errno(0), m_error("")
{
}

Config::~Config()
{
}

bool Config::Create(const char* fileName)
{
  m_parser.SetFilename(fileName);
  if (!m_parser.Ok())
  {
    m_errno = m_parser.Errno();
    m_error = m_parser.Error();
    return false;
  }

  const char* v;
#define SAFE_LOAD(section, entry, dest) do { \
  v = m_parser.GetValue(#section, #entry); \
  if (v == 0) \
  { \
    m_errno = -1; \
    m_error = "Unable to load entry '" #entry "' from section '" #section "'"; \
    return false; \
  } \
  (dest) = v; \
} while ( false )

  /* ircserver */
  SAFE_LOAD(ircserver, address, this->data.ircserver.address);
  SAFE_LOAD(ircserver, service, this->data.ircserver.service);
  SAFE_LOAD(ircserver, password, this->data.ircserver.password);
  SAFE_LOAD(ircserver, ssl, v);
  this->data.ircserver.ssl = (!strcasecmp(v, "true") ? true : false);
  SAFE_LOAD(ircserver, reconnectdelay, v);
  this->data.ircserver.reconnectdelay = atoi(v);

  /* bot */
  SAFE_LOAD(bot, nickname, this->data.bot.nickname);
  SAFE_LOAD(bot, username, this->data.bot.username);
  SAFE_LOAD(bot, fullname, this->data.bot.fullname);
  SAFE_LOAD(bot, password, this->data.bot.password);

  /* channels */
  char sectionName[128];
  const char* channelName;
  const char* channelPw;

  for (int i = 1; true; i++)
  {
    snprintf(sectionName, sizeof(sectionName), "channel%d", i);
    channelName = m_parser.GetValue(sectionName, "name");
    channelPw = m_parser.GetValue(sectionName, "password");

    if (channelName)
    {
      __ConfigChannel ch;
      ch.name = channelName;
      ch.password = channelPw;
      this->data.channels.push_back(ch);
    }
    else
      break;
  }

  /* wiki */
  SAFE_LOAD(wiki, address, this->data.wiki.address);
  SAFE_LOAD(wiki, service, this->data.wiki.service);
  SAFE_LOAD(wiki, ssl, v);
  this->data.wiki.ssl = (!strcasecmp(v, "true") ? true : false);
  SAFE_LOAD(wiki, path, this->data.wiki.path);

#undef SAFE_LOAD
  return true;
}

bool Config::Ok() const
{
  return !m_errno;
}

int Config::Errno() const
{
  return m_errno;
}

const char* Config::Error() const
{
  return m_error.c_str();
}
