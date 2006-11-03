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

#ifndef _TABLE_NPCDIALOGS_H_
#define _TABLE_NPCDIALOGS_H_

class Database;
class ResultSet;

#include <string>
#include "common/util/ptstring.h"

class NpcDialogsTableVO
{
public:
  int dialogid;
  std::string text;
  int isstart;
  ptString action;
};

class NpcDialogsTable
{
private:
  Database* db;

  NpcDialogsTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<NpcDialogsTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  NpcDialogsTable(Database* db);

  void createTable();

  void insert(int dialogid, const char* text, int start, const char* action);
  void remove(int dialogid);

  Array<NpcDialogsTableVO*> getAll();
};

#endif // _TABLE_NPCDIALOGS_H_

