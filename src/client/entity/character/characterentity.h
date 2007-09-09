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

#include "client/entity/ptentity.h"

#include "client/entity/character/equipment/equipment.h"

class PtCharacterEntity : public PtEntity
{
private:
  Equipment* equipment;

protected:
  PtCharacterEntity(EntityType type);
  virtual ~PtCharacterEntity(){delete equipment;}
  unsigned int maxStamina;
  unsigned int currentStamina;

public:
  void Move(MovementData* movement);
  bool MoveTo(MoveToData* moveTo);
  void DrUpdate(DrUpdateData* drupdate);
  void Teleport(csVector3 pos, csString sector);
  unsigned int getCurrentStamina() { return currentStamina; }
  unsigned int getMaxStamina() { return maxStamina; }
  void setCurrentStamina(unsigned int x);
  void setMaxStamina(unsigned int x) { maxStamina = x; }

  Equipment* GetEquipment() {return equipment;}
};

#endif // PTCHARACTERENTITY_H
