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

#ifndef _TABLE_CHARACTERSTATS_H_
#define _TABLE_CHARACTERSTATS_H_

#include "common/util/array.h"
#include "table.h"

class Database;
class Stat;
class CharStat;

class CharacterStatsTable : public Table
{
public:
  CharacterStatsTable(Database* db);
  void createTable();
  void insert(int characterstat, int stat, int level);
  void set(int characterstat, Stat* stat, int level);
  int get(int characterstat, Stat* stat);
  void dropTable();
  void getAllEntries(Array<CharStat*>& entries, int id);
};

#endif //_TABLE_CHARACTERSTATS_H_
