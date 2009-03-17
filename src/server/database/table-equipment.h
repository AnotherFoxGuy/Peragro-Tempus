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

#ifndef TABLE_EQUIPMENT_H
#define TABLE_EQUIPMENT_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_EQUIPMENT Equipment
#define DB_TABLE_EQUIPMENT_FIELDS \
  ((entity_id,         size_t,             1,            (Entities, id) )) \
  ((equipType_id,      size_t,             0,            (EquipTypes, id) )) \
  ((item_id,           size_t,             0,            (Entities, id) ))

PT_DECLARE_VO(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class EquipmentTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)
  PT_DECLARE_ParseMultiResultSet(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)

  PT_DECLARE_CreateTable(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)

public:
  EquipmentTable(Database* db);

  PT_DECLARE_DropTable(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)

  PT_DECLARE_Insert(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)
  PT_DECLARE_GetAll(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)

  PT_DECLARE_Get(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)
  PT_DECLARE_Delete(EquipmentTable, DB_TABLE_EQUIPMENT, DB_TABLE_EQUIPMENT_FIELDS)
};

#endif //TABLE_EQUIPMENT_H
