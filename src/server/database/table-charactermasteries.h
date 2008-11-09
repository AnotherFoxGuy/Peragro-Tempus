/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef TABLE_CHARACTER_MASTERIES_H
#define TABLE_CHARACTER_MASTERIES_H

class Database;
class ResultSet;

#include <string>
#include "common/util/ptstring.h"
#include "common/database/table.h"


class CharacterMasteryTableVO
{
public:
  unsigned int masteryType;
  unsigned int masteryID;
  int masteryLevel;
};

class CharacterMasteriesTable : public Table
{
public:
  CharacterMasteriesTable(Database* db);
  void createTable();
  void insert(unsigned int characterID, unsigned int masteryType, int level);
  void set(unsigned int characterID, unsigned int masteryType, int level);
  int get(int characterID, unsigned int masteryType);
  void dropTable();
  Array<CharacterMasteryTableVO*> getAllMasteries(unsigned int characterID);
  CharacterMasteryTableVO* parseSingleResultSet(ResultSet* rs, size_t row);
  Array<CharacterMasteryTableVO*> parseMultiResultSet(ResultSet* rs);
};

#endif //TABLE_CHARACTER_MASTERIES_H
