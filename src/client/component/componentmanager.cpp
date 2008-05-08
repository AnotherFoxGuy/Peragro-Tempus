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

#include "componentmanager.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

namespace PT
{

  ComponentManager::ComponentManager(PointerLibrary* ptrlib)
  {
    this->ptrlib = ptrlib;
    obj_reg = ptrlib->getObjectRegistry();
  }

  ComponentManager::~ComponentManager()
  {
  }

  bool ComponentManager::Initialize()
  {
    plugin_mgr = csQueryRegistry<iPluginManager> (obj_reg);

    return true;
  }

  template<class Interface>
  csRef<ComponentInterface> ComponentManager::CreateComponent(PT::Entity::Entity * entity, const char* name)
  {
    csRef<ComponentInterface> comp = CreateComponent(name);
    if (!comp.IsValid()) return 0;

    csRef<Interface> infa = scfQueryInterface<Interface> (comp);

    if (!infa.IsValid()) return 0;

    infa->Initialize(ptrlib, entity);

    return infa;
  }

  csRef<ComponentInterface> ComponentManager::CreateComponent(const char* name)
  {
    if (!LoadComponentFactory(name)) return 0;

    csRef<ComponentFactoryInterface> fact = components.Get(name, 0);
    csRef<ComponentInterface> interf = fact->CreateComponent(name);

    return interf;
  }

  bool ComponentManager::LoadComponentFactory(const char* name)
  {
    if (components.Contains(name)) return true;

    csRef<ComponentFactoryInterface> fact =
      csLoadPlugin<ComponentFactoryInterface> (plugin_mgr, name);

    if (fact)
    {
      components.Put(name, fact);
      return true;
    }
    else
    {
      return false;
    }
  }

}
