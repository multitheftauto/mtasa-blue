//  tinygettext - A gettext replacement that works directly on .po files
//  Copyright (C) 2006 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <assert.h>
#include "log_stream.hpp"
#include "dictionary.hpp"

namespace tinygettext {

Dictionary::Dictionary(const std::string& charset_,const std::string& filepath_) :
  entries(),
  ctxt_entries(),
  charset(charset_),
  filepath(filepath_),
  plural_forms()
{
}

Dictionary::~Dictionary()
{
}

std::string
Dictionary::get_charset() const
{
  return charset;
}

std::string
Dictionary::get_filepath() const
{
  return filepath;
}

void
Dictionary::set_plural_forms(const PluralForms& plural_forms_)
{
  plural_forms = plural_forms_;
}

PluralForms
Dictionary::get_plural_forms() const
{
  return plural_forms;
}

void
Dictionary::set_metadata(const std::map<std::string,std::string>& metadata_)
{
  metadata = metadata_;
}

std::map<std::string,std::string>
Dictionary::get_metadata() const
{
  return metadata;
}

std::string
Dictionary::translate_plural(const std::string& msgid, const std::string& msgid_plural, int num)
{
  return translate_plural(entries, msgid, msgid_plural, num);
}

std::string
Dictionary::translate_plural_silent(const std::string& msgid, const std::string& msgid_plural, int num)
{
  return translate_plural_impl(entries, msgid, msgid_plural, num, true);
}

std::string
Dictionary::translate_plural(const Entries& dict, const std::string& msgid, const std::string& msgid_plural, int count)
{
  return translate_plural_impl(dict, msgid, msgid_plural, count, false);
}

std::string
Dictionary::translate_plural_impl(const Entries& dict, const std::string& msgid, const std::string& msgid_plural, int count, bool silent)
{
  Entries::const_iterator i = dict.find(msgid);

  if (&i && i != dict.end())
  {
    const std::vector<std::string>& msgstrs = i->second;

    unsigned int n = 0;
    n = plural_forms.get_plural(count);
    // assert(/*n >= 0 &&*/ n < msgstrs.size());

    if (n < msgstrs.size() && !msgstrs[n].empty())
      return msgstrs[n];
  }
  else
  {
    if (!silent)
    {
      log_info << "Couldn't translate: " << msgid << std::endl;
      log_info << "Candidates: " << std::endl;
      for (i = dict.begin(); i != dict.end(); ++i)
        log_info << "'" << i->first << "'" << std::endl;
    }
  }

  if (count == 1) // default to english rules
    return msgid;
  else
    return msgid_plural;
}

std::string
Dictionary::translate(const std::string& msgid)
{
  return translate(entries, msgid);
}

std::string
Dictionary::translate_silent(const std::string& msgid)
{
  return translate_impl(entries, msgid, true);
}

std::string
Dictionary::translate(const Entries& dict, const std::string& msgid)
{
  return translate_impl(dict, msgid, false);
}

std::string
Dictionary::translate_impl(const Entries& dict, const std::string& msgid, bool silent)
{
  Entries::const_iterator i = dict.find(msgid);
  if (i != dict.end() && !i->second.empty())
  {
    return i->second[0];
  }
  else
  {
    if (!silent)
    {
      log_info << "Couldn't translate: " << msgid << std::endl;
    }
    return msgid;
  } 
}

std::string
Dictionary::translate_ctxt(const std::string& msgctxt, const std::string& msgid)
{
  CtxtEntries::iterator i = ctxt_entries.find(msgctxt);
  if (i != ctxt_entries.end())
  {
    return translate(i->second, msgid);
  }
  else
  {
    log_info << "Couldn't translate: " << msgid << std::endl;
    return msgid;
  }
}

std::string
Dictionary::translate_ctxt_plural(const std::string& msgctxt, 
                                  const std::string& msgid, const std::string& msgidplural, int num)
{
  CtxtEntries::iterator i = ctxt_entries.find(msgctxt);
  if (i != ctxt_entries.end())
  {
    return translate_plural(i->second, msgid, msgidplural, num);
  }
  else
  {
    log_info << "Couldn't translate: " << msgid << std::endl;
    if (num != 1) // default to english
      return msgidplural;
    else
      return msgid;
  }
}

void
Dictionary::add_translation(const std::string& msgid, const std::string& ,
                            const std::vector<std::string>& msgstrs)
{
  // Do we need msgid2 for anything? its after all supplied to the
  // translate call, so we just throw it away here
  entries[msgid] = msgstrs;
}

void
Dictionary::add_translation(const std::string& msgid, const std::string& msgstr)
{
  std::vector<std::string>& vec = entries[msgid];
  if (vec.empty())
  {
    vec.push_back(msgstr);
  }
  else
  {
    log_warning << "collision in add_translation: '" 
                << msgid << "' -> '" << msgstr << "' vs '" << vec[0] << "'" << std::endl;
    vec[0] = msgstr;
  }
}

void
Dictionary::add_translation(const std::string& msgctxt, 
                            const std::string& msgid, const std::string& msgid_plural,
                            const std::vector<std::string>& msgstrs)
{
  std::vector<std::string>& vec = ctxt_entries[msgctxt][msgid];
  if (vec.empty())
  {
    vec = msgstrs;
  }
  else
  {
    log_warning << "collision in add_translation(\"" << msgctxt << "\", \"" << msgid << "\", \"" << msgid_plural << "\")" << std::endl;
    vec = msgstrs;
  }
}

void
Dictionary::add_translation(const std::string& msgctxt, const std::string& msgid, const std::string& msgstr)
{
  std::vector<std::string>& vec = ctxt_entries[msgctxt][msgid];
  if (vec.empty())
  {
    vec.push_back(msgstr);
  }
  else
  {
    log_warning << "collision in add_translation(\"" << msgctxt << "\", \"" << msgid << "\")" << std::endl;
    vec[0] = msgstr;
  }
}

} // namespace tinygettext

/* EOF */
