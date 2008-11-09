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

#ifndef TABLE_CHARACTERREPUTATIONS_H
#define TABLE_CHARACTERREPUTATIONS_H

#include "common/util/array.h"
#include "common/database/table.h"

class Database;
class Reputation;
class CharReputation;

class CharacterReputationsTable : public Table
{
public:
  CharacterReputationsTable(Database* db);
  void createTable();
  void insert(int characterreputation, int reputation, int level);
  void set(int characterreputation, Reputation* reputation, int level);
  int get(int characterreputation, Reputation* reputation);
  void dropTable();
  void getAllEntries(Array<CharReputation*>& entries, int id);
};

#endif //TABLE_CHARACTERREPUTATIONS_H
