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

#include "database.h"
#include "table-spawnpoints.h"

SpawnPointsTable::SpawnPointsTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from spawnpoints;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

SpawnPointsTableVO* SpawnPointsTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  SpawnPointsTableVO* vo = new SpawnPointsTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->item = atoi(rs->GetData(row,1).c_str());
  vo->pos_x = (float) atof(rs->GetData(row,2).c_str());
  vo->pos_y = (float) atof(rs->GetData(row,3).c_str());
  vo->pos_z = (float) atof(rs->GetData(row,4).c_str());
  vo->sector = ptString(rs->GetData(row,5).c_str(), rs->GetData(row,3).length());
  vo->interval = atoi(rs->GetData(row,6).c_str());
  return vo;
}

Array<SpawnPointsTableVO*> SpawnPointsTable::parseMultiResultSet(ResultSet* rs)
{
  Array<SpawnPointsTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    SpawnPointsTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}

void SpawnPointsTable::createTable()
{
  printf("Creating Table spawnpoints...\n");
  db->update("create table spawnpoints ("
             "id INTEGER,"
             "item INTEGER,"
             "pos_x FLOAT, pos_y FLOAT, pos_z FLOAT,"
             "sector TEXT,"
             "interval INTEGER,"
             "PRIMARY KEY (Id) );");
}

void SpawnPointsTable::insert(SpawnPointsTableVO* vo)
{
  const char* query = { "insert into spawnpoints(id, item, pos_x, pos_y, pos_z, sector, interval) values (%d, %d, %.4f, %.4f, %.4f, '%s', %d);" };
  db->update(query, vo->id, vo->item, vo->pos_x, vo->pos_y, vo->pos_z, *vo->sector, vo->interval);
}

void SpawnPointsTable::remove(int id)
{
  db->update("delete from spawnpoints where id = %d");
}

Array<SpawnPointsTableVO*> SpawnPointsTable::getAll()
{
  ResultSet* rs = db->query("select * from spawnpoints;");
  Array<SpawnPointsTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

