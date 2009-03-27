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

#ifndef TABLE_NPCDIALOGS_H
#define TABLE_NPCDIALOGS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type        | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_NPCDIALOGS NpcDialogs
#define DB_TABLE_NPCDIALOGS_FIELDS \
  ((entity_id,         size_t,             PT_PrimaryKey,   (Entities, id) )) \
  ((id,                size_t,             PT_CandidateKey, 0)) \
  ((text,              std::string,        0,               0)) \
  ((action,            std::string,        0,               0)) \

PT_DECLARE_VO(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class NpcDialogsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)

  PT_DECLARE_CreateTable(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)

public:
  NpcDialogsTable(Database* db);

  PT_DECLARE_DropTable(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)

  PT_DECLARE_Insert(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)
  //PT_DECLARE_Delete(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)
  PT_DECLARE_GetAll(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)
  PT_DECLARE_DeleteAll(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)

  PT_DECLARE_Get(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)
  //PT_DECLARE_GetSingle(NpcDialogsTable, DB_TABLE_NPCDIALOGS, DB_TABLE_NPCDIALOGS_FIELDS)
};

#endif //TABLE_NPCDIALOGS_H
