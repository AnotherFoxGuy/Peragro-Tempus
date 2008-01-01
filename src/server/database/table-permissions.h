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

#ifndef TABLE_PERMISSIONS_H
#define TABLE_PERMISSIONS_H

class Database;
class ResultSet;

#include "common/util/ptstring.h"

class PermissionsTableVO
{
public:
  unsigned int userid;
  unsigned int permissionid;
  unsigned char permissionlevel;
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

  void insert(unsigned int userid, unsigned int permissionid, unsigned char level);
  void remove(unsigned int userid, unsigned int permissionid);

  Array<PermissionsTableVO*> getUserAll(unsigned int userid);
  PermissionsTableVO* get(unsigned int userid, unsigned int permissionid);
};

#endif // TABLE_PERMISSIONS_H
