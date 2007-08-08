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
}

int Database::init()
{
  userstable = new UsersTable(this);
  if (!userstable) return 0;
  charactertable = new CharacterTable(this);
  if (!charactertable) return 0;
  entitytable = new EntityTable(this);
  if (!entitytable) return 0;
  inventorytable = new InventoryTable(this);
  if (!inventorytable) return 0;
  stattable = new StatTable(this);
  if (!stattable) return 0;
  characterstattable = new CharacterStatsTable(this);
  if (!characterstattable) return 0;
  racetable = new RaceTable(this);
  if (!racetable) return 0;
  racestattable = new RaceStatsTable(this);
  if (!racestattable) return 0;
  skilltable = new SkillTable(this);
  if (!skilltable) return 0;
  raceskilltable = new RaceSkillsTable(this);
  if (!raceskilltable)  return 0;
  characterskillstable = new CharacterSkillsTable(this);
  if (!characterskillstable) return 0;
  npcaisettingtable = new NpcAiSettingTable(this);
  if (!npcaisettingtable) return 0;
  npcdialogstable = new NpcDialogsTable(this);
  if (!npcdialogstable) return 0;
  npcdialoganswerstable = new NpcDialogAnswersTable(this);
  if (!npcdialoganswerstable) return 0;
  npcentitiestable = new NpcEntitiesTable(this);
  if (!npcentitiestable) return 0;
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

