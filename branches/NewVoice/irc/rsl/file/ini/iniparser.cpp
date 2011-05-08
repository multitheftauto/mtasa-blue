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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iniparser.h"

using Rsl::File::Ini::IniParser;

IniParser::IniParser()
  : m_filename(0), m_errno(0), m_sections(0)
{
}

IniParser::IniParser(const char* filename)
  : m_filename(0), m_errno(0), m_sections(0)
{
  SetFilename(filename);
}

IniParser::~IniParser()
{
  Reset();
}

void IniParser::Reset()
{
  __IniSection *sec1;
  __IniSection *sec2;
  __IniEntry *ent1;
  __IniEntry *ent2;

  if (m_filename)
  {
    free(m_filename);
    m_filename = 0;
  }
    
  for (sec1 = m_sections; sec1; sec1 = sec2)
  {
    sec2 = sec1->next;
    for (ent1 = sec1->entries; ent1; ent1 = ent2)
    {
      ent2 = ent1->next;
      delete [] ent1->name;
      delete [] ent1->value;
      delete ent1;
    }
    delete [] sec1->name;
    delete sec1;
  }
  m_sections = 0;
}

bool IniParser::SetFilename(const char* filename)
{
  FILE *fd;
  char line[1024];
  bool inSection = false;
  size_t len;
  char *p;
  __IniSection *section;
  __IniEntry *entry;
  
  Reset();
  m_filename = strdup(filename);
  fd = fopen(m_filename, "r");
  if (!fd)
  {
    m_errno = errno;
    return false;
  }

  do
  {
    *line = '\0';
    fgets(line, sizeof(line), fd);
    len = strlen(line);
    
    for (p = line + len - 1; (*p == '\r') || (*p == '\n'); p--, len--)
    {
      *p = '\0';
    }

    if (!len)
    {
      inSection = false;
    }
    else
    {
      if (!inSection)
      {
        /* We must read a section */
        if ((line[0] != '[') || (line[len - 1] != ']'))
        {
          Reset();
          m_errno = -1;
          return false;
        }
        line[len - 1] = '\0';
        section = new __IniSection;
        section->name = new char[len - 2];
        strcpy(section->name, line + 1);
        section->next = m_sections;
        section->entries = 0;
        m_sections = section;
        inSection = true;
      }
      else
      {
        p = strchr(line, '=');
        if (!p)
        {
          Reset();
          m_errno = -1;
          return false;
        }
        *p = '\0';
        p++;
        entry = new __IniEntry;
        entry->name = new char[strlen(line) + 1];
        strcpy(entry->name, line);
        entry->value = new char[strlen(p) + 1];
        strcpy(entry->value, p);
        entry->next = section->entries;
        section->entries = entry;
      }
    }
  }
  while (!feof(fd));

  fclose(fd);

  return true;
}

const char* IniParser::GetValue(const char* section_name, const char* entry_name) const
{
  size_t len = 0;
  const __IniSection *section;
  const __IniEntry *entry;

  for (section = m_sections; section; section = section->next)
  {
    if (!strcmp(section->name, section_name))
    {
      for (entry = section->entries; entry; entry = entry->next)
      {
        if (!strcmp(entry->name, entry_name))
        {
          return entry->value;
        }
      }
      break;
    }
  }

  return 0;
}

bool IniParser::Ok() const
{
  return !m_errno;
}

int IniParser::Errno() const
{
  return m_errno;
}

const char* IniParser::Error() const
{
  return strerror(m_errno);
}

