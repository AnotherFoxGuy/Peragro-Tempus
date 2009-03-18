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

#ifndef TABLE_ENTITYCHANNELS_H
#define TABLE_ENTITYCHANNELS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ENTITYCHANNELS EntityChannels
#define DB_TABLE_ENTITYCHANNELS_FIELDS \
  ((entity_id,         size_t,             1,            (Entities, id) )) \
  ((channel_id,        size_t,             0,            (Channels, id) ))

PT_DECLARE_VO(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)

/**
 * Provides an interface to the database to handle storage of the list of
 * chat channels that each entity has joined.
 */
class EntityChannelsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)

  PT_DECLARE_CreateTable(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)

public:
  EntityChannelsTable(Database* db);

  PT_DECLARE_DropTable(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)

  PT_DECLARE_Insert(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)
  PT_DECLARE_GetAll(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)

  //PT_DECLARE_Get(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)
  //PT_DECLARE_GetSingle(EntityChannelsTable, DB_TABLE_ENTITYCHANNELS, DB_TABLE_ENTITYCHANNELS_FIELDS)
};

#endif //TABLE_ENTITYCHANNELS_H
