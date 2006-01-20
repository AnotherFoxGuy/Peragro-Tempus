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

#include <sstream>
#include <iomanip>

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
}

void CharacterTable::createTable()
{
  db->update("create table characters ("
    "id INTEGER, "
    "name TEXT, "
    "user INTEGER, "
    "mesh TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");
}

void CharacterTable::insert(int id, const std::string& name, int user_id,
  const std::string& mesh, const float pos[3], const std::string& sector)
{
  std::ostringstream query;
  query << "insert into characters (id, name, user, mesh, pos_x, pos_y, pos_z, sector) values "
        << "('" << id << "','" << db->escape(name) << "'," << user_id << ",'"
        << db->escape(mesh) << "',"
        << std::setprecision(2)
        << pos[0] << "," << pos[1] << "," << pos[2] << ",'"
        << db->escape(sector) << "');";
  db->update(query.str());
}

int CharacterTable::getMaxId() const
{
  ResultSet* rs = db->query("select max(id) from characters");
  if (rs->GetRowCount() == 0) 
    return 0;

  return atoi(rs->GetData(0,0).c_str());
}

void CharacterTable::dropTable()
{
  db->update("drop table characters;");
}

void CharacterTable::remove(int id)
{
  std::ostringstream query;
  query << "delete from characters where id = " << id << ";";
  db->update(query.str());
}

bool CharacterTable::existsCharacter(const std::string& name)
{
  std::ostringstream query;
  query << "select id from characters where name = '"
        << db->escape(name) << "';";
  ResultSet* rs = db->query(query.str());
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Character* CharacterTable::findCharacterById(int id)
{
  std::ostringstream query;
  query << "select * from characters where id = '" << id << "';";
  ResultSet* rs = db->query(query.str());
  if (rs->GetRowCount() == 0) 
    return 0;

  Character* character = new Character();
  character->setId(atoi(rs->GetData(0,0).c_str()));
  character->setName(rs->GetData(0,1));
  //character->setUser(atoi(rs->GetData(0,2).c_str()));
  character->setMesh(rs->GetData(0,3));
  character->setPos((float)atof(rs->GetData(0,4).c_str()),
    (float)atof(rs->GetData(0,5).c_str()),
    (float)atof(rs->GetData(0,6).c_str()));
  character->setSector(rs->GetData(0,7));
  delete rs;

  return character;
}

void CharacterTable::getAllCharacters(std::vector<Character*>& characters, User* user)
{
  std::ostringstream query;
  query << "select * from characters where user = '" << user->getId() << "';";
  ResultSet* rs = db->query(query.str());
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Character* character = new Character();
    character->setId(atoi(rs->GetData(i,0).c_str()));
    character->setName(rs->GetData(i,1));
    character->setUser(user);
    character->setMesh(rs->GetData(0,3));
    character->setPos((float)atof(rs->GetData(0,4).c_str()),
      (float)atof(rs->GetData(0,5).c_str()),
      (float)atof(rs->GetData(0,6).c_str()));
    character->setSector(rs->GetData(0,7));
    
    characters.push_back(character);
  }
}  
