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

#ifndef TABLE_TRIANGLES_H
#define TABLE_TRIANGLES_H

class Database;
class ResultSet;

#include "common/util/ptstring.h"

class TrianglesTableVO
{
public:
  int mesh;
  int num;
  int a;
  int b;
  int c;
};

class TrianglesTable
{
private:
  Database* db;

  TrianglesTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<TrianglesTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  TrianglesTable(Database* db);

  void createTable();

  void insert(TrianglesTableVO* vo);
  void remove(int mesh, int num);

  Array<TrianglesTableVO*> getAll();
  Array<TrianglesTableVO*> getAllByMesh(int mesh);
};

#endif // TABLE_TRIANGLES_H
