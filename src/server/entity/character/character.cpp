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

#include <exception>

#include "character.h"
#include "server/entity/mountentity.h"

#include "server/database/tablemanager.h"
#include "server/database/table-characters.h"
#include "server/database/table-entitypositions.h"
#include "server/database/table-meshes.h"

void Character::SetMount(boost::shared_ptr<MountEntity> mount)
{
  this->mount = mount;
}

void Character::walkTo(const WFMath::Point<3>& dst_pos, float speed)
{
  if (mount.lock()) return;

  final_dst = dst_pos;

  const WFMath::Point<3>& pos = Entity::GetPosition();

  const float dist = Distance(final_dst, pos);

  //v = s / t => t = s / v
  t_stop = (size_t) (dist / speed + time(0));

  isWalking = true;
}

WFMath::Point<3> Character::GetPosition()
{
  if (mount.lock())
  {
    return mount.lock()->GetPosition();
  }

  if (!isWalking)
  {
    return Entity::GetPosition();
  }
  else
  {
    if ((size_t)time(0) >= t_stop)
    {
      Entity::SetPosition(final_dst);

      isWalking = false;
      return final_dst;
    }
    else
    {
      const WFMath::Point<3> pos = Entity::GetPosition();
      //Not sure that's correct...
      size_t delta = t_stop - (size_t) time(0);
      tmp_pos = WFMath::Point<3>((final_dst - pos) * (float)delta);
      return tmp_pos;
    }
  }
}

void Character::LoadFromDB()
{
  Entity::LoadFromDB();

  CharactersTable* table = Server::getServer()->GetTableManager()->Get<CharactersTable>();
  CharactersTableVOp c = table->GetSingle(GetId());
  SetName(c->name);
  size_t meshId = c->meshes_id;
  SetHairColour(c->hairColor);
  SetSkinColour(c->skinColor);
  SetDecalColour(c->decalColor);

  MeshesTable* mtable = Server::getServer()->GetTableManager()->Get<MeshesTable>();
  MeshesTableVOp m = mtable->GetSingle(meshId);
  SetMeshName(m->factoryName);
  SetFileName(m->fileName);

  EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
  EntityPositionsTableVOp p = ptable->GetSingle(GetId());
  SetPosition(p->position);
  SetRotation(p->rotation[1]); //TODO: just Y atm.

  inventory->LoadFromDB();
  equipment->LoadFromDB();

  skills->LoadFromDB();
  abilities->LoadFromDB();
  reputations->LoadFromDB();
}

void Character::SaveToDB()
{
  Entity::SaveToDB();

  size_t meshId = 0;
  MeshesTable* mtable = Server::getServer()->GetTableManager()->Get<MeshesTable>();
  meshId = mtable->FindBy(GetMeshName(), GetFileName());
  if (!meshId)
  {
    if (GetMeshName().empty() || GetFileName().empty())
    {
      printf("E: Invalid mesh for Character %d!\n", GetId());
      throw "Invalid mesh for Character!";
    }
    else // Create a new mesh.
    {
      meshId = mtable->GetMaxId();
      meshId++;
      mtable->Insert(meshId, 1, GetMeshName(), GetFileName());
    }
  }

  CharactersTable* table = Server::getServer()->GetTableManager()->Get<CharactersTable>();
  table->Insert(GetId(), GetName(), meshId, GetHairColour(), GetSkinColour(), GetDecalColour());

  EntityPositionsTable* ptable = Server::getServer()->GetTableManager()->Get<EntityPositionsTable>();
  ptable->Insert(GetId(), GetPosition(), WFMath::Point<3>(0, GetRotation(),0)); //TODO: just Y atm.

  inventory->SaveToDB();
  equipment->SaveToDB();

  skills->SaveToDB();
  abilities->SaveToDB();
  reputations->SaveToDB();
}