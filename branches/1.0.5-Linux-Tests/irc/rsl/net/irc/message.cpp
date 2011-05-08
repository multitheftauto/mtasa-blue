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

#include <stdlib.h>
#include <strings.h>
#include "message.h"

using namespace Rsl::Net::IRC;

IRCMessage::IRCMessage() { }

IRCMessage::~IRCMessage() { }



/*************************************
 **            NUMERICS             **
 *************************************/
IRCMessageNumeric::IRCMessageNumeric() { }
IRCMessageNumeric::~IRCMessageNumeric() { }

unsigned int IRCMessageNumeric::GetNumeric() const { return m_numeric; }
const IRCText& IRCMessageNumeric::GetText() const { return m_text; }

bool IRCMessageNumeric::IsValidCmd(const std::string& cmd,
                                   const IRCUser& source,
                                   const IRCUser& dest,
                                   const IRCText& text)
{
  m_numeric = atoi(cmd.c_str());
  m_text = text;
  /* We return false because this message
   * can't be used as a normal callback.
   */
  return false;
}

void IRCMessageNumeric::ConstructMessage(std::string& str) const { }



/*************************************
 **              PING               **
 *************************************/
IRCMessagePing::IRCMessagePing() { }
IRCMessagePing::~IRCMessagePing() { }
IRCMessagePing::IRCMessagePing(const IRCText& text) :
  m_text(text)
{ }

bool IRCMessagePing::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  if (cmd == "PING")
  {
    m_text = text;
    return true;
  }
  return false;
}

void IRCMessagePing::ConstructMessage(std::string& str) const
{
  str = "PING :" + m_text.GetText() + RSLIRC_CRLF;
}

const IRCText& IRCMessagePing::GetText() const { return m_text; }



/*************************************
 **              PONG               **
 *************************************/
IRCMessagePong::IRCMessagePong() { }
IRCMessagePong::~IRCMessagePong() { }
IRCMessagePong::IRCMessagePong(const IRCText& text) :
  m_text(text)
{ }

bool IRCMessagePong::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  if (cmd == "PONG")
  {
    m_text = text;
    return true;
  }
  return false;
}

void IRCMessagePong::ConstructMessage(std::string& str) const
{
  str = "PONG :" + m_text.GetText() + RSLIRC_CRLF;
}

const IRCText& IRCMessagePong::GetText() const { return m_text; }


/*************************************
 **              USER               **
 *************************************/
IRCMessageUser::IRCMessageUser() { }
IRCMessageUser::~IRCMessageUser() { }
IRCMessageUser::IRCMessageUser(const IRCText& username, const IRCText& fullname) :
  m_username(username), m_fullname(fullname)
{ }

bool IRCMessageUser::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  return false;
}

void IRCMessageUser::ConstructMessage(std::string& str) const
{
  str = "USER " + m_username.GetText() + " x x :" + m_fullname.GetText() + RSLIRC_CRLF;
}



/*************************************
 **              NICK               **
 *************************************/
IRCMessageNick::IRCMessageNick() { }
IRCMessageNick::~IRCMessageNick() { }
IRCMessageNick::IRCMessageNick(const IRCUser& nickname) :
  m_nick(nickname)
{ }

const IRCUser& IRCMessageNick::GetNick() const { return m_nick; }

bool IRCMessageNick::IsValidCmd(const std::string& cmd,
           const IRCUser& source,
           const IRCUser& dest,
           const IRCText& text)
{
  if (cmd == "NICK")
  {
    m_nick.SetName(text.GetText());
    if (m_nick.GetType() == IRCUSER_USER)
    {
      return true;
    }
  }
  
  return false;
}

void IRCMessageNick::ConstructMessage(std::string& str) const
{
  if (m_nick.GetType() == IRCUSER_USER)
  {
    str = "NICK :" + m_nick.GetName() + RSLIRC_CRLF;
  }
}



/*************************************
 **              PASS               **
 *************************************/
IRCMessagePass::IRCMessagePass() { }
IRCMessagePass::~IRCMessagePass() { }
IRCMessagePass::IRCMessagePass(const IRCText& password) :
  m_password(password)
{ }

bool IRCMessagePass::IsValidCmd(const std::string& cmd,
           const IRCUser& source,
           const IRCUser& dest,
           const IRCText& text)
{
  return false;
}

void IRCMessagePass::ConstructMessage(std::string& str) const
{
  str = "PASS :" + m_password.GetText() + RSLIRC_CRLF;
}



/*************************************
 **             PRIVMSG             **
 *************************************/
IRCMessagePrivmsg::IRCMessagePrivmsg() { }
IRCMessagePrivmsg::~IRCMessagePrivmsg() { }
IRCMessagePrivmsg::IRCMessagePrivmsg(const IRCUser& to, const IRCText& text) :
  m_dest(to), m_text(text)
{ }

const IRCUser& IRCMessagePrivmsg::GetDest() const { return m_dest; }
const IRCText& IRCMessagePrivmsg::GetText() const { return m_text; }

bool IRCMessagePrivmsg::IsValidCmd(const std::string& cmd,
                                   const IRCUser& source,
                                   const IRCUser& dest,
                                   const IRCText& text)
{
  if (!strcasecmp(cmd.c_str(), "PRIVMSG"))
  {
    m_dest = dest;
    m_text = text;
    return true;
  }
  return false;
}

void IRCMessagePrivmsg::ConstructMessage(std::string& str) const
{
  str = "PRIVMSG " + m_dest.GetName() + " :" + m_text.GetText() + RSLIRC_CRLF;
}



/*************************************
 **              JOIN               **
 *************************************/
IRCMessageJoin::IRCMessageJoin() { }
IRCMessageJoin::~IRCMessageJoin() { }
IRCMessageJoin::IRCMessageJoin(const IRCUser& channel, const IRCText& password)
  : m_channel(channel), m_password(password)
{
}

const IRCUser& IRCMessageJoin::GetChannel() const { return m_channel; }

bool IRCMessageJoin::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  if (cmd == "JOIN")
  {
    m_channel.SetName(text.GetText());
    return true;
  }

  return false;
}

void IRCMessageJoin::ConstructMessage(std::string& str) const
{
  if (m_channel.GetType() == IRCUSER_CHANNEL)
  {
    str = "JOIN " + m_channel.GetName();
    if (m_password.Length())
    {
      str += " :" + m_password.GetText();
    }
    str += RSLIRC_CRLF;
  }
}



/*************************************
 **              PART               **
 *************************************/
IRCMessagePart::IRCMessagePart() { }
IRCMessagePart::~IRCMessagePart() { }
IRCMessagePart::IRCMessagePart(const IRCUser& channel, const IRCText& partmsg)
  : m_channel(channel), m_partmsg(partmsg)
{
}

const IRCUser& IRCMessagePart::GetChannel() const { return m_channel; }
const IRCText& IRCMessagePart::GetPartmsg() const { return m_partmsg; }

bool IRCMessagePart::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  if (cmd == "PART")
  {
    m_channel = dest.GetName();
    m_partmsg = text.GetText();
    return true;
  }

  return false;
}

void IRCMessagePart::ConstructMessage(std::string& str) const
{
  if (m_channel.GetType() == IRCUSER_CHANNEL)
  {
    str = "PART " + m_channel.GetName();
    if (m_partmsg.Length())
    {
      str += " :" + m_partmsg.GetText();
    }
    str += RSLIRC_CRLF;
  }
}



/*************************************
 **               KICK              **
 *************************************/
IRCMessageKick::IRCMessageKick() { }
IRCMessageKick::~IRCMessageKick() { }
IRCMessageKick::IRCMessageKick(const IRCUser& channel, const IRCUser& dest, const IRCText& reason)
  : m_channel(channel), m_dest(dest), m_reason(reason)
{
}

const IRCUser& IRCMessageKick::GetChannel() const { return m_channel; }
const IRCUser& IRCMessageKick::GetDest() const { return m_dest; }
const IRCText& IRCMessageKick::GetReason() const { return m_reason; }

bool IRCMessageKick::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  if (cmd == "KICK")
  {
    int p;
    const std::string& str = text.GetText();
    m_channel = dest;
    p = str.find(":");
    if (p)
    {
      m_dest.SetName(str.substr(0, p - 1));
      m_reason.SetText(str.substr(p + 1));
    }
    else
    {
      m_dest.SetName(str);
      m_reason.SetText("");
    }
    
    return true;
  }

  return false;
}

void IRCMessageKick::ConstructMessage(std::string& str) const
{
  if (m_channel.GetType() == IRCUSER_CHANNEL && m_dest.GetType() == IRCUSER_USER)
  {
    str = "KICK " + m_channel.GetName() + " " + m_dest.GetName();
    if (m_reason.Length())
    {
      str += " :" + m_reason.GetText();
    }
    str += RSLIRC_CRLF;
  }
}



/*************************************
 **              QUIT               **
 *************************************/
IRCMessageQuit::IRCMessageQuit() { }
IRCMessageQuit::~IRCMessageQuit() { }
IRCMessageQuit::IRCMessageQuit(const IRCText& quitmsg)
  : m_reason(quitmsg)
{
}

const IRCText& IRCMessageQuit::GetReason() const { return m_reason; }

bool IRCMessageQuit::IsValidCmd(const std::string& cmd,
                                const IRCUser& source,
                                const IRCUser& dest,
                                const IRCText& text)
{
  if (cmd == "QUIT")
  {
    m_reason = text;
    return true;
  }

  return false;
}

void IRCMessageQuit::ConstructMessage(std::string& str) const
{
  str = "QUIT";
  if (m_reason.Length())
  {
    str += " :" + m_reason.GetText();
  }
  str += RSLIRC_CRLF;
}



/*************************************
 **              UMODE              **
 *************************************/
IRCMessageUmode::IRCMessageUmode() { }
IRCMessageUmode::~IRCMessageUmode() { }
IRCMessageUmode::IRCMessageUmode(const IRCUser& dest, const IRCText& modestr)
  : m_dest(dest), m_modestr(modestr)
{
}

const IRCUser& IRCMessageUmode::GetDest() const { return m_dest; }
const IRCText& IRCMessageUmode::GetModes() const { return m_modestr; }

bool IRCMessageUmode::IsValidCmd(const std::string& cmd,
                                 const IRCUser& source,
                                 const IRCUser& dest,
                                 const IRCText& text)
{
  if (cmd == "MODE")
  {
    m_dest = dest;
    if (dest.GetType() == IRCUSER_USER)
    {
      m_modestr = text;
      return true;
    }
  }

  return false;
}

void IRCMessageUmode::ConstructMessage(std::string& str) const
{
  if (m_dest.GetType() == IRCUSER_USER)
  {
    str = "MODE " + m_dest.GetName() + " " + m_modestr.GetText() + RSLIRC_CRLF;
  }
}



/*************************************
 **              CMODE              **
 *************************************/
IRCMessageCmode::IRCMessageCmode() { }
IRCMessageCmode::~IRCMessageCmode() { }
IRCMessageCmode::IRCMessageCmode(const IRCUser& channel, const IRCText& modestr)
  : m_channel(channel), m_modestr(modestr)
{
}

const IRCUser& IRCMessageCmode::GetChannel() const { return m_channel; }
const IRCText& IRCMessageCmode::GetModes() const { return m_modestr; }

bool IRCMessageCmode::IsValidCmd(const std::string& cmd,
                                 const IRCUser& source,
                                 const IRCUser& dest,
                                 const IRCText& text)
{
  if (cmd == "MODE")
  {
    m_channel = dest;
    if (m_channel.GetType() == IRCUSER_CHANNEL)
    {
      m_modestr = text;
      return true;
    }
  }

  return false;
}

void IRCMessageCmode::ConstructMessage(std::string& str) const
{
  if (m_channel.GetType() == IRCUSER_CHANNEL)
  {
    str = "MODE " + m_channel.GetName() + " " + m_modestr.GetText() + RSLIRC_CRLF;
  }
}



/*************************************
 **             PROTOCTL            **
 *************************************/
IRCMessageProtoctl::IRCMessageProtoctl() { }
IRCMessageProtoctl::~IRCMessageProtoctl() { }
IRCMessageProtoctl::IRCMessageProtoctl(const IRCText& protoctl)
  : m_protoctl(protoctl)
{
}

bool IRCMessageProtoctl::IsValidCmd(const std::string& cmd,
                                    const IRCUser& source,
                                    const IRCUser& dest,
                                    const IRCText& text)
{
  /* PROTOCTL is never a server message, it's sent with numeric messages  */
  return false;
}

void IRCMessageProtoctl::ConstructMessage(std::string& str) const
{
  str = "PROTOCTL " + m_protoctl.GetText() + RSLIRC_CRLF;
}

/*************************************
 **              INVITE             **
 *************************************/
IRCMessageInvite::IRCMessageInvite() { }
IRCMessageInvite::~IRCMessageInvite() { }
IRCMessageInvite::IRCMessageInvite(const IRCUser& target, const IRCUser& channel)
  : m_target(target), m_channel(channel)
{
}

const IRCUser& IRCMessageInvite::GetTarget() const { return m_target; }
const IRCUser& IRCMessageInvite::GetChannel() const { return m_channel; }

bool IRCMessageInvite::IsValidCmd(const std::string& cmd,
                                  const IRCUser& source,
                                  const IRCUser& dest,
                                  const IRCText& text)
{
  if (cmd == "INVITE")
  {
    m_target = dest;
    m_channel.SetName(text.GetText());
    return true;
  }

  return false;
}

void IRCMessageInvite::ConstructMessage(std::string& str) const
{
  str = "INVITE " + m_target.GetName() + " :" + m_channel.GetName() + RSLIRC_CRLF;
}
