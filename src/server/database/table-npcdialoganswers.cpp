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
#include "table-npcdialoganswers.h"

NpcDialogAnswersTable::NpcDialogAnswersTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from npcdialoganswers;");
  delete rs;
  if (rs == 0)
  {
    createTable();
  }
  ResultSet* rs2 = db->query("select count(*) from npcdialoganswers;");
  delete rs2;
}

NpcDialogAnswersTableVO* NpcDialogAnswersTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  NpcDialogAnswersTableVO* vo = new NpcDialogAnswersTableVO();
  vo->dialogid = atoi(rs->GetData(row,0).c_str());
  vo->answerid = atoi(rs->GetData(row,1).c_str());
  vo->text = rs->GetData(row,2);
  vo->isend = atoi(rs->GetData(row,3).c_str());
  vo->nextdialogid = atoi(rs->GetData(row,4).c_str());
  return vo;
}

Array<NpcDialogAnswersTableVO*> NpcDialogAnswersTable::parseMultiResultSet(ResultSet* rs)
{
  Array<NpcDialogAnswersTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    NpcDialogAnswersTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}

void NpcDialogAnswersTable::createTable()
{
  printf("Creating Table npcdialoganswers...\n");
  db->update("create table npcdialoganswers ("
             "dialogid INTEGER,"
             "answerid INTEGER,"
             "text TEXT,"
             "isend INTEGER,"
             "nextdialogid INTEGER,"
             "PRIMARY KEY (DialogId, AnswerId) );");

  insert(0,0,"Yes, I'm new here, please tell me.", 0, 1);
  insert(0,1,"No, I'm not interested.", 0, 5);
  insert(0,2,"I'm in a hurry, can you help me?", 0, 10);
  insert(0,3,"I'm in a hurry, maybe another time.", 1, 0);

  insert(1,0,"No, it's ok. After all, it was worth the try.", 0, 2);
  insert(1,1,"How dare you... If fighting were implemented I would so kill you, u n00b!!111", 0, 3);

  insert(3,0,"No, I don't want anything from you.", 0, 5);
  insert(3,1,"Yes, that would be very kind of you.", 0, 4);

  insert(6,0,"I heard you sell items.", 0, 8);
  insert(6,1,"I want to sell my goods!", 0, 7);
  insert(6,2,"N..nothing...", 1, 0);

  insert(10, 0, "I think I'll stay here for a bit.", 1, 0);
  insert(10, 1, "To the tavern!", 0, 11);
  insert(10, 2, "To sueastside's place or whatever!", 0, 12);
}

void NpcDialogAnswersTable::insert(int dialogid, int answerid, const char* text, int end, int nextdialogid)
{
  const char* query = { "insert into npcdialoganswers(dialogid, answerid, text, isend, nextdialogid) values (%d, %d, '%q', %d, %d);" };
  db->update(query, dialogid, answerid, text, end, nextdialogid);
}

void NpcDialogAnswersTable::remove(int dialogid, int answerid)
{
  db->update("delete from npcdialoganswers where dialogid = %d and answerid = %d", dialogid, answerid);
}

Array<NpcDialogAnswersTableVO*> NpcDialogAnswersTable::getAll()
{
  ResultSet* rs = db->query("select * from npcdialoganswers;");
  Array<NpcDialogAnswersTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

