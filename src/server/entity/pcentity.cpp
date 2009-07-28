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

#include <time.h>

#include <wfmath/point.h>
#include <wfmath/vector.h>

#include "server/entity/usermanager.h"
#include "server/entity/user.h"
#include "server/entity/pcentity.h"
#include "server/group/charchats.h"

#include "server/database/tablemanager.h"
#include "server/database/table-pcentities.h"
#include "server/database/table-entitypositions.h"


void PcEntity::SetUser(User* user)
{
  this->user = user;
}

void PcEntity::setCharChats(const CharChats* charchats)
{
  if (!charchats)
    this->charchats.clear();
  else
    this->charchats = charchats->getRef();
}

void PcEntity::LoadFromDB()
{
  Character::LoadFromDB();

  PcEntitiesTable* table = Server::getServer()->GetTableManager()->Get<PcEntitiesTable>();
  PcEntitiesTableVOArray arr = table->Get(GetId());
  if (arr.size() != 1)
  {
    throw PT_EX(InvalidPcEntity("None or multiple rows for pcentity"))
      << EntityIdInfo(GetId());
  }

  EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
  EntityPositionsTableVOp p = ptable->GetSingle(GetId());
  if (p)
  {
    SetPosition(p->position);
    SetRotation(p->rotation[1]); //TODO: just Y atm.
  }

  //User* user = Server::getServer()->getUserManager()->FindByName(arr[0]->users_login);
  //SetUser(user);
}

void PcEntity::SaveToDB()
{
  Character::SaveToDB();

  PcEntitiesTable* pcTable = Server::getServer()->GetTableManager()->Get<PcEntitiesTable>();
  if (user)
  {
    pcTable->Insert(GetId(), user->GetName());
    EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
    ptable->Insert(GetId(), GetPosition(), WFMath::Point<3>(0, GetRotation(),0)); //TODO: just Y atm.
  }
}
