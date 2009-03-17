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

#ifndef TABLE_CHARACTERS_H
#define TABLE_CHARACTERS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_CHARACTERS Characters
#define DB_TABLE_CHARACTERS_FIELDS \
  ((entity_id,          size_t,             1,            0)) \
  ((name,               std::string,        0,            0)) \
  ((meshes_id,          size_t,             0,            (Meshes, id) )) \
  ((hairColor,          WFMath::Point<3>,   0,            0)) \
  ((skinColor,          WFMath::Point<3>,   0,            0)) \
  ((decalColor,         WFMath::Point<3>,   0,            0)) \


PT_DECLARE_VO(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)

/**
 * Provides an interface to the database to handle storage of characters.
 */
class CharactersTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)

  PT_DECLARE_CreateTable(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)

public:
  CharactersTable(Database* db);

  PT_DECLARE_DropTable(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)

  PT_DECLARE_Insert(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)
  PT_DECLARE_GetAll(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)

  PT_DECLARE_GetSingle(CharactersTable, DB_TABLE_CHARACTERS, DB_TABLE_CHARACTERS_FIELDS)
};

#endif //TABLE_CHARACTERS_H
