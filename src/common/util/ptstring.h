/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PTSTRING_H
#define PTSTRING_H

#include <stdio.h>
#include "stringstore.h"

class ptString
{
public:
  static ptString Null;

private:
  size_t string_id;

  // just for caching...
  const char* str_cache;

public:
  inline ptString()
  {
    string_id = 0;
    str_cache = 0;
  }

  inline ptString(const char* str, size_t len)
  {
    string_id = StringStore::getStore()->lookupId(str, len);
    str_cache = str;
  }

  inline ptString(const ptString& str)
  {
    string_id = str.string_id;
    str_cache = str.str_cache;
  }

  static ptString create(const std::string& str);

  inline const char* operator*() const
  {
    if (!str_cache && string_id > 0)
    {
      //not needed anymore! can be removed _later_ when it proved to work.
      printf("WARNING! Unexpected ptString lookup!\n");
      //str_cache = StringStore::getStore()->lookupString(string_id);
    }
    return str_cache;
  }

  inline bool operator==(const ptString& other) const
  {
    return string_id == other.string_id;
  }

  inline bool operator!=(const ptString& other) const
  {
    return string_id != other.string_id;
  }

  inline bool isNull() const
  {
    return string_id == 0;
  }
};

#endif // PTSTRING_H
