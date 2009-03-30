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

#ifndef CLIENT_RESOURCES_H
#define CLIENT_RESOURCES_H

#include <csutil/scf_implementation.h>
#include <csutil/refarr.h>
#include "common/event/eventhandler.h"

#include <stdio.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

//#include "common/entity/character/base.h"

struct iObjectRegistry;
struct iEvent;

namespace PT
{
  namespace Entity
  {
    class Entity;
    class ResourcesFactory;

    class Resources
    {
    private:
      class Resource
      {
      public:
        Resource(size_t id, float value, float maxValue);
        virtual ~Resource() {}
        size_t GetId() { return id; }
        virtual float Get() const;
        virtual void Set(float value, float maxValue = -1);
        virtual float GetMax() const;
      protected:
        size_t id;
        float value;
        float maxValue;
      };

    private:
      csRefArray<PT::Events::EventHandlerCallback> eventHandlers;
      bool List(iEvent& ev);
      bool Add(iEvent& ev);
      bool Update(iEvent& ev);

      std::map<size_t, boost::shared_ptr<Resource> > resources;
      typedef std::map<size_t, boost::shared_ptr<Resource> >::iterator Iterator;
      typedef std::map<size_t, boost::shared_ptr<Resource> >::const_iterator ConstIterator;
      Resource* GetResource(const std::string& name);
      void Set(const std::string& name, float value, float maxValue = -1);

    private:
      ResourcesFactory* fact;
      Entity* entity;

    public:
      Resources(ResourcesFactory* fact, Entity* entity);

      float Get(const std::string& name);
      float GetMax(const std::string& name);
    };

    class ResourcesFactory
    {
    public:
      class Exception
      {
      };
    public:
      ResourcesFactory();
      virtual ~ResourcesFactory() {}

      bool listRecieved;

      boost::shared_ptr<Resources> Create(Entity* entity);
      size_t GetID(const std::string& name) const;
      const std::string& GetName(size_t id) const;
      void Add(size_t id, const std::string& name);

    private:
      std::map<std::string, size_t> names;
      std::map<size_t, std::string> ids;
    };

  } // namespace Entity
} //  namespace PT

#endif // CLIENT_RESOURCES_H
