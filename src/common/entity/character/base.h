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

#include <stdio.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

class TableManager;
class Entity;
class BasesFactory;

class Exception
{
};

template<size_t multiplier, typename T, size_t startXp = 0>
class Bases
{
public:
  Bases(const std::string& type, BasesFactory* fact, Entity* entity,
    TableManager* db);
  virtual ~Bases();

  T Get(const std::string& name);
  void Set(const std::string& name, T xp);
  void Add(const std::string& name, T xp);
  void Sub(const std::string& name, T xp);
  size_t GetLevel(const std::string& name);

  virtual void LoadFromDB() = 0;
  virtual void SaveToDB() = 0;

protected:
  class Base
  {
  public:
    Base(size_t id, T xp);
    T Get();
    void Set(T xp);
    bool IsDirty();
    void SetDirty(bool dirty);
  private:
    size_t id;
    T xp;
    bool dirty;
  };

  size_t GetLevel(T xp) const;
  Base* GetBase(const std::string& name);

  std::map<size_t, boost::shared_ptr<Base> > bases;

  BasesFactory* fact;
  Entity* entity;
  TableManager* db;

private:
  std::string type;
};

class BasesFactory
{
public:
  BasesFactory(TableManager* db);
  virtual ~BasesFactory();

  size_t GetID(const std::string& name) const;

  const std::string& GetName(size_t id) const;

  //boost::shared_ptr<Bases> Create(Entity* entity);

protected:
  TableManager* db;
  virtual void LoadFromDB() = 0;
  void Add(size_t id, const std::string& name);

private:
  std::map<std::string, size_t> names;
  std::map<size_t, std::string> ids;
};

template<size_t multiplier, typename T, size_t startXp>
inline Bases<multiplier, T, startXp>::Base::Base(size_t id, T xp)
  : id(id), xp(xp), dirty(false) {}

template<size_t multiplier, typename T, size_t startXp>
inline T Bases<multiplier, T, startXp>::Base::Get()
{ return xp; }

template<size_t multiplier, typename T, size_t startXp>
inline void Bases<multiplier, T, startXp>::Base::Set(T xp)
{ dirty = true; Base::xp = xp; }

template<size_t multiplier, typename T, size_t startXp>
inline bool Bases<multiplier, T, startXp>::Base::IsDirty()
{ return dirty; }

template<size_t multiplier, typename T, size_t startXp>
inline void Bases<multiplier, T, startXp>::Base::SetDirty(bool dirty)
{ Base::dirty = dirty; }

template<size_t multiplier, typename T, size_t startXp>
inline Bases<multiplier, T, startXp>::Bases(const std::string& type, BasesFactory* fact, Entity* entity,
  TableManager* db)
  : fact(fact), entity(entity),  db(db), type(type) {}

template<size_t multiplier, typename T, size_t startXp>
inline Bases<multiplier, T, startXp>::~Bases()
{}

template<size_t multiplier, typename T, size_t startXp>
inline T Bases<multiplier, T, startXp>::Get(const std::string& name)
{ return GetBase(name)->Get(); }

template<size_t multiplier, typename T, size_t startXp>
inline void Bases<multiplier, T, startXp>::Set(const std::string& name, T xp)
{ GetBase(name)->Set(xp); }

template<size_t multiplier, typename T, size_t startXp>
inline void Bases<multiplier, T, startXp>::Add(const std::string& name, T xp)
{ Base* base = GetBase(name); base->Set(base->Get() + xp); }

template<size_t multiplier, typename T, size_t startXp>
inline void Bases<multiplier, T, startXp>::Sub(const std::string& name, T xp)
{ Base* base = GetBase(name); base->Set(base->Get() - xp); }

template<size_t multiplier, typename T, size_t startXp>
inline size_t Bases<multiplier, T, startXp>::GetLevel(const std::string& name)
{ return GetLevel(GetBase(name)->Get()); }

template<size_t multiplier, typename T, size_t startXp>
size_t Bases<multiplier, T, startXp>::GetLevel(T xp) const
{
  //@todo Maybe this could be done in a faster way?
  size_t nextLevel = 1;
  size_t nextLevelXp;
  while (xp >= (nextLevelXp = nextLevel * multiplier))
  {
    xp -= nextLevelXp;
    ++nextLevel;
  }
  return nextLevel - 1;
}

template<size_t multiplier, typename T, size_t startXp>
typename Bases<multiplier, T, startXp>::Base*
  Bases<multiplier, T, startXp>::GetBase(const std::string& name)
{
  try
  {
    size_t id = fact->GetID(name);
    typename std::map<size_t, boost::shared_ptr<Base> >::const_iterator it
      = bases.find(id);
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
  catch (Exception&)
  {
    printf("No %s with name '%s'!\n", type.c_str(), name.c_str());
    throw;
  }
}

#endif // BASE_H
