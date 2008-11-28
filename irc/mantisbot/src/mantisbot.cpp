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
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <rsl/net/http/client.h>
#include <rsl/net/socket/ipv4.h>
#include <rsl/net/socket/select.h>
#include "mantisbot.h"
#include "commands.h"
#include "configuration.h"
#include "keys.h"
#include "tinyxml.h"

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
}  

void* CheckForChanges_thread(void*);

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
        MantisBot::Instance()->OnBotConnect();
        break;
    }
  }
}

static int do_privmsg(IRCClient* irc, const IRCUser* source, const IRCMessage* _msg)
{
  const IRCMessagePrivmsg* msg = dynamic_cast<const IRCMessagePrivmsg *>(_msg);

  if (msg && msg->GetDest().GetType() == IRCUSER_CHANNEL)
  {
    CommandHandler::Instance()->Handle(source, &msg->GetDest(), msg->GetText().GetText());
  }
}
/* */


MantisBot* MantisBot::Instance()
{
  static MantisBot* instance = 0;
  if (instance == 0)
    instance = new MantisBot();
  return instance;
}

MantisBot::MantisBot()
  : m_error(""), m_errno(0), m_selector(2), m_initialized(false), m_lastMantisId(0), m_lastGoogleCodeId(0)
{
}

MantisBot::~MantisBot()
{
}

int MantisBot::Errno() const
{
  return m_errno;
}

const char* MantisBot::Error() const
{
  return m_error.c_str();
}

bool MantisBot::Ok() const
{
  return !m_errno;
}

void MantisBot::Daemonize()
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

bool MantisBot::Initialize(int argc, char* argv[], char* environment[])
{
  /* Read command-line options */
  static struct option long_options[] = {
    { "conffile", true,   0,  'f' },
    { "help",     false,  0,  'h' },
    { "verbose",  false,  0,  'v' },
    { 0,          0,      0,   0  },
  };
  int option_index = 0;
  int getopt_retval = 0;

  const char* configFile = 0;
  m_daemonize = true;

  while (1)
  {
    getopt_retval = getopt_long(argc, argv, "f:hv", long_options, &option_index);
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

  m_httpMantisAddr.Create(m_config.data.mantis.address, m_config.data.mantis.service);
  m_httpMantisBindAddr.Create("0.0.0.0", "0");

  m_httpGoogleCodeAddr.Create(m_config.data.googlecode.address, m_config.data.googlecode.service);
  m_httpGoogleCodeBindAddr.Create("0.0.0.0", "0");

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

void MantisBot::OnBotConnect()
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

bool MantisBot::Run()
{
  SocketClient& socket = m_client.GetSocket();
  int updatedelay;
  int n;
  time_t nextWebCheck;
  time_t currentTime;
  pthread_t mantisThread;

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

  updatedelay = m_config.data.bot.updatedelay;
  if (updatedelay < 10)
    updatedelay = 10;
  currentTime = time(0);
  nextWebCheck = currentTime + updatedelay;

  m_selector.Add(&socket, RSL_SELECT_EVENT_IN);
  m_selector.SetTimeout(updatedelay * 1000);

  debugok();

  SocketClient* sockets[1];
  while ((n = m_selector.Select(sockets, 1)) > -1)
  {
    if (m_client.Ok() == false || socket.GetStream().Ok() == false || socket.GetStream().Eof())
    {
      debugentry("Some errror occurred, reconnecting bot");
      Close();
      sleep(m_config.data.ircserver.reconnectdelay);
      m_client.Connect();
      m_selector.Add(&socket, RSL_SELECT_EVENT_IN);
      debugok();
      continue;
    }
    else if (n > 0)
    {
      m_client.Loop();
    }

    currentTime = time(0);
    if (currentTime >= nextWebCheck)
    {
      m_client.Send(IRCMessagePing("."));
      nextWebCheck = currentTime + updatedelay;
      pthread_detach(mantisThread);
      pthread_cancel(mantisThread);
      pthread_create(&mantisThread, 0, CheckForChanges_thread, this);
      m_selector.SetTimeout(updatedelay * 1000);
    }
    else
    {
      m_selector.SetTimeout((nextWebCheck - currentTime) * 1000);
    }
  }

  if (n == -1)
  {
    Close();
    m_errno = m_selector.Errno();
    m_error = std::string("Socket selection failed: ") + m_selector.Error();
    return false;
  }

  return true;
}

void MantisBot::Close(const std::string& quitMessage)
{
  if (quitMessage != "")
  {
    m_client.Send(IRCMessageQuit(quitMessage.c_str()));
  }
  m_selector.Del(&m_client.GetSocket());
  m_client.Close();
}

const Config& MantisBot::GetConfig() const
{
  return m_config;
}

void MantisBot::SendChannel(const IRCText& msg, const char* channel)
{
  if (!channel)
    channel = m_config.data.channels[0].name;
  m_client.Send(IRCMessagePrivmsg(channel, msg));
}

bool MantisBot::CheckForGoogleCodeChanges()
{
    char tmp[HTTP_BUFFER_SIZE];
    std::string buffer;
    size_t len;
    
    // Download the feed from the specified Google Code URL
    HTTPClient http(m_config.data.googlecode.address, m_httpGoogleCodeAddr, m_httpGoogleCodeBindAddr, false);
    http.Connect(m_config.data.googlecode.path);
    http.Send();
    
    if (!http.Ok()) return false;

    if (http.ResponseStatus() != 200)
    {
        printf("Unable to connect to the web page (%s%s): %d %s\n",
                m_config.data.googlecode.address, m_config.data.googlecode.path,
                http.ResponseStatus(), http.StatusText());
        return false;
    }

    // Grab the feed data
    HTTPStream& stream = http.GetStream();
    if (!stream.Ok()) return false;
    while (stream.Eof() == false)
    {
        len = stream.Read(tmp, HTTP_BUFFER_SIZE);
        buffer.append(tmp, len);
    }
    
    // We got the feed, now start processing it's XML contents
    TiXmlDocument xml;
    TiXmlNode *feed, *node;
    xml.Parse(buffer.c_str());
    if (xml.Error())
    {
        printf("Unable to parse XML feed in %s: %s\n", xml.Value(), xml.ErrorDesc());
        return false;
    }
    feed = xml.FirstChild("feed");
    if (!feed) return false;
    node = feed->FirstChild("entry");
    
    // Loop through the entry nodes
    size_t index;
    int currentId, newestId = -1;
    std::string strAuthor, strDescription, strLink;
    const char *attr;
    while (node)
    {
        TiXmlNode *data = node->FirstChild("title");
        if (!data) return false;
        TiXmlElement *elem = data->ToElement ();
        if (!elem) return false;

        std::string strText = elem->GetText ();

        // Revision
        index = strText.find("Revision ") + 9;
        currentId = atoi(strText.substr(index, strText.find(":",index) - index).c_str());

        // Description
        index = strText.find(":",index) + 2;
        strDescription = strText.substr(index);
        TrimSpaces(strDescription);

        // Author
        data = node->FirstChild("author");
        if (!data) return false;
        data = data->FirstChild("name");
        if (!data) return false;
        elem = data->ToElement ();
        if (!elem) return false;
        strAuthor = elem->GetText ();
        
        // Link
        data = node->FirstChild("link");
        if (!data) return false;
        elem = data->ToElement ();
        if (!elem) return false;
        attr = elem->Attribute ("href");
        if (!attr) return false;
        strLink = std::string ( attr );

        // First entry will always be the newest
        if (newestId == -1) {
            newestId = currentId;
            // If this is our first run, store the newest entry id
            if (!m_lastGoogleCodeId)
                m_lastGoogleCodeId = newestId;
        }
            
        // If this entry is newer than the last stored id
        if (currentId > m_lastGoogleCodeId)
        {   // This is a new entry
            SendTextToChannels(
                IRCText(
                    "%C02%Br%d%B%C %C12(%s)%C %C10%s%C %C03-%C "
                    "%C14%s%C",
                    currentId, strAuthor.c_str (), strDescription.c_str (),
                    strLink.c_str ()
                )
            );
        }
        // Advance to next sibling
        node = node->NextSiblingElement ();
    }
    m_lastGoogleCodeId = newestId;
    
    return true;
}

bool MantisBot::CheckForMantisChanges()
{
  char tmp[HTTP_BUFFER_SIZE];
  std::string buffer;
  size_t len;

  HTTPClient http(m_config.data.mantis.address, m_httpMantisAddr, m_httpMantisBindAddr, m_config.data.mantis.ssl);
  http.Connect(m_config.data.mantis.path);
  http.Send();

  if (!http.Ok()) return false;

  if (http.ResponseStatus() != 200)
  {
    printf("Unable to connect to the web page: %d %s\n", http.ResponseStatus(), http.StatusText());
    return false;
  }

  HTTPStream& stream = http.GetStream();

  if (!stream.Ok()) return false;

  while (stream.Eof() == false)
  {
    len = stream.Read(tmp, HTTP_BUFFER_SIZE);
    buffer.append(tmp, len);
  }

  int p;
  int p2;
  std::string id;
  std::string author;
  std::string note;
  std::string change;
  std::string bug;
  std::string summary;
  int newestId = -1;

  while ((p = buffer.find("\n")) != -1)
  {
    std::string line(buffer, 0, p);
    buffer.erase(0, p + 1);

    if (buffer[buffer.length() - 1] == '\r')
      buffer.erase(buffer.length() - 1);
    
    p = line.find(";_1_;");
    if (p == -1)
      return false;
    id = line.substr(0, p);
    p += 5;

    p2 = line.find(";_2_;", p);
    if (p2 == -1)
      return false;
    author = line.substr(p, p2 - p);
    p = p2 + 5;

    p2 = line.find(";_3_;", p);
    if (p2 == -1)
      return false;
    note = line.substr(p, p2 - p);
    p = p2 + 5;

    p2 = line.find(";_4_;", p);
    if (p2 == -1)
      return false;
    change = line.substr(p, p2 - p);
    p = p2 + 5;

    p2 = line.find(";_5_;", p);
    if (p2 == -1)
      return false;
    bug = line.substr(p, p2 - p);
    p = p2 + 5;

    p2 = line.find("||||", p);
    if (p2 == -1)
      return false;
    summary = line.substr(p, p2 - p);

    int ID = atoi(id.c_str());
    if (newestId == -1)
      newestId = ID;

    if (m_initialized && ID > m_lastMantisId)
    {
      SendTextToChannels(
        IRCText(
           "%C02Issue #%d:%C %C12%B%s%B%C %C03-%C %C02Updated by %s%C %C03- %C10%s (%s)%C %C03-%C "
           "%C14%s://%s/view.php?id=%s%C",
           atoi(bug.c_str()), summary.c_str(), author.c_str(),
           note.c_str(), change.c_str(),
           m_config.data.mantis.service, m_config.data.mantis.address, bug.c_str()
        )
      );
    }
  }

  if (newestId > -1)
  {
    m_initialized = true;
    m_lastMantisId = newestId;
  }

  return true;
}

void MantisBot::SendTextToChannels(const IRCText text)
{
  for (std::vector<__ConfigChannel>::const_iterator i = m_config.data.channels.begin();
       i != m_config.data.channels.end();
       i++)
  {
    const __ConfigChannel& ch = (*i);
    if (ch.notifyChanges)
    {
      SendChannel(text, ch.name);
    }
  }
}

void* CheckForChanges_thread(void *_bot)
{
  // Wait a few seconds before doing anything, so we can be sure the bot is settled
  MantisBot* bot = static_cast<MantisBot*>(_bot);
  
  debugentry("Checking for Mantis changes");
  if (bot->CheckForMantisChanges())
    debugok();
  else
    debugerror();
    
  debugentry("Checking for Google Code changes");
  if (bot->CheckForGoogleCodeChanges())
    debugok();
  else
    debugerror();
}
