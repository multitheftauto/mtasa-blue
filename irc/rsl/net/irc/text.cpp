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

#include <string.h>
#include <stdarg.h>
#include "text.h"

using Rsl::Net::IRC::IRCText;

IRCText::IRCText()
{
}

IRCText::IRCText(const char* text, ...)
{
  va_list vl;
  
  va_start(vl, text);
  ParseText(text, vl);
  va_end(vl);
}

IRCText::IRCText(const std::string& text, ...)
{
  va_list vl;

  va_start(vl, text);
  ParseText(text.c_str(), vl);
  va_end(vl);
}

void IRCText::SetText(const char* text, ...)
{
  va_list vl;

  va_start(vl, text);
  ParseText(text, vl);
  va_end(vl);
}

void IRCText::SetText(const std::string& text, ...)
{
  va_list vl;

  va_start(vl, text);
  ParseText(text.c_str(), vl);
  va_end(vl);
}

void IRCText::SetRawText(const char* text)
{
  m_text = text;
}

void IRCText::SetRawText(const std::string& text)
{
  m_text = text;
}

void IRCText::ParseText(const char* text, va_list vl)
{
  char format[4096];
  char buffer[4096];
  size_t len;
  register const char* p = text;
  register const char* s;
  register char* w = format;
  
  /* Process own formats */
  while ((s = strchr(p, '%')))
  {
    len = (size_t)(s - p);
    memcpy(w, p, len);
    w += len;
    s++;
    
    switch (*s)
    {
      case 'C':
        *w++ = '\003';
        break;
      case 'U':
        *w++ = '\037';
        break;
      case 'B':
        *w++ = '\002';
        break;
      case 'O':
        *w++ = '\017';
        break;
      default:
        *w++ = '%';
        s--;
        break;
    }
    p = s + 1;
  }
  strcpy(w, p);

  vsnprintf(buffer, sizeof(buffer), format, vl);
  m_text = buffer;
}

const std::string& IRCText::GetText() const
{
  return m_text;
}

size_t IRCText::Length() const
{
  return m_text.length();
}
