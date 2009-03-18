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

#ifndef TABLE_CHANNELS_H
#define TABLE_CHANNELS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_CHANNELS Channels
#define DB_TABLE_CHANNELS_FIELDS \
  ((id,                 size_t,             1,            0)) \
  ((name,               std::string,        0,            0)) \
  ((channelType_id,     size_t,             0,            (ChannelTypes, id) ))

PT_DECLARE_VO(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)

/**
 * Provides an interface to the database to handle storage of definitions
 * of chat channelspaces.
 */
class ChannelsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)

  PT_DECLARE_CreateTable(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)

public:
  ChannelsTable(Database* db);

  PT_DECLARE_DropTable(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)

  PT_DECLARE_Insert(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)
  PT_DECLARE_GetAll(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)

  //PT_DECLARE_Get(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)
  //PT_DECLARE_GetSingle(ChannelsTable, DB_TABLE_CHANNELS, DB_TABLE_CHANNELS_FIELDS)
};

#endif //TABLE_CHANNELS_H
