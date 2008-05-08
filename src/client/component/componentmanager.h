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

#ifndef COMPONENTMANAGER_H_
#define COMPONENTMANAGER_H_

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>
#include <csutil/scf_implementation.h>
#include <csutil/hash.h>

#include "include/client/component/component.h"
#include "include/client/component/componenttmpl.h"

class PointerLibrary;

namespace PT
{
  namespace Entity
  {
    class Entity;
  }

  class ComponentManager
  {
  private:
    PointerLibrary* ptrlib;
    iObjectRegistry* obj_reg;
    csRef<iPluginManager> plugin_mgr;

    csHash<csRef<ComponentFactoryInterface>, const char*> components;

    csRef<ComponentInterface> CreateComponent(const char* name);

  public:
   /**
    * Base constructor
    */
    ComponentManager(PointerLibrary* ptrlib);
    ~ComponentManager();

   /**
    *Initialize the ComponentManager
    *@return True, indicating success
    */
    bool Initialize();

    template<class Interface>
    csRef<ComponentInterface> CreateComponent(PT::Entity::Entity * entity, const char* name);

    bool LoadComponentFactory(const char* name);

  };
}

#endif // COMPONENTMANAGER_H_
