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

#ifndef TABLE_NPCAISETTING_H
#define TABLE_NPCAISETTING_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_NPCAISETTING NpcAiSetting
#define DB_TABLE_NPCAISETTING_FIELDS \
  ((entity_id,          size_t,             PT_PrimaryKeyS,   (Entities, id) )) \
  ((name,               std::string,        PT_PrimaryKey,    0 )) \
  ((value,              std::string,        0,                0 )) \

PT_DECLARE_VO(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class NpcAiSettingTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
  PT_DECLARE_ParseMultiResultSet(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)

  PT_DECLARE_CreateTable(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)

public:
  NpcAiSettingTable(Database* db);

  PT_DECLARE_DropTable(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)

  PT_DECLARE_Insert(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
  PT_DECLARE_Delete(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
  //PT_DECLARE_GetAll(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
  //PT_DECLARE_DeleteAll(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)

  //PT_DECLARE_Get(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
  PT_DECLARE_GetSingle(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
};

#endif //TABLE_NPCAISETTING_H
