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

#include "client/entity/ptentity.h"
#include "client/entity/pc/pcentity.h"
#include "client/entity/npc/npcentity.h"
#include "client/entity/door/doorentity.h"
#include "client/entity/item/itementity.h"
#include "client/entity/mount/mountentity.h"

#include "common/util/mutex.h"

#include "client/network/network.h"

#include "client/entity/movement.h"

#include "client/event/entityevent.h"

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
			csPDelArray<PtEntity> entities;

			csRef<iEngine> engine;
			csRef<iVFS> vfs;
			csRef<iStringSet> stringset;
			csRef<iVirtualClock> vc;
			csRef<iLoader> loader;
			csRef<iCelPlLayer> pl;
			csRef<iObjectRegistry> obj_reg;

			Client* client;

		private:
			csArray<PT::Events::Eventp> events;
			void ProcessEvents();

		private:
			csPDelArray<MoveToData> move_to_entity_name;
			csPDelArray<DrUpdateData> drupdate_entity_name;
			csPDelArray<UpdatePcPropData> update_pcprop_entity_name;
			csPDelArray<EquipData> equip_entity_name;
			csPDelArray<MountData> mount_entity_name;
			csPDelArray<UnMountData> unmount_entity_name;
			csPDelArray<TeleportData> teleport_entity_name;

			Mutex mutex;

			unsigned int own_char_id;
			csWeakRef<iPcDefaultCamera> owncam;
			csWeakRef<iCelEntity> owncelent;
			PtEntity* ownent;
			csString ownname;

			bool playing;
			bool world_loaded;

			void moveToEntity();
			void DrUpdateEntity();
			void updatePcProp();
			void equip();
			void mount();
			void unmount();
			void teleport();

			float GetAngle (const csVector3& v1, const csVector3& v2);

		public:

			EntityManager (iObjectRegistry* obj_reg);
			~EntityManager ();

			bool Initialize ();
			void Handle();

			bool GetEntityEvents(PT::Events::Eventp ev);

			bool AddEntity(PT::Events::Eventp ev);
			bool RemoveEntity(PT::Events::Eventp ev);
			bool MoveEntity(PT::Events::Eventp ev);

			void moveEntity(int entity_id, float walk_speed, float* ori, float* dst); 
			void DrUpdateEntity(DrUpdateData* drupdate);
			void teleport(int entity_id, float* pos, const char* sector);
			void updatePcProp(int entity_id, const char *pcprop,celData &value);
			void DrUpdateOwnEntity();
			void equip(int entity_id, int item_id, int slot_id);
			void mount(int entity_id, int mount_id, bool control);
			void unmount(int entity_id, int mount_id);

			iCelEntity* findCelEntById(int id);
			PtEntity* findPtEntById(int id);

			void setCharacter(unsigned int own_char) { own_char_id = own_char; }
			iPcDefaultCamera* getOwnCamera() { return owncam; }
			iCelEntity* getOwnCelEntity() { return owncelent; }
			PtEntity* getOwnPtEntity() { return ownent; }
			unsigned int GetOwnId() { return own_char_id; }
			csString GetOwnName() { return ownname; }

			void setPlaying(bool value) { playing = value; }
			void setWorldloaded(bool value) { world_loaded = value; }

			void delAllEntities();
		};

	} // Entity namespace 
} // PT namespace 

#endif // PTENTITYMANAGER_H
