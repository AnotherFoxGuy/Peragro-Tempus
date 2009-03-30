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

#include "table-npcaisetting.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

NpcAiSettingTable::NpcAiSettingTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_NPCAISETTING) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
PT_DEFINE_DropTable(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
PT_DEFINE_ParseSingleResultSet(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
PT_DEFINE_ParseMultiResultSet(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
PT_DEFINE_Insert(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
PT_DEFINE_Delete(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
//PT_DEFINE_GetAll(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
//PT_DEFINE_DeleteAll(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
//PT_DEFINE_Get(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)
PT_DEFINE_GetSingle(NpcAiSettingTable, DB_TABLE_NPCAISETTING, DB_TABLE_NPCAISETTING_FIELDS)

