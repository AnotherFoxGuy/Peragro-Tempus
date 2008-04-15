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
#include "table-config.h"

ConfigTable::ConfigTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from config;");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

ConfigTableVO* ConfigTable::ParseSingleResultSet(ResultSet* rs, size_t row)
{
  ConfigTableVO* vo = new ConfigTableVO();
  vo->name = ptString(rs->GetData(row,0).c_str(), rs->GetData(row,0).length());
  vo->value = ptString(rs->GetData(row,1).c_str(), rs->GetData(row,1).length());
  return vo;
}

Array<ConfigTableVO*> ConfigTable::ParseMultiResultSet(ResultSet* rs)
{
  Array<ConfigTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    ConfigTableVO* obj = ParseSingleResultSet(rs, i);
    arr.add(obj);
  }
  return arr;
}

void ConfigTable::CreateTable()
{
  printf("Creating Table config...\n");
  db->update("create table config ("
             "name TEXT,"
             "value TEXT,"
             "PRIMARY KEY (name) );");
}

void ConfigTable::Insert(ConfigTableVO* vo)
{
  const char* query = { "delete from config where name='%s'; insert into config(name, value) values ('%s', '%s');" };
  db->update(query, *vo->name, *vo->name, *vo->value);
}

void ConfigTable::Remove(ptString name)
{
  db->update("delete from config where id = '%s'", *name);
}

ptString ConfigTable::GetConfigValue(ptString name)
{
  ResultSet* rs = db->query("select * from config;");
  if (!rs) return ptString("",0);
  Array<ConfigTableVO*> vo = ParseMultiResultSet(rs);
  unsigned int i = 0;
  while(i<vo.getCount())
  {
    if(vo[i]->name == name){return vo[i]->value;}
    i++;
  }
  delete rs;
  return ptString("",0);
}
