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

#ifndef TABLE_MESHES_H
#define TABLE_MESHES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_MESHES Meshes
#define DB_TABLE_MESHES_FIELDS \
  ((id,                 size_t,             1,            0)) \
  ((revision,           size_t,             0,            0)) \
  ((factoryName,        std::string,        0,            0)) \
  ((fileName,           std::string,        0,            0)) \

PT_DECLARE_VO(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class MeshesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

  PT_DECLARE_CreateTable(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

public:
  MeshesTable(Database* db);

  PT_DECLARE_DropTable(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

  PT_DECLARE_Insert(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
  PT_DECLARE_GetAll(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

  PT_DECLARE_GetSingle(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

  size_t GetMaxId();
  size_t FindBy(const std::string& factoryName, const std::string& fileName);
};

#endif //TABLE_MESHES_H
