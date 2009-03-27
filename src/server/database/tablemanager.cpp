/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "tablemanager.h"
#include "common/database/database.h"

// Chat stuff
#include "table-jointypes.h"
#include "table-channeltypes.h"
#include "table-defaultchannels.h"
#include "table-channels.h"
#include "table-entitychannels.h"

#include "table-users.h"
#include "table-permissions.h"

// Entity stuff
#include "table-entitytypes.h"
#include "table-entities.h"
#include "table-entitypositions.h"

#include "table-characters.h"
#include "table-pcentities.h"
#include "table-npcentities.h"
#include "table-items.h"
#include "table-doors.h"

// Item stuff
#include "table-itemtemplates.h"
#include "table-equiptypes.h"

// Character stuff
#include "table-inventory.h"
#include "table-equipment.h"
#include "table-reputationtypes.h"
#include "table-reputations.h"
#include "table-skilltypes.h"
#include "table-skills.h"
#include "table-abilitytypes.h"
#include "table-abilities.h"
#include "table-hobbytypes.h"
#include "table-hobbies.h"
#include "table-resourcetypes.h"
#include "table-resources.h"

// NPC stuff
#include "table-npcaisetting.h"
#include "table-npcdialogs.h"
#include "table-npcdialoganswers.h"

#include "table-spawnpoints.h"
#include "table-books.h"

#include "table-meshes.h"
#include "table-config.h"

#include "table-zones.h"
#include "table-zonenodes.h"


TableManager::TableManager(Database* db) : db(db)
{
}

void TableManager::Initialize()
{
  db->update("begin transaction");

  // Chat stuff
  Register(new JoinTypesTable(db));
  Register(new ChannelTypesTable(db));
  Register(new DefaultChannelsTable(db));
  Register(new ChannelsTable(db));
  Register(new EntityChannelsTable(db));

  Register(new UsersTable(db));
  Register(new PermissionsTable(db));

  // Entity stuff
  Register(new EntityTypesTable(db));
  Register(new EntityTable(db));
  Register(new EntityPositionsTable(db));

  Register(new CharactersTable(db));
  Register(new PcEntitiesTable(db));
  Register(new NpcEntitiesTable(db));
  Register(new ItemsTable(db));
  Register(new DoorsTable(db));

  // Item stuff
  Register(new ItemTemplatesTable(db));
  Register(new EquipTypesTable(db));

  // Character stuff
  Register(new InventoryTable(db));
  Register(new EquipmentTable(db));
  Register(new ReputationTypesTable(db));
  Register(new ReputationsTable(db));
  Register(new SkillTypesTable(db));
  Register(new SkillsTable(db));
  Register(new AbilityTypesTable(db));
  Register(new AbilitiesTable(db));
  Register(new HobbyTypesTable(db));
  Register(new HobbiesTable(db));
  Register(new ResourceTypesTable(db));
  Register(new ResourcesTable(db));

  // NPC stuff
  Register(new NpcAiSettingTable(db));
  Register(new NpcDialogsTable(db));
  Register(new NpcDialogAnswersTable(db));


  Register(new SpawnPointsTable(db));
  Register(new BooksTable(db));

  Register(new MeshesTable(db));
  Register(new ConfigTable(db));

  Register(new ZonesTable(db));
  Register(new ZoneNodesTable(db));

  db->update("commit");
}

TableManager::~TableManager()
{
}

void TableManager::Register(Table* table)
{
  boost::shared_ptr<Table> t(table);
  tables.push_back(t);
}

void TableManager::Register(boost::shared_ptr<Table> table)
{
  tables.push_back(table);
}

