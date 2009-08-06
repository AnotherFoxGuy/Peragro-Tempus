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

#include "client/network/questhandler.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/pointer/pointer.h"

void QuestHandler::handleNpcDialog(GenericMessage* msg)
{
  NpcDialogMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("quest.dialog.npc.add", true);
    pEvent->Add("dialogId", pmsg.getDialogId());
    pEvent->Add("dialogText", pmsg.getDialogText());
    csRef<iEvent> list = evmgr->CreateEvent("answersList", true);
    for (unsigned char i = 0; i < pmsg.getAnswersCount(); i++)
    {
      std::stringstream itemName;
      itemName << "answers" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("answerId", pmsg.getAnswerId(i));
      item->Add("answerText", pmsg.getAnswerText(i));
      list->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("answersList", list);

    evmgr->AddEvent(pEvent);
  }

} // end handleNpcDialog

void QuestHandler::handleNpcEndDialog(GenericMessage* msg)
{
  NpcEndDialogMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("quest.dialog.npc.end", true);
    pEvent->Add("npcId", pmsg.getNpcId());

    evmgr->AddEvent(pEvent);
  }

} // end handleNpcEndDialog

