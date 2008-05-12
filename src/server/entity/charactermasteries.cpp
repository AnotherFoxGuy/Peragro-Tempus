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

#include "server/entity/charactermasteries.h"
#include "server/entity/statmanager.h"
#include "common/network/serialiser.h"
#include "common/network/playermessages.h"
#include "server/network/connection.h"
#include "server/server.h"

/**
 *
 * Find the mastery with a certain id.
 *
 * @param masteryID The id to match against.
 *
 */
CharacterMastery* CharacterMasteries::findEntry(size_t masteryID)
{
  for(unsigned int i=0; i<masteries.getCount(); i++)
  {
    if (masteries.get(i)->masteryID == masteryID)
    {
      return masteries.get(i);
    }
  }
  return 0;
}

/**
 * Will send all the masteries associated with a player.
 *
 * @param conn The connection ot the players client.
 *
 */
void CharacterMasteries::sendAllMasteries(Connection* conn)
{
  MasteriesListMessage masterieslist_msg;
  masterieslist_msg.setMasteriesCount((char)masteries.getCount());
  for (size_t i=0; i<masteries.getCount(); i++)
  {
    masterieslist_msg.setID(int(i),short(masteries.get(i)->masteryID));
    masterieslist_msg.setType(int(i),masteries.get(i)->masteryType);
    masterieslist_msg.setLevel(int(i),masteries.get(i)->masteryLevel);
  }
  ByteStream bs2;
  masterieslist_msg.serialise(&bs2);
  conn->send(bs2);
}
