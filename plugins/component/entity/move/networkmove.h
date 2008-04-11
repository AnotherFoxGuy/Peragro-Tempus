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

#ifndef CLIPBOARD_DEFAULT_HEADER
#define CLIPBOARD_DEFAULT_HEADER

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csgeom/vector3.h>
#include <csutil/csstring.h>
#include <csutil/refarr.h>
#include "include/client/component/entity/move/networkmove.h"
#include "include/client/component/componenttmpl.h"

#include "client/component/stdcomponentimp.h"

struct iObjectRegistry;
struct iEvent;

namespace PT
{
  namespace Events
  {
    struct EventHandlerCallback;
  }
  namespace Entity
  {
    class Entity;
  }
} //PT namespace

DECLARE_COMPONENTFACTORY (NetworkMove)

class ComponentNetworkMove : public scfImplementationExt1<ComponentNetworkMove, ComponentCommon, iNetworkMove>
{
private:
  struct MoveToData
    {
      csVector3 destination;
      bool walking;
      float dest_angle;
      float walk_speed;
      float turn_speed;
      float elapsed_time;
      float walk_duration;
    };

private:
  iObjectRegistry* object_reg;
  PointerLibrary* pointerlib;

  PT::Entity::Entity* entity;
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  bool local_movement;
  bool UpdateOptions(iEvent& ev);

  /**
   * Change entity's 'linear' movement information (changing turning and
   * speed of an entity, or jumping status).
   * @param movement Movement data for the entity.
   */
  bool Move(iEvent& ev);

  /**
   * Move entity from point A to point B, in linear manner.
   * @param moveTo Movement data for the entity.
   * @return True if the movement has been done, false otherwise.
   * @internal The reason we use non-const pointer (and not a const
   * reference) is that moveTo data needs to be changed inside this method.
   */
  bool MoveTo(iEvent& ev);

  MoveToData* moveTo;
  csRef<PT::Events::EventHandlerCallback> cbMoveToUpdate;
  bool MoveToUpdate(iEvent& ev);
  bool RemoveMoveToUpdate();

  bool Teleport(iEvent& ev);

  /**
   * Move entity using 'dead reckoning' method.
   * @see http://en.wikipedia.org/wiki/Dead_reckoning
   * @param drupdate Dead reckoning movement data.
   */
  bool DrUpdate(iEvent& ev);

public:
    ComponentNetworkMove (iObjectRegistry*);
    virtual ~ComponentNetworkMove();

    virtual bool Initialize (PointerLibrary*, PT::Entity::Entity*);
};

#endif
