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

#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include <boost/shared_ptr.hpp>

#include "common/util/array.h"

class GenericMessage;
class User;
class Character;
class Entity;
class PcEntity;
class ByteStream;

class NetworkHelper
{
public:
  static boost::shared_ptr<User> getUser(GenericMessage* msg);
  static Entity* getEntity(GenericMessage* msg);

  static void sendMessage(Character*, const ByteStream& bs);
  static void sendMessage(Entity*, const ByteStream& bs);
  static void sendMessage(const User*, const ByteStream& bs);

  static void localcast(const ByteStream& bs, Entity* entity);
  static void distancecast(const ByteStream& bs, Entity* entity, unsigned dist);
  static void broadcast(const ByteStream& bs);

  /* returns an array of players associated with the given player/channelname
     combination.  */
  static Array<PcEntity*> getUserList(PcEntity*, const char* channel);
};

#endif // NETWORKHELPER_H
