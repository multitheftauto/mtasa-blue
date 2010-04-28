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

#ifndef _RSL_NET_IRC_USER_H
#define _RSL_NET_IRC_USER_H

#include <string>

enum IRCUserType
{
  IRCUSER_USER = 1,
  IRCUSER_CHANNEL,
  IRCUSER_SERVER
};

namespace Rsl { namespace Net { namespace IRC {

  class IRCUser
  {
  public:
    IRCUser();
    IRCUser(const IRCUser& copy);
    IRCUser(const char* name);
    IRCUser(const std::string& name);

    void SetName(const char* name);
    void SetName(const std::string& name);
    const std::string& GetName() const;
    const std::string& GetUsername() const;
    const std::string& GetHost() const;
    IRCUserType GetType() const;
    
  private:
    std::string m_name;
    std::string m_username;
    std::string m_host;
    IRCUserType m_type;
  };
  
}; }; };

#endif /* #ifndef _RSL_NET_IRC_MESSAGE_H */
