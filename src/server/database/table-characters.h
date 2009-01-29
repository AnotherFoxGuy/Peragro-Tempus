/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef TABLE_CHARACTERS_H
#define TABLE_CHARACTERS_H

#include "common/database/table.h"

class Database;
class ResultSet;

class User;
class Mesh;

#include "common/util/ptstring.h"
#include <wfmath/point.h>

class CharactersTableVO
{
public:
  int id;
  ptString name;
  int user;
  unsigned int mesh;
  int race;
  unsigned char hair_r;
  unsigned char hair_g;
  unsigned char hair_b;
  unsigned char skin_r;
  unsigned char skin_g;
  unsigned char skin_b;
  unsigned char decal_r;
  unsigned char decal_g;
  unsigned char decal_b;
  float pos_x;
  float pos_y;
  float pos_z;
  float rotation;
  ptString sector;
};

/**
 * Provides an interface to the database for handle storage of characters.
 */
class CharacterTable : public Table
{
private:
  CharactersTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<CharactersTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  /**
   * Constructor for the CharacterTable.
   * If no character table is found in the database, the createTable function
   * will be called in order to populate the database.
   * @param db A pointer to the database.
   */
  CharacterTable(Database* db);
  /**
   * Creates a table in the database that will store characters.
   */
  void createTable();
  /**
   * Insert a character into the database.
   * @param id The id of the character.
   * @param name The name of the character.
   * @param user_id The id of the user that owns this character.
   * @param mesh The mesh for this character.
   * @param race_id The id this character is based on.
   * @param haircolour The hair color for this character.
   * @param skincolour The skin color for this character.
   * @param decalcolour The decal color for this character.
   * @param pos The location for this character.
   * @param sector The sector in which the character is lcoated.
   */
  void insert(int id, const ptString& name, int user_id, const Mesh* mesh,
              int race_id, unsigned char haircolour[3],
              unsigned char skincolour[3], unsigned char decalcolour[3],
              const WFMath::Point<3>& pos, const ptString& sector);
  /**
   * Returns the highest ID any character has.
   * @return The maximum ID any character has, or zero if no character avaialbe.
   */
  int getMaxId();
  /**
   * Removes all characters currently stored in the database.
   */
  void dropTable();
  /**
   * Deletes a signle character from the database based on its id.
   * @param id The id of the character to be deleted.
   */
  void remove(int id);
  /**
   * Stores updated data for the character.
   * @param pos The new location for the character.
   * @param rot Rotation of the character.
   * @param sector The new sector location.
   * @param char_id The id of the character for which to update the location.
   */
  void update(const WFMath::Point<3>& pos, float rot, const ptString& sector,
              int char_id);
  /**
   * Checks if a character exists in the database based on its name.
   * @param name The name of the character.
   * @return True if the character was found, otherwise false.
   */
  bool existsCharacter(const ptString& name);
  /**
   * Find a character based on user id and character id.
   * The caller is responsible for freeing the returned
   * character.
   * @param id The id of the character.
   * @param user_id The user id that owns the character.
   * @return Returns the character, or 0 if not found.
   */
  CharactersTableVO* findCharacterById(int id, size_t user_id);
  /**
   * Find a character based on character name.
   * The caller is responsible for freeing the returned
   * character.
   * @param name The name of the character.
   * @return Returns the character, or 0 if not found.
   */
  CharactersTableVO* FindCharacterByName(const char* name);
  /**
   * Finds all characters that belongs to a single user.
   * The caller is responsible for deleting all characters in the array.
   * @param user The user to find all characters for.
   * @return The array that will contain all characters that
   * belongs to this user.
   */
  Array<CharactersTableVO*>  getAllCharacters(User* user);
};

#endif // TABLE_CHARACTERS_H
