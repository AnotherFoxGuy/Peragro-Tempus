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

#ifndef TABLE_NPCENTITIES_H
#define TABLE_NPCENTITIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_NPCENTITIES NpcEntities
#define DB_TABLE_NPCENTITIES_FIELDS \
  ((entity_id,         size_t,             PT_PrimaryKey, (Entities, id) )) \
  ((ainame,            std::string,        0,             0)) \

PT_DECLARE_VO(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)

/**
 * Provides an interface to the database to handle storage of NPCs.
 */
class NpcEntitiesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)

  PT_DECLARE_CreateTable(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)

public:
  NpcEntitiesTable(Database* db);

  PT_DECLARE_DropTable(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)

  PT_DECLARE_Insert(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)
  PT_DECLARE_Delete(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)
  PT_DECLARE_DeleteAll(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)
  PT_DECLARE_GetAll(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)

  PT_DECLARE_GetSingle(NpcEntitiesTable, DB_TABLE_NPCENTITIES, DB_TABLE_NPCENTITIES_FIELDS)
};

#endif //TABLE_NPCENTITIES_H
