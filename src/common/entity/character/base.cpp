/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "base.h"

BasesFactory::BasesFactory(TableManager* db)
  : db(db)
{
}

BasesFactory::~BasesFactory()
{
}

size_t BasesFactory::GetID(const std::string& name) const
{
  std::map<std::string, size_t>::const_iterator it = names.find(name);
  if (it == names.end())
    throw PT_EX(BaseNotFound()) << BaseNameInfo(name);
  return it->second;
}

const std::string& BasesFactory::GetName(size_t id) const
{
  std::map<size_t, std::string>::const_iterator it = ids.find(id);
  if (it == ids.end())
    throw PT_EX(BaseNotFound());
  return it->second;
}

void BasesFactory::Add(size_t id, const std::string& name)
{
  names[name] = id;
  ids[id] = name;
}

