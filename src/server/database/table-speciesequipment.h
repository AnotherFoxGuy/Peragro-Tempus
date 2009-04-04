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

#ifndef TABLE_SPECIESEQUIPMENT_H
#define TABLE_SPECIESEQUIPMENT_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIESEQUIPMENT SpeciesEquipment
#define DB_TABLE_SPECIESEQUIPMENT_FIELDS \
  ((species_id,         size_t,             PT_PrimaryKeyS,  (Species, id) )) \
  ((equipType_id,       size_t,             PT_PrimaryKey,   (EquipTypes, id) )) \
  ((itemTemplates_id,   size_t,             0,               (ItemTemplates, id) )) \
  ((chance,             float,              0,                0))

PT_DECLARE_VO(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesEquipmentTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)

  PT_DECLARE_CreateTable(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)

public:
  SpeciesEquipmentTable(Database* db);

  PT_DECLARE_DropTable(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)

  PT_DECLARE_Insert(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)
  //PT_DECLARE_Delete(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)
  //PT_DECLARE_GetAll(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)

  //PT_DECLARE_Get(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)
  //PT_DECLARE_GetSingle(SpeciesEquipmentTable, DB_TABLE_SPECIESEQUIPMENT, DB_TABLE_SPECIESEQUIPMENT_FIELDS)
};

#endif //TABLE_SPECIESEQUIPMENT_H
