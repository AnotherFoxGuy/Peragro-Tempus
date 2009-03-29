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

#ifndef PATH_AI_H
#define PATH_AI_H

#include "ai.h"

#include <wfmath/point.h>

class PathAI : public AI
{
private:
  boost::weak_ptr<NpcEntity> npc;

  WFMath::Point<3>* points;
  unsigned int* intervals;
  unsigned int pointCount;

  unsigned int currentPoint;

  float speed;

protected:
  virtual void timeOut();

public:
  PathAI(boost::shared_ptr<NpcEntity> npc) : npc(npc), points(0), intervals(0) {}
  virtual ~PathAI();

  virtual void think();

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

#endif // PATH_AI_H
