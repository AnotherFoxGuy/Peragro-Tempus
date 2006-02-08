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

#ifndef _PT_SQLITE_H_
#define _PT_SQLITE_H_

#include "server/database/database.h"
#include "common/util/array.h"
#include "common/util/mutex.h"
#include "common/util/thread.h"

class ResultSet;
struct sqlite3;

class dbSQLite : public Database, Thread
{
  Mutex mutex;

  Array<char*> updates;

  sqlite3 *db;

  /**Callback needed for sqlite
   * Gets called once for each row.
   * \param ptr same pointer as given to sqlite3_exec, preferably the result object
   * \param cols number of colums for this row.
   * \param colArg array of strings representing the value ôf all collums in this row.
   * \param colArg array of strings representing the name of all collums.
   */
  static int callback(void *rs, int cols, char **colArg, char **colName);
  ResultSet* query(const char*);
  void update();
  void update(const char*, ...);
  int getLastInsertedId();

  void Run();

public:
  dbSQLite();
  ~dbSQLite();
};

#endif // _PT_SQLITE_H_
