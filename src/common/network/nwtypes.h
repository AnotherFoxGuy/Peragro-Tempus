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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#ifndef NWTYPES_H
#define NWTYPES_H

#define MSG_NET_VERSION 3695417476

#define MSG_CLIENT_PEER_VERSION 1803529449
#define MSG_SERVER_PEER_VERSION 1679073811
#define MSG_AUTH_PEER_VERSION 2534847773

#define MSG_HANDLER_COUNT 15

namespace MESSAGES
{
  enum HANDLERS
  {
    CONNECTION = 0,
    USER = 1,
    ENTITY = 2,
    CHAT = 3,
    SKILL = 4,
    RESOURCE = 5,
    DOOR = 6,
    QUEST = 7,
    TRADE = 8,
    ENVIRONMENT = 9,
    BOOK = 10,
    ADMIN = 11,
    PLAYER = 12,
    COMBAT = 13,
    AUTH = 14
  };
}

#endif // NWTYPES_H
