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

#include <stdarg.h>
#include <iostream>
#include <memory>

#include "sqlite.h"
#include "resultset.h"

#include "ext/sqlite/sqlite3.h"

DbSQLite::DbSQLite(const char* database)
{
  const int rc = sqlite3_open(database, &db);
  if (rc != SQLITE_OK)
  {
    std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
  }
}

DbSQLite::~DbSQLite()
{
  sqlite3_close(db);
}

ResultSet* DbSQLite::Query(const char* query, ...)
{
  va_list args;
  va_start(args, query);
  char* escaped_query = sqlite3_vmprintf(query, args);
  va_end(args);

  std::auto_ptr<SQLITEResultSet> result(new SQLITEResultSet);

  char *errorMsg = 0;
  int rc = SQLITE_OK;
  {
    LockType lock(updatesMutex);
    while (!updates.empty())
    {
      updatesEmpty.wait(lock);
    }

     rc = sqlite3_exec(db, escaped_query, Callback, result.get(), &errorMsg);
  }
  sqlite3_free(escaped_query);

  if (rc != SQLITE_OK)
  {
    std::cout << "SQL error: " << errorMsg << std::endl;
    sqlite3_free(errorMsg);
    return 0;
  }

  return result.release();
}

void DbSQLite::Update(const char* query, ...)
{
  va_list args;
  va_start(args, query);
  char* escaped_query = sqlite3_vmprintf(query, args);
  va_end(args);

  LockType lock(updatesMutex);
  updates.push(escaped_query);
  updatesQueued.notify_one();
}

void DbSQLite::Run()
{
  LockType lock(updatesMutex);
  while (updates.empty())
  {
    updatesQueued.wait(lock);
  }
  do
  {
    char* query = updates.front();
    updates.pop();

    lock.unlock();
    ApplyUpdate(query);
    lock.lock();
  }
  while (!updates.empty());
  updatesEmpty.notify_one();
}

void DbSQLite::ApplyUpdate(char* query)
{
  char* errorMsg = 0;
  const int rc = sqlite3_exec(db, query, Callback, 0, &errorMsg);
  if (rc != SQLITE_OK)
  {
    std::cout << "SQL query: " << query << std::endl;
    std::cout << "SQL error: " << errorMsg << std::endl;
  }
  sqlite3_free(query);
}

size_t DbSQLite::GetLastInsertedId() const
{
  return static_cast<size_t>(sqlite3_last_insert_rowid(db));
}

int DbSQLite::Callback(void *rs, int cols, char **colArg, char ** /*colName*/)
{
  if (!rs) return 0;

  SQLITEResultSet* result = static_cast<SQLITEResultSet*>(rs);

  const size_t row = result->GetRowCount();

  for (size_t i = 0; i < size_t(cols); ++i)
  {
    result->AddData(row, i, colArg[i]);
  }
  return 0;
}
