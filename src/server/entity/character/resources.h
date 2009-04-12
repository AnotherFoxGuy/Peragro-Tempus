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
#include <list>
#include <boost/shared_ptr.hpp>

#include "src/server/database/table-resourcetypes.h"

#include "common/util/timer.h"
#include "common/util/exception.h"

class TableManager;
class Entity;
class ResourcesFactory;
class Connection;

class Resources
{
private:
  class Resource
  {
  public:
    Resource(Resources* resources, size_t id, int val);
    virtual ~Resource();
    size_t GetId() { return id; }
    virtual int Get() const;
    virtual void Set(float val, bool update = true);
    virtual int GetMax() const;
    virtual void Regenerate(size_t elapsedTime);
  protected:
    Resources* resources;
    size_t id;
    float value; // Float value for regeneration.
    bool registered;
    virtual void Register();
    virtual void UnRegister();
    virtual void SendUpdate();
    size_t GetAbilityLevel() const;

    friend class ResourcesFactory;
  };

  class HitPoints : public Resource
  {
  public:
    HitPoints(Resources* resources, size_t id, int value);
    virtual void Set(float val, bool update = true);
    virtual void Regenerate(size_t elapsedTime);
  };

  class Stamina : public Resource
  {
  public:
    Stamina(Resources* resources, size_t id, int value);
    virtual int Get() const;
    virtual void Set(float val, bool update = true);
  };

  friend class Resource;
  friend class ResourcesFactory;

private:
  ResourcesFactory* fact;
  Entity* entity;
  TableManager* db;

  std::map<size_t, boost::shared_ptr<Resource> > resources;
  typedef std::map<size_t, boost::shared_ptr<Resource> >::iterator Iterator;
  typedef std::map<size_t, boost::shared_ptr<Resource> >::const_iterator ConstIterator;

private:
  boost::shared_ptr<Resource> Create(const std::string& name, Resources* r, size_t id, int value);
  Resource* GetResource(const std::string& name);
  void SaveResourceToDB(Resource* resource);

public:
  Resources(ResourcesFactory* fact, Entity* entity, TableManager* db);

  int Get(const std::string& name);
  int GetMax(const std::string& name);
  void Set(const std::string& name, int value);
  void Add(const std::string& name, int value);
  void Sub(const std::string& name, int value);

  void LoadFromDB();
  void SaveToDB();

  void SendAll(Connection* conn);
};

PT_DEFINE_EXCEPTION_WHAT(ResourceNotFound, "Resource not found");
PT_DEFINE_ERRORINFO(ResourceName, std::string);
PT_DEFINE_ERRORINFO(ResourceId, size_t);

class ResourcesFactory : public Timer
{
private:
  std::map<std::string, size_t> names;
  std::map<size_t, ResourceTypesTableVOp> ids;

  void timeOut();

private:
  std::list<Resources::Resource*> resources;

protected:
  TableManager* db;
  void LoadFromDB();
  void Add(const ResourceTypesTableVOp& type)
  {
    names[type->name] = type->id;
    ids[type->id] = type;
  }

public:
  ResourcesFactory(TableManager* db);
  virtual ~ResourcesFactory() {}

  const std::map<std::string, size_t>& GetNames() { return names; }

  const ResourceTypesTableVOp& Get(size_t id) const
  {
    std::map<size_t, ResourceTypesTableVOp>::const_iterator it = ids.find(id);
    if (it == ids.end())
      throw PT_EX(ResourceNotFound()) << ResourceIdInfo(id);
    return it->second;
  }

  size_t GetID(const std::string& name) const
  {
    std::map<std::string, size_t>::const_iterator it = names.find(name);
    if (it == names.end())
      throw PT_EX(ResourceNotFound()) << ResourceNameInfo(name);
    return it->second;
  }

  const std::string& GetName(size_t id) const
  {
    std::map<size_t, ResourceTypesTableVOp>::const_iterator it = ids.find(id);
    if (it == ids.end())
      throw PT_EX(ResourceNotFound()) << ResourceIdInfo(id);
    return it->second->name;
  }

  void Register(Resources::Resource* resource);
  void UnRegister(Resources::Resource* resource);

  boost::shared_ptr<Resources> Create(Entity* entity);
};

#endif // RESOURCES_H
