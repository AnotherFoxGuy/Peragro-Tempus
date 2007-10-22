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

#ifndef _TABLE_USERS_H_
#define _TABLE_USERS_H_

#include "table.h"

class Database;
class User;
class ptString;

/**
 * Provides an interface to the database for handle storage of Users.
 */
class UsersTable : public Table
{
public:
  /**
   * Constructor for the UsersTable.
   * If no user table is found in the database, the createTable function
   * will be called in order to populate the database.
   * @param db A pointer to the database.
   */
  UsersTable(Database* db);
  /**
   * Creates a table in the database that will store users.
   */
  void createTable();
  /**
   * Insert a user into the database.
   * @param name The name of the user.
   * @param pwhash TODO what exactly is this
   */
  void insert(ptString name, const char* pwhash);
  /**
   * Removes all users currently stored in the database.
   */
  void dropTable();
  /**
   * Checks if a user exists or not.
   * @return True if the user exists, false otherwise
   */
  bool existsUser(ptString name);
  /**
   * Does a lookup in the database to find a user.
   * The calller is responsible for freeing the User returned.
   * @param name The name of the user.
   * @return User if found, otherwise 0.
   */
  User* getUser(ptString name);
};

#endif //_TABLE_USERS_H_
