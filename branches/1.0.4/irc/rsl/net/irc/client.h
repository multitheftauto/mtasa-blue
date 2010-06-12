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

#ifndef _RSL_NET_IRC_CLIENT_H
#define _RSL_NET_IRC_CLIENT_H

#include <map>
#include "../socket/ip.h"
#include "../socket/tcpclient.h"
#include "../socket/tcpclientssl.h"
#include "message.h"

namespace Rsl { namespace Net { namespace IRC {

  class IRCClient
  {
    public:
      IRCClient();
      IRCClient(const Rsl::Net::Socket::IPAddr& addr, const Rsl::Net::Socket::IPAddr& bindAddr,
                const std::string& nickname,
                const std::string& username,
                const std::string& fullname,
                const std::string& password = "",
                bool useSSL = false,
                const char* certFile = 0);
      virtual ~IRCClient();

      void Create(const Rsl::Net::Socket::IPAddr& addr, const Rsl::Net::Socket::IPAddr& bindAddr,
                  const std::string& nickname,
                  const std::string& username,
                  const std::string& fullname,
                  const std::string& password = "",
                  bool useSSL = false,
                  const char* certFile = 0);

      bool Connect();
      void Close();
      bool IsConnected() const;
      ssize_t Loop();
      bool Ok() const;
      Rsl::Net::Socket::SocketClientTCP& GetSocket();

      int Errno() const;
      const char* Error() const;

      void RegisterCallback(IRCMessage *message, IRCMessage_callback_t callback);
      void RegisterNumericsCallback(IRCMessage_callback_t callback);
      void Send(const IRCMessage& message);

      bool IsMe(const IRCUser& usr) const;
      const IRCUser& GetMe() const;
      void ChangeNick(const IRCUser& newnick);

    private:
      void DispatchMessage(const std::string& msg);

      Rsl::Net::Socket::SocketClientTCP* m_sock;
      IRCUser m_me;
      std::string m_nickname;
      std::string m_username;
      std::string m_fullname;
      std::string m_password;
      int m_errno;
      const char* m_error;
      bool m_connected;
      std::map<IRCMessage *, IRCMessage_callback_t> m_callbacks;
      IRCMessage_callback_t m_numericsCallback;
      std::string m_buffer;
  };

}; }; };

#endif /* #ifndef _RSL_NET_IRC_CLIENT_H */
