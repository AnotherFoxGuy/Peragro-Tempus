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

#ifndef PTENTITYMANAGER_H
#define PTENTITYMANAGER_H

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

#include "client/entity/entity.h"
#include "client/entity/pc/pcentity.h"
#include "client/entity/player/playerentity.h"
#include "client/entity/npc/npcentity.h"
#include "client/entity/door/doorentity.h"
#include "client/entity/item/itementity.h"
#include "client/entity/mount/mountentity.h"

#include "client/network/network.h"

#include "common/event/entityevent.h"
#include "common/event/stateevent.h"
#include "common/event/inputevent.h"


class Effect;

struct iObjectRegistry;
struct iLoader;

namespace PT
{
  namespace Entity
  {
    class EntityManager
    {
    private:
      csPDelArray<Entity> entities;

      csRef<iEngine> engine;
      csRef<iVFS> vfs;
      csRef<iStringSet> stringset;
      csRef<iVirtualClock> vc;
      csRef<iLoader> loader;
      csRef<iCelPlLayer> pl;
      csRef<iObjectRegistry> obj_reg;

      Client* client;

    private:
      csRefArray<iEvent> events;
      void ProcessEvents();

      void ProcessLostEntities();

      // A default sector for entities to be added in.
      csRef<iSector> defaultSector;

    private:
      unsigned int playerId;

    private:
      bool world_loaded;

    private:
      PT_CALLBACK_HANDLER_LISTENERS

      bool GetEntityEvents(iEvent& ev);

      bool AddEntity(iEvent& ev);
      bool RemoveEntity(iEvent& ev);
      bool Equip(iEvent& ev);
      bool Mount(iEvent& ev);
      bool UnMount(iEvent& ev);
      /**
       * Handler for received EntityPose events.
       * @param ev Entity pose event to be processed.
       * @return True if successful, false otherwise.
       */
      bool EntityPose(iEvent& ev);

      /**
       * Handler for when the world has been loaded.
       * @param ev WorldLoaded event to be processed.
       * @return True if successful, false otherwise.
       */
      bool WorldLoaded(iEvent& ev);

      bool OnInteract(iEvent& ev);

      bool SetOwnId(iEvent& ev);

    public:
      EntityManager ();
      ~EntityManager ();

      bool Initialize ();
      void Handle();

      void DrUpdateOwnEntity();

      iCelEntity* findCelEntById(unsigned int id);

      Entity* findPtEntById(unsigned int id);

      unsigned int GetPlayerId() { return playerId; }

      void setWorldloaded(bool value) { world_loaded = value; }

      void delAllEntities();
    };

  } // Entity namespace
} // PT namespace

#endif // PTENTITYMANAGER_H
