/*
    Copyright (C) 2006 Development Team of Peragro Tempus

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

#ifndef STRINGSTORE_H
#define STRINGSTORE_H

#include <string.h>

#include "array.h"
#include "mutex.h"

class StringStore
{
private:

  Mutex mutex;

  class StoreString
  {
  public:
    size_t len;
    char* string;

    StoreString() : len(0), string(0) {}
    ~StoreString() 
    { 
      delete [] string; 
    }
  };
  Array<StoreString*> strings;

  static StringStore* store;

  StringStore()
  {
    StoreString* null_str = new StoreString();
    strings.add(null_str); 
  }

  ~StringStore()
  {
    strings.delAll();
  }

  static StringStore* getStore();

  size_t add(const char*& str)
  {
    StoreString* string = new StoreString();
    string->len = strlen(str);
    string->string = new char[string->len+1];
    strncpy(string->string, str, string->len+1);
    strings.add(string);
    str = string->string;
    return strings.getCount()-1; // if we have 1 string, it has id 0.
  }

  size_t lookupId(const char*& string, size_t len)
  {
    if (string == 0 || len == 0) return 0;

    mutex.lock();

    for (size_t i=0; i<strings.getCount(); i++)
    {
      if (strings.get(i)->len == len)
      {
        if (!strcmp(strings.get(i)->string, string))
        {
          string = strings.get(i)->string;
          mutex.unlock();
          return i;
        }
      }
    }
    size_t str_id = add(string);
    mutex.unlock();

    //String not found, time to add it.
    return str_id;
  }

  const char* lookupString(size_t id)
  {
    mutex.lock();

    if (id >= strings.getCount()) 
    {
      mutex.unlock();
      return 0;
    }
    mutex.unlock();
    return strings.get(id)->string;
  }

  friend class ptString;

public:
  static void destroy() { delete getStore(); }
};

#endif //STRINGSTORE_H
