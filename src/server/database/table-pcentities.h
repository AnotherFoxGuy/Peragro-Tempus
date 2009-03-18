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

#ifndef TABLE_PCENTITIES_H
#define TABLE_PCENTITIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_PCENTITIES PcEntities
#define DB_TABLE_PCENTITIES_FIELDS \
  ((entity_id,         size_t,             1,            (Entities, id) )) \
  ((users_login,       std::string,        0,            (Users, login) )) \

PT_DECLARE_VO(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

/**
 * Provides an interface to the database to handle storage of Player Characters.
 */
class PcEntitiesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

  PT_DECLARE_CreateTable(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

public:
  PcEntitiesTable(Database* db);

  PT_DECLARE_DropTable(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

  PT_DECLARE_Insert(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
  PT_DECLARE_GetAll(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

  PT_DECLARE_Get(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

  PcEntitiesTableVOArray GetAll(const std::string& login);
};

#endif //TABLE_PCENTITIES_H
