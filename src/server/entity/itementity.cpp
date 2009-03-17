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

#include "entity.h"
#include "itementity.h"

#include "server/server.h"

#include "itemtemplatesmanager.h"

#include "server/database/tablemanager.h"
#include "server/database/table-items.h"
#include "server/database/table-entitypositions.h"

void ItemEntity::LoadFromDB()
{
  Entity::LoadFromDB();

  ItemsTable* table = Server::getServer()->GetTableManager()->Get<ItemsTable>();
  ItemsTableVOp i = table->GetSingle(GetId());

  itemTemplate = Server::getServer()->GetItemTemplatesManager()->Get(i->itemTemplates_id);
  itemTemplate->SetDataOn(this);

  EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
  try
  {
    EntityPositionsTableVOp p = ptable->GetSingle(GetId());
    SetPosition(p->position);
    SetRotation(p->rotation[1]); //TODO: just Y atm.
    SetInWorld(true);
  }
  catch (char*)
  {
    SetInWorld(false);
  }
}

void ItemEntity::SaveToDB()
{
  Entity::SaveToDB();

  if (GetInWorld())
  {
    EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
    ptable->Insert(GetId(), GetPosition(), WFMath::Point<3>(0, GetRotation(),0)); //TODO: just Y atm.
  }
  else
  {
    EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
    ptable->Remove(GetId());
  }
}

