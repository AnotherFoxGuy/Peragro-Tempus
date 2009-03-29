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

#ifndef TABLE_OBJECTS_H
#define TABLE_OBJECTS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_OBJECTS Objects
#define DB_TABLE_OBJECTS_FIELDS \
  ((id,                  size_t,         PT_PrimaryKey,    0)) \
  ((name,                std::string,         0,                0)) \
  ((factoryFile,         std::string,         0,                (Factories, factoryFile))) \
  ((factoryName,         std::string,         0,                (Factories, factoryName))) \
  ((position,            WFMath::Point<3>,    0,                0)) \
  ((rotation,            WFMath::RotMatrix<3>,0,                0)) \
  ((sector,              std::string,         0,                0)) \
  ((boundingBox,         WFMath::AxisBox<3>,  0,                0)) \
  ((detailLevel,         size_t,              0,                0)) \

PT_DECLARE_VO(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ObjectsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)

  PT_DECLARE_CreateTable(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)

public:
  ObjectsTable(Database* db);

  PT_DECLARE_DropTable(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)

  size_t GetMaxId();

  PT_DECLARE_Insert(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
  PT_DECLARE_Delete(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
  PT_DECLARE_GetAll(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
  PT_DECLARE_DeleteAll(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)

  //PT_DECLARE_Get(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
  //PT_DECLARE_GetSingle(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
};

#endif //TABLE_OBJECTS_H
