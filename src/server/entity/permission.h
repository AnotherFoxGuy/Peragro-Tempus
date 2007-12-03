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

#ifndef _PERMISSION_H_
#define _PERMISSION_H_

#define PT_PERMISSION_COUNT 1

namespace Permission
{
  enum Type
  {
    Admin=0
  };
}

class PermissionList
{
  int user_id;
  unsigned char levels[PT_PERMISSION_COUNT];

public:
  PermissionList(int user_id);
  virtual ~PermissionList() {}

  size_t getLevel(Permission::Type type) const { return levels[type]; }
  void setLevel(Permission::Type type, size_t level);
};

#endif // _PERMISSION_H_
