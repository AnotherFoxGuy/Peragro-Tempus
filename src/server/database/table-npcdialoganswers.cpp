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

#include <stdlib.h>

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
  vo->npcid = atoi(rs->GetData(row,0).c_str());
  vo->dialogid = atoi(rs->GetData(row,1).c_str());
  vo->answerid = atoi(rs->GetData(row,2).c_str());
  vo->text = rs->GetData(row,3);
  vo->isend = atoi(rs->GetData(row,4).c_str());
  vo->nextdialogid = atoi(rs->GetData(row,5).c_str());
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
             "npcid INTEGER,"
             "dialogid INTEGER,"
             "answerid INTEGER,"
             "text TEXT,"
             "isend INTEGER,"
             "nextdialogid INTEGER );");

/*  insert(5,0,0,"Yes, I'm new here, please tell me.", 0, 1);
  insert(5,0,1,"No, I'm not interested.", 0, 5);
  insert(5,0,2,"I'm in a hurry, can you help me?", 0, 10);
  insert(5,0,3,"I'm in a hurry, maybe another time.", 1, 0);

  insert(5,1,0,"No, it's ok. After all, it was worth the try.", 0, 2);
  insert(5,1,1,"How dare you... If fighting was implemented I would so kill you, u n00b!!111", 0, 3);

  insert(5,3,0,"No, I don't want anything from you.", 0, 5);
  insert(5,3,1,"Yes, that would be very kind of you.", 0, 4);

  insert(3,0,0,"I heard you sell items.", 0, 8);
  insert(3,0,1,"I want to sell my goods!", 0, 7);
  insert(3,0,2,"N..nothing...", 1, 0);

  insert(5,10, 0, "I think I'll stay here for a bit.", 1, 0);
  insert(5,10, 1, "To the tavern!", 0, 11);
  insert(5,10, 2, "To Vaalnor!", 0, 12);*/
}

void NpcDialogAnswersTable::dropTable()
{
  db->update("drop table npcdialoganswers");
}

void NpcDialogAnswersTable::insert(int npcid, int dialogid, int answerid, const char* text, int end, int nextdialogid)
{
  const char* query = { "insert into npcdialoganswers(npcid, dialogid, answerid, text, isend, nextdialogid) values (%d, %d, %d, '%q', %d, %d);" };
  db->update(query, npcid, dialogid, answerid, text, end, nextdialogid);
}

void NpcDialogAnswersTable::remove(int npcid, int dialogid, int answerid)
{
  db->update("delete from npcdialoganswers where dialogid = %d and answerid = %d and npcid = %d", dialogid, answerid, npcid);
}

void NpcDialogAnswersTable::removeAll()
{
  db->update("delete from npcdialoganswers");
}

Array<NpcDialogAnswersTableVO*> NpcDialogAnswersTable::getAll()
{
  ResultSet* rs = db->query("select * from npcdialoganswers;");
  Array<NpcDialogAnswersTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

