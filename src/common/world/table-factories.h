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

#ifndef TABLE_FACTORIES_H
#define TABLE_FACTORIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name      | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_FACTORIES Factories
#define DB_TABLE_FACTORIES_FIELDS \
  ((factoryFile,         std::string,         PT_CandidateKey,    0)) \
  ((factoryName,         std::string,         PT_CandidateKey,    0)) \
  ((boundingBox,         WFMath::AxisBox<3>,  0,                  0)) \
  ((detailLevel,         size_t,              0,                  0)) \
  ((hash,                std::string,         0,                  0)) \

PT_DECLARE_VO(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class FactoriesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)

  PT_DECLARE_CreateTable(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)

public:
  FactoriesTable(Database* db);

  PT_DECLARE_DropTable(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)

  PT_DECLARE_Insert(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)
  PT_DECLARE_Delete(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)
  PT_DECLARE_GetAll(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)
  PT_DECLARE_DeleteAll(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)

  //PT_DECLARE_Get(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)
  PT_DECLARE_GetSingle(FactoriesTable, DB_TABLE_FACTORIES, DB_TABLE_FACTORIES_FIELDS)
};

#endif //TABLE_FACTORIES_H
