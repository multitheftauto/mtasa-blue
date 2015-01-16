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

#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#include <string>
#include <vector>
#include <rsl/file/ini/iniparser.h>

struct __ConfigChannel
{
  const char* name;
  const char* password;
};

class Config
{
public:
  Config();
  ~Config();

  bool Create(const char* fileName);

  bool Ok() const;
  int Errno() const;
  const char* Error() const;

  struct
  {
    struct
    {
      const char* address;
      const char* service;
      const char* password;
      bool ssl;
      int reconnectdelay;
    } ircserver;

    struct
    {
      const char* nickname;
      const char* username;
      const char* fullname;
      const char* password;
    } bot;

    std::vector<__ConfigChannel> channels;

    struct
    {
      const char* address;
      const char* service;
      bool ssl;
      const char* path;
    } wiki;
  } data;
private:
  int m_errno;
  std::string m_error;
  Rsl::File::Ini::IniParser m_parser;
};

#endif /* #ifndef __CONFIGURATION_H */
