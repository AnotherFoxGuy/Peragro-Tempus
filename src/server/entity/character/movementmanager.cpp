/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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


#include "movementmanager.h"

#include <math.h>
#include <algorithm>

#include "common/network/serialiser.h"
#include "common/network/resourcemessages.h"
#include "server/network/connection.h"
#include "server/network/networkhelper.h"

#include "../entity.h"

#include "server/server.h"

#include "common/util/printhelper.h"

#include "server/entity/entitymanager.h"
#include "server/entity/entity.h"
#include "server/entity/character/character.h"

#include "server/combat/progressionmanager.h"


MovementManager::MovementManager()
{
  this->setInterval(10); // Every second. 10 * 100MS
  start();
}

bool MovementManager::Degenerate(boost::shared_ptr<Character> entity, size_t elapsedTime)
{
  /*
  "A hustle expends one point of Stamina per 10 minutes of movement."
  "Running consumes a point of Stamina per minute."
  "A point of Stamina is spent every second the character sprints."
  Stamina is detucted is based on the total 'relative' time
  in movement. Faster speeds add more to the relative time.
  Once the total time is reached, the stamina is detucted.
  */
  switch (entity->GetState())
  {
  case Character::StateProne:
  case Character::StateSitting:
  case Character::StateStanding:
  case Character::StateWalking:
    // TODO: instead of doing a UnRegister and loosing the current time,
    // let the 'idle states' rewind the total time.
    return false;
  case Character::StateHustling: // Per 10 minutes.
    { // Nothing to be done.
    } break;
  case Character::StateRunning: // Per minute.
    {
      //TODO elapsedTime *= 10;
      elapsedTime *= 600; // Some test code.
    } break;
  case Character::StateSprinting: // Per second.
    {
      elapsedTime *= 600;
    } break;
  }

  entities[entity] += elapsedTime;
  size_t current = entities[entity];
  if (current > 600000) // 10 minutes
  {
    entities[entity] = current % 600000;
    int points = current / 600000;
    entity->GetResources()->Sub("Stamina", points);

    // 10% chance of gaining an experience point towards Speed
    // each time a point of Stamina is expended for movement.
    unsigned int xp = ProgressionManager::XPIncrease(points, 10);
    entity->GetAbilities()->Add("Speed", xp);

    //printf("MovementManager::Degenerate %d   %d\n", points, xp);
    /*
    if (entity->GetResources()->Get("Stamina") < entity->GetResources()->GetMax("Stamina")/10) // Below 10%
    {
      // http://wiki.peragro.org/index.php/Stamina
      // TODO: handle fallback to lower speed
    }
    */
  }

  return true;
}

float MovementManager::GetMovementSpeed(boost::shared_ptr<Character> entity)
{
  //http://wiki.peragro.org/index.php/Movement
  float baseSpeed = entity->GetAbilities()->Get("Speed");
  baseSpeed = sqrt(baseSpeed); //square root

  switch (entity->GetState())
  {
  case Character::StateProne: return 0.0f;
  case Character::StateSitting: return 0.0f;
  case Character::StateStanding: return 0.0f;
  case Character::StateWalking: return baseSpeed;
  case Character::StateHustling: return 2*baseSpeed;
  case Character::StateRunning: return 3*baseSpeed;
  case Character::StateSprinting: return 6*baseSpeed;
  }
  return 0.0f;
}

void MovementManager::timeOut()
{
  size_t elapsedTime = 1000; //ms
  EntityList::iterator it;
  for ( it = entities.begin() ; it != entities.end(); )
  {
    if ((!(*it).first.lock()) || !Degenerate((*it).first.lock(), elapsedTime))
      entities.erase(it++);
    else
      it++;
  }
}

void MovementManager::Register(boost::shared_ptr<Character> entity)
{
  EntityList::iterator it = entities.find(entity);
  if (it == entities.end())
    entities[entity] = 0;
}

void MovementManager::UnRegister(boost::weak_ptr<Character> entity)
{
  EntityList::iterator it = entities.find(entity);
  if (it != entities.end())
    entities.erase(it);
}
