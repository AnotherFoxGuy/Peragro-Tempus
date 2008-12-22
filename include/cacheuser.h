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
 * @file cacheuser.h
 *
 * @basic .
 */

#ifndef ICACHEUSER_H
#define ICACHEUSER_H

#include <cssysdef.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <csutil/refcount.h>
#include <csutil/refarr.h>
#include <iutil/plugin.h>

#include <string>

#include "include/cachemanager.h"
struct iObjectRegistry;
struct iCacheManager;
class CacheManager;

/**
 * The iCacheUser interface.
 */
struct iCacheUser /*: public csRefCount */
{
  friend CacheManager;

private:
  csRef<iCacheManager> cacheManager;

  void Process(size_t amount = ~0) 
  {
    size_t am = std::min(loadingCacheEntries.GetSize(), amount); 
    for (size_t i = 0; i < am; i++)
    {
      csRef<iCacheEntry> e = loadingCacheEntries.Get(i);
      if (e->IsFinished())
      {
        Loaded(e);
        loadedCacheEntries.Push(e);
        loadingCacheEntries.DeleteIndex(i);
      }
    }
    if (loadingCacheEntries.IsEmpty())
    {
      bool success = true;
      for (size_t i = 0; i < loadedCacheEntries.GetSize(); i++)
        if (!loadingCacheEntries.Get(i)->WasSuccessful())
        { success = false; break;}
      DoneLoading(success);
      cacheManager->RemoveListener(this);
    }
  }

protected:
  csRefArray<iCacheEntry> loadingCacheEntries;
  csRefArray<iCacheEntry> loadedCacheEntries;

public:
  iCacheUser(iObjectRegistry* object_reg, const std::string& tag) 
  {
    cacheManager = csQueryRegistryTagInterface<iCacheManager> (object_reg, tag.c_str());
    if (!cacheManager.IsValid())
    {
      csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));
      cacheManager = csLoadPlugin<iCacheManager> (plugin_mgr, "peragro.cachemanager");
      object_reg->Register (cacheManager, tag.c_str());
    }
    cacheManager->AddListener(this);
  }

  iCacheUser(iCacheManager* cacheManager) 
  {
    this->cacheManager = cacheManager;
    cacheManager->AddListener(this);
  }

  virtual ~iCacheUser () 
  {
    cacheManager->RemoveListener(this);
  }

  virtual void Loaded(iCacheEntry* cacheEntry) = 0;

  virtual void DoneLoading(bool success) = 0;
};

#endif // ICACHEUSER_H
