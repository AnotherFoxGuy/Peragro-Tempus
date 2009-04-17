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

#include "table-skilltypes.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

SkillTypesTable::SkillTypesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_SKILLTYPES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(0, "N", "None");

    Insert(1, "O", "One Handed Axe");
    Insert(2, "O", "One Handed Blunt");
    Insert(3, "O", "One Handed Flail");
    Insert(4, "O", "One Handed Sword");
    Insert(5, "O", "One Handed Unarmed");

    Insert(6, "T", "Two Handed Axe");
    Insert(7, "T", "Two Handed Blunt");
    Insert(8, "T", "Two Handed Flail");
    Insert(9, "T", "Two Handed Sword");
    Insert(10, "T", "Two Handed Unarmed");

    Insert(11, "R", "Bow");
    Insert(12, "R", "Crossbow");
    Insert(13, "R", "Throw");

    Insert(13, "D", "Armor Proficiency");
    Insert(14, "D", "Block");
    Insert(15, "D", "Dodge");
    Insert(16, "D", "Parry");

    Insert(17, "RO", "Sneak");
    Insert(18, "RO", "Pick Lock");
    Insert(19, "RO", "Pick Pocket");
    Insert(20, "RO", "Perception");

    Insert(21, "P", "Farmer");
    Insert(22, "P", "Fisher");
    Insert(23, "P", "Herbalist");
    Insert(24, "P", "Miner");
    Insert(25, "P", "Skinner");

    Insert(26, "C", "Apothecary");
    Insert(27, "C", "Blacksmith");
    Insert(28, "C", "Carpenter");
    Insert(29, "C", "Cook");
    Insert(30, "C", "Mason");
    Insert(31, "C", "Tailor");

    Insert(32, "S", "Bard");
    Insert(33, "S", "Merchant");
    Insert(34, "S", "Sage");
    Insert(35, "S", "Teacher");

    Insert(36, "OT", "Healer");
    Insert(37, "OT", "Trainer");
  }
  delete rs;
}

PT_DEFINE_CreateTable(SkillTypesTable, DB_TABLE_SKILLTYPES, DB_TABLE_SKILLTYPES_FIELDS)
PT_DEFINE_DropTable(SkillTypesTable, DB_TABLE_SKILLTYPES, DB_TABLE_SKILLTYPES_FIELDS)
PT_DEFINE_Insert(SkillTypesTable, DB_TABLE_SKILLTYPES, DB_TABLE_SKILLTYPES_FIELDS)
PT_DEFINE_ParseSingleResultSet(SkillTypesTable, DB_TABLE_SKILLTYPES, DB_TABLE_SKILLTYPES_FIELDS)
PT_DEFINE_ParseMultiResultSet(SkillTypesTable, DB_TABLE_SKILLTYPES, DB_TABLE_SKILLTYPES_FIELDS)
PT_DEFINE_GetAll(SkillTypesTable, DB_TABLE_SKILLTYPES, DB_TABLE_SKILLTYPES_FIELDS)

