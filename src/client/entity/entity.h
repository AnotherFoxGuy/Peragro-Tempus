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

#include <iutil/string.h>
#include <iutil/vfs.h>
#include <csutil/ref.h>
#include <csutil/csstring.h>
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

#include "client/entity/movement.h"

namespace PT
{
  namespace Events
  {
    //Forward declaration. Including "entityevent.h" file
    //will cause circular dependency.
    class EntityAddEvent;
  }

  namespace Entity
  {
    enum EntityType
    {
      PCEntityType=0,
      NPCEntityType=1,
      DoorEntityType=2,
      ItemEntityType=3,
      MountEntityType=4
    };

    /**
     * All entities are dervied from this class. This class provides an
     * abstract mechanism for handling various operations on entities.
     */
    class Entity
    {
    protected:
      unsigned int id;
      EntityType type;
      csString name;
      csString meshname;
      csString sectorname;
      csVector3 pos;

      Entity() : id(-1), celentity(0) {}

      csWeakRef<iCelEntity> celentity;

      void CreateCelEntity ();

      virtual void Create() {}

    public:

      Entity(EntityType type) : id(-1), type(type), celentity(0) {}

      Entity(const Events::EntityAddEvent& ev);

      virtual ~Entity() {}

      unsigned int GetId () const { return id; }
      void SetId (int value) { id = value; }
      int GetType () const { return type; }
      void SetType (EntityType value)  { type = value; }
      csString GetName () const { return name; }
      void SetName (const csString& value) { name = value; }
      csString GetMeshName () const { return meshname; }
      void SetMeshName (const csString& value) { meshname = value; }
      csString GetSectorName() const { return sectorname; }
      void SetSectorName (csString value) { sectorname = value; }
      csVector3 GetPosition() const { return pos; }
      void SetPosition (csVector3 value) { pos = value; }
      iCelEntity* GetCelEntity () const { return celentity; }
      void SetCelEntity (iCelEntity* value) { celentity = value; }

      virtual void Move(MovementData* movement) {}
      virtual bool MoveTo(MoveToData* moveTo) {return true;}
      virtual void DrUpdate(DrUpdateData* drupdate) {}
      virtual void Teleport(csVector3 pos, csString sector) {}
      virtual void UpdatePcProp(UpdatePcPropData* update_pcprop) {}
      virtual void Interact() {}
      /**
       * Overload this method in children for activating "pose" animation on
       * entities, like waving, shaking head etc.
       * @todo We need to introduce some kind of "PoseManager" in order to acommodate new poses easily.
       * @param poseId ID of the pose.
       */
      virtual void Pose(unsigned int poseId) {}
    };
  }
}

#endif // PTENTITY_H
