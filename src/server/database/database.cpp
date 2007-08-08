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

Database::Database()
{
  userstable = NULL;
  charactertable = NULL;
  entitytable = NULL;
  inventorytable = NULL;
  stattable = NULL;
  characterstattable = NULL;
  racetable = NULL;
  racestattable = NULL;
  skilltable = NULL;
  raceskilltable = NULL;
  characterskillstable = NULL;
  npcaisettingtable = NULL;
  npcdialogstable = NULL;
  npcdialoganswerstable = NULL;
  npcentitiestable = NULL;
}

int Database::init()
{
  userstable = new UsersTable(this);
  if (userstable == NULL) return 0;
  charactertable = new CharacterTable(this);
  if (charactertable  == NULL) return 0;
  entitytable = new EntityTable(this);
  if (entitytable == NULL) return 0;
  inventorytable = new InventoryTable(this);
  if (inventorytable == NULL) return 0;
  stattable = new StatTable(this);
  if (stattable == NULL) return 0;
  characterstattable = new CharacterStatsTable(this);
  if (characterstattable == NULL) return 0;
  racetable = new RaceTable(this);
  if (racetable == NULL) return 0;
  racestattable = new RaceStatsTable(this);
  if (racestattable == NULL) return 0;
  skilltable = new SkillTable(this);
  if (skilltable == NULL) return 0;
  raceskilltable = new RaceSkillsTable(this);
  if (raceskilltable == NULL) return 0;
  characterskillstable = new CharacterSkillsTable(this);
  if (characterskillstable == NULL) return 0;
  npcaisettingtable = new NpcAiSettingTable(this);
  if (npcaisettingtable == NULL) return 0;
  npcdialogstable = new NpcDialogsTable(this);
  if (npcdialogstable == NULL) return 0;
  npcdialoganswerstable = new NpcDialogAnswersTable(this);
  if (npcdialoganswerstable == NULL) return 0;
  npcentitiestable = new NpcEntitiesTable(this);
  if (npcentitiestable == NULL) return 0;
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
}

