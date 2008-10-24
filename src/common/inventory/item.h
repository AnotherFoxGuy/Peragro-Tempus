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

#ifndef PT_COMMON_ITEM_H
#define PT_COMMON_ITEM_H

#include <string>

#include "object.h"

namespace PT
{
  namespace Common
  {
    namespace Inventory
    {

      class Item : public Object
      {
      protected:
        unsigned int variationId;
        unsigned int price;

      public:
        Item() : Object(ObjectType::Item), variationId(-1) {};
        virtual ~Item();

        virtual void SetVariationId(unsigned int variation) { variationId = variation; }
        virtual unsigned int GetVariationId() const { return variationId; }

        virtual void SetPrice(unsigned int value) { price = value; }
        virtual unsigned int GetPrice() const { return price; }

        virtual bool Execute() {};
        virtual bool Execute(const std::string& action) {};

      };

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_OBJECT_H
