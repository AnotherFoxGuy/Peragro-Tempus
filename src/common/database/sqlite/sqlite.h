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

#include <queue>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

#include "common/database/database.h"

class ResultSet;
struct sqlite3;
struct sqlite3_stmt;

class DbSQLite : public Database
{
public:
  /// Ctor.
  DbSQLite(const char* database);
  /// Dtor.
  ~DbSQLite();

  /// Query the database.
  virtual ResultSet* Query(const char*, ...);
  /// Queue a database update.
  virtual void Update(const char*, ...);
  /// Get the id of the last inserted row.
  virtual size_t GetLastInsertedId() const;
  /// Apply queued updates.
  virtual void Run();

private:
  /**
   * Callback needed for sqlite; Gets called once for each row.
   * @param ptr same pointer as given to sqlite3_exec, preferably the result object
   * @param cols number of colums for this row.
   * @param colArg array of strings representing the value ôf all collums in this row.
   * @param colArg array of strings representing the name of all collums.
   * @return 0.
   */
  static int Callback(void *rs, int cols, char **colArg, char **colName);

  /// Apply a database update.
  void ApplyUpdate(char* query);

  typedef boost::mutex MutexType;
  typedef boost::unique_lock<MutexType> LockType;
  MutexType updatesMutex;
  boost::condition_variable updatesQueued;
  boost::condition_variable updatesEmpty;
  std::queue<char*> updates;
  sqlite3* db;
};

#endif // _PT_SQLITE_H_
