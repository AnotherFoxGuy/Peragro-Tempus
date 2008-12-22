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
 * @file cachemanager.h
 *
 * @basic
 */

#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include "include/cachemanager.h"

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>

#include <csutil/ref.h>
#include <csutil/weakref.h>
#include <csutil/refarr.h>

#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>

#include "cachehash.h"

#include <string>

struct iObjectRegistry;
struct iCacheEntry;

class CacheManager : public scfImplementation3<CacheManager,iCacheManager,iEventHandler,iComponent>
{
private:
  size_t cacheSize;
  size_t maintainFPS;
  bool UpdateOptions();

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);

private:
  /// The object registry.
  iObjectRegistry* object_reg;

  /// The hash of loaded factories.
  //CacheHash<csWeakRef> cacheEntries;
  CacheHash<csWeakRef<iCacheEntry> > cacheEntries;

  /// The actual cache.
  //CacheHash<csRef> cache;
  CacheHash<csRef<iCacheEntry> > cache;
  csArray<std::string> cacheQueue;

private:
  csArray<iCacheUser*> listeners;

  /// The queue of events waiting to be handled.
  csRef<iEventQueue> eventQueue;
  /// The event name registry, used to convert event names to IDs and back.
  csRef<iEventNameRegistry> nameRegistry;

  bool HandleEvent(iEvent& ev);

  CS_EVENTHANDLER_NAMES ("peragro.cachemanager")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

private:
  typedef std::pair<std::string, csRef<iCacheEntry> > Entry;

  void InsertInCache(const std::string& realPath, iCacheEntry* entry);

  void RemoveFromCache(size_t index);

  Entry GetFromCache(size_t index);

  void Overflow();

public:
  /// Constructor.
  CacheManager(iBase* iParent);
  /// Destructor.
  ~CacheManager();

  /// From iComponent.
  virtual bool Initialize(iObjectRegistry*);

  /// Returns the object registry.
  iObjectRegistry* GetObjectRegistry() { return object_reg; }

  /// Returns the cache size.
  size_t GetCacheSize() { return cacheSize; }

  /// Returns the used cache size.
  size_t GetUsedCacheSize();

  /**
   * Get or create a new CacheEntry.
   * @param fileName The file name of the factory.
   */
  csRef<iCacheEntry> Get(const std::string& fileName);

  void AddListener(iCacheUser* user);

  void RemoveListener(iCacheUser* user);
};

#endif // CACHEMANAGER_H