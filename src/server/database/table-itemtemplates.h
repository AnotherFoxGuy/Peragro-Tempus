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

#ifndef TABLE_ITEMTEMPLATES_H
#define TABLE_ITEMTEMPLATES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ITEMTEMPLATES ItemTemplates
#define DB_TABLE_ITEMTEMPLATES_FIELDS \
  ((id,                size_t,             1,            0 )) \
  ((name,              std::string,        0,            0)) \
  ((meshes_id,         size_t,             0,            (Meshes, id) )) \
  ((iconFile,          std::string,        0,            0)) \
  ((description,       std::string,        0,            0)) \
  ((weight,            float,              0,            0)) \
  ((equipType_id,      size_t,             0,            (EquipTypes, id) ))

PT_DECLARE_VO(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)

/**
 * Provides an interface to the database to handle storage of Item Templates.
 */
class ItemTemplatesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)

  PT_DECLARE_CreateTable(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)

public:
  ItemTemplatesTable(Database* db);

  PT_DECLARE_DropTable(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)

  PT_DECLARE_Insert(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)
  PT_DECLARE_GetAll(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)
  PT_DECLARE_GetSingle(ItemTemplatesTable, DB_TABLE_ITEMTEMPLATES, DB_TABLE_ITEMTEMPLATES_FIELDS)
};

#endif //TABLE_ITEMTEMPLATES_H
