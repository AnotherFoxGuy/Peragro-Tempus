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

#ifndef TABLE_INVENTORY_H
#define TABLE_INVENTORY_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_INVENTORY Inventory
#define DB_TABLE_INVENTORY_FIELDS \
  ((entity_id,         size_t,             PT_PrimaryKeyS,  (Entities, id) )) \
  ((slot,              size_t,             PT_PrimaryKey,   0)) \
  ((item_id,           size_t,             0,               (Entities, id) )) \

PT_DECLARE_VO(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)

/**
 * Provides an interface to the database to handle storage of character
 * inventory.
 */
class InventoryTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)
  PT_DECLARE_ParseMultiResultSet(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)

  PT_DECLARE_CreateTable(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)

public:
  InventoryTable(Database* db);

  PT_DECLARE_DropTable(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)

  PT_DECLARE_Insert(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)
  PT_DECLARE_Delete(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)

  PT_DECLARE_GetAll(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)

  PT_DECLARE_Get(InventoryTable, DB_TABLE_INVENTORY, DB_TABLE_INVENTORY_FIELDS)
};

#endif //TABLE_INVENTORY_H
