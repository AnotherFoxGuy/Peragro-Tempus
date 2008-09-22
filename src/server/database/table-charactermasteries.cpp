/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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
#include <stdlib.h>

#include "database.h"
#include "table-charactermasteries.h"

/**
 *
 * Constructor that will create the table that stores a characters mastery
 * stats if it doesn't already exist, will also store the database pointer for
 * later availability.
 *
 * @param db The database to use for storing character skills.
 *
 */
CharacterMasteriesTable::CharacterMasteriesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from charactermasteries;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

/**
 *
 * Method that will create the table that holds a characters masteries.
 *
 * The table will consist the id of the character, the id of the mastery and the
 * level of the mastery.
 *
 */
void CharacterMasteriesTable::createTable()
{
  printf("Creating Table charactermasteries...\n");
  db->update("create table charactermasteries ("
    "characterID INTEGER, "
    "masteryType INTEGER, "
    "level INTEGER, "
    "PRIMARY KEY (characterID, masteryType) );");
}

/**
 *
 * Method that will insert or update a characters specific mastery level.
 *
 * @param characterID The ID of the character to store the mastery level for.
 * @param masteryType The type of mastery.
 * @param level The level of the mastery.
 *
 */
void CharacterMasteriesTable::insert(unsigned int characterID,
                                     unsigned  int masteryType,
                                     int level)
{
  db->update("insert or replace into characterskill (characterID, masteryType)"\
             " values ('%u','%u',%d);", characterID, masteryType, level);
}

/**
 *
 * Method that will set a characters specific mastery level.
 *
 * @param characterID The ID of the character to store the mastery level for.
 * @param masteryType The type of mastery.
 * @param level The level of the mastery.
 *
 */
void CharacterMasteriesTable::set(unsigned int characterID,
                                  unsigned int masteryType,
                                  int level)
{
  insert(characterID, masteryType, level);
}

/**
 *
 * Function that will return the level of the mastery that matches the character
 * id and mastery type.
 *
 * @param characterID The ID of the character to store the mastery level for.
 * @param masteryType The type of mastery.
 *
 * @return The level of the requested skill.
 *
 */
int CharacterMasteriesTable::get(int characterID, unsigned int masteryType)
{
  ResultSet* rs = db->query("select * from charactermasteries where " \
                            "characterID = '%u' and masteryType = '%u';",
                            characterID, masteryType);

  int level = 0;

  if (rs->GetRowCount() == 1)
  {
    level = atoi(rs->GetData(0,0).c_str());
  }
  else if (rs->GetRowCount() == 0)
  {
    level = 0;
  }
  else
  {
    printf("DB inconsistency: multiple equal masteries in the charactermasteries!");
    // This is really bad, lets panic here.
    assert(0);
  }

  delete rs;

  return level;
}

/**
 *
 * Method that will drop the entire charactermasteries table from the database.
 *
 */
void CharacterMasteriesTable::dropTable()
{
  db->update("drop table charactermasteries;");
}

/**
 *
 * Function that will return an array containing all masteries for a certain
 * character.
 *
 * @param characterID The id of the chracter for which to find the masteries.
 *
 * @return An array containing CharacterMasteryTableVO objects.
 *
 */
Array<CharacterMasteryTableVO*> CharacterMasteriesTable::getAllMasteries(
                                                    unsigned int characterID)
{
  ResultSet* rs = db->query("select masteryType from charactermasteries " \
                            "where characterID = '%u';", characterID);

  Array<CharacterMasteryTableVO*> masteries = parseMultiResultSet(rs);
  delete rs;
  return masteries;
}

/**
 *
 * Function that will return an array containing all masteries for a certain
 * character.
 *
 * @param The result set containing what the database returned.
 *
 * @return An array containing CharacterMasteryTableVO objects.
 *
 */
Array<CharacterMasteryTableVO*> CharacterMasteriesTable::parseMultiResultSet(
                                                    ResultSet* rs)
{
  Array<CharacterMasteryTableVO*> arr;
  for (size_t i = 0; rs && i < rs->GetRowCount(); i++)
  {
    CharacterMasteryTableVO* obj = parseSingleResultSet(rs, i);
    arr.add(obj);
  }
  return arr;
}
/**
 *
 * Function that will return a single mastery table from the result set.
 *
 * @param rs The result set containing what the database returned.
 * @param row The row to select from the result set.
 *
 * @return A single CharacterMasteryTableVO object.
 *
 */
CharacterMasteryTableVO* CharacterMasteriesTable::parseSingleResultSet(
                                             ResultSet* rs, size_t row)
{
  CharacterMasteryTableVO* vo = new CharacterMasteryTableVO();
  vo->masteryType = (unsigned int) atoi(rs->GetData(row,0).c_str());
  vo->masteryLevel = atoi(rs->GetData(row,0).c_str());
  return vo;
}

