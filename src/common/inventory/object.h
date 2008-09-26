/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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

#ifndef PT_COMMON_OBJECT_H
#define PT_COMMON_OBJECT_H

#include <string>

namespace PT
{
  namespace Common
  {
    namespace Inventory
    {
      class Object
      {
      public:
        enum ObjectType
        {
          Bag = 0,
          Item,
          Spell,
          Action,
          Stack
        };

      protected:
        unsigned int objectId;
        ObjectType objectType;
        std::string objectIcon;
        std::string objectDescription;

      public:
        Object(ObjectType type) : objectType(type) {};
        virtual ~Object();

        virtual void SetId(int type) { objectId = type; }
        virtual unsigned int GetId() const { return objectId; }

        virtual void SetDescription(std::string description) { objectDescription = description; }
        virtual std::string GetDescription() const { return objectDescription; }

        virtual void SetIcon(std::string icon) { objectIcon = icon; }
        virtual std::string GetIcon() const { return objectIcon; }

        ObjectType GetType() const { return objectType; }

        virtual bool Execute() = 0;
        virtual bool Execute(const std::string& action) = 0;

      };

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_OBJECT_H
