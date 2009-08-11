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

#ifndef TABLE_AVATARMESHES_H
#define TABLE_AVATARMESHES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;


//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_AVATARMESHES AvatarMeshes
#define DB_TABLE_AVATARMESHES_FIELDS \
  ((id,                      size_t,             PT_PrimaryKeyS,     0 )) \
  ((mesh_id,                 size_t,             0,                  0 )) \
  ((species_id,              size_t,             0,                  0 )) \
  ((name,                    std::string,        0,                  0 )) \

PT_DECLARE_VO(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

/**
 * Provides an interface to the database to handle storage of character
 * inventory.
 */
class AvatarMeshesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

  PT_DECLARE_CreateTable(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

public:
  AvatarMeshesTable(Database* db);

  PT_DECLARE_DropTable(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

  PT_DECLARE_Insert(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
  PT_DECLARE_Delete(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

  PT_DECLARE_GetAll(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

  PT_DECLARE_Get(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
  PT_DECLARE_GetSingle(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
  AvatarMeshesTableVOArray GetAvatarList();

};

#endif //TABLE_AVATARMESHES_H
