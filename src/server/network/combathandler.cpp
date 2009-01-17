
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

#include "network.h"
#include "networkhelper.h"

#include "server/entity/entitymanager.h"
#include "server/network/combathandler.h"
#include "server/combat/interactionmanager.h"

/**
 * Handler function for attack request messages.
 * @param msg The attack request message, not yet deserialized.
 */
void CombatHandler::handleAttackRequest(GenericMessage* msg)
{
  InteractionManager *interactionManager =
    Server::getServer()->getInteractionManager();
  const PcEntity* ent = NetworkHelper::getPcEntity(msg);
  if (!ent) return;

  ptScopedMonitorable<PcEntity> ent1 (ent);

  AttackRequestMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  // TODO this is slightly wrong.
  interactionManager->QueueInteraction(ent1, 
                                       in_msg.getAttackType(),
                                       in_msg.getTargetID());
}
