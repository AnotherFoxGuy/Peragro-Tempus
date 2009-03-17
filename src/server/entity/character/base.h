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

#ifndef BASE_H
#define BASE_H

#include <string>
#include <map>

class TableManager;
class Entity;
class BasesFactory;

class Exception
{
};

template<size_t multiplier, typename T, size_t startXp = 0>
class Bases
{
private:
  std::string type;
protected:
  BasesFactory* fact;
  Entity* entity;
  TableManager* db;

  class Base
  {
  private:
    size_t id;
    T xp;
    bool dirty;
  public:
    Base(size_t id, T xp) : id(id), xp(xp), dirty(false) {}
    T Get() { return xp; }
    void Set(T xp) { dirty = true; Base::xp = xp; }
    bool IsDirty() { return dirty; }
    void SetDirty(bool dirty) { Base::dirty = dirty; }
  };

  std::map<size_t, boost::shared_ptr<Base> > bases;

  size_t GetLevel(T xp) const
  {
    if (multiplier == 1) return xp;
    size_t level = 0
    while (xp >= (level+1) * multiplier)
    {
      xp -= ((level+1) * multiplier);
      level++;
    }
    return level;
  }

  Base* GetBase(const std::string& name)
  {
    try
    {
      size_t id = fact->GetID(name);
      std::map<size_t, boost::shared_ptr<Base> >::const_iterator it = bases.find(id);
      if (it == bases.end())
      {
        // The base wasn't found for this entity(although the type exists!)
        // so let's create it.
        boost::shared_ptr<Base> b(new Base(id, 0));
        bases[id] = b;
        b->Set(startXp); // Make it dirty!
        this->SaveToDB();
        return b.get();
      }
      return it->second.get();
    }
    catch (Exception)
    {
      printf("No %s with name '%s'", type.c_str(), name.c_str());
      throw Exception();
    }
  }

public:
  Bases(const std::string& type, BasesFactory* fact, Entity* entity, TableManager* db) : type(type), fact(fact), entity(entity),  db(db) {}
  virtual ~Bases() {}

  T Get(const std::string& name)
  {
    return GetBase(name)->Get();
  }

  void Set(const std::string& name, T xp)
  {
    GetBase(name)->Set(xp);
  }

  void Add(const std::string& name, T xp)
  {
    GetBase(name)->Set(GetBase(name)->Get() + xp);
  }

  void Sub(const std::string& name, T xp)
  {
    GetBase(name)->Set(GetBase(name)->Get() - xp);
  }

  size_t GetLevel(const std::string& name)
  { 
    return GetLevel(GetBase(name)->Get());
  }

  virtual void LoadFromDB() = 0;
  virtual void SaveToDB() = 0;
};

class BasesFactory
{
private:
  std::map<std::string, size_t> names;
  std::map<size_t, std::string> ids;

protected:
  TableManager* db;
  virtual void LoadFromDB() = 0;
  void Add(size_t id, const std::string& name)
  {
    names[name] = id;
    ids[id] = name;
  }

public:
  BasesFactory(TableManager* db) : db(db) {}
  virtual ~BasesFactory() {}

  size_t GetID(const std::string& name) const
  {
    std::map<std::string, size_t>::const_iterator it = names.find(name);
    if (it == names.end())
      throw Exception();
    return it->second;
  }

  const std::string& GetName(size_t id) const
  {
    std::map<size_t, std::string>::const_iterator it = ids.find(id);
    if (it == ids.end())
      throw Exception();
    return it->second;
  }

  //boost::shared_ptr<Bases> Create(Entity* entity);
};

#endif // BASE_H
