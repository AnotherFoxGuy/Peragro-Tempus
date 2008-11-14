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

#include "permission.h"

#include "server/server.h"
#include "common/database/database.h"
#include "server/database/tables.h"
#include "server/database/table-permissions.h"

PermissionList::PermissionList(unsigned int user_id) : user_id(user_id)
{
  Tables* tables = Server::getServer()->getTables();
  Array<PermissionsTableVO*> vos = tables->getPermissionsTable()->getUserAll(user_id);
  for (size_t i = 0; i < vos.getCount(); i++)
  {
    if (vos.get(i)->permissionid < PT_PERMISSION_COUNT)
    {
      levels[vos.get(i)->permissionid] = vos.get(i)->permissionlevel;
    }
  }
}

void PermissionList::setLevel(Permission::Type type, unsigned char level)
{
  levels[type] = level;
  Tables* tables = Server::getServer()->getTables();
  tables->getPermissionsTable()->insert(user_id, type, level);
}
