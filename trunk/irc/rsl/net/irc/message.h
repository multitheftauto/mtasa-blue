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

#ifndef _RSL_NET_IRC_MESSAGE_H
#define _RSL_NET_IRC_MESSAGE_H

#include "text.h"
#include "user.h"

#define RSLIRC_CRLF "\r\n"

namespace Rsl { namespace Net { namespace IRC {

  class IRCClient;
  
  class IRCMessage
  {
    friend class IRCClient;
    
  public:
    IRCMessage();
    virtual ~IRCMessage();

  private:
    virtual bool IsValidCmd(const std::string& cmd,
                            const IRCUser& source,
                            const IRCUser& dest,
                            const IRCText& text) = 0;
    virtual void ConstructMessage(std::string& str) const = 0;
  };

#define BEGIN_MESSAGE_DECLARATION(msg, ...) \
  class msg : public IRCMessage \
  { \
  public: \
    msg(); \
    msg(__VA_ARGS__); \
    ~msg(); 

#define END_MESSAGE_DECLARATION() \
  private: \
    bool IsValidCmd(const std::string& cmd, \
                    const IRCUser& source, \
                    const IRCUser& dest, \
                    const IRCText& text); \
    void ConstructMessage(std::string& str) const; \
  };

  BEGIN_MESSAGE_DECLARATION(IRCMessageNumeric, unsigned int unused)
  public:
    unsigned int GetNumeric() const;
    const IRCText& GetText() const;
  private:
    unsigned int m_numeric;
    IRCText m_text;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessagePing, const IRCText& text)
  public:
    const IRCText& GetText() const;
  private:
    IRCText m_text;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessagePong, const IRCText& text)
  public:
    const IRCText& GetText() const;
  private:
    IRCText m_text;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageUser, const IRCText& username, const IRCText& fullname)
  private:
    IRCText m_username;
    IRCText m_fullname;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageNick, const IRCUser& nick)
  public:
    const IRCUser& GetNick() const;
  private:
    IRCUser m_nick;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessagePass, const IRCText& password)
  private:
    IRCText m_password;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessagePrivmsg, const IRCUser& to, const IRCText& text)
  public:
    const IRCUser& GetDest() const;
    const IRCText& GetText() const;
  private:
    IRCUser m_dest;
    IRCText m_text;
  END_MESSAGE_DECLARATION()
  
  BEGIN_MESSAGE_DECLARATION(IRCMessageJoin, const IRCUser& channel, const IRCText& password = "")
  public:
    const IRCUser& GetChannel() const;
  private:
    IRCUser m_channel;
    IRCText m_password;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessagePart, const IRCUser& channel, const IRCText& partmsg = "")
  public:
    const IRCUser& GetChannel() const;
    const IRCText& GetPartmsg() const;
  private:
    IRCUser m_channel;
    IRCText m_partmsg;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageKick, const IRCUser& channel, const IRCUser& dest, const IRCText& reason = "")
  public:
    const IRCUser& GetChannel() const;
    const IRCUser& GetDest() const;
    const IRCText& GetReason() const;
  private:
    IRCUser m_channel;
    IRCUser m_dest;
    IRCText m_reason;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageQuit, const IRCText& quitmsg)
  public:
    const IRCText& GetReason() const;
  private:
    IRCText m_reason;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageUmode, const IRCUser& dest, const IRCText& modestr)
  public:
    const IRCUser& GetDest() const;
    const IRCText& GetModes() const;
  private:
    IRCUser m_dest;
    IRCText m_modestr;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageCmode, const IRCUser& channel, const IRCText& modestr)
  public:
    const IRCUser& GetChannel() const;
    const IRCText& GetModes() const;
  private:
    IRCUser m_channel;
    IRCText m_modestr;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageProtoctl, const IRCText& protoctl)
  private:
    IRCText m_protoctl;
  END_MESSAGE_DECLARATION()

  BEGIN_MESSAGE_DECLARATION(IRCMessageInvite, const IRCUser& target, const IRCUser& channel)
  public:
    const IRCUser& GetTarget() const;
    const IRCUser& GetChannel() const;
  private:
    IRCUser m_target;
    IRCUser m_channel;
  END_MESSAGE_DECLARATION()
  
#undef BEGIN_MESSAGE_DECLARATION
#undef END_MESSAGE_DECLARATION

  typedef int (*IRCMessage_callback_t)(IRCClient* irc, const IRCUser* source, const IRCMessage *);
  
}; }; };

#endif /* #ifndef _RSL_NET_IRC_MESSAGE_H */
