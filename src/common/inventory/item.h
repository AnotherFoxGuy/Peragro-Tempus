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

#ifndef COMMON_ITEM_H
#define COMMON_ITEM_H

#include <string>

#include "object.h"

namespace Common
{
  namespace Inventory
  {

    class Item : public Object
    {
    protected:

      unsigned int price;
      float weight;
      std::string equipType;
      std::string equipTypeName;

    public:
      Item() : Object(Object::Item), price(0), weight(0.0f)  {}
      virtual ~Item() {}

      virtual void SetPrice(unsigned int value) { price = value; }
      virtual unsigned int GetPrice() const { return price; }

      virtual void SetWeight(float value) { weight = value; }
      virtual float GetWeight() const { return weight; }

      virtual void SetEquipType(const std::string& value) { equipType = value; }
      virtual const std::string& GetEquipType() const { return equipType; }

      virtual void SetEquipTypeName(const std::string& value) { equipTypeName = value; }
      virtual const std::string& GetEquipTypeName() const { return equipTypeName; }

      virtual bool Execute() { return false; };
      virtual bool Execute(const std::string& action) { return false; };

    };

  } // Inventory namespace
} // Common namespace

#endif // COMMON_ITEM_H
