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

#ifndef _AI_H_
#define _AI_H_

#include "common/util/ptstring.h"
#include "common/util/timer.h"

class NpcEntity;

class AI : protected Timer
{
private:
  static ptString idle;
  static ptString stray;
  static ptString guard;
  static ptString path;

protected:
  virtual void think() = 0;
  bool paused;

public:
  virtual ~AI() {}

  virtual void setNPC(NpcEntity* npc) = 0;

  void pause(bool pause) { paused = pause; }
  bool isPaused(void) { return paused; }

  static AI* createAI(ptString ai_name);
};

#endif // _AI_H_
