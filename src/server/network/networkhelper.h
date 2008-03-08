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

class GenericMessage;
class User;
class Character;
class Entity;
class PcEntity;
class ByteStream;

class NetworkHelper
{
public:
  static const User* getUser(GenericMessage* msg);
  static const Entity* getEntity(GenericMessage* msg);
  static const PcEntity* getPcEntity(GenericMessage* msg);
  static const Character* getCharacter(GenericMessage* msg);

  static void sendMessage(const Character*, const ByteStream& bs);
  static void sendMessage(const PcEntity*, const ByteStream& bs);
  static void sendMessage(const Entity*, const ByteStream& bs);
  static void sendMessage(const User*, const ByteStream& bs);

  static void localcast(const ByteStream& bs, const Entity* entity);
  static void broadcast(const ByteStream& bs);
};

#endif // NETWORKHELPER_H
