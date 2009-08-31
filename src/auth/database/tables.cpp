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

#include "common/database/database.h"
#include "tables.h"
#include "table-users.h"
#include "table-servers.h"

Tables::Tables()
{
  userstable = 0;
  serverstable = 0;
}

void Tables::init(Database* db)
{
  db->Update("begin transaction");
  userstable = new UsersTable(db);
  serverstable = new ServersTable(db);
  db->Update("commit");
}

Tables::~Tables()
{
  delete userstable;
  delete serverstable;
}
