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

#ifndef _TABLE_DOORS_H_
#define _TABLE_DOORS_H_

class Database;
class ResultSet;

#include "common/util/ptstring.h"
#include "table.h"

class DoorsTableVO
{
public:
  int id;
  ptString name;
  int islocked;
  int isopen;
};

class DoorsTable : public Table
{
private:
  DoorsTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<DoorsTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  DoorsTable(Database* db);

  void createTable();
  void dropTable();

  void insert(DoorsTableVO* vo);
  void remove(int id);

  unsigned int getCount();
  bool existsByName(ptString name);
  bool existsById(int id);
  DoorsTableVO* getByName(ptString name);
  DoorsTableVO* getById(int id);
  Array<DoorsTableVO*> getAll();
};

#endif // _TABLE_DOORS_H_

