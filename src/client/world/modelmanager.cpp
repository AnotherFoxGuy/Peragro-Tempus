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

#include "modelmanager.h"

#include <csutil/scf_implementation.h>
#include <iengine/region.h>

#include "factory.h"

namespace PT
{
  ModelManager::ModelManager(iObjectRegistry* object_reg)
  {
    this->object_reg = object_reg;
  }

  ModelManager::~ModelManager()
  {
  }

  csRef<Factory> ModelManager::Get(const std::string& fileName)
  {
    // Search.
    factories.Compact();
    for (size_t i = 0; i < factories.GetSize(); i++)
    {
      csRef<Factory> current = csRef<Factory>(factories.Get(i));
      if (current->fileName == fileName)
      {
        return current;
      }
    }

    // Not found: Create a new one.
    csRef<Factory> fact; fact.AttachNew(new Factory(fileName, object_reg));
    fact->Load();
    factories.Push(fact);

    return fact;
  } // end Get()

} // PT namespace
