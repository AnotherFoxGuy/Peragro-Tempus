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
#include "include/cacheentry.h"
struct iObjectRegistry;
class CacheManager;

/**
 * The iCacheUser interface.
 */
struct iCacheUser /*: public csRefCount */
{
  friend class CacheManager;

private:
  csRef<iCacheManager> cacheManager;
  bool registered;

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
        if (!loadedCacheEntries.Get(i)->WasSuccessful())
        { success = false; break;}
      DoneLoading(success);
      UnRegister();
    }
  }

  void Register()
  {
    if (!registered)
    {
      cacheManager->AddListener(this);
      registered = true;
    }
  }

  void UnRegister()
  {
    cacheManager->RemoveListener(this);
    registered = false;
  }

protected:
  csRefArray<iCacheEntry> loadingCacheEntries;
  csRefArray<iCacheEntry> loadedCacheEntries;

public:
  iCacheUser(iObjectRegistry* object_reg, const std::string& tag = "iCacheManager") : registered(false)
  {
    cacheManager = csQueryRegistryTagInterface<iCacheManager> (object_reg, tag.c_str());
    if (!cacheManager.IsValid())
    {
      csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));
      cacheManager = csLoadPlugin<iCacheManager> (plugin_mgr, "peragro.cachemanager");
      object_reg->Register (cacheManager, tag.c_str());
    }
  }

  iCacheUser(iCacheManager* cacheManager) : registered(false)
  {
    this->cacheManager = cacheManager;
  }

  virtual ~iCacheUser () 
  {
    UnRegister();
  }

  void Load(const std::string& fileName)
  {
    csRef<iCacheEntry> e(cacheManager->Get(fileName));
    // Check if we haven't loaded this before.
    if ((loadedCacheEntries.Contains(e) != csArrayItemNotFound)
        || (loadingCacheEntries.Contains(e)) != csArrayItemNotFound)
      return;
    loadingCacheEntries.Push(e);
    Register();
  }

  bool IsReady()
  {
    return loadingCacheEntries.GetSize() == 0;
  }

  virtual void Loaded(iCacheEntry* cacheEntry) = 0;

  virtual void DoneLoading(bool success) = 0;
};

#endif // ICACHEUSER_H
