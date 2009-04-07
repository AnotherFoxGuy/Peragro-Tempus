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

#include "server.h"
#include "entity/character/character.h"
#include "entity/entity.h"
#include "entity/itementity.h"
#include "entity/pcentity.h"
#include "entity/mountentity.h"
#include "entity/npcentity.h"
#include "entity/usermanager.h"
#include "server/entity/entitymanager.h"
#include "common/database/database.h"
#include "server/database/table-entities.h"
#include "server/database/table-characters.h"
#include "server/database/table-doors.h"
#include "common/network/entitymessages.h"
#include "server/network/connection.h"
#include "server/network/networkhelper.h"
#include "common/util/sleep.h"
#include "server/colldet/colldet.h"

Server* Server::server;

void Server::moveEntity(boost::shared_ptr<Character> entity, const WFMath::Point<3>& pos, float speed, bool run)
{
  MoveToMessage response_msg;
  response_msg.setTo(pos);
  response_msg.setFrom(entity->GetPosition());
  response_msg.setSpeed(speed);
  response_msg.setEntityId(entity->GetId());
  response_msg.setRun(run);
  response_msg.setTurn(0); // No continuous rotation
  response_msg.setJump(false);

  entity->walkTo(pos, speed);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::localcast(bs, entity);
}
