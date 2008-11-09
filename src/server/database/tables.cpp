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

#include "common/database/database.h"

#include "table-users.h"
#include "table-characters.h"
#include "table-entities.h"
#include "table-items.h"
#include "table-inventory.h"
#include "table-stats.h"
#include "table-characterstats.h"
#include "table-races.h"
#include "table-racestats.h"
#include "table-raceskills.h"
#include "table-skills.h"
#include "table-characterskills.h"
#include "table-npcaisetting.h"
#include "table-npcdialogs.h"
#include "table-npcdialoganswers.h"
#include "table-npcentities.h"
#include "table-spawnpoints.h"
#include "table-books.h"
#include "table-doors.h"
#include "table-sectors.h"
#include "table-permissions.h"
#include "table-meshes.h"
#include "table-vertices.h"
#include "table-triangles.h"
#include "table-config.h"
#include "table-zones.h"
#include "table-zonenodes.h"
#include "table-reputations.h"
#include "table-characterreputations.h"
#include "table-charactermasteries.h"

#include "tables.h"

Tables::Tables()
{
  userstable = 0;
  charactertable = 0;
  entitytable = 0;
  inventorytable = 0;
  stattable = 0;
  characterstattable = 0;
  racetable = 0;
  racestattable = 0;
  skilltable = 0;
  raceskilltable = 0;
  characterskillstable = 0;
  npcaisettingtable = 0;
  npcdialogstable = 0;
  npcdialoganswerstable = 0;
  npcentitiestable = 0;
  bookstable = 0;
  doorstable = 0;
  sectorstable = 0;
  configtable = 0;
  zonestable = 0;
  zonenodestable = 0;
  reputationstable = 0;
  characterreputationstable = 0;
}

void Tables::init(Database* db)
{
  db->update("begin transaction");
  userstable = new UsersTable(db);
  charactertable = new CharacterTable(db);
  entitytable = new EntityTable(db);
  itemtable = new ItemTable(db);
  inventorytable = new InventoryTable(db);
  stattable = new StatTable(db);
  characterstattable = new CharacterStatsTable(db);
  racetable = new RaceTable(db);
  racestattable = new RaceStatsTable(db);
  skilltable = new SkillTable(db);
  raceskilltable = new RaceSkillsTable(db);
  characterskillstable = new CharacterSkillsTable(db);
  npcaisettingtable = new NpcAiSettingTable(db);
  npcdialogstable = new NpcDialogsTable(db);
  npcdialoganswerstable = new NpcDialogAnswersTable(db);
  npcentitiestable = new NpcEntitiesTable(db);
  spawnpointstable = new SpawnPointsTable(db);
  bookstable = new BooksTable(db);
  doorstable = new DoorsTable(db);
  sectorstable = new SectorsTable(db);
  permissionstable = new PermissionsTable(db);
  meshestable = new MeshesTable(db);
  verticestable = new VerticesTable(db);
  trianglestable = new TrianglesTable(db);
  configtable = new ConfigTable(db);
  zonestable = new ZonesTable(db);
  zonenodestable = new ZonenodesTable(db);
  reputationstable = new ReputationsTable(db);
  characterreputationstable = new CharacterReputationsTable(db);
  db->update("commit");
}

Tables::~Tables()
{
  delete userstable;
  delete charactertable;
  delete entitytable;
  delete itemtable;
  delete inventorytable;
  delete stattable;
  delete characterstattable;
  delete racetable;
  delete racestattable;
  delete raceskilltable;
  delete skilltable;
  delete characterskillstable;
  delete npcaisettingtable;
  delete npcdialogstable;
  delete npcdialoganswerstable;
  delete npcentitiestable;
  delete spawnpointstable;
  delete bookstable;
  delete doorstable;
  delete sectorstable;
  delete permissionstable;
  delete meshestable;
  delete verticestable;
  delete trianglestable;
  delete configtable;
  delete zonestable;
  delete zonenodestable;
  delete reputationstable;
  delete characterreputationstable;
}
