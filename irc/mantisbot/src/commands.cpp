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

#include <string>
#include <vector>
#include <rsl/net/socket/ipv4.h>
#include <rsl/net/http/client.h>
#include <pthread.h>
#include "commands.h"
#include "mantisbot.h"
#include "keys.h"

using namespace Rsl::Net::HTTP;
using Rsl::Net::Socket::IPV4Addr;
using namespace Rsl::Net::IRC;

#define COMMAND(x) void CommandHandler::cmd_ ## x (MantisBot* bot, const IRCUser* source, const IRCUser* dest, const std::vector<std::string>& params)


/****
 **** COMMANDS
 ****/
static std::string cookie("");

static inline void MantisLogin(const IRCUser& source, const char* channel)
{
  if (cookie == "")
  {
    debugentry("Initial login in mantis");
    MantisBot* bot = MantisBot::Instance();
    char password[1024];
    char poststr[1024];
    const Config& config = bot->GetConfig();
    IPV4Addr addr(config.data.mantis.address, config.data.mantis.service);
    IPV4Addr bindAddr("0.0.0.0", "0");

    keysDecode(config.data.mantis.password, password);
    snprintf(poststr, 1024, "username=%s&password=%s", config.data.mantis.username, password);
    memset(password, 0, sizeof(password));

    HTTPClient login(config.data.mantis.address, addr, bindAddr, config.data.mantis.ssl);
    login.SendHeader("Content-Type", "application/x-www-form-urlencoded");
    login.Connect("/login.php", HTTP_METHOD_POST);
    login.Send(poststr);
    memset(poststr, 0, sizeof(poststr));

    if (!login.Ok() || !login.GetStream().Ok())
    {
      bot->SendChannel(IRCText("%s: Error logging in mantis", source.GetName().c_str()), channel);
      puts("Unable to connect to the login page!");
      return;
    }

    const std::vector<HTTPHeader>& headers = login.GetResponseHeaders();
    for (std::vector<HTTPHeader>::const_iterator i = headers.begin();
         i != headers.end();
         i++)
    {
      const HTTPHeader* header = &(*i);
      if (header->name == "Set-Cookie" && !strncmp(header->value.c_str(), "MANTIS_STRING_COOKIE=", 21))
      {
        const char* p = strchr(header->value.c_str() + 21, ';');
        if (!p)
        {
          bot->SendChannel(IRCText("%s: Error parsing the login response", source.GetName().c_str()), channel);
          puts("Unable to parse the login response");
          return;
        }
        cookie.append(header->value.c_str() + 21, p - header->value.c_str() - 21);
        break;
      }
    }

    if (cookie == "")
    {
      bot->SendChannel(IRCText("%s: Error retreiving the login cookie in mantis", source.GetName().c_str()), channel);
      puts("Unable to retreive the login cookie");
      return;
    }

    HTTPClient cookieTest(config.data.mantis.address, addr, bindAddr, config.data.mantis.ssl);
    cookieTest.SendHeader("Cookie", std::string("MANTIS_STRING_COOKIE=") + cookie);
    cookieTest.Connect("/login_cookie_test.php", HTTP_METHOD_GET);
    cookieTest.Send();

    if (cookieTest.ResponseStatus() != 200 && cookieTest.ResponseStatus() != 302)
    {
      bot->SendChannel(IRCText("%s: Error verifying the mantis login cookie (%d - %s)",
                               source.GetName().c_str(), cookieTest.ResponseStatus(), cookieTest.StatusText()), channel);
      cookie = "";
      puts("Unable to verify the login cookie");
      return;
    }
    debugok();
  }
}

/* !issue */
struct issuecmdData
{
  int number;
  IRCUser source;
  IRCUser dest;
};

static inline void dumpHTTPOutput(HTTPClient& cli, std::string& output)
{
  char buffer[512];
  ssize_t len;
  HTTPStream& stream = cli.GetStream();

  while (!stream.Eof())
  {
    len = stream.Read(buffer, sizeof(buffer));
    if (len > 0)
      output.append(buffer, len);
  }
}

static void* issuecmdThread(void* data_)
{
  MantisBot* bot = MantisBot::Instance();
  issuecmdData* data = (issuecmdData *)data_;
  const Config& config = bot->GetConfig();

  if(data->number > 9000) {
    bot->SendChannel(IRCText("%s: It's over 9000 %BD:%B", data->source.GetName().c_str()), data->dest.GetName().c_str());
    delete data;
    return 0;
  }


  IPV4Addr addr(config.data.mantis.address, config.data.mantis.service);
  IPV4Addr bindAddr("0.0.0.0", "0");

  MantisLogin(data->source, data->dest.GetName().c_str());

  if (cookie == "")
  {
    delete data;
    return 0;
  }

  char path[1024];
  snprintf(path, sizeof(path), "/view.php?id=%d", data->number);
  HTTPClient client(config.data.mantis.address, addr, bindAddr, config.data.mantis.ssl);
  client.SendHeader("Cookie", std::string("MANTIS_STRING_COOKIE=") + cookie);
  client.Connect(path, HTTP_METHOD_GET);
  client.Send();

  if (!client.Ok() || !client.GetStream().Ok())
  {
    bot->SendChannel(IRCText("%s: Cannot connect to mantis web", data->source.GetName().c_str()), data->dest.GetName().c_str());
    delete data;
    return 0;
  }

  std::string output;
  dumpHTTPOutput(client, output);

  int p;
  if (output.find("APPLICATION ERROR #1100") != -1 || output.find("Access Denied") != -1)
  {
    bot->SendChannel(IRCText("%s: %C04Issue not found%C", data->source.GetName().c_str()), data->dest.GetName().c_str());
    delete data;
    return 0;
  }

  std::string summary("");
  std::string status("");
  std::string updated("");

  p = output.find("<!-- Summary -->");
  if (p != -1)
  {
    char n[32];
    snprintf(n, sizeof(n), "%07d: ", data->number);
    p = output.find(n, p);
    if (p != -1)
    {
      int begin = p + 9;
      p = output.find("\t</td>", begin);
      if (p != -1)
      {
        summary = output.substr(begin, p - begin);
      }
    }
  }

  if (summary != "")
  {
    p = output.find("<!-- Status -->");
    if (p != -1)
    {
      p = output.find("\t\t", p);
      if (p != -1)
      {
        p = output.find("\t\t", p + 2);
        if (p != -1)
        {
          int begin = p + 2;
          p = output.find("\t", begin);
          if (p != -1)
          {
            status = output.substr(begin, p - begin);
          }
        }
      }
    }
  }

  if (status != "")
  {
    p = output.find("<!-- Date Updated -->");
    if (p != -1)
    {
      p = output.find("\t\t", p);
      if (p != -1)
      {
        int begin = p + 2;
        p = output.find("\t", begin);
        if (p != -1)
        {
          updated = output.substr(begin, p - begin);
        }
      }
    }
  }

  if (updated != "" && summary != "" && status != "")
  {
    bot->SendChannel(IRCText("%s: %C02Issue #%d:%C "
                             "%B%C12%s%B%C %C03-%C "
                             "%C02Status:%C %s %C03-%C "
                             "%C02Last updated:%C %s %C03-%C "
                             "%C14%s://%s/view.php?id=%d%C",
                             data->source.GetName().c_str(), data->number,
                             summary.c_str(), status.c_str(), updated.c_str(),
                             config.data.mantis.service, config.data.mantis.address,
                             data->number), data->dest.GetName().c_str());
  }

  delete data;
  return 0;
}

COMMAND(issue)
{
  if (params.size() > 1)
  {
    int number = atoi(params[1].c_str());
    if (number > 0)
    {
      issuecmdData* data = new issuecmdData;
      data->number = number;
      data->source = *source;
      data->dest = *dest;
      pthread_t thread;
      pthread_create(&thread, 0, issuecmdThread, data);
    }
  }
}
/* */





CommandHandler* CommandHandler::Instance()
{
  static CommandHandler* instance = 0;
  if (instance == 0)
    instance = new CommandHandler();
  return instance;
}

CommandHandler::CommandHandler()
{
#define ADDCOMMAND(x) RegisterCommand( #x , CommandHandler::cmd_ ##x )
  ADDCOMMAND(issue);
#undef ADDCOMMAND
}

void CommandHandler::RegisterCommand(const std::string& name, cmd_t callback)
{
  Command cmd;
  cmd.name = name;
  cmd.callback = callback;
  m_commands.push_back(cmd);
}

static inline void Split(const std::string& str, std::vector<std::string>& dest)
{
  int p;
  int i = 0;

  do
  {
    while (str[i] == ' ')
    {
      i++;
    }

    p = str.find(" ", i);
    if (p > -1)
    {
      dest.push_back(str.substr(i, p - i));
      i = p + 1;
    }
  } while (p > -1);

  if (str[i] != '\0')
  {
    dest.push_back(str.substr(i));
  }
}

void CommandHandler::Handle(const IRCUser* source, const IRCUser* dest, const std::string& text)
{
  if (text[0] != '!' || text[1] == '\0')
    return;

  std::vector<std::string> params;
  Split(text.c_str() + 1, params);

  for (std::vector<Command>::iterator i = m_commands.begin();
       i != m_commands.end();
       i++)
  {
    if (!strcasecmp((*i).name.c_str(), params[0].c_str()))
    {
      (*i).callback(MantisBot::Instance(), source, dest, params);
      break;
    }
  }
}

#undef COMMAND
