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

#ifndef AI_H
#define AI_H

#include <string>

#include "common/util/ptstring.h"
#include "common/util/timer.h"

class NpcEntity;

class AI : protected Timer
{
private:
  static std::string idle;
  static std::string stray;
  static std::string guard;
  static std::string path;

protected:
  std::string name;
  virtual void think() = 0;
  bool paused;

public:
  virtual ~AI() {}

  const std::string& GetName() { return name; }

  void pause(bool pause) { paused = pause; }
  bool isPaused(void) { return paused; }

  static AI* createAI(const std::string& ai_name, NpcEntity* npc);

  virtual void LoadFromDB() = 0;
  virtual void SaveToDB() = 0;
};

#endif // AI_H
