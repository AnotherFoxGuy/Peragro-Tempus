/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef PTCHARACTERENTITY_H
#define PTCHARACTERENTITY_H

#include "client/entity/entity.h"

#include "client/entity/character/equipment/equipment.h"

namespace PT
{
  namespace Entity
  {
    class CharacterEntity : public Entity
    {
    private:
      Equipment equipment;

    protected:
      CharacterEntity() : equipment(this) {}
      CharacterEntity(const Events::EntityAddEvent& ev);

      virtual ~CharacterEntity() {}

      unsigned int maxStamina;
      unsigned int currentStamina;

    public:
      void Move(MovementData* movement);
      bool MoveTo(MoveToData* moveTo);
      void DrUpdate(DrUpdateData* drupdate);
      void Teleport(csVector3 pos, csString sector);

      unsigned int GetCurrentStamina() const { return currentStamina; }
      void SetCurrentStamina(unsigned int x);
      unsigned int GetMaxStamina() const { return maxStamina; }
      void SetMaxStamina(unsigned int x) { maxStamina = x; }

      Equipment& GetEquipment() {return equipment;}
      void Pose(unsigned int poseId);
    };
  }
}

#endif // PTCHARACTERENTITY_H
