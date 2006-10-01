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

#ifndef _QUESTHANDLER_H_
#define _QUESTHANDLER_H_

#include "common/network/nwtypes.h"

#include "common/network/questmessages.h"

class Client;

class QuestHandler : public MessageHandler
{
private:
  Client* client;
  GUIManager* guimanager;

public:
  QuestHandler(Client* client)
  : client(client)
  {
  }

  char getType() { return MESSAGES::QUEST; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::QUEST) assert("wrong message type");
    char id = msg->getMsgId();

    if (id ==  QUEST::NPCDIALOG) handleNpcDialog(msg);
  }

  void handleNpcDialog(GenericMessage* msg);
};

#endif // _QUESTHANDLER_H_
