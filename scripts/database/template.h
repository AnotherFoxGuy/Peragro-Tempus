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

#ifndef TABLE_REPUTATIONS_H
#define TABLE_REPUTATIONS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_REPUTATIONS Reputations
#define DB_TABLE_REPUTATIONS_FIELDS \
  ((entity_id,         size_t,             1,            (Entities, id) )) \

PT_DECLARE_VO(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ReputationsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)

  PT_DECLARE_CreateTable(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)

public:
  ReputationsTable(Database* db);

  PT_DECLARE_DropTable(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)

  PT_DECLARE_Insert(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)
  PT_DECLARE_GetAll(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)

  //PT_DECLARE_Get(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)
  //PT_DECLARE_GetSingle(ReputationsTable, DB_TABLE_REPUTATIONS, DB_TABLE_REPUTATIONS_FIELDS)
};

#endif //TABLE_REPUTATIONS_H
