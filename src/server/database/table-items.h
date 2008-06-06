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

#ifndef TABLE_ITEMS_H
#define TABLE_ITEMS_H

#include "table.h"

class Database;
class Item;
class ptString;

class ItemTable : public Table
{
public:
  ItemTable(Database* db);
  void createTable();
  void insert(int id, ptString name, ptString icon, ptString description, ptString file, ptString mesh, float weight, ptString equiptype);
  void remove(int id);
  void dropTable();
  bool existsItem(ptString name);
  Item* getItem(ptString name);
  Item* getItem(int id);
  void getAllItems(Array<Item*>& items);
};

#endif //TABLE_ITEMS_H
