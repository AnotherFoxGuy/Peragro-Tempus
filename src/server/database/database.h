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

#ifndef DATABASE_H
#define DATABASE_H

class UsersTable;
class CharacterTable;
class EntityTable;
class ItemTable;
class InventoryTable;
class StatTable;
class CharacterStatsTable;
class RaceTable;
class RaceStatsTable;
class SkillTable;
class CharacterSkillsTable;

#include "resultset.h"

class Database
{
private:
  UsersTable* userstable;
  CharacterTable* charactertable;
  EntityTable* entitytable;
  ItemTable* itemtable;
  InventoryTable* inventorytable;
  StatTable* stattable;
  CharacterStatsTable* characterstattable;
  RaceTable* racetable;
  RaceStatsTable* racestattable;
  SkillTable* skilltable;
  CharacterSkillsTable* characterskillstable;

public:
  virtual ResultSet* query(const char*) = 0;
  virtual void update(const char*, ...) = 0;
  //virtual int getLastInsertedId() = 0;

  Database();
  virtual ~Database();

  void init();

  virtual void shutdown() = 0;

  UsersTable* getUsersTable() { return userstable; }
  CharacterTable* getCharacterTable() { return charactertable; }
  EntityTable* getEntityTable() { return entitytable; }
  ItemTable* getItemTable() { return itemtable; }
  InventoryTable* getInventoryTable() { return inventorytable; }
  StatTable* getStatTable() { return stattable; }
  CharacterStatsTable* getCharacterStatTable() { return characterstattable; }
  RaceTable* getRaceTable() { return racetable; }
  RaceStatsTable* getRaceStatsTable() { return racestattable; }
  SkillTable* getSkillTable() { return skilltable; }
  CharacterSkillsTable* getCharacterSkillsTable() { return characterskillstable; }
};

#endif // DATABASE_H
