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

#include "server/entity/character.h"
#include "server/entity/user.h"

CharacterTable::CharacterTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from characters;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void CharacterTable::createTable()
{
  printf("Creating Table characters...\n");
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
    "rot FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");

  //unsigned char haircolour[3] = {255,255,255};
  //unsigned char skincolour[3] = {255,255,255};
  //unsigned char decalcolour[3] = {255,255,255};
  //float pos[3] = {0,0,0};
  //insert(1, ptString("test-dummy", 10), 0, ptString("test",4), 1, haircolour,
  //          skincolour, decalcolour, pos, ptString("room",4));
  //insert(2, ptString("baby-dragonfly", 14), 0, ptString("test1",5), 1, haircolour,
  //          skincolour, decalcolour, pos, ptString("room",4));
  //insert(3, ptString("skeleton", 8), 0, ptString("skeleton",8), 1, haircolour,
  //          skincolour, decalcolour, pos, ptString("room",4));

  //// Creating Undead Squad.
  //for (int i = 0; i < 100; i++)
  //{
  //  std::string name("Evil Undead");
  //  insert(4 + i, ptString(name.c_str(), name.length()), 0, ptString("skeleton",8), 1, haircolour,
  //                skincolour, decalcolour, pos, ptString("room",4));
  //}
}

void CharacterTable::insert(int id, ptString name, int user_id,
                            ptString mesh, int race_id,
                            unsigned char haircolour[3], 
                            unsigned char skincolour[3],
                            unsigned char decalcolour[3],
                            float pos[3], ptString sector)
{
  db->update("insert into characters (id, name, user, mesh, race, "
             "haircolour_r, haircolour_g, haircolour_b, "
             "skincolour_r, skincolour_g, skincolour_b, "
             "decalcolour_r, decalcolour_g, decalcolour_b, "
             "pos_x, pos_y, pos_z, rot, sector)"
             "values ('%d','%q',%d,'%q',%d,"
             "%d,%d,%d,"  "%d,%d,%d,"  "%d,%d,%d,"
             "%.2f,%.2f,%.2f,0,'%q');", id, *name, user_id, *mesh, race_id, 
             haircolour[0],haircolour[1],haircolour[2],
             skincolour[0],skincolour[1],skincolour[2],
             decalcolour[0],decalcolour[1],decalcolour[2],
             pos[0], pos[1], pos[2], *sector);
}

int CharacterTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from characters");
  if (!rs || rs->GetRowCount() == 0) 
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
  db->update("delete from characters where id = %d;", id);
}

void CharacterTable::update(const float* pos, float rotation, ptString sector, int char_id)
{
  if (!pos) 
  {
    return;
  }
  db->update("update characters set pos_x=%.2f, pos_y=%.2f, pos_z=%.2f, rot=%.2f, sector='%q' where id = %d;",
    pos[0], pos[1], pos[2], rotation, *sector, char_id);
}

CharactersTableVO* CharacterTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  CharactersTableVO* vo = new CharactersTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->name = ptString(rs->GetData(row,1).c_str(), rs->GetData(row,1).length());
  vo->user = atoi(rs->GetData(row,2).c_str());
  vo->mesh = ptString(rs->GetData(row,3).c_str(), rs->GetData(row,3).length());
  vo->race = atoi(rs->GetData(row,4).c_str());
  vo->hair_r = atoi(rs->GetData(row,5).c_str());
  vo->hair_g = atoi(rs->GetData(row,6).c_str());
  vo->hair_b = atoi(rs->GetData(row,7).c_str());
  vo->skin_r = atoi(rs->GetData(row,8).c_str());
  vo->skin_g = atoi(rs->GetData(row,9).c_str());
  vo->skin_b = atoi(rs->GetData(row,10).c_str());
  vo->decal_r = atoi(rs->GetData(row,11).c_str());
  vo->decal_g = atoi(rs->GetData(row,12).c_str());
  vo->decal_b = atoi(rs->GetData(row,13).c_str());
  vo->pos_x = (float) atof(rs->GetData(row,14).c_str());
  vo->pos_y = (float) atof(rs->GetData(row,15).c_str());
  vo->pos_z = (float) atof(rs->GetData(row,16).c_str());
  vo->rotation = (float) atof(rs->GetData(row,17).c_str());
  vo->sector = ptString(rs->GetData(row,18).c_str(), rs->GetData(row,18).length());
  return vo;
}

Array<CharactersTableVO*> CharacterTable::parseMultiResultSet(ResultSet* rs)
{
  Array<CharactersTableVO*> arr;
  for (size_t i = 0; rs && i < rs->GetRowCount(); i++)
  {
    CharactersTableVO* obj = parseSingleResultSet(rs, i);
    arr.add(obj);
  }
  return arr;
}

bool CharacterTable::existsCharacter(ptString name)
{
  ResultSet* rs = db->query("select id from characters where name = '%q';", *name);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

CharactersTableVO* CharacterTable::findCharacterById(int id, size_t user_id)
{
  ResultSet* rs = db->query("select * from characters where id = '%d' and user = '%d';", id, user_id);
  if (!rs || rs->GetRowCount() == 0) 
  {
    delete rs;
    return 0;
  }

  CharactersTableVO* character = parseSingleResultSet(rs);
  delete rs;

  return character;
}

Array<CharactersTableVO*> CharacterTable::getAllCharacters(User* user)
{
  ResultSet* rs = db->query("select * from characters where user = '%d';", user->getId());
  Array<CharactersTableVO*> characters = parseMultiResultSet(rs);
  delete rs;
  return characters;
}  
