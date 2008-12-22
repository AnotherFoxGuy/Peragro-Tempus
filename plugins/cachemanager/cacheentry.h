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
 * @file cacheentry.h
 *
 * @basic Used to create instances of a model.
 */

#ifndef CACHEENTRY_H
#define CACHEENTRY_H

#include <cssysdef.h>

#include "include/cacheentry.h"

#include <string>

struct iObjectRegistry;
struct iCollection;
struct iThreadReturn;

/**
 * Used to load/fetch factories of a model.
 */
struct CacheEntry : public scfImplementation1<CacheEntry, iCacheEntry>
{
private:
  /// ThreadReturn of the load, let's us know when loading is done.
  csRef<iThreadReturn> threadReturn;
  /// The collection that is used for this factory.
  csRef<iCollection> collection;
  /// The object registry.
  iObjectRegistry* object_reg;
  /// Whether all textures have been precached.
  bool isPrecached;
  /// Cached size of this cacheEntry.
  mutable size_t cachedSize;

private:
  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);

public:
  /// Returns true if all textures have been precached.
  bool IsPrecached() const { return isPrecached; }

  /**
   * Precaches 'one' texture (if any) and returns.
   * Call this repeatedly over a period of time to smooth out texture
   * loading.
   * Call IsPrecached() to see if everything has been precached.
   */
  void Precache();

  /// Get the collection that is used internally for holding the data.
  iCollection* GetCollection() { return collection;  }

public:
  /**
   * Constructor.
   * @param fileName The file name.
   * @param object_reg The object registry.
   */
  CacheEntry(const std::string& fileName, iObjectRegistry* object_reg);
  /// Destructor.
  ~CacheEntry();

  /// The file name.
  std::string fileName;

  /// Returns true if finished loading, false otherwise.
  bool IsFinished() const;

  /// Returns true if successfully loaded, false otherwise.
  bool WasSuccessful() const;

  // Convience function.
  iMeshWrapper* Create(const std::string& meshName,
                       const std::string& factoryName);

  size_t GetSize() const;
};

#endif // FACTORY_H
