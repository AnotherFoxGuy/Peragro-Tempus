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

#include <stdlib.h>

#include <wfmath/point.h>
#include <wfmath/vector.h>

#include "stray-ai.h"

#include "server/server.h"
#include "common/database/database.h"
#include "server/database/tablemanager.h"
#include "server/database/table-npcaisetting.h"

#include "server/entity/character/character.h"
#include "server/entity/npcentity.h"

void StrayAI::LoadFromDB()
{
  NpcAiSettingTable* table =
    Server::getServer()->GetTableManager()->Get<NpcAiSettingTable>();
}

void StrayAI::SaveToDB()
{
  NpcAiSettingTable* table =
    Server::getServer()->GetTableManager()->Get<NpcAiSettingTable>();
}

/*
void StrayAI::setNPC(NpcEntity* npc)
{
  this->npc = npc;

  // load settings
  int id = npc->GetId();
  NpcAiSettingTable* table =
    Server::getServer()->GetTableManager()->Get<NpcAiSettingTable>();

  base[0] = (float) atof(*table->getValue(id, ptString("base_x",6)));
  base[1] = (float) atof(*table->getValue(id, ptString("base_y",6)));
  base[2] = (float) atof(*table->getValue(id, ptString("base_z",6)));

  radius[0] = (float) atof(*table->getValue(id, ptString("radius_x",8)));
  radius[1] = (float) atof(*table->getValue(id, ptString("radius_y",8)));
  radius[2] = (float) atof(*table->getValue(id, ptString("radius_z",8)));

  interval_base = atoi(*table->getValue(id, ptString("interval_base", 13)));
  interval_rand = atoi(*table->getValue(id, ptString("interval_rand", 13)));

  // Timer
  setInterval(interval_base);
  start();
}
*/
void StrayAI::timeOut()
{
  think();
}

void StrayAI::think()
{
  // Busy trading, chatting or something else
  if (isPaused())
  {
    return;
  }
  // Timer
  float random = ( RAND_MAX / 2.0f - rand() ) / RAND_MAX;
  setInterval((int) (interval_base + random * interval_rand));

  WFMath::Point<3> pos;
  pos[0] = ( RAND_MAX / 2.0f - rand() ) / RAND_MAX;
  pos[1] = ( RAND_MAX / 2.0f - rand() ) / RAND_MAX;
  pos[2] = ( RAND_MAX / 2.0f - rand() ) / RAND_MAX;

  pos = base + WFMath::Vector<3>(pos * radius);

  Server::getServer()->moveEntity(npc.lock(), pos, 3.0f, false);
}
