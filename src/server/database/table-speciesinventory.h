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

#ifndef TABLE_SPECIESINVENTORY_H
#define TABLE_SPECIESINVENTORY_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIESINVENTORY SpeciesInventory
#define DB_TABLE_SPECIESINVENTORY_FIELDS \
  ((species_id,         size_t,             PT_PrimaryKeyS,  (Species, id) )) \
  ((position,          Common::Inventory::PositionRef, PT_PrimaryKey,   0)) \
  ((itemTemplates_id,   size_t,             0,               (ItemTemplates, id) )) \
  ((chance,             size_t,             0,                0))

PT_DECLARE_VO(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesInventoryTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)

  PT_DECLARE_CreateTable(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)

public:
  SpeciesInventoryTable(Database* db);

  PT_DECLARE_DropTable(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)

  PT_DECLARE_Insert(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)
  //PT_DECLARE_Delete(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)
  //PT_DECLARE_GetAll(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)

  PT_DECLARE_Get(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)
  //PT_DECLARE_GetSingle(SpeciesInventoryTable, DB_TABLE_SPECIESINVENTORY, DB_TABLE_SPECIESINVENTORY_FIELDS)
};

#endif //TABLE_SPECIESINVENTORY_H
