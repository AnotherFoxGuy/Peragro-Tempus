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

#include "database.h"
#include "table-users.h"
#include "table-characters.h"
#include "table-entities.h"
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
#include "table-books.h"
#include "table-doors.h"
#include "table-permissions.h"

Database::Database()
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
}

void Database::init()
{
  userstable = new UsersTable(this);
  charactertable = new CharacterTable(this);
  entitytable = new EntityTable(this);
  inventorytable = new InventoryTable(this);
  stattable = new StatTable(this);
  characterstattable = new CharacterStatsTable(this);
  racetable = new RaceTable(this);
  racestattable = new RaceStatsTable(this);
  skilltable = new SkillTable(this);
  raceskilltable = new RaceSkillsTable(this);
  characterskillstable = new CharacterSkillsTable(this);
  npcaisettingtable = new NpcAiSettingTable(this);
  npcdialogstable = new NpcDialogsTable(this);
  npcdialoganswerstable = new NpcDialogAnswersTable(this);
  npcentitiestable = new NpcEntitiesTable(this);
  bookstable = new BooksTable(this);
  doorstable = new DoorsTable(this);
  permissionstable = new PermissionsTable(this);

}

Database::~Database()
{
  delete userstable;
  delete charactertable;
  delete entitytable;
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
  delete bookstable;
  delete doorstable;
  delete permissionstable;
}

