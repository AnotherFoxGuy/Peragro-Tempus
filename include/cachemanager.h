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

#ifndef ICACHEMANAGER_H
#define ICACHEMANAGER_H

#include "include/cachemanager.h"

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>

#include <csutil/ref.h>

#include <string>

struct iObjectRegistry;
struct iCacheEntry;
struct iCacheUser;

struct iCacheManager : public virtual iBase
{
  SCF_INTERFACE(iCacheManager, 1,0,0);

  /// From iComponent.
  virtual bool Initialize(iObjectRegistry*) = 0;

  /// Returns the object registry.
  virtual iObjectRegistry* GetObjectRegistry() = 0;

  /// Returns the cache size.
  virtual size_t GetCacheSize() = 0;

  /// Returns the used cache size.
  virtual size_t GetUsedCacheSize() = 0;

  /**
   * Get or create a new CacheEntry.
   * @param fileName The file name of the factory.
   */
  virtual csRef<iCacheEntry> Get(const std::string& fileName) = 0;

  virtual void AddListener(iCacheUser* user) = 0;

  virtual void RemoveListener(iCacheUser* user) = 0;
};

#endif // ICACHEMANAGER_H
