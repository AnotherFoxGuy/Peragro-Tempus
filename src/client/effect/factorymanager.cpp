/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "factorymanager.h"

#include <csutil/scf_implementation.h>
#include <iengine/collection.h>
#include <iutil/cfgmgr.h>
#include <iutil/objreg.h>

#include "effect.h"

namespace PT
{
  namespace Effect
  {
    FactoryManager::FactoryManager(iObjectRegistry* object_reg)
    {
      this->object_reg = object_reg;

      csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager>(object_reg);
      cachesize = app_cfg->GetInt("Client.effectmanager.cachesize", 10);
    }

    FactoryManager::~FactoryManager()
    {
    }

    void FactoryManager::Overflow()
    {
      if (normalfactories.GetSize() > cachesize)
      {
        csRef<Factory> oldest = csRef<Factory>(normalfactories.Get(0));
        weakFactories.Push(oldest);
        normalfactories.DeleteIndex(0);
      }
    }

    csRef<Factory> FactoryManager::Get(const std::string& fileName)
    {
      // Search.
      for (size_t i = 0; i < normalfactories.GetSize(); i++)
      {
        csRef<Factory> current = csRef<Factory>(normalfactories.Get(i));
        if (current->fileName == fileName)
        {
          // Move to the back(most recent).
          normalfactories.DeleteIndex(i);
          normalfactories.Push(current);
          return current;
        }
      }

      weakFactories.Compact();
      for (size_t i = 0; i < weakFactories.GetSize(); i++)
      {
        csRef<Factory> current = csRef<Factory>(weakFactories.Get(i));
        if (current->fileName == fileName)
        {
          // Move to the back(most recent) of our cache.
          weakFactories.DeleteIndex(i);
          normalfactories.Push(current);
          Overflow();
          return current;
        }
      }

      // Not found: Create a new one.
      csRef<Factory> fact; fact.AttachNew(new Factory(fileName, object_reg));
      fact->Load();
      normalfactories.Push(fact);

      Overflow();

      return fact;
    } // end Get()
  } // Effect namespace 
} // PT namespace
