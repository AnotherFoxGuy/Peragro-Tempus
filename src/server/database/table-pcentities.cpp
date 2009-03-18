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

#include "table-pcentities.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

PcEntitiesTable::PcEntitiesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_PCENTITIES) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
PT_DEFINE_DropTable(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
PT_DEFINE_Insert(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
PT_DEFINE_ParseSingleResultSet(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
PT_DEFINE_ParseMultiResultSet(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
PT_DEFINE_GetAll(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)
PT_DEFINE_Get(PcEntitiesTable, DB_TABLE_PCENTITIES, DB_TABLE_PCENTITIES_FIELDS)

PcEntitiesTableVOArray PcEntitiesTable::GetAll(const std::string& login)
{
  ResultSet* rs = db->query(
    "select * " \
    "from PcEntities " \
    "where users_login='%s';",
    login.c_str());
  PcEntitiesTableVOArray arr;
  if (!rs) return arr;
  arr = ParseMultiResultSet(rs);
  delete rs;
  return arr;
}

