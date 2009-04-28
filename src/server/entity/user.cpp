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

User::~User()
{
}

void User::SetEntity(boost::shared_ptr<PcEntity> entity)
{
  own_entity = entity;
  entity->SetUser(this);
}

void DontDelete1(Common::Entity::Entity*){}

void User::Destroyed(WFMath::iShape* shape)
{
  Entity* ent = dynamic_cast<Entity*>(shape);
  if (ent)
  {
    Entityp entity(ent, DontDelete1); //TODO: hack :/
    SendRemoveEntity(entity);
  }
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
        { SendAddEntity(*it); }
    }
  }

  {
    std::list<Common::Entity::Entityp>::const_iterator found;
    std::map<size_t, Common::Entity::WeakEntityp>::iterator it;
    for ( it=knownEntitites.begin() ; it != knownEntitites.end(); it++ )
    {
      found = std::find(entities.begin(), entities.end(), it->second.lock());
      if (found == entities.end())
        { SendRemoveEntity(it->second.lock()); }
    }
  }
}

void User::SendAddEntity(Common::Entity::Entityp entity)
{
  if (knownEntitites.count(entity->GetId()) > 0) return;
  knownEntitites[entity->GetId()] = entity;

  entity->AddListener(this);

  printf("send addentity '%s' to '%s'\n", entity->GetName().c_str(), this->GetName().c_str());

  ByteStream bs;
  if (entity->GetType() == Common::Entity::DoorEntityType)
  {
    DoorEntity* door = dynamic_cast<DoorEntity*>(entity.get());
    AddDoorEntityMessage msg;
    msg.setEntityId(door->GetId());
    msg.setIsOpen(door->GetOpen());
    msg.setIsLocked(door->GetLocked());
    msg.setAnimationName(door->GetAnimationName());
    msg.setMeshName(door->GetMeshName());
    msg.setFileName(door->GetFileName());
    //msg.SetSectorId(entity->GetSector());
    msg.setEntityName(entity->GetName());
    msg.serialise(&bs);
  }
  else if (entity->GetType() == Common::Entity::ItemEntityType)
  {
    AddItemEntityMessage msg;
    msg.setEntityId(entity->GetId());
    msg.setPosition(entity->GetPosition());
    msg.setRotation(entity->GetRotation());
    //msg.SetSectorId(entity->GetSector());

    msg.setEntityName(entity->GetName());
    msg.setMeshName(entity->GetMeshName());
    msg.setFileName(entity->GetFileName());

    msg.serialise(&bs);
  }
  else if (entity->GetType() == Common::Entity::PCEntityType)
  {
    AddPlayerEntityMessage msg;
    msg.setEntityName(entity->GetName());
    msg.setEntityId(entity->GetId());
    msg.setMeshName(entity->GetMeshName());
    msg.setFileName(entity->GetFileName());
    msg.setPosition(entity->GetPosition());
    msg.setRotation(entity->GetRotation());
    //msg.SetSectorId(entity->GetSector());
    //msg.setPoseId(entity->GetPoseId());

    boost::shared_ptr<Character> c = boost::shared_dynamic_cast<Character>(entity);

    //msg.setDecalColour(c->GetDecalColour());
    //msg.setHairColour(c->GetHairColour());
    //msg.setSkinColour(c->GetSkinColour());
    boost::shared_ptr<Equipment> eq = c->GetEquipment();
    eq->AddEquipment<AddPlayerEntityMessage>(msg);
    msg.serialise(&bs);
  }

  else if (entity->GetType() == Common::Entity::NPCEntityType)
  {
    AddNpcEntityMessage msg;
    msg.setEntityName(entity->GetName());
    msg.setEntityId(entity->GetId());
    msg.setMeshName(entity->GetMeshName());
    msg.setFileName(entity->GetFileName());
    msg.setPosition(entity->GetPosition());
    msg.setRotation(entity->GetRotation());
    //msg.SetSectorId(entity->GetSector());
    //msg.setPoseId(entity->GetPoseId());

    boost::shared_ptr<Character> c = boost::shared_dynamic_cast<Character>(entity);

    //msg.setDecalColour(c->GetDecalColour());
    //msg.setHairColour(c->GetHairColour());
    //msg.setSkinColour(c->GetSkinColour());
    boost::shared_ptr<Equipment> eq = c->GetEquipment();
    eq->AddEquipment<AddNpcEntityMessage>(msg);
    msg.serialise(&bs);
  }
  else if (entity->GetType() == Common::Entity::MountEntityType)
  {
    AddMountEntityMessage msg;
    msg.setEntityName(entity->GetName());
    msg.setEntityId(entity->GetId());
    msg.setMeshName(entity->GetMeshName());
    msg.setFileName(entity->GetFileName());
    msg.setPosition(entity->GetPosition());
    msg.setRotation(entity->GetRotation());
    //msg.SetSectorId(entity->GetSector());
    msg.serialise(&bs);
  }
  else
  {
    printf("Unkown entity type!\n");
  }

  if (connection.get()) connection.get()->send(bs);
}

void User::SendRemoveEntity(Common::Entity::Entityp entity)
{
  if (!entity) return;

  size_t id = entity->GetId();
  std::string name = entity->GetName();

  std::map<size_t, Common::Entity::WeakEntityp>::iterator it;
  it = knownEntitites.find(id);
  if (it != knownEntitites.end())
  {
    printf("send delentity '%s' to '%s'\n", name.c_str(), this->GetName().c_str());
    RemoveEntityMessage msg;
    msg.setEntityId(id);
    ByteStream bs;
    msg.serialise(&bs);
    if (connection.get()) connection.get()->send(bs);
    knownEntitites.erase(it);
  }
}

void User::remove()
{
  if (GetEntity())
  {
    printf("own_entity!\n");
    Server::getServer()->getEntityManager()->Remove(GetEntity());
  }

  Server::getServer()->getUserManager()->RemoveUser(this);
}

