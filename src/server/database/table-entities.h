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

#ifndef _TABLE_ENTITIES_H_
#define _TABLE_ENTITIES_H_

#include "table.h"

class Database;
class ResultSet;

#include "common/util/ptstring.h"

class EntitiesTableVO
{
public:
  int id;
  ptString name;
  int type;
  int item;
  int variation;
  ptString mesh;
  float pos_x;
  float pos_y;
  float pos_z;
  float rotation;
  ptString sector;
};

class EntityTable : public Table
{
private:
  EntitiesTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<EntitiesTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  EntityTable(Database* db);
  void createTable();
  void insert(int id, ptString name, int type, int item, unsigned int variation, ptString mesh, const float pos[3], float rot, ptString sector);
  int getMaxId();
  void dropTable();
  void remove(int id);
  void update(EntitiesTableVO* entity);
  bool existsEntity(ptString name);
  EntitiesTableVO* getEntity(ptString name);
  Array<EntitiesTableVO*> getAllEntities();
};

#endif // _TABLE_ENTITIES_H_
