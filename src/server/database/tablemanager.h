/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#ifndef TABLES_H
#define TABLES_H

class Database;
class Table;

#include <vector>
#include <boost/shared_ptr.hpp>

class TableManager
{
private:
  Database* db;
  std::vector<boost::shared_ptr<Table> > tables;

  /// Takes ownership of the object!
  void Register(Table* table);
  
public:
  TableManager(Database* db);
  virtual ~TableManager();

  void Initialize();
  
  void Register(boost::shared_ptr<Table> table);

  template<typename T>
  T* Get()
  {
    std::vector<boost::shared_ptr<Table> >::const_iterator it;
    for (it = tables.begin(); it != tables.end(); it++)
    {
      if (!*it) continue;
      T* t = static_cast<T*>((*it).get());
      if (t) return t;
    }
    return 0;
  }
};

#endif // TABLES_H
