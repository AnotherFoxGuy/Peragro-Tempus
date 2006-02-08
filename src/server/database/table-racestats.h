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

#ifndef _TABLE_RACESTATS_H_
#define _TABLE_RACESTATS_H_

#include "common/util/array.h"
#include "table.h"

class Database;
class Stat;
class RaceStat;

class RaceStatsTable : public Table
{
public:
  RaceStatsTable(Database* db);
  void createTable();
  void insert(int race, int stat_id, int level);
  void set(int racestat, Stat* stat, int level);
  int get(int racestat, Stat* stat);
  void dropTable();
  void getAllEntries(Array<RaceStat*>& entries, int id);
};

#endif //_TABLE_RACESTATS_H_
