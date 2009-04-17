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

#ifndef TABLE_ATTRIBUTES_H
#define TABLE_ATTRIBUTES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ATTRIBUTES Attributes
#define DB_TABLE_ATTRIBUTES_FIELDS \
  ((entity_id,         size_t,             PT_PrimaryKeyS,  (Entities, id) )) \
  ((AttributeTypes_id, size_t,             PT_PrimaryKey,   (AttributeTypes, id) )) \
  ((value,             int,                0,               0))

PT_DECLARE_VO(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class AttributesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)

  PT_DECLARE_CreateTable(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)

public:
  AttributesTable(Database* db);

  PT_DECLARE_DropTable(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)

  PT_DECLARE_Insert(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)
  //PT_DECLARE_Delete(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)
  //PT_DECLARE_GetAll(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)
  //PT_DECLARE_DeleteAll(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)

  PT_DECLARE_Get(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)
  //PT_DECLARE_GetSingle(AttributesTable, DB_TABLE_ATTRIBUTES, DB_TABLE_ATTRIBUTES_FIELDS)
};

#endif //TABLE_ATTRIBUTES_H
