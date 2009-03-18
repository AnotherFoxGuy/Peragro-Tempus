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

#ifndef TABLE_CHANNELTYPES_H
#define TABLE_CHANNELTYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_CHANNELTYPES ChannelTypes
#define DB_TABLE_CHANNELTYPES_FIELDS \
  ((id,                 size_t,             1,            0)) \
  ((create_id,          size_t,             0,            (JoinTypes, id) )) \
  ((admin_id,           size_t,             0,            (JoinTypes, id) )) \
  ((join_id,            size_t,             0,            (JoinTypes, id) )) \
  ((invite_id,          size_t,             0,            (JoinTypes, id) )) \
  ((visibilityChannel,  bool,               0,            0)) \
  ((visibilityMembers,  bool,               0,            0))

PT_DECLARE_VO(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ChannelTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)

  PT_DECLARE_CreateTable(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)

public:
  ChannelTypesTable(Database* db);

  PT_DECLARE_DropTable(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)

  PT_DECLARE_Insert(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)
  PT_DECLARE_GetAll(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)

  //PT_DECLARE_Get(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)
  //PT_DECLARE_GetSingle(ChannelTypesTable, DB_TABLE_CHANNELTYPES, DB_TABLE_CHANNELTYPES_FIELDS)
};

#endif //TABLE_CHANNELTYPES_H
