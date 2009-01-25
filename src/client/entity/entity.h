/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PTENTITY_H
#define PTENTITY_H

#include <cssysdef.h>

#include <csutil/ref.h>

#include <physicallayer/entity.h>
#include <physicallayer/pl.h>

#include <string>

#include "common/event/event.h"
#include "common/event/eventmanager.h"

#include "include/client/component/component.h"

#include "common/entity/entity.h"

struct iMeshWrapper;

namespace PT
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Provides abstract mechanism and basic functionality for the entity
     * manipulation. If you want to add a new entity type, inherit this class,
     * or one of its children. Overload the appropriate methods when doing so.
     */
    class Entity : public Common::Entity::Entity
    {
    protected:
      /// List of components this entity has.
      csRefArray<ComponentInterface> components;

      /// List of listeners this entity has.
      csRefArray<Events::EventHandlerCallback> eventHandlers;

      /**
       * This is a convenience constructor possibly needed for children classes.
       * @todo Recheck if this is actually needed.
       */
      Entity() : celEntity(0) {}

      ///CEL entity of the entity. See the CEL documentation for more info.
      csWeakRef<iCelEntity> celEntity;

      /**
       * Initialises the object's CEL entity. This includes creation of CEL
       * entity, creation and setup of some of its properties, and placing of
       * the entity in game world.
       */
      void CreateCelEntity ();

      /**
       * This method provides an abstract interface for initialising all the
       * required properties of the entity when it's created.
       * @todo Might be unneeded and even somewhat harmful. The method/function
       * calls from here maybe should be moved to the constructor.
       */
      virtual void Create() = 0;

    public:
      /**
       * Constructor that sets up the entity using the information provided by
       * EntityAddEvent event. Each class along the inheritance path should take
       * care of its own properties it defines in order to avoid code
       * duplication.
       * @param ev Event used for initialising the entity properties.
       */
      Entity(const iEvent& ev);

      /**
       * Virtual destructor.
       */
      virtual ~Entity() {}

      ///@return Entity's CEL entity.
      iCelEntity* GetCelEntity () const { return celEntity; }
      ///Make entity use some other CEL entity instead of its own.
      ///@todo Should we really be allowed to do this?
      void SetCelEntity (iCelEntity* value) { celEntity = value; }

      template<class Interface>
      csRef<Interface> GetComponent(const char* name)
      {
        for (size_t i = 0; i < components.GetSize(); i++)
        {
          csRef<ComponentInterface> comp = components.Get(i);
          if (comp.IsValid() && strcmp(name, comp->GetName()) == 0)
          {
            csRef<Interface> infa = scfQueryInterface<Interface> (comp);
            return infa;
          }
        }

        return 0;
      } // end GetComponent()

      /**
       * Changes the entity position and sector immediatelly.
       * @param pos New position of an entity.
       * @param sector New sector where the entity should reside.
       */
      virtual void Teleport(const csVector3& pos, float rotation, const std::string& sector) {}

      /**
       * Method called when player wants to interact with an entity.
       */
      virtual void Interact() {}

      /**
       * Method for activating "pose" animations on entities, like waving,
       * shaking head, apple rolling etc.
       * @todo We need some form of PoseManager or something.
       * @param poseId ID of the pose.
       */
      virtual void Pose(unsigned int poseId) {}

      /**
       * Changes the entity position and sector immediatelly
       * to the stored values.
       */
      virtual void SetFullPosition() { SetFullPosition(GetPosition(), GetRotation(), GetSectorName()); }

      /**
       * Changes the entity position and sector immediatelly.
       * @param pos New position of an entity.
       * @param sector New sector where the entity should reside.
       */
      virtual void SetFullPosition(const csVector3& pos,
                                   float rotation,
                                   const std::string& sector);

      /**
       * Reset any changes this entity might have caused.
       */
      virtual void Reset() {}

      /**
       * Set the mesh this entity uses.
       */
      virtual void SetMesh(iMeshWrapper* mesh);
    };
  } // Entity namespace
} // PT namespace

#endif // PTENTITY_H
