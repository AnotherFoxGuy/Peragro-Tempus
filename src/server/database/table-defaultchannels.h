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

#ifndef TABLE_DEFAULTCHANNELS_H
#define TABLE_DEFAULTCHANNELS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_DEFAULTCHANNELS DefaultChannels
#define DB_TABLE_DEFAULTCHANNELS_FIELDS \
  ((name,               std::string,        1,            0)) \

PT_DECLARE_VO(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)

/**
 * Provides an interface to the database to handle storage of the list of
 * chat channels to join by default.
 */
class DefaultChannelsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)

  PT_DECLARE_CreateTable(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)

public:
  DefaultChannelsTable(Database* db);

  PT_DECLARE_DropTable(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)

  PT_DECLARE_Insert(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)
  PT_DECLARE_GetAll(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)

  //PT_DECLARE_Get(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)
  //PT_DECLARE_GetSingle(DefaultChannelsTable, DB_TABLE_DEFAULTCHANNELS, DB_TABLE_DEFAULTCHANNELS_FIELDS)
};

#endif //TABLE_DEFAULTCHANNELS_H
