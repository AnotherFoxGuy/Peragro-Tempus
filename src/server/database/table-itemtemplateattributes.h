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

#ifndef TABLE_ITEMTEMPLATEATTRIBUTES_H
#define TABLE_ITEMTEMPLATEATTRIBUTES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ITEMTEMPLATEATTRIBUTES ItemTemplateAttributes
#define DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS \
  ((ItemTemplates_id,  size_t,             PT_PrimaryKeyS,  (ItemTemplates, id) )) \
  ((AttributeTypes_id, size_t,             PT_PrimaryKey,   (AttributeTypes, id) )) \
  ((value,             int,                0,               0))

PT_DECLARE_VO(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ItemTemplateAttributesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)

  PT_DECLARE_CreateTable(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)

public:
  ItemTemplateAttributesTable(Database* db);

  PT_DECLARE_DropTable(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)

  PT_DECLARE_Insert(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)
  //PT_DECLARE_Delete(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)
  //PT_DECLARE_GetAll(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)
  //PT_DECLARE_DeleteAll(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)

  //PT_DECLARE_Get(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)
  //PT_DECLARE_GetSingle(ItemTemplateAttributesTable, DB_TABLE_ITEMTEMPLATEATTRIBUTES, DB_TABLE_ITEMTEMPLATEATTRIBUTES_FIELDS)
};

#endif //TABLE_ITEMTEMPLATEATTRIBUTES_H
