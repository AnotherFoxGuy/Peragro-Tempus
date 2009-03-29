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

#include <cssysdef.h>
#include "components/entity/skills/skills.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>
#include <iengine/sector.h>
#include <iutil/object.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/entity/entity.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
PT_IMPLEMENT_COMPONENTFACTORY (Skills, "peragro.entity.skills")

ComponentSkills::ComponentSkills(iObjectRegistry* object_reg) :
  scfImplementationType (this, object_reg), listRecieved(false), fact(new SkillsFactory(0))
{
}

ComponentSkills::~ComponentSkills()
{
}

bool ComponentSkills::Initialize (PointerLibrary* pl, Client::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  skills = fact->Create(0);

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  PT_REGISTER_LISTENER_ENTITY(ComponentSkills, List, "entity.skill.list", true)
  PT_REGISTER_LISTENER_ENTITY(ComponentSkills, UpdateSkill, "entity.skill.update", true)

  return true;
} // end Initialize()

bool ComponentSkills::UpdateSkill(iEvent& ev)
{
  if (!listRecieved)
  {
    pointerlib->getReporter()->Report(PT::Error, "ComponentSkills failed updating skill!");
    return true;
  }

  using namespace PT::Events;
  using namespace PT::Entity;

  unsigned int skillId = -1;
  ev.Retrieve("skillId", skillId);

  float XP = 0.0f;
  ev.Retrieve("xp", XP);

  skills->Set(fact->GetName(skillId), XP);

  return true;
} // end UpdateSkill()

bool ComponentSkills::List(iEvent& ev)
{
  using namespace PT::Events;

  csRef<iEvent> list;
  if (ev.Retrieve("skillsList", list) == csEventErrNone)
  {
    csRef<iEventAttributeIterator> skills = list->GetAttributeIterator();
    while (skills->HasNext())
    {
      csRef<iEvent> skill; list->Retrieve(skills->Next(), skill);
      AddSkill(*skill);
    } // end while
    listRecieved = true;
  }
  else
    pointerlib->getReporter()->Report(PT::Error, "ComponentSkills failed to get skillsList!");

  return true;
} // end List()

bool ComponentSkills::AddSkill(iEvent& ev)
{
  using namespace PT::Events;

  unsigned int skillId;
  const char* tmpname;

  ev.Retrieve("skillId", skillId);
  ev.Retrieve("name", tmpname);
  std::string name = tmpname;

  fact->add(skillId, name);

  float XP;
  ev.Retrieve("xp", XP);
  
  skills->Set(name, XP);

  return true;
} // end AddSkill()

size_t ComponentSkills::GetSkillLevel(const std::string& name)
{
  if (!listRecieved) 
  {
    pointerlib->getReporter()->Report(PT::Error, "ComponentSkills failed to get skill %s!", name.c_str());
    return 0;
  }

  return skills->GetLevel(name);
} // end GetSkillLevel()
