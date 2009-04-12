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

#ifndef NPCENTITY_H
#define NPCENTITY_H

#include "common/util/ptstring.h"
#include "common/util/monitorable.h"

#include "entity.h"
#include "character/character.h"

#include "server/ai/ai.h"

#include "common/util/exception.h"

PT_DEFINE_EXCEPTION(InvalidNpcEntity);

class NpcEntity : public Character
{
private:
  AI* ai;

  unsigned int dialog_id;

public:
  NpcEntity() : Character(Common::Entity::NPCEntityType)
  {
    isWalking = false;
    ai = 0;
    dialog_id = 0;
  }

  ~NpcEntity()
  {
    delete ai;
  }

  void setAI(AI* ai) { this->ai = ai; }
  AI* getAI() { return ai; }

  void pause(bool pause) { if (ai) ai->pause(pause); }

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

#endif // NPCENTITY_H
