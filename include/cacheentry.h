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

#ifndef ICACHEENTRY_H
#define ICACHEENTRY_H

#include <cssysdef.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <csutil/refcount.h>
#include <csutil/refarr.h>

#include <string>

struct iObjectRegistry;
struct iCollection;
struct iMeshWrapper;
class CacheManager;

/**
 * The CacheEntry interface.
 */
struct iCacheEntry : public virtual iBase
{
  SCF_INTERFACE(iCacheEntry, 1,0,0);

  virtual bool IsFinished() const = 0;
  virtual bool WasSuccessful() const = 0;
  virtual size_t GetSize() const = 0;

  // Convience function.
  virtual iMeshWrapper* Create(const std::string& meshName,
                               const std::string& factoryName) = 0;
};

/**
 * The iCacheUser interface.
 */
struct iCacheUser : public csRefCount
{
  friend class CacheManager;

private:
  virtual void Process() 
  {
    for (size_t i = 0; i < loadingCacheEntries.GetSize(); i++)
    {
      csRef<iCacheEntry> e = loadingCacheEntries.Get(i);
      if (e->IsFinished())
      {
        DoneLoading(e);
        loadingCacheEntries.DeleteIndex(i);
        loadedCacheEntries.Push(e);
      }
    }
  }

protected:
  csRefArray<iCacheEntry> loadingCacheEntries;
  csRefArray<iCacheEntry> loadedCacheEntries;

public:
  iCacheUser(iObjectRegistry* object_reg) {}
  virtual ~iCacheUser () {}
  virtual void DoneLoading(iCacheEntry* cacheEntry) = 0;
};

#endif // ICACHEENTRY_H
