//  tinygettext - A gettext replacement that works directly on .po files
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "unix_file_system.hpp"

#include <sys/types.h>
#include <fstream>
#ifdef WIN32
  #include <dirent_win32.h>
#else
  #include <dirent.h>
#endif
#include <stdlib.h>
#include <string.h>

namespace tinygettext {

UnixFileSystem::UnixFileSystem()
{
}

bool
UnixFileSystem::open_directory(const std::string& pathname, std::vector<std::string>& files, std::vector<std::string>& dirs)
{
  DIR* dir = opendir(pathname.c_str());
  if (!dir)
  {
    // FIXME: error handling
    return false;
  }
  else
  {
    struct dirent* dp;
    while((dp = readdir(dir)) != 0)
    {
	  const char* path = dp->d_name;
      switch (dp->d_type)
      {
        case DT_REG:
          files.push_back(path);
          break;
        case DT_DIR:
        {
          if( !strcmp(path, ".")) continue;
          if( !strcmp(path, "..")) continue;
          dirs.push_back(path);
          break;
        }
        default: 
          break;
      }
    }
    closedir(dir);

    return true;
  }
}

std::auto_ptr<std::istream>
UnixFileSystem::open_file(const std::string& filename)
{
  return std::auto_ptr<std::istream>(new std::ifstream(filename.c_str()));
}

bool
UnixFileSystem::file_exists(const std::string& filename)
{
  std::ifstream file(filename.c_str());
  if (file.good())
  {
    file.close();
    return true;
  }
  else
  {
    file.close();
	return false;
  }
}

} // namespace tinygettext

/* EOF */
