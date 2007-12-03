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

#ifndef _TABLE_PERMISSIONS_H_
#define _TABLE_PERMISSIONS_H_

class Database;
class ResultSet;

#include "common/util/ptstring.h"

class PermissionsTableVO
{
public:
  int userid;
  int permissionid;
  int permissionlevel;
};

class PermissionsTable
{
private:
  Database* db;

  PermissionsTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  Array<PermissionsTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  PermissionsTable(Database* db);

  void createTable();

  void insert(int userid, int permissionid, unsigned char level);
  void remove(int userid, int permissionid);

  Array<PermissionsTableVO*> getUserAll(int userid);
  PermissionsTableVO* get(int userid, int permissionid);
};

#endif // _TABLE_PERMISSIONS_H_

