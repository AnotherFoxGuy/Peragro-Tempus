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

#include "character/character.h"
#include "doorentity.h"
#include "entity.h"
#include "pcentity.h"
#include "npcentity.h"
#include "itementity.h"
#include "user.h"
#include "usermanager.h"

#include "common/network/entitymessages.h"

#include "server/server.h"

void User::SetEntity(PcEntity* entity)
{
  own_entity = entity;
  entity->SetUser(this);
}

void User::SendEntityDiff(const std::list<Common::Entity::Entityp>& entities)
{
  {
    std::map<size_t, Common::Entity::WeakEntityp>::iterator found;
    std::list<Common::Entity::Entityp>::const_iterator it;
    for ( it=entities.begin() ; it != entities.end(); it++ )
    {
      found = knownEntitites.find((*it)->GetId());
      if (found == knownEntitites.end())
        { knownEntitites[(*it)->GetId()] = *it; SendAddEntity(*it); }
    }
  }

  {
    std::list<Common::Entity::Entityp>::const_iterator found;
    std::map<size_t, Common::Entity::WeakEntityp>::iterator it;
    for ( it=knownEntitites.begin() ; it != knownEntitites.end(); it++ )
    {
      found = std::find(entities.begin(), entities.end(), it->second.lock());
      if (found == entities.end())
        { SendRemoveEntity(it->second.lock()); knownEntitites.erase(it); }
    }
  }
}

void User::SendAddEntity(Common::Entity::Entityp entity)
{
  printf("send addentity '%s' to '%s'\n", entity->GetName().c_str(), this->GetName().c_str());

  ByteStream bs;
  if (entity->GetType() == Common::Entity::DoorEntityType)
  {
    DoorEntity* door = dynamic_cast<DoorEntity*>(entity.get());
    AddDoorEntityMessage msg;
    //msg.setDoorId(entity->getDoorEntity()->getDoorId());
    msg.setEntityId(door->GetId());
    msg.setIsOpen(door->GetOpen());
    msg.setIsLocked(door->GetLocked());
    msg.setAnimationName(door->GetAnimationName());
    //msg.setMeshId(entity->getMesh()->GetId()); // Not used yet!
    msg.setMeshName(door->GetMeshName());
    msg.setFileName(door->GetFileName());
    //msg.SetSectorId(entity->GetSector());
    msg.setEntityName(entity->GetName());
    msg.serialise(&bs);
  }
  /*
  else if (entity->GetType() == Common::Entity::ItemEntityType)
  {
    const Item* item = entity->getItemEntity()->getItem();

    AddItemEntityMessage msg;
    msg.setEntityId(entity->GetId());
    msg.setItemId(item->GetId());
    msg.SetPosition(entity->GetPosition());
    msg.SetRotation(entity->GetRotation());
    msg.SetSectorId(entity->GetSector());

    msg.setEntityName(item->getName());
    msg.setMeshId(item->getMesh()->GetId()); // Not used yet!
    msg.setMeshName(item->getMesh()->getName());
    msg.setFileName(item->getMesh()->getFile());

    msg.serialise(&bs);
  }
  */
  else if (entity->GetType() == Common::Entity::PCEntityType)
  {
    AddPlayerEntityMessage msg;
    msg.setEntityName(entity->GetName());
    msg.setEntityId(entity->GetId());
    //msg.setMeshId(entity->getMesh()->GetId()); // Not used yet!
    msg.setMeshName(entity->GetMeshName());
    msg.setFileName(entity->GetFileName());
    msg.SetPosition(entity->GetPosition());
    msg.SetRotation(entity->GetRotation());
    //msg.SetSectorId(entity->GetSector());
    //msg.setPoseId(entity->GetPoseId());

    PcEntity* pc = dynamic_cast<PcEntity*>(entity.get());

    //msg.setDecalColour(pc->GetDecalColour());
    //msg.setHairColour(pc->GetHairColour());
    //msg.setSkinColour(pc->GetSkinColour());
    //Inventory* inv = pc->GetInventory();
    //inv->addEquipment<AddPlayerEntityMessage>(msg);
    msg.serialise(&bs);
  }
  /*
  else if (entity->GetType() == Common::Entity::NPCEntityType)
  {
    AddNpcEntityMessage msg;
    msg.setEntityName(entity->GetNameId());
    msg.setEntityId(entity->GetId());
    msg.setMeshId(entity->getMesh()->GetId()); // Not used yet!
    msg.setMeshName(entity->getMesh()->getName());
    msg.setFileName(entity->getMesh()->getFile());
    msg.SetPosition(entity->GetPosition());
    msg.SetRotation(entity->GetRotation());
    msg.SetSectorId(entity->GetSector());
    ptScopedMonitorable<Character> character (entity->getNpcEntity()->getCharacter());
    Inventory* inv = character->getInventory();
    inv->addEquipment(msg);
    msg.serialise(&bs);
  }
  else if (entity->GetType() == Common::Entity::MountEntityType)
  {
    AddMountEntityMessage msg;
    msg.setEntityName(entity->GetNameId());
    msg.setEntityId(entity->GetId());
    msg.setMeshId(entity->getMesh()->GetId()); // Not used yet!
    msg.setMeshName(entity->getMesh()->getName());
    msg.setFileName(entity->getMesh()->getFile());
    msg.SetPosition(entity->GetPosition());
    msg.SetRotation(entity->GetRotation());
    msg.SetSectorId(entity->GetSector());
    msg.serialise(&bs);
  }
  else
  {
    printf("Unkown entity type!\n");
  }
  */

  if (connection.get()) connection.get()->send(bs);
}

void User::SendRemoveEntity(Common::Entity::Entityp entity)
{
  printf("send delentity '%s' to '%s'\n", entity->GetName().c_str(), this->GetName().c_str());

  RemoveEntityMessage msg;
  msg.setEntityId(entity->GetId());
  ByteStream bs;
  msg.serialise(&bs);
  if (connection.get()) connection.get()->send(bs);
}

void User::remove()
{
  if (own_entity)
  {
    printf("own_entity!\n");
    Server::getServer()->delEntity(own_entity);
  }

  Server::getServer()->getUserManager()->RemoveUser(this);
}

