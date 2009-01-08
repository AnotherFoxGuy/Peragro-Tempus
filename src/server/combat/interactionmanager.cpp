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

InteractionManager::InteractionManager()
{
}

InteractionManager::~InteractionManager()
{
}

void Run()
{
  // TODO this is not good, need to check if we are still running etc.
  while (1)
  {
    Interaction *interaction = NULL;
    // caller must free allocation
    interaction = interactionQueue->GetInteraction();
    if (!interaction) {
      // No character have any outstanding interactions.
      sleep(SLEEP);
      continue;
    }
    performInteraction(interaction);
    free(interaction);
  }
}

// TODO support our own char as target? In that case don't mess up locking
bool InteractionManager::NormalAttack(Interaction *interaction)
{
  unsigned int targetID = 0;
  const Entity* entity = NULL;
  const Character* c_char = NULL;

  ptScopedMonitorable<Character>
    lockedAttacker(interaction->character);

  targetID = lockedAttacker->getTargetID();

  if (targetID == 0) {
    printf(IM "targetID 0, make sure that's not a legal one\n");
    return false;
  }

  targetEntity = Server::getServer()->getEntityManager()->findById(targetID);

  if (!targetEntity) {
    printf(IM "Invalid target\n");
    return false;
  }

  if (targetEntity->getType() == Entity::PlayerEntityType)
  {
    c_char = targetEntity->getPlayerEntity()->getCharacter();
  }
  else if (targetEntity->getType() == Entity::NPCEntityType)
  {
    c_char = targetEntity->getNpcEntity()->getCharacter();
  }
  else
  {
    // Should not happen, but do not crash on release build, since fake message
    // could bring down the server then.
    printf(IM "Target neither player nor npc\n");
    return false;
  }

  ptScopedMonitorable<Character> lockedTarget(c_char);


  if (!TargetAttackable(lockedAttacker, lockedTarget)) {
    return false;
  }
}

bool InteractionManager::PerformInteraction(Interaction *interaction)
{
  if (!interaction) {
    return false;
  }

  switch(interaction->actionID) {
    case NORMAL_ATTACK:
      NormalAttack(interaction);
      break;
    case HEAL:
      break;
  }
}

bool InteractionManager::TargetAttackable(Character* lockedAttacker,
                                          Character* lockedTarget)
{
  const float maxAttackDistance = 0;
  const float distance = 0;
  const float attackerRotation = 0;
  const float attackAngle = 0;
  const float angleDiff = 0;
  static const float allowdAngle = PT_PI * 0.3f;



  const PtVector3 attackerPos(attacker->getEntity()->getPos());
  const PtVector3 targetPos(target->getEntity()->getPos());

  maxAttackDistance = GetReach(lockedAttacker);
  distance = Distance(attackerPos, targetPos);

  printf(IM "attackerPos: %s, targetPos: %s, distance %f, "
    "maxAttackDistance: %f\n", attackerPos.ToString().c_str(),
    targetPos.ToString().c_str(), distance, maxAttackDistance);

  if (distance > maxAttackDistance)
  {
    // Target out of reach
    printf(IM "Target out of range\n");
    return false;
  }

  PtVector3 difference = attackerPos - targetPos;

  if (fabs(difference.y) > GetHightDeviation(lockedAttacker))
  {
    printf(IM "To much height difference\n");
    return false;
  }

  attackerRotation =
    PT::Math::NormalizeAngle(attacker->getEntity()->getRotation());

  if (difference.x == 0.0f) difference.x = PT_EPSILON;
  attackAngle =
    PT::Math::NormalizeAngle(atan2(difference.x, difference.z));

  angleDiff = attackAngle - attackerRotation;

  printf("attackerRotation: %f, attackAngle: %f angleDiff: %f allowedAngle: %f\n",
    attackerRotation, attackAngle, angleDiff, allowedAngle);

  if (fabs(angleDiff) > allowedAngle)
  {
    printf(IM "Not facing the target\n");
    return false;
  }

  return true;
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
  Interaction interaction = new Interaction();

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

  interaction->actionID = actionID;
  interaction->character = sourceEntity->getCharacter();

  // Caller must alloc interaction
  interaction = interactionQueue->SetInteraction(interaction);
}

