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

#ifndef _TABLE_BOOKS_H_
#define _TABLE_BOOKS_H_

class Database;
class ResultSet;

#include "common/util/ptstring.h"
#include "table.h"

class BooksTableVO
{
public:
  int id;
  int itemId;
  ptString name;
  ptString text;

  BooksTableVO() {}
  BooksTableVO(int id, ptString name, ptString text)
  : id(id), name(name), text(text)
  {
  }
};

class BooksTable : public Table
{
private:
  BooksTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<BooksTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  BooksTable(Database* db);

  void createTable();
  void dropTable();

  void insert(BooksTableVO* vo);
  void update(BooksTableVO* vo);
  void remove(BooksTableVO* id);

  unsigned int getCount(unsigned int itemId);
  bool existsByName(ptString name);
  bool existsById(int id);
  BooksTableVO* getByName(ptString name);
  BooksTableVO* getById(int id);
  Array<BooksTableVO*> getAll();
};

#endif // _TABLE_BOOKS_H_

