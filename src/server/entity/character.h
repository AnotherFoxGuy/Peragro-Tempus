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

#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "common/util/ptstring.h"
#include "common/util/monitorable.h"

#include "inventory.h"
#include "characterstats.h"
#include "characterskills.h"

#include "common/quest/npcdialogstate.h"

class Entity;
class User;
class Race;

class Character : public ptMonitorable<Character>
{
private:
  int id;

  ptString name_id;
  ptString mesh_id;
  ptString sector_id;

  ptMonitor<User> user;
  int race;

  ptMonitor<Entity> entity;

  float pos[3];

  unsigned char haircolour[3]; //24bit color
  unsigned char skincolour[3]; //24bit color
  unsigned char decalcolour[3]; //24bit color

  NPCDialogState dialog_state;

  Inventory inventory;
  CharacterStats stats;
  CharacterSkills skills;

public:
  Character() : id(-1), inventory(30) // slot count
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;

    dialog_state.setCharacter(this);
  }

  virtual ~Character()
  {
    //delete [] name;
    //delete [] mesh;
    //delete [] sector;
  }

  void setId(int id) { this->id = id; }
  int getId() const { return id; }

  void setPos(float x, float y, float z)
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
  }
  void setPos(float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  float* getPos()
  {
    return pos;
  }

  const ptString& getName() const { return name_id; }
  void setName(ptString id) { name_id = id; }

  ptString& getMesh() { return mesh_id; }
  void setMesh(ptString id) { mesh_id = id; }

  ptString& getSector() { return sector_id; }
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
  Inventory* getInventory() { return &inventory; }
  const Inventory* getInventory() const { return &inventory; }

  void setEntity(Entity* entity);
  const Entity* getEntity() const { return entity.get(); }
};

#endif // _CHARACTER_H_
