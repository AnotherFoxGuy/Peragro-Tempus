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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "server/entity/itementity.h"

class TableManager;
class Entity;
class ResourcesFactory;

class Resources
{
private:
  ResourcesFactory* fact;
  Entity* entity;
  TableManager* db;

  std::map<size_t, boost::shared_ptr<ItemEntity> > equipment;
  typedef std::map<size_t, boost::shared_ptr<ItemEntity> >::iterator Iterator;

private:
  void SaveResourceToDB(size_t resourceId);

public:
  Resources(ResourcesFactory* fact, Entity* entity, TableManager* db);

  void LoadFromDB();
  void SaveToDB();
};

class ResourcesFactory
{
public:
  class Exception
  {
  };

private:
  std::map<std::string, size_t> slots;
  std::map<size_t, std::string> ids;

protected:
  TableManager* db;
  void LoadFromDB();
  void Add(size_t id, const std::string& name)
  {
    slots[name] = id;
    ids[id] = name;
  }

public:
  ResourcesFactory(TableManager* db);
  virtual ~ResourcesFactory() {}

  size_t GetSlotID(const std::string& name) const
  {
    std::map<std::string, size_t>::const_iterator it = slots.find(name);
    if (it == slots.end())
      throw Exception();
    return it->second;
  }

  const std::string& GetSlotName(size_t id) const
  {
    std::map<size_t, std::string>::const_iterator it = ids.find(id);
    if (it == ids.end())
      throw Exception();
    return it->second;
  }

  boost::shared_ptr<Resources> Create(Entity* entity);
};

#endif // RESOURCES_H
