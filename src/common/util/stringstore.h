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

#ifndef _STRINGSTORE_H
#define _STRINGSTORE_H

#include "array.h"
#include "mutex.h"

class StringStore
{
private:

  Mutex mutex;

  class ptString
  {
  public:
    size_t len;
    char* string;

    ptString() : len(0), string(0) {}
    ~ptString() { delete [] string; }
  };
  Array<ptString*> strings;

  static StringStore* store;

  StringStore() {}

public:
  ~StringStore() { strings.delAll(); }

  static StringStore* getStore();

  size_t add(const char* str, bool check_duplicate = true)
  {
    printf("add string %s: ", str);
    if (check_duplicate)
    {
      printf("already exists!\n");
      return lookupId(str);
    }

    mutex.lock();
    ptString* string = new ptString();
    string->len = strlen(str);
    string->string = new char[string->len+1];
    strncpy(string->string, str, string->len+1);
    strings.add(string);
    mutex.unlock();
    printf("as %d!\n", strings.getCount()-1);
    return strings.getCount()-1; // if we have 1 string, it has id 0.
  }

  size_t lookupId(const char* string)
  {
    mutex.lock();

    printf("looking up string %s: ", string);

    size_t len = strlen(string);
    for (size_t i=0; i<strings.getCount(); i++)
    {
      if (strings.get(i)->len == len)
        if (!strcmp(strings.get(i)->string, string))
        {
          mutex.unlock();
          printf("%i\n", i);
          return i;
        }
    }

    mutex.unlock();
    //String not found, time to add it. We did already check if it exist,
    //therefor it's false here. Else we would get an endless recursion.
    printf("failed -> add");
    return add(string, false);
  }

  const char* lookupString(size_t id)
  {
    mutex.lock();

    printf("looking up string id %d: ", id);
    if (id >= strings.getCount()) 
    {
      mutex.unlock();
      printf("failed");
      return 0;
    }
    printf("%s\n", strings.get(id)->string);
    mutex.unlock();
    return strings.get(id)->string;
  }
};

#endif //_STRINGSTORE_H
