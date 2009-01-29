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

#include "charactermanager.h"
#include "server/server.h"
#include "common/database/database.h"
#include "server/database/table-characters.h"
#include "server/entity/user.h"
#include "server/entity/racemanager.h"
#include "server/entity/meshmanager.h"

CharacterManager::CharacterManager(Server* server)
: server(server)
{
  charId = server->getTables()->getCharacterTable()->getMaxId();
}

ptString CharacterManager::createCharacter(ptString name, int user_id, int& char_id, Race* race, unsigned char* haircolour, unsigned char* skincolour, unsigned char* decalcolour)
{
  CharacterTable* ct = server->getTables()->getCharacterTable();

  std::string username_str = *name;
  bool nonspace=false;
  for (size_t i=0; i < username_str.length(); i++)
  {
    if (username_str[i] != ' ') nonspace=true;
  }
  if (!nonspace) return ptString::create("Character name may not contain only space");

  if (ct->existsCharacter(name))
  {
    return ptString::create("Character exists already"); // <-- TODO: Error Message Container
  }

  if (!race) return ptString::create("Race not found!");  // <-- TODO: Error Message Container

  this->charId++;
  ct->insert(this->charId, name, user_id, race->getMesh(), race->getId(), haircolour, skincolour, decalcolour, race->getPos(), race->getSector()); // <-- TODO: Keep those ptString as class members

  char_id = this->charId;

  race->getStats()->copyToCharacter(char_id);
  race->getSkills()->copyToCharacter(char_id);

  return ptString(0,0);
}

Character* CharacterManager::getCharacter(int id, User* user)
{
  CharacterTable* ct = server->getTables()->getCharacterTable();

  unsigned int userid = (user ? user->getId() : 0 );

  CharactersTableVO* vo = ct->findCharacterById(id, userid);
  if (vo)
  {
    Character* character = new Character();
    character->setId(vo->id);
    character->setName(vo->name);
    if (user) character->setUser(user);

    const Mesh* mesh = server->getMeshManager()->findById(vo->mesh);
    character->setMesh(mesh);

    character->setRace(vo->race);
    character->setHairColour(vo->hair_r, vo->hair_g, vo->hair_b);
    character->setSkinColour(vo->skin_r, vo->skin_g, vo->skin_b);
    character->setDecalColour(vo->decal_r, vo->decal_g, vo->decal_b);
    character->setPos(WFMath::Point<3>(vo->pos_x, vo->pos_y, vo->pos_z));
    character->setRotation(vo->rotation);
    character->setSector(vo->sector);
    delete vo;
    return character;
  }

  return 0;
}

void CharacterManager::checkForSave(const PcEntity* e)
{
  const Entity* ent = e->getEntity();
  const float dist_square = ent->getDistanceTo2(ent->getLastSaved());
  if (dist_square > 100)
  {
    ptScopedMonitorable<Entity> l_ent (ent);
    l_ent->resetSavePos();
    printf("Save entity %s (%d / %d) at <%.2f,%.2f,%.2f> %f\n", *ent->getName(), e->getCharacter()->getId(),ent->getId(), ent->getPos()[0], ent->getPos()[1], ent->getPos()[2], ent->getRotation());
    server->getTables()->getCharacterTable()->update(ent->getPos(), ent->getRotation(), ent->getSectorName(), e->getCharacter()->getId());
  }
}

void CharacterManager::delCharacter(const Character* character)
{
  CharacterTable* ct = server->getTables()->getCharacterTable();
  ct->remove(character->getId());
}
