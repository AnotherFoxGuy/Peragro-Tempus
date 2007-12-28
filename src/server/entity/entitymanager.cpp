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

#include "entitymanager.h"

#include "server/database/database.h"
#include "server/database/table-entities.h"
#include "server/database/table-characters.h"
#include "server/database/table-npcentities.h"

#include "entity.h"
#include "itementity.h"
#include "itemmanager.h"
#include "pcentity.h"
#include "npcentity.h"
#include "doorentity.h"
#include "mountentity.h"
#include "charactermanager.h"

#include "server/server.h"

void EntityManager::loadFromDB(EntityTable* et)
{
  ent_id = et->getMaxId();

  //Load all Entities from Database
  Array<EntitiesTableVO*> entityVOs = et->getAllEntities();
  for (size_t i = 0; entityVOs.getCount(); i++)
  {
    const Entity* entity = 0;

    EntitiesTableVO* vo = entityVOs.get(i);

    switch (vo->type)
    {
      case Entity::ItemEntityType:
      {
        ItemEntity* ent = new ItemEntity();
        Item* item = Server::getServer()->getItemManager()->findById(vo->item);
        assert(item);
        ent->createFromItem(item->getId(), vo->variation);
        entity = ent->getEntity();
        break;
      }
      case Entity::PlayerEntityType:
      {
        entity = (new PcEntity())->getEntity();
        break;
      }
      case Entity::NPCEntityType:
      {
        Database* db = Server::getServer()->getDatabase();
        NpcEntitiesTableVO* npc_vo = 
          db->getNpcEntitiesTable()->getById(vo->id);

        if (!npc_vo)
          continue;

        CharacterManager* cmgr = Server::getServer()->getCharacterManager();
        Character* character = cmgr->getCharacter(npc_vo->character, 0 /* npc */);

        NpcEntity* npc = new NpcEntity();
        npc->setCharacter(character);
        npc->setAI(AI::createAI(npc_vo->ai));
        npc->setStartDialog(npc_vo->dialog);

        entity = npc->getEntity();

        delete npc_vo;
        break;
      }
      case Entity::DoorEntityType:
      {
        int packeddata = vo->item;
        DoorEntity* ent = new DoorEntity();
        ent->setOpen  ((packeddata & 1) != 0);
        ent->setLocked((packeddata & 2) != 0);
        entity = ent->getEntity();
        break;
      }
      case Entity::MountEntityType:
      {
        MountEntity* ent = new MountEntity();
        entity = ent->getEntity();
        break;
      }
      default:
      { 
        // Unknown Entity!
        continue;
      }
    };

    Entity* l_ent = entity->getLock();
    l_ent->setId(vo->id);
    l_ent->setName(vo->name);
    l_ent->setMesh(vo->mesh);
    l_ent->setPos(vo->pos_x, vo->pos_y, vo->pos_z);
    l_ent->setRotation(vo->rotation);
    l_ent->setSector(vo->sector);
    l_ent->freeLock();

    entity_list.addEntity(entity);
  }
}
