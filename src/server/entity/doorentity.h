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

#ifndef DOORENTITY_H
#define DOORENTITY_H

#include <string.h>
#include <time.h>
#include <math.h>

#include "entity.h"

struct DoorsTableVO;

class DoorEntity : public Entity
{
private:
  bool open;
  bool locked;
  std::string animationName;

public:
  DoorEntity() : Entity(Common::Entity::DoorEntityType), open(false), locked(false)
  {
  }

  ~DoorEntity() {}

  void SetOpen(bool open) { this->open = open; }
  bool GetOpen() const { return open; }

  void SetLocked(bool locked) { this->locked = locked; }
  bool GetLocked() const { return locked; }

  const std::string& GetAnimationName() const { return animationName; }
  void SetAnimationName(const std::string& value) { animationName = value; }

  void operator<< (DoorsTableVO* vo);

};

#endif // DOORENTITY_H
