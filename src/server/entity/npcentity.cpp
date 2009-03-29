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

#include <wfmath/point.h>

#include "server/entity/character/character.h"
#include "server/entity/npcentity.h"

#include "server/database/tablemanager.h"
#include "server/database/table-npcentities.h"

void NpcEntity::LoadFromDB()
{
  Character::LoadFromDB();

  NpcEntitiesTable* table = Server::getServer()->GetTableManager()->Get<NpcEntitiesTable>();
  NpcEntitiesTableVOArray arr = table->Get(GetId());
  if (arr.size() != 1)
  {
    printf("None or multiple npcentity rows for npcentity?!\n");
    throw "None or multiple npcentity rows for npcentity?!";
  }

  AI* ai = AI::createAI(arr[0]->ainame, boost::shared_dynamic_cast<NpcEntity>(this_));
  if (!ai)
  {
    printf("E: Invalid AI setting for NpcEntity!\n");
    //throw "Invalid AI setting for NpcEntity!";
  }
  else
  {
    ai->LoadFromDB();
    setAI(ai);
  }
}

void NpcEntity::SaveToDB()
{
  Character::SaveToDB();

  NpcEntitiesTable* table = Server::getServer()->GetTableManager()->Get<NpcEntitiesTable>();
  std::string aiName = "idle";
  if (ai)
  {
    ai->SaveToDB();
    aiName = ai->GetName();
  }
  table->Insert(GetId(), aiName);
}
