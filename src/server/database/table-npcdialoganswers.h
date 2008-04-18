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

#ifndef TABLE_NPCDIALOGANSWERS_H
#define TABLE_NPCDIALOGANSWERS_H

class Database;
class ResultSet;

#include <string>

class NpcDialogAnswersTableVO
{
public:
  int npcid;
  int dialogid;
  int answerid;
  std::string text;
  int isend;
  int nextdialogid;
};

class NpcDialogAnswersTable
{
private:
  Database* db;

  NpcDialogAnswersTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<NpcDialogAnswersTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  NpcDialogAnswersTable(Database* db);

  void createTable();
  void dropTable();

  void insert(int npcid, int dialogid, int answerid, const char* text, int end, int nextdialogid);
  void remove(int npcid, int dialogid, int answerid);
  void removeAll();

  Array<NpcDialogAnswersTableVO*> getAll();
};

#endif // TABLE_NPCDIALOGANSWERS_H
