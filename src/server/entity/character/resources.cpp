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


#include "resources.h"

#include <algorithm>

#include "common/network/serialiser.h"
#include "common/network/resourcemessages.h"
#include "server/network/connection.h"
#include "server/network/networkhelper.h"

#include "../entity.h"

#include "server/server.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-resourcetypes.h"
#include "src/server/database/table-resources.h"

#include "common/util/printhelper.h"

#include "server/entity/entitymanager.h"
#include "server/entity/entity.h"
#include "server/entity/character/character.h"

//==[ Resource ]=============================================================
Resources::Resource::Resource(Resources* resources, size_t id, int val)
  : resources(resources), id(id), value(0.0f), registered(false)
{
  Set(val, false);
}

Resources::Resource::~Resource()
{
  UnRegister();
}

void Resources::Resource::Register()
{
  if (!registered)
  {
    registered = true;
    resources->fact->Register(this);
  }
}

void Resources::Resource::UnRegister()
{
  if (registered)
  {
    registered = false;
    resources->fact->UnRegister(this);
  }
}

void Resources::Resource::SendUpdate()
{
  ResourceUpdateMessage msg;
  msg.setEntityId(resources->entity->GetId());
  msg.setResourceId(id);
  msg.setValue(Get());
  msg.setMaxValue(GetMax());
  ByteStream bs;
  msg.serialise(&bs);
  NetworkHelper::sendMessage(resources->entity, bs);

  printf("Send ResourceUpdateMessage: Resource: %"SIZET" value: %d(%.0f) to %s.\n", id, Get(), value, resources->entity->GetName().c_str());
}

int Resources::Resource::Get() const
{
  return (int)value;
}

void Resources::Resource::Set(float value, bool update)
{
  bool changed = Get() != (int)value;

  float max = (float)GetMax();
  this->value = std::min<float>(value, max);
  if (this->value < max)
    Register();

  if (changed)
  {
    if (update) SendUpdate();
    resources->SaveResourceToDB(this);
  }
}

size_t Resources::Resource::GetAbilityLevel() const
{
  size_t level = 0;
  Character* c = dynamic_cast<Character*>(resources->entity);
  if (c)
  {
    const ResourceTypesTableVOp& type = resources->fact->Get(id);
    const std::string& name = Server::getServer()->GetAbilitiesFactory()->GetName(type->abilityType_id);
    level = c->GetAbilities()->GetLevel(name);
  }
  return level;
}

int Resources::Resource::GetMax() const
{
  const ResourceTypesTableVOp& type = resources->fact->Get(id);
  return GetAbilityLevel() * type->multiplier;
}

void Resources::Resource::Regenerate(size_t elapsedTime)
{
  float speed = 1.0f;
  Character* c = dynamic_cast<Character*>(resources->entity);
  if (c)
  {
    if (c->GetState() == Character::StateProne)
      speed = 10.0f; // Ten times as fast
    else if (c->GetState() == Character::StateSitting)
      speed = 2.0f; // Twice as fast
    else if (c->GetState() == Character::StateHustling)
      speed = 0.0f; // No regeneration
    else if (c->GetState() == Character::StateRunning)
      speed = 0.0f; // No regeneration
    else if (c->GetState() == Character::StateSprinting)
      speed = 0.0f; // No regeneration
  }
  /* TODO
    Take hunger into the calculation:
    http://wiki.peragro.org/index.php/Hunger
  */
  // Entity regenerates Resource equal to their Ability every 'multiplier' seconds.
  const ResourceTypesTableVOp& type = resources->fact->Get(id);
  float regen = ( ((float)GetAbilityLevel() / type->multiplier) * ((float)elapsedTime/1000.0f) ) * speed;
  Set(this->value + regen);
}

//==[ Hit Points ]===========================================================
Resources::HitPoints::HitPoints(Resources* resources, size_t id, int value)
: Resources::Resource(resources, id, value)
{
}

void Resources::HitPoints::Set(float value, bool update)
{
/* TODO
  if (value <= 0.0f)
    resources->entity->SetDead(true);
  else if (resources->entity->IsDead())
    resources->entity->SetDead(false);
*/
  Resource::Set(value, update);
}

void Resources::HitPoints::Regenerate(size_t elapsedTime)
{
  // Hit Points recover as Stamina recovers.
  // Do nothing.
}

//==[ Stamina ]==============================================================
Resources::Stamina::Stamina(Resources* resources, size_t id, int value)
: Resources::Resource(resources, id, value)
{
}

int Resources::Stamina::Get() const
{
  // Stamina is never shown as negative.
  return std::max<int>(0, (int)value);
}

void Resources::Stamina::Set(float value, bool update)
{
  //http://wiki.peragro.org/index.php/Talk:Stamina
  if (value >= GetMax())
  {
    if (resources->Get("Hit Points") < resources->GetMax("Hit Points"))
    {
      resources->Add("Hit Points", 1);
      value -= GetMax();
    }
  }
  else if (value <= 0)
  {
    // When stamina is depleted, Hit Points are reduced by 1
    // and current Stamina is "increased" by the Maximum Stamina.
    value += GetMax();
    resources->Sub("Hit Points", 1);
    value -= 0.0001f; // Hack: make sure value != GetMax()
  }

  Resource::Set(value, update);
}

//==[ Resources ]============================================================
Resources::Resources(ResourcesFactory* fact, Entity* entity, TableManager* db)
  : fact(fact), entity(entity),  db(db)
{
}

int Resources::Get(const std::string& name)
{
  return GetResource(name)->Get();
}

int Resources::GetMax(const std::string& name)
{
  return GetResource(name)->GetMax();
}

void Resources::Set(const std::string& name, int value)
{
  Resource* res = GetResource(name);
  res->Set(value);
}

void Resources::Add(const std::string& name, int value)
{
  Resource* res = GetResource(name);
  res->Set(res->Get() + value);
}

void Resources::Sub(const std::string& name, int value)
{
  Resource* res = GetResource(name);
  res->Set(res->Get() - value);
}

boost::shared_ptr<Resources::Resource> Resources::Create(const std::string& name, Resources* r, size_t id, int value)
{
  boost::shared_ptr<Resources::Resource> res;
  if (name == "Stamina")
    res = boost::shared_ptr<Resources::Stamina> (new Stamina(r, id, value));
  else if (name == "Hit Points")
    res = boost::shared_ptr<Resources::HitPoints> (new HitPoints(r, id, value));
  else
    res = boost::shared_ptr<Resources::Resource> (new Resource(r, id, value));

  return res;
}

void Resources::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw PT_EX(InvalidEntity("Entity id NoEntity for resource"));

  ResourcesTable* table = db->Get<ResourcesTable>();
  ResourcesTableVOArray arr = table->Get(entity->GetId());
  ResourcesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    size_t id = (*it)->resourceType_id;
    std::string name = fact->GetName(id);
    resources[id] = Create(name, this, id, (*it)->value);
    printf("Added entity resource %s (%d/%d)\n", name.c_str(), resources[id]->Get(), resources[id]->GetMax());
  }
}

Resources::Resource* Resources::GetResource(const std::string& name)
{
  try
  {
    size_t id = fact->GetID(name);
    ConstIterator it = resources.find(id);
    if (it == resources.end())
    {
      // The Resource wasn't found for this entity(although the type exists!)
      // so let's create it.
      boost::shared_ptr<Resource> r = Create(name, this, id, 0);
      r->Set(r->GetMax());
      resources[id] = r;
      SaveResourceToDB(r.get());
      return r.get();
    }
    return it->second.get();
  }
  catch (ResourceNotFound& ex)
  {
    ex << ResourceNameInfo(name);
    throw;
  }
}

void Resources::SaveResourceToDB(Resource* resource)
{
  ResourcesTable* table = Server::getServer()->GetTableManager()->Get<ResourcesTable>();
  table->Insert(entity->GetId(), resource->GetId(), resource->Get());
}

void Resources::SaveToDB()
{
  //Already taken care of.
}

void Resources::SendAll(Connection* conn)
{
  const std::map<std::string, size_t>& names = fact->GetNames();
  ResourceListMessage list_msg;
  list_msg.setResourcesCount((char)names.size());

  list_msg.setEntityId(entity->GetId());

  std::map<std::string, size_t>::const_iterator it;
  size_t n = 0;
  for (it=names.begin(); it != names.end(); it++)
  {
    list_msg.setResourceId(n, it->second);
    list_msg.setName(n, it->first);
    list_msg.setValue(n, Get(it->first));
    list_msg.setMaxValue(n, GetMax(it->first));
    n++;
  }

  ByteStream bs2;
  list_msg.serialise(&bs2);
  conn->send(bs2);
}

ResourcesFactory::ResourcesFactory(TableManager* db) : db(db)
{
  LoadFromDB();
  this->setInterval(10); // Every second. 10 * 100MS
  start();
}

void ResourcesFactory::LoadFromDB()
{
  ResourceTypesTable* table = db->Get<ResourceTypesTable>();
  ResourceTypesTableVOArray arr = table->GetAll();
  ResourceTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Add(*it);
    printf("Added Resources %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Resources> ResourcesFactory::Create(Entity* entity)
{
  boost::shared_ptr<Resources> resources(new Resources(this, entity, db));
  return resources;
}

void ResourcesFactory::timeOut()
{
  size_t elapsedTime = 1000; //ms

  std::list<Resources::Resource*> toBeUnRegistered;

  std::list<Resources::Resource*>::iterator it = resources.begin();
  for ( ; it != resources.end(); it++)
  {
    Resources::Resource* res = *it;
    if (res->Get() < res->GetMax())
    {
      res->Regenerate(elapsedTime);
    }
    else
      toBeUnRegistered.push_back(res);
  }

  std::list<Resources::Resource*>::iterator it2 = toBeUnRegistered.begin();
  for ( ; it2 != toBeUnRegistered.end(); it2++)
    (*it2)->UnRegister();
}

void ResourcesFactory::Register(Resources::Resource* resource)
{
  UnRegister(resource); // Make sure there are no doubles!
  resources.push_back(resource);
}

void ResourcesFactory::UnRegister(Resources::Resource* resource)
{
  resources.remove(resource);
}
