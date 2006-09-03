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

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "database.h"

#include "table-characters.h"

#include "common/entity/character.h"
#include "server/user.h"

CharacterTable::CharacterTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from characters;");
  delete rs;
  if (rs == 0)
  {
    createTable();
  }
  ResultSet* rs2 = db->query("select count(*) from characters;");
  delete rs2;
}

void CharacterTable::createTable()
{
  db->update("create table characters ("
    "id INTEGER, "
    "name TEXT, "
    "user INTEGER, "
    "mesh TEXT, "
    "race INTEGER, "
    "haircolour_r INTEGER, "
    "haircolour_g INTEGER, "
    "haircolour_b INTEGER, "
    "skincolour_r INTEGER, "
    "skincolour_g INTEGER, "
    "skincolour_b INTEGER, "
    "decalcolour_r INTEGER, "
    "decalcolour_g INTEGER, "
    "decalcolour_b INTEGER, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");
}

void CharacterTable::insert(int id, ptString name, int user_id, ptString mesh,
                            int race_id, unsigned char* haircolour, 
                            unsigned char* skincolour, unsigned char* decalcolour,
                            float pos[3], ptString sector)
{
  db->update("insert into characters (id, name, user, mesh, race, "
             "haircolour_r, haircolour_g, haircolour_b, "
             "skincolour_r, skincolour_g, skincolour_b, "
             "decalcolour_r, decalcolour_g, decalcolour_b, "
             "pos_x, pos_y, pos_z, sector)"
             "values ('%d','%s',%d,'%s',%d,"
             "%d,%d,%d,"  "%d,%d,%d,"  "%d,%d,%d,"
             "%.2f,%.2f,%.2f,'%s');", id, *name, user_id, *mesh, race_id, 
             haircolour[0],haircolour[1],haircolour[2],
             skincolour[0],skincolour[1],skincolour[2],
             decalcolour[0],decalcolour[1],decalcolour[2],
             pos[0], pos[1], pos[2], *sector);
}

int CharacterTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from characters");
  if (rs->GetRowCount() == 0) 
    return 0;

  int id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

void CharacterTable::dropTable()
{
  db->update("drop table characters;");
}

void CharacterTable::remove(int id)
{
  char query[1024];
  sprintf(query, "delete from characters where id = %d;", id);
  db->update(query);
}

void CharacterTable::update(PcEntity* e)
{
  db->update("update characters set pos_x=%.2f, pos_y=%.2f, pos_z=%.2f, sector='%s' where id = %d;",
    e->getPos()[0], e->getPos()[1], e->getPos()[2], *(e->getSector()), e->getCharId());
}

bool CharacterTable::existsCharacter(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select id from characters where name = '%s';", *name);
  ResultSet* rs = db->query(query);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Character* CharacterTable::findCharacterById(int id, size_t user_id)
{
  char query[1024];
  sprintf(query, "select * from characters where id = '%d' and user = '%d';", id, user_id);
  ResultSet* rs = db->query(query);
  if (rs->GetRowCount() == 0) 
    return 0;

  Character* character = new Character();
  character->setId(atoi(rs->GetData(0,0).c_str()));
  character->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
  character->setMesh(ptString(rs->GetData(0,3).c_str(), rs->GetData(0,3).length()));
  character->setRace(atoi(rs->GetData(0,4).c_str()));
  character->setHairColour(atoi(rs->GetData(0,5).c_str()), atoi(rs->GetData(0,6).c_str()), atoi(rs->GetData(0,7).c_str()));
  character->setSkinColour(atoi(rs->GetData(0,8).c_str()), atoi(rs->GetData(0,9).c_str()), atoi(rs->GetData(0,10).c_str()));
  character->setDecalColour(atoi(rs->GetData(0,11).c_str()), atoi(rs->GetData(0,12).c_str()), atoi(rs->GetData(0,13).c_str()));
  character->setPos((float)atof(rs->GetData(0,14).c_str()), (float)atof(rs->GetData(0,15).c_str()), (float)atof(rs->GetData(0,16).c_str()));
  character->setSector(ptString(rs->GetData(0,17).c_str(), rs->GetData(0,17).length()));
  delete rs;

  return character;
}

void CharacterTable::getAllCharacters(Array<Character*>& characters, User* user)
{
  char query[1024];
  sprintf(query, "select * from characters where user = '%d';", user->getId());
  ResultSet* rs = db->query(query);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Character* character = new Character();
    character->setId(atoi(rs->GetData(i,0).c_str()));
    character->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
    character->setUser(user);
    character->setMesh(ptString(rs->GetData(i,3).c_str(), rs->GetData(i,3).length()));
    character->setRace(atoi(rs->GetData(i,4).c_str()));
    character->setHairColour(atoi(rs->GetData(i,5).c_str()), atoi(rs->GetData(i,6).c_str()), atoi(rs->GetData(i,7).c_str()));
    character->setSkinColour(atoi(rs->GetData(i,8).c_str()), atoi(rs->GetData(i,9).c_str()), atoi(rs->GetData(i,10).c_str()));
    character->setDecalColour(atoi(rs->GetData(i,11).c_str()), atoi(rs->GetData(i,12).c_str()), atoi(rs->GetData(i,13).c_str()));
    character->setPos((float)atof(rs->GetData(i,14).c_str()), (float)atof(rs->GetData(i,15).c_str()), (float)atof(rs->GetData(i,16).c_str()));
    character->setSector(ptString(rs->GetData(i,17).c_str(), rs->GetData(i,17).length()));
    characters.add(character);
  }
  delete rs;
}  
