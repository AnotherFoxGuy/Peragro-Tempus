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

#include "creaturespawner.h"

#include <wfmath/stream.h>

#include "server/entity/entitymanager.h"
#include "server/entity/npcentity.h"

#include "server/species/speciesmanager.h"

//#include "common/util/thread.h"
//#include "common/util/sleep.h"
#include "common/util/timer.h"
#include "common/util/printhelper.h"
#include "server/server.h"

CreatureSpawner::CreatureSpawner()
{ 
  this->setInterval(10); 

/*
  //TODO: Remove me.
  boost::shared_ptr<NpcEntity> npc = Server::getServer()->GetSpeciesManager()->CreateNPCFromSpecies(2);
  npc->SaveToDB();
  npcentities.push_back(npc);
  */
}

void CreatureSpawner::CheckEntity(boost::shared_ptr<NpcEntity> npc)
{
/*
  if (npc->IsDeath())
  {
    //TODO: 'reset' the entity and reuse
    Server* server = Server::getServer();
    server->GetSpeciesManager()->Initialize(npc, npc->GetSpecies());
  }
*/
}

void CreatureSpawner::timeOut()
{
  mutex.lock();
  std::vector<boost::weak_ptr<NpcEntity> >::iterator it;
  for (it = npcentities.begin(); it != npcentities.end(); it++)
  {
    if (!it->lock()) { npcentities.erase(it); continue; }
    CheckEntity(it->lock());
  }
  mutex.unlock();

  //Remove me
  //Server* server = Server::getServer();
  //std::string pos = WFMath::ToString(server->GetSpeciesManager()->GetRandomPosition(2));
  //printf("BLAAAH %s\n", pos.c_str());
}