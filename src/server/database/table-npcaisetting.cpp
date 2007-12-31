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
#include "table-npcaisetting.h"

NpcAiSettingTable::NpcAiSettingTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from npcaisetting;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void NpcAiSettingTable::createTable()
{
  printf("Creating Table npcaisetting...\n");
  db->update("create table npcaisetting ("
             "id INTEGER,"
             "key TEXT,"
             "value TEXT,"
             "PRIMARY KEY (id, key) );");

  //insert(1, ptString("base_x", 6), "29.0");
  //insert(1, ptString("base_y", 6), "0.15");
  //insert(1, ptString("base_z", 6), "106.2");
  //insert(1, ptString("radius_x", 8), "10");
  //insert(1, ptString("radius_y", 8), "0");
  //insert(1, ptString("radius_z", 8), "10");
  //insert(1, ptString("interval_base", 13), "100");
  //insert(1, ptString("interval_rand", 13), "50");

  //insert(2, ptString("base_x", 6), "41");
  //insert(2, ptString("base_y", 6), "4");
  //insert(2, ptString("base_z", 6), "172");
  //insert(2, ptString("radius_x", 8), "10");
  //insert(2, ptString("radius_y", 8), "0");
  //insert(2, ptString("radius_z", 8), "10");
  //insert(2, ptString("interval_base", 13), "100");
  //insert(2, ptString("interval_rand", 13), "50");

  //insert(3, ptString("start_delay", 11), "10");
  //insert(3, ptString("speed",        5), "3");
  //insert(3, ptString("waypoints",    9), "4");
  //insert(3, ptString("point_0_x",    9), "51.75");
  //insert(3, ptString("point_0_y",    9), "2.03");
  //insert(3, ptString("point_0_z",    9), "90.95");
  //insert(3, ptString("interval_0",  10), "120");
  //insert(3, ptString("point_1_x",    9), "66.44");
  //insert(3, ptString("point_1_y",    9), "2.03");
  //insert(3, ptString("point_1_z",    9), "54.96");
  //insert(3, ptString("interval_1",  10), "120");
  //insert(3, ptString("point_2_x",    9), "6.16");
  //insert(3, ptString("point_2_y",    9), "2.03");
  //insert(3, ptString("point_2_z",    9), "68.14");
  //insert(3, ptString("interval_2",  10), "200");
  //insert(3, ptString("point_3_x",    9), "14.50");
  //insert(3, ptString("point_3_y",    9), "2.03");
  //insert(3, ptString("point_3_z",    9), "100.67");
  //insert(3, ptString("interval_3",  10), "110");
}

void NpcAiSettingTable::insert(int id, ptString key, const char* value)
{
  const char* query = { "insert into npcaisetting(id, key, value) values (%d, '%q', '%q');" };
  db->update(query, id, *key, value);
}

void NpcAiSettingTable::remove(int id, ptString key)
{
  db->update("delete from npcaisetting where id = %d and key like '%q'", id, *key);
}

void NpcAiSettingTable::removeAll(int id)
{
  db->update("delete from npcaisetting where id = %d", id);
}

ptString NpcAiSettingTable::getValue(int id, ptString key)
{
  ResultSet* rs = db->query("select value from npcaisetting where id = %d and key like '%s';", id, *key);
  if(!rs || rs->GetRowCount() == 0) return ptString::Null;
  ptString value(rs->GetData(0,0).c_str(), rs->GetData(0,0).length());
  delete rs;
  return value;
}

