/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef TABLE_REPUTATIONTYPES_H
#define TABLE_REPUTATIONTYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name    | C++ type name     | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_REPUTATIONTYPES ReputationTypes
#define DB_TABLE_REPUTATIONTYPES_FIELDS \
  ((id,       size_t,             PT_PrimaryKey,  0)) \
  ((name,     std::string,        0,              0))

PT_DECLARE_VO(ReputationTypesTable, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)

/**
 * Provides an interface to the database to handle storage of different 
 * possible reputations.
 */
class ReputationTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ReputationTypes, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(HobbyTypesTable, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)

  PT_DECLARE_CreateTable(ReputationTypes, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)

  PT_DECLARE_Insert(ReputationTypes, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)

public:
  ReputationTypesTable(Database* db);

  PT_DECLARE_DropTable(ReputationTypes, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)
  PT_DECLARE_GetAll(ReputationTypes, DB_TABLE_REPUTATIONTYPES, DB_TABLE_REPUTATIONTYPES_FIELDS)

};

#endif //TABLE_REPUTATIONTYPES_H
