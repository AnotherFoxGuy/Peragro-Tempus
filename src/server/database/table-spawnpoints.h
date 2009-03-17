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

#ifndef TABLE_SPAWNPOINTS_H
#define TABLE_SPAWNPOINTS_H

class Database;
class ResultSet;

#include "common/database/table.h"

#include "common/util/ptstring.h"

class SpawnPointsTableVO
{
public:
  int id;
  int item;
  float pos_x;
  float pos_y;
  float pos_z;
  ptString sector;
  int interval;
};

class SpawnPointsTable : public Table
{
private:
  SpawnPointsTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<SpawnPointsTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  SpawnPointsTable(Database* db);

  void createTable();
  void truncate();

  void insert(SpawnPointsTableVO* vo);
  void remove(int id);

  Array<SpawnPointsTableVO*> getAll();
};

#endif // TABLE_SPAWNPOINTS_H
