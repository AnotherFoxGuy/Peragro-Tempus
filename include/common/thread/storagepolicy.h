/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
 * @file storagepolicy.h
 *
 * @basic Wrappers for various object storage policies.
 */

#ifndef PT_THREAD_STORAGEPOLICY_H
#define PT_THREAD_STORAGEPOLICY_H

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace PT
{
  namespace Thread
  {
    template<typename T>
    struct OwnedStorage
    {
      typedef boost::scoped_ptr<T> StorageType;
      typedef T* ArgType;
      OwnedStorage() {}
      OwnedStorage(ArgType obj) : object(obj) {}
      inline T* Get() const { return object.get(); }
      inline void Set(ArgType obj) { object.reset(obj); }
      StorageType object;
    };

    template<typename T>
    struct ExternalStorage
    {
      typedef T* StorageType;
      typedef T* ArgType;
      ExternalStorage() : object(0) {}
      ExternalStorage(ArgType obj) : object(obj) {}
      inline T* Get() const { return object; }
      inline void Set(ArgType obj) { object = obj; }
      StorageType object;
    };

    template<typename T>
    struct SharedStorage
    {
      typedef boost::shared_ptr<T> StorageType;
      typedef const StorageType& ArgType;
      SharedStorage() {}
      SharedStorage(ArgType obj) : object(obj) {}
      inline T* Get() const { return object.get(); }
      inline void Set(ArgType obj) { object = obj; }
      StorageType object;
    };

  } // Thread namespace
} // PT namespace

#endif
