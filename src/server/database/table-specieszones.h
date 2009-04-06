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

#ifndef TABLE_SPECIESZONES_H
#define TABLE_SPECIESZONES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIESZONES SpeciesZones
#define DB_TABLE_SPECIESZONES_FIELDS \
  ((species_id,         size_t,             PT_PrimaryKeyS,  (Species, id) )) \
  ((zone_id,            size_t,             PT_PrimaryKey,   (Zones, id) )) \
  ((maxPopulation,      size_t,             0,                0 )) \

PT_DECLARE_VO(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesZonesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)

  PT_DECLARE_CreateTable(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)

public:
  SpeciesZonesTable(Database* db);

  PT_DECLARE_DropTable(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)

  PT_DECLARE_Insert(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)
  //PT_DECLARE_Delete(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)
  //PT_DECLARE_GetAll(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)

  PT_DECLARE_Get(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)
  //PT_DECLARE_GetSingle(SpeciesZonesTable, DB_TABLE_SPECIESZONES, DB_TABLE_SPECIESZONES_FIELDS)
};

#endif //TABLE_SPECIESZONES_H
