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

#include <string>
#include <vector>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <rsl/net/http/client.h>
#include <rsl/net/socket/ipv4.h>
#include <rsl/net/socket/select.h>
#include "wikibot.h"
#include "configuration.h"
#include "keys.h"

// HTTP download buffer size
#define HTTP_BUFFER_SIZE   1024

using namespace Rsl::Net::IRC;
using namespace Rsl::Net::Socket;
using namespace Rsl::Net::HTTP;

static void TrimSpaces( std::string &str )
{   // http://sarathc.wordpress.com/2007/01/31/how-to-trim-leading-or-trailing-spaces-of-string-in-c/
    size_t startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces  
    size_t endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse af  
 
    // if all spaces or empty return an empty string  
    if(( std::string::npos == startpos ) || ( std::string::npos == endpos))  
    {  
        str = "";
    }  
    else  
        str = str.substr( startpos, endpos-startpos+1 );  

    /* ryden: Remove duplicated spaces */
    int p = 0;
    int p2;
    while ( (p = str.find(" ", p)) != std::string::npos )
    {
      p2 = p + 1;
      while ( str[p2] == ' ' ) ++p2;

      if ( p2 > p + 1 )
      {
        str = str.substr(0, p+1) + str.substr(p2);
      }

      ++p;
    }
}  

static inline void Split(const std::string& str, std::vector<std::string>& dest, char separator)
{
  int p;
  int i = 0;

  do
  {
    while (str[i] == separator)
    {
      i++;
    }

    p = str.find(separator, i);
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

/* Bot callbacks */
static int do_ping(IRCClient* irc, const IRCUser* source, const IRCMessage* _msg)
{
  const IRCMessagePing* msg = dynamic_cast<const IRCMessagePing *>(_msg);

  if (msg)
  {
    irc->Send(IRCMessagePong(msg->GetText()));
  }
}

static int do_numeric(IRCClient* irc, const IRCUser* source, const IRCMessage* _msg)
{
  const IRCMessageNumeric* msg = dynamic_cast<const IRCMessageNumeric *>(_msg);

  if (msg)
  {
    switch (msg->GetNumeric())
    {
      case 1:
        WikiBot::Instance()->OnBotConnect();
        break;
    }
  }
}

static int do_privmsg(IRCClient* irc, const IRCUser* source, const IRCMessage* _msg)
{
  const IRCMessagePrivmsg* msg = dynamic_cast<const IRCMessagePrivmsg *>(_msg);

  if (msg && msg->GetDest().GetType() == IRCUSER_CHANNEL)
  {
    std::string text(msg->GetText().GetText());
    TrimSpaces(text);

    if ( text.length() > 5 && !strncasecmp ( text.c_str(), "wiki ", 5 ) )
    {
      size_t p = text.find ( " ", 6 );
      if ( p != std::string::npos )
      {
        text = text.substr(0, p);
      }
      WikiBot::Instance()->OnWikiRequest(source, msg->GetDest().GetName(), text.substr(5));
    }
  }
}
/* */


WikiBot* WikiBot::Instance()
{
  static WikiBot* instance = 0;
  if (instance == 0)
    instance = new WikiBot();
  return instance;
}

WikiBot::WikiBot()
  : m_error(""), m_errno(0), m_initialized(false)
{
}

WikiBot::~WikiBot()
{
}

int WikiBot::Errno() const
{
  return m_errno;
}

const char* WikiBot::Error() const
{
  return m_error.c_str();
}

bool WikiBot::Ok() const
{
  return !m_errno;
}

void WikiBot::Daemonize()
{
  bool intty = isatty(0) && isatty(1) && isatty(2);
  if (intty)
  {
    close(0);
    close(1);
    close(2);
  }
  if (fork())
  {
    exit(EXIT_SUCCESS);
  }
}

bool WikiBot::Initialize(int argc, char* argv[], char* environment[])
{
  /* Read command-line options */
  static struct option long_options[] = {
    { "conffile",       true,   0,  'f' },
    { "help",           false,  0,  'h' },
    { "verbose",        false,  0,  'v' },
    { "reservedfile",   true,   0,  'r' },
    { 0,                0,      0,   0  },
  };
  int option_index = 0;
  int getopt_retval = 0;

  const char* configFile = 0;
  m_daemonize = true;

  while (1)
  {
    getopt_retval = getopt_long(argc, argv, "f:hvr", long_options, &option_index);
    if (getopt_retval == -1)
    {
      break;
    }

    switch (getopt_retval)
    {
      case 'f':
      {
        configFile = optarg;
        break;
      }
      case 'h':
      {
        ShowHelp(argc, argv);
        exit(EXIT_SUCCESS);
        break;
      }
      case 'v':
      {
        m_daemonize = false;
        break;
      }
      case 'r':
      {
        setReservedWordsPath ( optarg );
        break;
      }
    }
  }

  if (!configFile)
    configFile = DEFAULT_CONFIG_FILE;

  if (!m_config.Create(configFile))
  {
    m_errno = m_config.Errno();
    m_error = std::string("Unable to load the configuration file: ") + m_config.Error();
    return false;
  }

  m_httpWikiAddr.Create(m_config.data.wiki.address, m_config.data.wiki.service);
  m_httpWikiBindAddr.Create("0.0.0.0", "0");

  IPV4Addr ircaddr(m_config.data.ircserver.address, m_config.data.ircserver.service);
  if (!ircaddr.Ok())
  {
    m_errno = ircaddr.Errno();
    m_error = std::string("Cannot create the IRC address: ") + ircaddr.Error();
    return false;
  }
  IPV4Addr bindAddr("0.0.0.0", "0");
  m_client.Create(ircaddr, bindAddr,
                  m_config.data.bot.nickname,
                  m_config.data.bot.username,
                  m_config.data.bot.fullname,
                  m_config.data.ircserver.password,
                  m_config.data.ircserver.ssl);
  if (!m_client.Ok())
  {
    m_errno = m_client.Errno();
    m_error = std::string("Unable to create the irc client: ") + m_client.Error();
    return false;
  }

  m_client.RegisterCallback(new IRCMessagePing, do_ping);
  m_client.RegisterNumericsCallback(do_numeric);
  m_client.RegisterCallback(new IRCMessagePrivmsg, do_privmsg);

  return true;
}

void WikiBot::OnBotConnect()
{
  const char* pw = m_config.data.bot.password;
  size_t len = strlen(pw);

  if (len > 0)
  {
    char decodedPassword[len + 1];
    char identifyMsg[1024];

    memset(decodedPassword, 0, len + 1);
    keysDecode(pw, decodedPassword);

    snprintf(identifyMsg, sizeof(identifyMsg), "IDENTIFY %s", decodedPassword);

    m_client.Send(IRCMessagePrivmsg("NickServ", identifyMsg)); 

    /* Clear passwords from memory */
    memset(identifyMsg, 0, sizeof(identifyMsg));
    memset(decodedPassword, 0, sizeof(char) * (len + 1));
  }

  for (std::vector<__ConfigChannel>::const_iterator i = m_config.data.channels.begin();
       i != m_config.data.channels.end();
       i++)
  {
    const __ConfigChannel& ch = (*i);
    m_client.Send(IRCMessageJoin(ch.name, ch.password ? ch.password : ""));
  }
}

bool WikiBot::Run()
{
  SocketClient& socket = m_client.GetSocket();

  m_client.Connect();
  if (!m_client.Ok())
  {
    m_errno = m_client.Errno();
    m_error = std::string("Can't connect the IRC client: ") + m_client.Error();
    return false;
  }

  if (m_daemonize)
  {
    Daemonize();
    m_daemonize = false;
  }

  debugok();

  while ( true )
  {
    if ( ! m_client.Ok() )
    {
      debugentry("Some errror occurred, reconnecting bot");
      Close();
      sleep(m_config.data.ircserver.reconnectdelay);
      m_client.Connect();
      debugok();
      continue;
    }

    if ( m_client.Loop() < 0 )
    {
      m_errno = m_client.Errno();
      m_error = std::string("Can't loop the IRC client: ") + m_client.Error();
      return false;
    }
  }

  return true;
}

void WikiBot::Close(const std::string& quitMessage)
{
  if (quitMessage != "")
  {
    m_client.Send(IRCMessageQuit(quitMessage.c_str()));
  }
  m_client.Close();
}

const Config& WikiBot::GetConfig() const
{
  return m_config;
}

void WikiBot::SendChannel(const IRCText& msg, const char* channel)
{
  if (!channel)
    channel = m_config.data.channels[0].name;
  m_client.Send(IRCMessagePrivmsg(channel, msg));
}

/* Commands */
struct __CmdInfo
{
  std::string source;
  std::string channel;
  std::string funcName;
  WikiBot* bot;
  pthread_t thread;
};

void WikiBot::OnWikiRequest(const IRCUser* source, const std::string& channel, const std::string& funcName)
{
  __CmdInfo* info = new __CmdInfo;
  info->source = source->GetName();
  info->channel = channel;
  info->funcName = funcName;
  info->bot = this;

  pthread_create(&info->thread, 0, WikiRequest_Thread, info);
  pthread_detach(info->thread);
}

void* WikiBot::WikiRequest_Thread ( void* _info )
{
  __CmdInfo* info = static_cast<__CmdInfo*>(_info);
  info->bot->WikiRequest(info->source, info->channel, info->funcName);
  delete info;
  return 0;
}

void WikiBot::WikiRequest(const std::string& source, const std::string& channel, const std::string& funcName)
{
  char tmp[HTTP_BUFFER_SIZE];
  std::string buffer;
  size_t len;

  HTTPClient http(m_config.data.wiki.address, m_httpWikiAddr, m_httpWikiBindAddr, m_config.data.wiki.ssl);

  std::string path ( m_config.data.wiki.path );
  std::string funcName_ ( funcName );
  funcName_[0] = toupper(funcName_[0]);
  path.append ( funcName_ );
  http.Connect( path );
  http.Send();

  std::string fullPath = "http://";
  fullPath.append ( m_config.data.wiki.address );
  fullPath.append ( path );

  if (!http.Ok()) return;

  if (http.ResponseStatus() == 404)
  {
    SendChannel(IRCText("%B%s%B: '%s' not found. Try using the search page: "
                        "http://%s%s?search=%s&go=Go",
                        source.c_str(), funcName.c_str(), m_config.data.wiki.address,
                        m_config.data.wiki.path, funcName.c_str()), channel.c_str());
    return;
  }

  if (http.ResponseStatus() != 200)
  {
    printf("Unable to connect to the wiki: %d %s\n", http.ResponseStatus(), http.StatusText());
    return;
  }

  HTTPStream& stream = http.GetStream();

  if (!stream.Ok()) return;

  while (stream.Eof() == false)
  {
    len = stream.Read(tmp, HTTP_BUFFER_SIZE);
    buffer.append(tmp, len);
  }

  int p;
  int p2;

  p = buffer.find("There is currently no text in this page");
  if ( p != std::string::npos )
  {
    SendChannel(IRCText("%B%s%B: '%s' not found. Try using the search page: "
                        "http://%s%s?search=%s&go=Go",
                        source.c_str(), funcName.c_str(), m_config.data.wiki.address,
                        m_config.data.wiki.path, funcName.c_str()), channel.c_str());
    return;
  }

  /* Search for functions */
  p = buffer.find("Syntax</span></h2>");
  if ( p != std::string::npos )
  {
    size_t server;
    size_t client;
    size_t def;

    bool isClient = buffer.find("Client-only function") == std::string::npos ? false : true;
    bool isServer = buffer.find("Server-only function") == std::string::npos ? false : true;
    if ( buffer.find("Client and Server function") != std::string::npos )
    {
      isClient = isServer = true;
    }

    server = buffer.find("<!-- BEGIN SECTION |1|server|Server|-->", p);
    client = buffer.find("<!-- BEGIN SECTION |2|client|Client|-->", p);
    def = buffer.find("<!-- PLAIN TEXT CODE FOR BOTS |", p);

    if ( server != std::string::npos && client != std::string::npos && isClient && isServer && server < p + 45 )
    {
      /* Client and server function with different syntax */
      server = buffer.find ( "<!-- PLAIN TEXT CODE FOR BOTS |", server );
      client = buffer.find ( "<!-- PLAIN TEXT CODE FOR BOTS |", client );

      if ( server != std::string::npos && client != std::string::npos )
      {
        p2 = buffer.find ( "|", server + 31 );
        if ( p2 != std::string::npos )
        {
          std::string serverSyntax = buffer.substr ( server + 31, p2 - server - 31 );

          p2 = buffer.find ( "|", client + 31 );
          if ( p2 != std::string::npos )
          {
            std::string clientSyntax = buffer.substr ( client + 31, p2 - client - 31 );
            char colorizedSyntax [ 1024 ];

            colorizeSyntax ( serverSyntax.c_str(), colorizedSyntax, sizeof(colorizedSyntax) );
            SendChannel(IRCText("%B%C07Server%C:%B %s", colorizedSyntax), channel.c_str());
            colorizeSyntax ( clientSyntax.c_str(), colorizedSyntax, sizeof(colorizedSyntax) );
            SendChannel(IRCText("%B%C04Client%C:%B %s", colorizedSyntax), channel.c_str());
          }
        }
      }
    }

    else if ( def != std::string::npos && def - p < 20 )
    {
      p2 = buffer.find( "|", def + 31 );
      if ( p2 != std::string::npos )
      {
        std::string syntax = buffer.substr ( def + 31, p2 - def - 31 );
        char colorizedSyntax [ 1024 ];
        colorizeSyntax ( syntax.c_str(), colorizedSyntax, sizeof(colorizedSyntax) );

        if ( isServer && isClient )
          SendChannel(IRCText("%B%C07Server%C/%C04Client%C:%B %s", colorizedSyntax), channel.c_str());
        else if ( isServer )
          SendChannel(IRCText("%B%C07Server%C:%B %s", colorizedSyntax), channel.c_str());
        else if ( isClient )
          SendChannel(IRCText("%B%C04Client%C:%B %s", colorizedSyntax), channel.c_str());
        else
          SendChannel(IRCText("%BSyntax:%B %s", colorizedSyntax), channel.c_str());
      }
    }
  }

  p = buffer.find ( "Parameters</span></h2>" );
  if ( p != std::string::npos )
  {
    /* We have an event */
    size_t server = buffer.find("Serverside event");
    size_t client = buffer.find("Clientside event");
    p2 = buffer.find ( "<!-- PLAIN TEXT CODE FOR BOTS |", p );
    std::string params;

    if ( p2 == std::string::npos || p2 > p + 25 )
      params = "No parameters";
    else
    {
      int def = p2;
      p2 = buffer.find("|", def + 31 );
      if ( p2 != std::string::npos )
      {
        char colorizedSyntax [ 1024 ];
        params = buffer.substr ( def + 31, p2 - def - 31 );
        colorizeSyntax ( params.c_str(), colorizedSyntax, sizeof(colorizedSyntax) );
        params.assign ( colorizedSyntax );
      }
    }

    if ( params.length ( ) > 0 )
    {
      if ( server != std::string::npos )
        SendChannel(IRCText("%B%C07Server%C:%B %s", params.c_str()), channel.c_str());
      else if ( client != std::string::npos )
        SendChannel(IRCText("%B%C07Client%C:%B %s", params.c_str()), channel.c_str());
      else
        SendChannel(IRCText("%BParameters:%B %s", params.c_str()), channel.c_str());
    }
  }

  SendChannel(fullPath, channel.c_str());
}
