/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#define IM "InteractionManager: "
#define SLEEP 10

void Run()
{
  // TODO this is not good, need to check if we are still running etc.
  while (1)
  {
    Interaction *interaction = NULL;
    interaction = interactionQueue->GetInteraction();
    if (!interaction) {
      // No character have any outstanding interactions.
      sleep(SLEEP);
      continue;
    }
    performInteraction(interaction);
  }
}

bool InteractionManager::PerformInteraction(Interaction *interaction)
{
  if (!interaction) {
    return false;
  }

  switch(interaction->actionID) {
    case NORMAL_ATTACK:
      break;
  }
}

bool InteractionManager::SelectTarget(const PcEntity *sourceEntity,
                                      unsigned int targetID)
{
  printf(IM "Got selection request, target: %d'n", targetID);
  
  if (!sourceEntity || !sourceEntity->getCharacter())
  {
    // Invalid source.
    printf(IM "Invalid source.\n");
    return false;
  }

  ptScopedMonitorable<Character>
    lockedSource(sourceEntity->getCharacter());

  if (!lockedSource)
  {
    printf(IM "Unable to lock source.\n");
    return false;
  }

  lockedSource->SetTargetID(targetID);
}

const Character* InteractionManager::GetTargetCharacter(
                    Character *lockedCharacter)
{
  return Server::getServer()->getEntityManager()->
                              findById(lockedCharacter->GetTargetID());

}

bool InteractionManager::QueueAction(const PcEntity *sourceEntity,
                                     unsigned int actionID)
{
  printf(IM "Got selection request, target: %d'n", targetID);
  
  if (!sourceEntity || !sourceEntity->getCharacter())
  {
    // Invalid source.
    printf(IM "Invalid source.\n");
    return false;
  }

  ptScopedMonitorable<Character>
    lockedSource(sourceEntity->getCharacter());

  if (!lockedSource)
  {
    printf(IM "Unable to lock source.\n");
    return false;
  }

  lockedSource->SetTargetID(targetID);
  
}

