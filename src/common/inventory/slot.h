/*
  Copyright (C) 2005 - 2007 Development Team of Peragro Tempus

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

#ifndef PT_COMMON_SLOT_H
#define PT_COMMON_SLOT_H


namespace PT
{
  namespace Common
  {
    namespace Inventory
    {
      class Object;
      class Inventory;

      class Slot
      {
      protected:
        Inventory* parent;
        PositionRef position;
        Object* slotContents;

      public:
        Slot(Inventory* parent, PositionRef position);
        virtual ~Slot();

        /**
         * Get the inventory this slot belongs to.
         * @return A pointer to the Inventory instance.
         */
        Inventory* GetParent() const;

        /**
         * Get position of this slot.
         * @return A PositionRef.
         */
        const PositionRef& GetPosition() const;

        /**
         * Set the contents of this slot.
         * @param object  The object to set this slot to.
         * @return void.
         */
        void SetContents(Object* object);

        /**
         * Clear the contents of this slot.
         * @return void.
         */
        void Clear();

        /**
         * Get the contents this slot holds.
         * @return A pointer to the Object instance.
         */
        Object* GetContents() const;

        /**
         * Returns if this slot has contents.
         * @return True if so, false if not.
         */
        bool HasContents() const;
      };

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_SLOT_H
