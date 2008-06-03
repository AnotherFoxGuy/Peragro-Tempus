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
/**
 * @file componentmanager.h
 *
 * @basic The component manager.
 */

#ifndef COMPONENTMANAGER_H_
#define COMPONENTMANAGER_H_

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>
#include <csutil/hash.h>

#include "include/client/component/component.h"
#include "include/client/component/componenttmpl.h"

#include "common/reporter/reporter.h"

//--------------------------------------------------------------------------------
#define ADD_COMPONENT(componentManager, Interface, ID)                           \
csRef<Interface> component_##Interface =                                         \
  componentManager->CreateComponent<Interface>(this, ID);                        \
if (component_##Interface.IsValid())                                             \
{                                                                                \
  csRef<ComponentInterface> ca_##Interface =                                     \
    scfQueryInterface<ComponentInterface>(component_##Interface);                \
  components.Push(ca_##Interface);                                               \
}                                                                                \
else                                                                             \
  Report(PT::Error, "Failed to load the %s component!", #Interface);             \
//--------------------------------------------------------------------------------

class PointerLibrary;

namespace PT
{
  namespace Entity
  {
    class Entity;
  } // Entity namespace

  namespace Component
  {
    /**
     * Contains an array of component factories, and creates from them components
     * as required.
     */
    class ComponentManager
    {
    private:
      /// The pointer library.
      PointerLibrary* ptrlib;
      /// The object registry.
      iObjectRegistry* obj_reg;
      /// The CS plugin manager.
      csRef<iPluginManager> plugin_mgr;

      /// The hash table of component factories.
      csHash<csRef<ComponentFactoryInterface>, const char*> components;

      /**
       * Create a component interface.
       * @param name The name of the component to create.
       * @return The component interface.
       */
      csRef<ComponentInterface> CreateComponent(const char* name);

    public:
      /**
       * Base constructor.
       */
      ComponentManager(PointerLibrary* ptrlib);
      /**
       * Destructor.
       */
      ~ComponentManager();

      /**
       * Initialize the ComponentManager.
       * @return True, indicating success.
       */
      bool Initialize();

      /**
       * Create an instance of the templated component, initialize it, and
       * return it.
       * @param Interface The interface of the component to be created.
       * @param entity The entity to create the component for.
       * @param name The name of the component.
       * @return The component interface.
       */
      template<class Interface>
      csRef<Interface> CreateComponent(PT::Entity::Entity* entity, const char* name);

      /**
       * Check if a component factory is in the hash table, and load it if not.
       * @param name The name of the component to get the factory of.
       * @return Whether the factory was successfully loaded.
       */
      bool LoadComponentFactory(const char* name);

    };

    // Template implementations must reside in the header, both its declaration
    // and definition must be available in every translation unit that uses it.
    template<class Interface>
    csRef<Interface> ComponentManager::CreateComponent(PT::Entity::Entity * entity, const char* name)
    {
      csRef<ComponentInterface> comp = CreateComponent(name);
      if (!comp.IsValid()) return 0;

      csRef<Interface> infa = scfQueryInterface<Interface> (comp);

      if (!infa.IsValid()) return 0;

      infa->Initialize(ptrlib, entity);

      return infa;
    } // end CreateComponent()

  } // Component namespace
} // PT namespace

#endif // COMPONENTMANAGER_H_
