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

#ifndef _TABLE_MESHLIST_H_
#define _TABLE_MESHLIST_H_

class Database;
class ResultSet;

class Mesh;

#include <vector>

#include "common/util/ptstring.h"

class MeshListTable
{
private:
  Database* db;

  const Mesh* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  void parseMultiResultSet(ResultSet* rs, std::vector<const Mesh*>& list);
public:
  MeshListTable(Database* db);

  void createTable();

  void insert(const Mesh* vo);
  void remove(int id);

  unsigned int getMaxId();
  unsigned int getMaxRevision();

  unsigned int findByName(ptString name);
  void getAll(std::vector<const Mesh*>& list);
};

#endif // _TABLE_MESHLIST_H_

