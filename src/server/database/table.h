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

#ifndef _TABLE_H_
#define _TABLE_H_

class Database;

/**
 * Basic abstract class for all database table classes.
 */
class Table
{
protected:
  /// Pointer to the database.
  Database* db;
  /**
   * Basic constructor.
   * @param db The database pointer.
   */
  Table(Database* db) : db(db) {}
  /**
   * Virtual destructor
   */
  virtual ~Table() {}

public:
  /**
   * Virtual function definition for how to create a table.
   */
  virtual void createTable() = 0;
  /**
   * Virtual function definition for how to delete a table.
   */
  virtual void dropTable() = 0;
};

#endif // _TABLE_H_
