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

#ifndef COMMON_OBJECT_H
#define COMMON_OBJECT_H

#include <string>
#include <iostream>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>

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
      std::string name;
      ObjectType objectType;
      std::string fileName;
      std::string meshFactName;
      std::string objectDescription;
      WFMath::AxisBox<2> objectSize;

    public:
      friend inline std::ostream& operator<< (std::ostream&, const Object&);

    public:
      Object(ObjectType type) : parent(0), objectType(type), objectSize(WFMath::Point<2>(0),WFMath::Point<2>(1))
      {
      }

      virtual ~Object() {}

      /**
       * Get the inventory this object belongs to.
       * @return A pointer to the Inventory instance.
       */
      Inventory* GetParent() const { return parent; }
      virtual void SetParent(Inventory* p) { parent = p; }
/*
      virtual void SetId(unsigned int type) { objectId = type; }
      virtual unsigned int GetId() const { return objectId; }
*/
      virtual void SetObjectName(const std::string& value) { name = value; }
      virtual const std::string& GetObjectName() const { return name; }

      virtual void SetDescription(const std::string& description) { objectDescription = description; }
      virtual const std::string& GetDescription() const { return objectDescription; }

      virtual void SetObjectFileName(const std::string& file) { fileName = file; }
      virtual const std::string& GetObjectFileName() const { return fileName; }

      virtual void SetMeshFactName(const std::string& mesh) { meshFactName = mesh; }
      virtual const std::string& GetMeshFactName() const { return meshFactName; }

      ObjectType GetType() const { return objectType; }

      virtual void SetSize(const WFMath::AxisBox<2>& size) { objectSize = size; }
      virtual const WFMath::AxisBox<2>& GetSize() const { return objectSize; }

      virtual bool Execute() = 0;
      virtual bool Execute(const std::string& action) = 0;

    };

    inline std::ostream& operator<< (std::ostream& os, const Object& i)
    {
      return os << i.GetObjectName();
    }

  } // Inventory namespace
} // Common namespace

#endif // COMMON_OBJECT_H
