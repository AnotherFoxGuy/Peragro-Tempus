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

#ifndef _TABLE_NPCAISETTING_H_
#define _TABLE_NPCAISETTING_H_

class Database;
class ResultSet;

#include "common/util/ptstring.h"

class NpcAiSettingTableVO
{
public:
  int id;
  ptString key;
  ptString value;
};

class NpcAiSettingTable
{
private:
  Database* db;

public:
  NpcAiSettingTable(Database* db);

  void createTable();

  void insert(int id, ptString key, const char* value);
  void remove(int id, ptString key);

  ptString getValue(int id, ptString key);
};

#endif // _TABLE_NPCAISETTING_H_

