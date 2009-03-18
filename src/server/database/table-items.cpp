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

#include "table-items.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

ItemsTable::ItemsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_ITEMS) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)
PT_DEFINE_DropTable(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)
PT_DEFINE_Insert(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)
PT_DEFINE_ParseSingleResultSet(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)
PT_DEFINE_ParseMultiResultSet(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)
PT_DEFINE_GetAll(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)
PT_DEFINE_GetSingle(ItemsTable, DB_TABLE_ITEMS, DB_TABLE_ITEMS_FIELDS)

