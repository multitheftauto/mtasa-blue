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

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include <rsl/net/irc/client.h>
#include <rsl/net/irc/user.h>
#include <string>
#include <vector>
#include "mantisbot.h"

class CommandHandler
{
  typedef void (*cmd_t)(MantisBot*, const Rsl::Net::IRC::IRCUser*, const Rsl::Net::IRC::IRCUser*, const std::vector<std::string>& params);

  struct Command
  {
    std::string name;
    cmd_t callback;
  };

public:
  static CommandHandler* Instance();

public:
  CommandHandler();
  void RegisterCommand(const std::string& commandName, cmd_t callback);

  void Handle(const Rsl::Net::IRC::IRCUser* source, const Rsl::Net::IRC::IRCUser* dest, const std::string& text);

private:
  std::vector<Command> m_commands;

#define COMMAND(x) static void cmd_ ## x (MantisBot* bot, const Rsl::Net::IRC::IRCUser*, const Rsl::Net::IRC::IRCUser*, const std::vector<std::string>& params)
  COMMAND(issue);
#undef COMMAND
};

#endif /* #ifndef __COMMANDS_H */
