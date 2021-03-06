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

#include "cachemanager.h"

#include "cacheentry.h"

#include "include/cacheuser.h"

#include <ivaria/reporter.h>
#include <iutil/vfs.h>
#include <iutil/cfgmgr.h>

#include <csutil/csevent.h>

SCF_IMPLEMENT_FACTORY (CacheManager)

void CacheManager::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.cachemanager", msg, arg);
  va_end (arg);
}

CacheManager::CacheManager(iBase* iParent)
  : scfImplementationType(this, iParent), object_reg(0)
{
} // end CacheManager()

bool CacheManager::Initialize(iObjectRegistry* obj_reg)
{
  object_reg = obj_reg;

  UpdateOptions();

  eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return false;

  nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
  if (!nameRegistry) return false;

  eventQueue->RegisterListener(this);

  //Register for the Frame event, for Handle().
  eventQueue->RegisterListener (this, nameRegistry->GetID("crystalspace.frame"));

  return true;
} // end Initialize()

CacheManager::~CacheManager()
{
  if (eventQueue)
    eventQueue->RemoveListener(this);
} // end ~CacheManager()

bool CacheManager::UpdateOptions()
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (object_reg);
  if (!app_cfg) return false;

  // Convert from megabytes to bytes.
  cacheSize = app_cfg->GetInt("Peragro.CacheManager.CacheSize", 32) * 1048576;

  maintainFPS = app_cfg->GetInt("Peragro.CacheManager.maintainFPS", 40);

  return true;
} // end UpdateOptions()

void CacheManager::InsertInCache(const std::string& realPath, iCacheEntry* entry)
{
  cache.Put(realPath, entry);
  cacheQueue.Push(realPath);
} // end InsertInCache()

void CacheManager::RemoveFromCache(size_t index)
{
  const std::string realPath(cacheQueue.Get(index));
  cache.DeleteAll(realPath);
  cacheQueue.DeleteIndex(index);
} // end RemoveFromCache()

CacheManager::Entry CacheManager::GetFromCache(size_t index)
{
  const std::string realPath(cacheQueue.Get(0));
  csRef<iCacheEntry> entry(cache.Get(realPath, 0));
  return Entry(realPath, entry);
} // end GetFromCache()

void CacheManager::Overflow()
{
  if (GetUsedCacheSize() > cacheSize)
  {
    cacheEntries.Compact();
    cache.Compact();
    int delta = (int)GetUsedCacheSize() - (int)cacheSize;
    while (delta > 0)
    {
      const Entry entry(GetFromCache(0));
      cacheEntries.Put(entry.first, entry.second);
      RemoveFromCache(0);
      delta -= (int)entry.second->GetSize();
    } // end while
  } // end if
} // end Overflow()

size_t CacheManager::GetUsedCacheSize()
{
  size_t size = 0;
  CacheHash<csRef<iCacheEntry> >::GlobalIterator it(cache.GetIterator());
  while(it.HasNext())
  {
    csRef<iCacheEntry> el(it.Next());
    if (el) size += el->GetSize();
  }
  return size;
} // end GetUsedCacheSize()

std::string RealPath(iObjectRegistry* object_reg, const std::string& fileName)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs) return "ERROR";

  // Seperate the path of the filename.
  size_t p = fileName.find_last_of("/");
  const std::string path(fileName.substr(0,p+1));
  const std::string file(fileName.substr(p+1,fileName.length()));

  // Get the real path.
  csRef<iDataBuffer> buf(vfs->GetRealPath(path.c_str()));
  if (!buf) return "ERROR";
  const std::string realPath(buf->GetData());

  return realPath+"/"+file;
}

csRef<iCacheEntry> CacheManager::Get(const std::string& fileName)
{
  // Since multiple relative paths can point to the same file,
  // we convert to the real path.
  const std::string realPath(RealPath(object_reg, fileName));

  // Search cache.
  if (cache.Contains(realPath))
    return cache.Get(realPath, 0);

  // Search entries.
  if (cacheEntries.Contains(realPath))
  {
    // Entry has been used again, so move it to the cache.
    csRef<iCacheEntry> entry(cacheEntries.Get(realPath, 0));
    cacheEntries.Delete(realPath, entry);
    InsertInCache(realPath, entry);
    Overflow();
    return entry;
  }

  // Not found: Create a new one.
  csRef<iCacheEntry> entry; entry.AttachNew(new CacheEntry(fileName, object_reg));
  cache.Put(realPath, entry);

  return entry;
} // end Get()

void CacheManager::AddListener(iCacheUser* user)
{
  RemoveListener(user); // Make sure there are no duplicates.
  listeners.Push(user);
} // end AddListener()

void CacheManager::RemoveListener(iCacheUser* user)
{
  listeners.Delete(user);
} // end RemoveListener()

bool CacheManager::HandleEvent(iEvent& ev)
{
  for (size_t i = 0; i < listeners.GetSize(); i++)
  {
    iCacheUser* u = listeners.Get(i);
    u->Process();
  }

  return true;
} // end HandleEvent()
