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

#include "common/eventcs/event.h"
#include "common/eventcs/eventmanager.h"

#include "include/client/component/component.h"

#include "client/entity/base/entity.h"

#include "common/util/geomhelper.h"

struct iMeshWrapper;

namespace Client
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Provides abstract mechanism and basic functionality for the entity
     * manipulation. If you want to add a new entity type, inherit this class,
     * or one of its children. Overload the appropriate methods when doing so.
     */
    class Entity : public PT::Entity::Entity
    {
    friend class EntityManager;

    protected:
      /// List of components this entity has.
      csRefArray<ComponentInterface> components;

      /// List of listeners this entity has.
      csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

      virtual void CreateCelEntity ();

    public:
      Entity(Common::Entity::EntityType type);

      /**
       * Virtual destructor.
       */
      virtual ~Entity() {}

      /**
       * This method provides an abstract interface for initialising all the
       * required properties of the entity with the EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the character properties.
       */
      virtual void Initialize(const iEvent& ev);

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
      virtual void Teleport(const WFMath::Point<3>& pos, float rotation, const std::string& sector)
        { SetFullPosition (pos,rotation,sector); }

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

      ///Set the position of entity, an overload that takes csVector3.
      void SetPosition(const csVector3& v)
        { Common::Entity::Entity::SetPosition(VectorHelper::Convert(v)); }

      //using Common::Entity::Entity::SetFullPosition;
      virtual void SetFullPosition() { SetFullPosition(GetPosition(), GetRotation(), GetSectorName()); }

      /**
       * Changes the entity position and sector immediatelly.
       * @param pos New position of an entity.
       * @param sector New sector where the entity should reside.
       */
      virtual void SetFullPosition(const WFMath::Point<3>& pos,
                                   float rotation,
                                   const std::string& sector);

      /**
       * Set the mesh this entity uses.
       */
      virtual void SetMesh(iMeshWrapper* mesh);
    };
  } // Entity namespace
} // Client namespace

#endif // PTENTITY_H
