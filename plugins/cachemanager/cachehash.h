/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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
/**
 * @file cachehash.h
 *
 * @basic
 */

#ifndef CACHEHASH_H
#define CACHEHASH_H

#include <cssysdef.h>

#include <csutil/hash.h>
#include <string>

struct iCacheEntry;

class csHashComputerSTD
{
public:
  static uint ComputeHash (const std::string& key)
  {
    return csHashCompute (key.c_str());
  }
};

template<>
class csHashComputer<std::string> : public csHashComputerSTD {};

template<typename Ref>
//class CacheHash : public csHash<Ref<iCacheEntry>, std::string>
class CacheHash : public csHash<Ref, std::string>
{
public:
  typedef typename csHash<Ref, std::string>::GlobalIterator GlobalIterator;

public:
  CacheHash() {}

  bool Contains (const std::string& key) const
  {
    if (this->GetSize() == 0) return false;
    //csArray<Ref<iCacheEntry> > values = this->GetAll(key);
    csArray<Ref> values(this->GetAll(key));
    for (size_t i = 0; i < values.GetSize (); ++i)
      if (values[i] != 0) return true;
    return false;
  }

  /// Compact this hash by removing all weak references that have become 0. 
  void Compact()
  {
    GlobalIterator it(this->GetIterator());
    while(it.HasNext())
    {
      iCacheEntry* el = it.Next();
      if (el == 0) this->DeleteElement(it);
    }
  }

  //Ref<iCacheEntry> Put(const std::string& key, const Ref<iCacheEntry>& value)
  Ref Put(const std::string& key, const Ref& value)
  {
    return PutUnique (key, value);
  }
};

#endif // CACHEHASH_H
