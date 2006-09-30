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
#include "table-npcdialogs.h"

NpcDialogsTable::NpcDialogsTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from npcdialogs;");
  delete rs;
  if (rs == 0)
  {
    createTable();
  }
  ResultSet* rs2 = db->query("select count(*) from npcdialogs;");
  delete rs2;
}

NpcDialogsTableVO* NpcDialogsTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  NpcDialogsTableVO* vo = new NpcDialogsTableVO();
  vo->dialogid = atoi(rs->GetData(row,0).c_str());
  vo->text = rs->GetData(row,1);
  vo->isstart = atoi(rs->GetData(row,2).c_str());
  return vo;
}

Array<NpcDialogsTableVO*> NpcDialogsTable::parseMultiResultSet(ResultSet* rs)
{
  Array<NpcDialogsTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    NpcDialogsTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}

void NpcDialogsTable::createTable()
{
  db->update("create table npcdialogs ("
             "dialogid INTEGER,"
             "text TEXT,"
             "isstart INTEGER,"
             "PRIMARY KEY (DialogId) );");

  insert(0,"Hello\nWould you like to know more about keep?",1);
  insert(1,"Well, this is a bit embarrasing.\nI just wanted to sound smart, "
           "you know.\nNoone ever wanted to know something about this town "
           "and honestly, I don''t know anything about it either...",0);
  insert(2,"I'm glad you have no hard feelings. But ask me in a few days, "
           "maybe I'll have found out something about this place by then.\n\n"
           "Have a nice day.", 0);
  insert(3,"Look, I'm really sorry to have upset you... \n\n  hm... do you"
           "want an apple instead?", 0);
  insert(4,"I've seen an apple laying in the grass over there.\n"
           "If you hurry, it might still be there.", 0);
  insert(5,"Well, then.\n\nHave a nice day.",0);
}

void NpcDialogsTable::insert(int dialogid, const char* text, int start)
{
  const char* query = { "insert into npcdialogs(dialogid, text, isstart) values (%d, '%q', %d);" };
  db->update(query, dialogid, text, start);
}

void NpcDialogsTable::remove(int dialogid)
{
  db->update("delete from npcdialogs where dialogid = %d", dialogid);
}

Array<NpcDialogsTableVO*> NpcDialogsTable::getAll()
{
  ResultSet* rs = db->query("select * from npcdialogs;");
  return parseMultiResultSet(rs);
}

