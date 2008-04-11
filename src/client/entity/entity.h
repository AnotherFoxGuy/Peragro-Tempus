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
#include <iutil/virtclk.h>

#include <physicallayer/entity.h>
#include <physicallayer/pl.h>
#include <iutil/objreg.h>
#include <iengine/engine.h>
#include <iengine/movable.h>
#include <iengine/sector.h>

#include <iengine/mesh.h>
#include <imesh/spritecal3d.h>
#include <imesh/object.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <physicallayer/persist.h>
#include <celtool/initapp.h>
#include <celtool/persisthelper.h>
#include <celtool/stdparams.h>
#include <behaviourlayer/bl.h>
#include <propclass/test.h>
#include <propclass/mesh.h>
#include <propclass/meshsel.h>
#include <propclass/solid.h>
#include <propclass/inv.h>
#include <propclass/chars.h>
#include <propclass/move.h>
#include <propclass/prop.h>
#include <propclass/tooltip.h>
#include <propclass/defcam.h>
#include <propclass/gravity.h>
#include <propclass/timer.h>
#include <propclass/region.h>
#include <propclass/input.h>
#include <propclass/linmove.h>
#include <propclass/actormove.h>
#include <propclass/trigger.h>
#include <propclass/zone.h>
#include <propclass/sound.h>
#include <propclass/colldet.h>
#include <propclass/quest.h>

#include <string>

#include "client/entity/movement.h"
#include "client/event/event.h"
#include "client/event/eventmanager.h"

#include "include/client/component/component.h"

namespace PT
{

  namespace Entity
  {
    /**
     * @ingroup entities
     * Enum representing the possible entity types that are handled by the game
     * engine.
     */
    enum EntityType
    {
      PCEntityType      =0,
      NPCEntityType     =1,
      DoorEntityType    =2,
      ItemEntityType    =3,
      MountEntityType   =4,
      TeleportEntityType=5
    };

    /**
     * @ingroup entities
     * Provides abstract mechanism and basic functionality for the entity
     * manipulation. If you want to add a new entity type, inherit this class,
     * or one of its children. Overload the appropriate methods when doing so.
     */
    class Entity
    {
    protected:
      ///Unique ID of the entity.
      unsigned int id;
      ///Type of the entity.
      EntityType type;
      ///Name of the entity. Shown on mouse-over (for example).
      std::string name;
      ///Name of the mesh used for this entity.
      std::string meshName;
      /// The file in which this entity's mesh resides.
      std::string fileName;
      ///Name of the sector where the entity resides.
      std::string sectorName;
      ///Position of the entity within a sector.
      csVector3 pos;
      ///Rotation of the entity.
      float rot;

      /// List of components this entity has.
      csRefArray<iBase> components;

      /// List of listeners this entity has.
      csRefArray<Events::EventHandlerCallback> eventHandlers;

      /**
       * This is a convenience constructor possibly needed for children classes.
       * @todo Recheck if this is actually needed.
       */
      Entity() : id(-1), celEntity(0) {}

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

      ///@return Entity's unique ID.
      unsigned int GetId () const { return id; }
      ///Set the entity's unique ID to a given value.
      ///@todo Should we really be allowed to do this?
      void SetId (unsigned int value) { id = value; }

      ///@return Entity's type.
      int GetType () const { return type; }
      ///Set the entity's type to a given value.
      ///@todo Should we really be allowed to do this?
      void SetType (EntityType value)  { type = value; }

      ///@return Entity's name.
      const std::string& GetName () const { return name; }
      ///Set the entity's name to a given value.
      void SetName (const std::string& value) { name = value; }

      ///@return Entity's mesh name.
      const std::string& GetMeshName () const { return meshName; }
      ///Set the entity's mesh name to a given value.
      void SetMeshName (const std::string& value) { meshName = value; }

      ///@return Name of the sector where entity resides.
      const std::string& GetSectorName() const { return sectorName; }
      ///Set the name of sector where the entity resides.
      void SetSectorName (std::string value) { sectorName = value; }

      ///@return Position of entity within a sector.
      const csVector3& GetPosition() const { return pos; }
      ///Set the position of entity within a sector to a given value.
      void SetPosition (csVector3 value) { pos = value; }

      ///@return Rotation of entity.
      const float GetRotation() const { return rot; }
      ///Set the rotation of entity to a given value.
      void SetRotation (float value) { rot = value; }

      ///@return Entity's CEL entity.
      iCelEntity* GetCelEntity () const { return celEntity; }
      ///Make entity use some other CEL entity instead of its own.
      ///@todo Should we really be allowed to do this?
      void SetCelEntity (iCelEntity* value) { celEntity = value; }

      /**
       * Move entity from point A to point B, in linear manner.
       * @param moveTo Movement data for the entity.
       * @return True if the movement has been done, false otherwise.
       * @internal The reason we use non-const pointer (and not a const
       * reference) is that moveTo data needs to be changed inside this method.
       */
      virtual bool MoveTo(MoveToData* moveTo) {return true;}

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
      virtual void SetFullPosition() { SetFullPosition(pos, rot, sectorName); }

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
    };
  } // Entity namespace
} // PT namespace

#endif // PTENTITY_H
