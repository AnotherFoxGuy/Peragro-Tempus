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

#include <stdlib.h>
#include <sstream>

#include "path-ai.h"

#include "server/server.h"
#include "common/database/database.h"
#include "server/database/table-npcaisetting.h"

#include "server/entity/character.h"
#include "server/entity/npcentity.h"

#include "common/geom/ptvector3.h"

PathAI::~PathAI()
{
  delete [] points;
  delete [] intervals;
}

void PathAI::setNPC(NpcEntity* npc)
{
  this->npc = npc;

  // load settings
  int id = npc->getCharacter()->getId();
  NpcAiSettingTable* table =
    Server::getServer()->getTables()->getNpcAiSettingTable();

  pointCount = atoi(*table->getValue(id, ptString("waypoints",9)));
  points = new PtVector3[pointCount];
  intervals = new unsigned int[pointCount];
  for (unsigned int i = 0; i < pointCount; i++)
  {
    std::stringstream strx;
    strx << "point_" << i << "_x";
    points[i].x = (float) atof(*table->getValue(id, ptString::create(strx.str())));
    std::stringstream stry;
    stry << "point_" << i << "_y";
    points[i].y = (float) atof(*table->getValue(id, ptString::create(stry.str())));
    std::stringstream strz;
    strz << "point_" << i << "_z";
    points[i].z = (float) atof(*table->getValue(id, ptString::create(strz.str())));
    std::stringstream stri;
    stri << "interval_" << i;
    intervals[i] = atoi(*table->getValue(id, ptString::create(stri.str())));
  }

  speed = (float) atof(*table->getValue(id, ptString::create("speed")));

  currentPoint = 0;

  setInterval(atoi(*table->getValue(id, ptString::create("start_delay"))));
  start();
}

void PathAI::timeOut()
{
  think();
}

void PathAI::think()
{
  currentPoint = (currentPoint + 1) % pointCount;

  Server::getServer()->moveEntity(npc, points[currentPoint], speed, false);

  setInterval(intervals[currentPoint]);
}
