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

#include "size.h"

namespace PT
{
  namespace Common
  {
    namespace Inventory
    {
      class Inventory;
      struct Size;

      class Object
      {
      public:
        enum ObjectType
        {
          Bag = 0,
          Item,
          Spell,
          Action
        };

      protected:
        Inventory* parent;
        unsigned int objectId;
        ObjectType objectType;
        std::string objectIcon;
        std::string objectDescription;
        Size objectSize;

      public:
        Object(ObjectType type) : objectType(type), parent(0), objectSize(1,1) {};
        virtual ~Object();

        /**
         * Get the inventory this object belongs to.
         * @return A pointer to the Inventory instance.
         */
        Inventory* GetParent() const;
        virtual void SetParent(Inventory* p) { parent = p; }

        virtual void SetId(unsigned int type) { objectId = type; }
        virtual unsigned int GetId() const { return objectId; }

        virtual void SetDescription(const std::string& description) { objectDescription = description; }
        virtual const std::string& GetDescription() const { return objectDescription; }

        virtual void SetIcon(const std::string& icon) { objectIcon = icon; }
        virtual const std::string& GetIcon() const { return objectIcon; }

        ObjectType GetType() const { return objectType; }

        virtual void SetSize(const Size& size) { objectSize = size; }
        virtual const Size& GetSize() const { return objectSize; }

        virtual bool Execute() = 0;
        virtual bool Execute(const std::string& action) = 0;

      };

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_OBJECT_H
