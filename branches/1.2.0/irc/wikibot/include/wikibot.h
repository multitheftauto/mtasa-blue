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
 *     * Neither the name of the Mantis Bot nor the names of its contributors may be used to endorse or
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

#ifndef __WIKIBOT_H
#define __WIKIBOT_H

#include <string>
#include <vector>
#include <rsl/net/socket/ipv4.h>
#include <rsl/net/irc/client.h>
#include <rsl/net/irc/text.h>
#include "configuration.h"
#include "syntax.h"

#define DEFAULT_CONFIG_FILE "/etc/wikibot.conf"

extern void ShowHelp(int argc, char* argv[]);
extern void debugentry(const char* msg, ...);
extern void debugok();
extern void debugerror();

class WikiBot
{
  friend void* CheckForChanges_thread(void*);

public:
  static WikiBot* Instance();

public:
  WikiBot();
  ~WikiBot();

  bool Initialize(int argc, char* argv[], char* environment[]);
  bool Run();
  void Close(const std::string& quitMessage = "");

  int Errno() const;
  const char* Error() const;
  bool Ok() const;

  void OnBotConnect();
  void SendChannel(const Rsl::Net::IRC::IRCText& msg, const char* channel = 0);
  const Config& GetConfig() const;

  void OnWikiRequest(const Rsl::Net::IRC::IRCUser* source, const std::string& channel, const std::string& funcName);

private:
  void Daemonize();

  static void* WikiRequest_Thread ( void* _info );
  void WikiRequest ( const std::string& source, const std::string& channel, const std::string& funcName );

private:
  Rsl::Net::IRC::IRCClient m_client;
  Config m_config;
  std::string m_error;
  int m_errno;
  bool m_daemonize;
  bool m_initialized;
  Rsl::Net::Socket::IPV4Addr m_httpWikiAddr;
  Rsl::Net::Socket::IPV4Addr m_httpWikiBindAddr;
};

#endif /* #ifndef __WIKIBOT_H */
