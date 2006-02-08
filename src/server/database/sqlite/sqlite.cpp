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

#include <stdio.h>
#include <stdarg.h>

#include "sqlite.h"

#include "ext/sqlite/sqlite3.h"

#include "common/util/sleep.h"

dbSQLite::dbSQLite()
{
  const char* database = "test_db.sqlite";

  int rc = sqlite3_open(database, &db);
  if( rc )
  {
    printf("Can't open database: %s\n", sqlite3_errmsg(db));
  }

  begin();

  init();
}

dbSQLite::~dbSQLite()
{
  sqlite3_close(db);
}

ResultSet* dbSQLite::query(const char* query)
{
  while (updates.getCount() > 0)
    pt_sleep(10);

  //printf("SQLite not implemented yet, dumping query: %s\n", query);
  char *zErrMsg = 0;
  ResultSet* result = new ResultSet;
  mutex.lock();
  int rc = sqlite3_exec(db, query, callback, result, &zErrMsg);
  mutex.unlock();
  if( rc!=SQLITE_OK )
  {
    printf("SQL error: %s\n", zErrMsg);
    return 0;
  }
  return result;
}

void dbSQLite::update(const char* query, ...)
{
  va_list args;
  va_start (args, query);
  char* escaped_query = sqlite3_vmprintf(query, args);
  va_end (args);

  mutex.lock();
  updates.add(escaped_query);
  mutex.unlock();
}

void dbSQLite::Run()
{
  if (updates.getCount() == 0)
    pt_sleep(10);
  else
    update();
}

void dbSQLite::update()
{
  //printf("SQLite not implemented yet, dumping query: %s\n", query);
  char *zErrMsg = 0;
  mutex.lock();
  char* query = updates.get(0);
  int rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  updates.remove(0);
  sqlite3_free(query);
  mutex.unlock();
  if( rc!=SQLITE_OK )
  {
    printf("SQL error: %s\n", zErrMsg);
  }
}

int dbSQLite::getLastInsertedId()
{
  return sqlite3_last_insert_rowid(db);
}

int dbSQLite::callback(void *rs, int cols, char **colArg, char **colName)
{
  if (!rs) return 0;

  ResultSet* result = (ResultSet*) rs;

  size_t row = result->GetRowCount();

  for(size_t i=0; i<size_t(cols); i++)
  {
    result->AddData(row, i, colArg[i]);
  }
  return 0;
}
