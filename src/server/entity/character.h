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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "common/util/ptstring.h"
#include "common/geom/vector3.h"
#include "common/util/monitorable.h"

#include "inventory.h"
#include "characterstats.h"
#include "characterskills.h"
#include "characterreputations.h"

#include "mesh.h"

#include "server/quest/npcdialogstate.h"

class Entity;
class User;
class Race;

class Character : public ptMonitorable<Character>
{
private:
  unsigned int id;

  ptString name_id;
  const Mesh* mesh;
  ptString sector_id;

  ptMonitor<User> user;
  int race;

  ptMonitor<Entity> entity;

  PtVector3 pos;
  float rotation;

  unsigned char haircolour[3]; //24bit color
  unsigned char skincolour[3]; //24bit color
  unsigned char decalcolour[3]; //24bit color

  NPCDialogState dialog_state;

  Inventory inventory;
  CharacterStats stats;
  CharacterSkills skills;
  CharacterReputations reputations;

public:
  Character() : id(-1), inventory(30) // slot count
  {
    dialog_state.setCharacter(this);
  }

  virtual ~Character()
  {
  }

  void setId(unsigned int id) { this->id = id; }
  unsigned int getId() const { return id; }

  void setPos(const PtVector3& newPos) { pos = newPos; }
  PtVector3 getPos() const { return pos; }

  void setRotation(float rot) { rotation = rot; }
  float getRotation() const { return rotation; }

  const ptString& getName() const { return name_id; }
  void setName(ptString id) { name_id = id; }

  const Mesh* getMesh() const { return mesh; }
  void setMesh(const Mesh* mesh) { this->mesh = mesh; }

  const ptString& getSector() const { return sector_id; }
  void setSector(ptString id) { sector_id = id; }

  void setUser(User* user);
  const User* getUser() const { return user.get(); }

  int getRace() const { return race; }
  void setRace(int race) { this->race = race; }

  const unsigned char* getSkinColour() const { return skincolour; }
  void setSkinColour(unsigned char* colour) { setSkinColour(colour[0],colour[1],colour[2]); }
  void setSkinColour(unsigned char r, unsigned char g, unsigned char b)
  {
    skincolour[0] = r; skincolour[1] = g; skincolour[2] = b;
  }

  const unsigned char* getHairColour() const { return haircolour; }
  void setHairColour(unsigned char* colour) { setHairColour(colour[0],colour[1],colour[2]); }
  void setHairColour(unsigned char r, unsigned char g, unsigned char b)
  {
    haircolour[0] = r; haircolour[1] = g; haircolour[2] = b;
  }

  const unsigned char* getDecalColour() const { return decalcolour; }
  void setDecalColour(unsigned char* colour) { setDecalColour(colour[0],colour[1],colour[2]); }
  void setDecalColour(unsigned char r, unsigned char g, unsigned char b)
  {
    decalcolour[0] = r; decalcolour[1] = g; decalcolour[2] = b;
  }

  NPCDialogState* getNPCDialogState() { return &dialog_state; }
  CharacterSkills* getSkills() { return &skills; }
  CharacterStats* getStats() { return &stats; }
  CharacterReputations* getReputation() { return &reputations; }
  Inventory* getInventory() { return &inventory; }
  const Inventory* getInventory() const { return &inventory; }

  void setEntity(const Entity* entity);
  const Entity* getEntity() const { return entity.get(); }
};

#endif // CHARACTER_H
