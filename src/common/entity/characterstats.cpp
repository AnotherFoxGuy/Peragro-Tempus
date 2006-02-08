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

#include "characterstats.h"
#include "common/entity/statmanager.h"
#include "common/network/serialiser.h"
#include "common/network/entitymessages.h"
#include "server/network/connection.h"
#include "server/server.h"

void CharacterStats::sendAllStats(Connection* conn)
{
  CharacterStatListMessage statlist_msg;
  statlist_msg.setStatCount((char)entries.getCount());
  for (size_t i=0; i<entries.getCount(); i++)
  {
    statlist_msg.setStatId(int(i),entries.get(i)->stat_id);
    Stat* stat = Server::getServer()->getStatManager()->findById(entries.get(i)->stat_id);
    assert(stat);
    statlist_msg.setName(int(i),stat->getName());
    statlist_msg.setStatLevel(int(i),entries.get(i)->level);
  }
  ByteStream bs2;
  statlist_msg.serialise(&bs2);
  conn->send(bs2);
}
