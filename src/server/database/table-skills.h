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

#ifndef TABLE_SKILLS_H
#define TABLE_SKILLS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name          | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SKILLS Skills
#define DB_TABLE_SKILLS_FIELDS \
  ((entity_id,    size_t,             1,            (Entities, id) )) \
  ((SkillTypes_id,size_t,             0,            (SkillTypes, id) )) \
  ((xp,           float,              0,             0))

PT_DECLARE_VO(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)

/**
 * Provides an interface to the database to handle storage of Skillss.
 */
class SkillsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)

  PT_DECLARE_CreateTable(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)

public:
  SkillsTable(Database* db);

  PT_DECLARE_DropTable(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)

  PT_DECLARE_Insert(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)
  PT_DECLARE_GetAll(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)

  PT_DECLARE_Get(SkillsTable, DB_TABLE_SKILLS, DB_TABLE_SKILLS_FIELDS)
};

#endif //TABLE_SKILLS_H
