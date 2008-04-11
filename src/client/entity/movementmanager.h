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

#ifndef PT_ENTITY_MOVEMENT_MANAGER_H
#define PT_ENTITY_MOVEMENT_MANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <csgeom/math3d.h>
#include <iengine/camera.h>
#include <iutil/object.h>

#include <physicallayer/pl.h>
#include <physicallayer/datatype.h>
#include <propclass/quest.h>

#include <queue>

#include "client/entity/entity.h"
#include "client/entity/pc/pcentity.h"
#include "client/entity/npc/npcentity.h"
#include "client/entity/door/doorentity.h"
#include "client/entity/item/itementity.h"
#include "client/entity/mount/mountentity.h"

#include "client/network/network.h"
#include "client/entity/movement.h"

struct iObjectRegistry;

namespace PT
{
  namespace Entity
  {
    class MovementManager
    {
    private:
      csRef<iEngine> engine;
      csRef<iVirtualClock> vc;
      csRef<iObjectRegistry> obj_reg;
      csRef<iConfigManager> app_cfg;
      csRef<iVFS> vfs;

    private:
      csPDelArray<MoveToData> move_to_entity;

    private:
      void moveToUpdate();

      float GetAngle (const csVector3& v1, const csVector3& v2);

    private:
      csRefArray<iEvent> events;
      void ProcessEvents();
    public:
      bool GetEntityEvents(iEvent& ev);

      void RemoveMoveTos(unsigned int id);

    public:
      MovementManager (iObjectRegistry* obj_reg);
      ~MovementManager ();

      bool Initialize ();
      void Handle();

      bool MoveToEntity(iEvent& ev);
    };

  } // Entity namespace
} // PT namespace

#endif // PT_ENTITY_MOVEMENT_MANAGER_H
