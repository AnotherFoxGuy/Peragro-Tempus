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
#include "resource.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>
#include <iengine/sector.h>
#include <iutil/object.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/entity/entity.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

namespace PT
{
  namespace Entity
  {
    //==[ Resources ]============================================================
    Resources::Resources(ResourcesFactory* fact, Entity* entity)
      : fact(fact), entity(entity)
    {   
    }

    void Resources::Initialize()
    {
      eventHandlers.DeleteAll();
      using namespace PT::Events;
      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      PT_REGISTER_LISTENER_ENTITY(Resources, Update, "entity.resource.update", true)
      PT_REGISTER_LISTENER_ENTITY(Resources, List, "entity.resource.list", true)
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
          boost::shared_ptr<Resource> r(new Resource(id, 0, 0));
          resources[id] = r;
          return r.get();
        }
        return it->second.get();
      }
      catch (ResourcesFactory::Exception&)
      {
        printf("No resource with name '%s'!\n", name.c_str());
        throw ResourcesFactory::Exception();
      }
    }

    int Resources::Get(const std::string& name)
    {
      return GetResource(name)->Get();
    }

    int Resources::GetMax(const std::string& name)
    {
      return GetResource(name)->GetMax();
    }

    int Resources::GetOld(const std::string& name)
    {
      return GetResource(name)->GetOld();
    }

    void Resources::Set(const std::string& name, int value, int maxValue)
    {
      Resource* res = GetResource(name); 
      res->Set(value, maxValue); 
    }

    bool Resources::Update(iEvent& ev)
    {
      if (!fact->listRecieved)
      {
        Report(PT::Error, "ComponentSkills failed updating skill!");
        return true;
      }

      using namespace PT::Events;
      using namespace PT::Entity;

      unsigned int resourceId = -1;
      ev.Retrieve("resourceId", resourceId);

      int value = -1;
      ev.Retrieve("value", value);
      int maxValue = -1;
      ev.Retrieve("maxValue", maxValue);

      Set(fact->GetName(resourceId), value, maxValue);

      return true;
    } // end UpdateSkill()

    bool Resources::List(iEvent& ev)
    {
      using namespace PT::Events;

      csRef<iEvent> list;
      if (ev.Retrieve("resourcesList", list) == csEventErrNone)
      {
        csRef<iEventAttributeIterator> evs = list->GetAttributeIterator();
        while (evs->HasNext())
        {
          csRef<iEvent> e; list->Retrieve(evs->Next(), e);
          Add(*e);
        } // end while
        fact->listRecieved = true;
      }
      else
        Report(PT::Error, "ResourcesFactory failed to get resourcesList!");

      return true;
    } // end List()

    bool Resources::Add(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int resourceId;
      const char* tmpname;

      ev.Retrieve("resourceId", resourceId);
      ev.Retrieve("name", tmpname);
      std::string name = tmpname;

      fact->Add(resourceId, name);

      int value = -1;
      ev.Retrieve("value", value);
      int maxValue = -1;
      ev.Retrieve("maxValue", maxValue);

      Set(name, value, maxValue);

      return true;
    } // end AddSkill()

    //==[ Resource ]=============================================================
    Resources::Resource::Resource(size_t id, int value, int maxValue) 
      : id(id), value(value), maxValue(maxValue), oldValue(value)
    {
    }

    int Resources::Resource::Get() const
    {
      return (int)value;
    }

    void Resources::Resource::Set(int value, int maxValue)
    { 
      this->oldValue = this->value;
      if (maxValue > 0)
      {
        this->maxValue = maxValue;
      }
      this->value = std::min<int>((int)value, GetMax());
    }

    int Resources::Resource::GetMax() const
    {
      return maxValue;
    }

    int Resources::Resource::GetOld() /*const*/
    {
      int v = this->oldValue;
      this->oldValue = Get();
      return v;
    }

    //==[ ResourcesFactory ]============================================================
    ResourcesFactory::ResourcesFactory() 
      : listRecieved(false)
    {
    }

    size_t ResourcesFactory::GetID(const std::string& name) const
    {
      std::map<std::string, size_t>::const_iterator it = names.find(name);
      if (it == names.end())
        throw Exception();
      return it->second;
    }

    const std::string& ResourcesFactory::GetName(size_t id) const
    {
      std::map<size_t, std::string>::const_iterator it = ids.find(id);
      if (it == ids.end())
        throw Exception();
      return it->second;
    }

    void ResourcesFactory::Add(size_t id, const std::string& name)
    {
      names[name] = id;
      ids[id] = name;
    }

    boost::shared_ptr<Resources> ResourcesFactory::Create(Entity* entity)
    {
      boost::shared_ptr<Resources> resources(new Resources(this, entity));
      return resources;
    }

    

  } // namespace Entity
} //  namespace PT
