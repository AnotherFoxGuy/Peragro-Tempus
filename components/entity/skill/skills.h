/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef COMPONENT_SKILLS_H
#define COMPONENT_SKILLS_H

#include <csutil/scf_implementation.h>
#include <csutil/refarr.h>

#include <vector>
#include <string>

#include "include/client/component/entity/stat/stats.h"
#include "include/client/component/componenttmpl.h"

#include "client/component/stdcomponentimp.h"

#include "common/entity/character/base.h"

struct iObjectRegistry;
struct iEvent;
struct iPcDefaultCamera;

namespace Client
{
  namespace Entity
  {
    class Entity;
  }
}

namespace PT
{
  namespace Events
  {
    struct EventHandlerCallback;
  }
} //PT namespace

//-----------------------------------------------------------------------------
class Skills : public Bases<3, float>
{
public:
  Skills(SkillsFactory* fact, Entity* entity, TableManager* db)
    : Bases<3, float>("Skills", fact, entity, db) {}

  virtual void LoadFromDB() {}
  virtual void SaveToDB() {}
};

class SkillsFactory : public BasesFactory
{
private:
  virtual void LoadFromDB() {}

public:
  SkillsFactory(TableManager* db) : BasesFactory(db) {}
  virtual ~SkillsFactory() {}

  boost::shared_ptr<Skills> Create(Entity* entity)
  {
    boost::shared_ptr<Skills> skills(new Skills(this, entity, db));
    return skills;
  }

  using BasesFactory::Add;
};
//-----------------------------------------------------------------------------

PT_DECLARE_COMPONENTFACTORY (Skills)

class ComponentSkills : public scfImplementationExt1<ComponentSkills, ComponentCommon, iSkills>
{
private:
  iObjectRegistry* object_reg;
  PointerLibrary* pointerlib;
  bool listRecieved;

  Client::Entity::Entity* entity;
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  bool UpdateSkill(iEvent& ev);
  bool List(iEvent& ev);
  bool AddSkill(iEvent& ev);

  // Bases
  boost::shared_ptr<SkillsFactory> fact;
  boost::shared_ptr<Skills> skills;

public:
    ComponentSkills (iObjectRegistry*);
    virtual ~ComponentSkills();

    virtual bool Initialize (PointerLibrary*, Client::Entity::Entity*);

    virtual size_t GetSkillLevel(const std::string& name);
};

#endif // COMPONENT_SKILLS_H
