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

#include "client/network/network.h"
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/entity/entitymanager.h"

#include "client/event/eventmanager.h"
#include "client/event/stateevent.h"
#include "client/event/regionevent.h"

#include "client/reporter/reporter.h"

void UserHandler::handleLoginResponse(GenericMessage* msg)
{
  Report(PT::Notify, "Received LoginResponse.");
  LoginResponseMessage response;
  response.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> stateEvent = evmgr->CreateEvent("state.loggedin", true);

  bool error = false;
  const char* emsg = 0;
  if (!response.getError().isNull()) // An error occured.
  {
    emsg  = *response.getError();
    error = true;
  }
  else
  {
    emsg  = "blah";
    error = false;
  }

  stateEvent->Add("error", error);
  stateEvent->Add("errorMessage", emsg);
  stateEvent->Add("isAdmin", response.getIsAdmin());

  evmgr->AddEvent(stateEvent);
}

void UserHandler::handleRegisterResponse(GenericMessage* msg)
{
  Report(PT::Notify, "Received RegisterResponse.");
  RegisterResponseMessage answer_msg;
  answer_msg.deserialise(msg->getByteStream());
  ptString error = answer_msg.getError();
  
  PT::GUI::GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  PT::GUI::Windows::OkDialogWindow* dialog = new PT::GUI::Windows::OkDialogWindow(guimanager);
  
  if (!error.isNull())
  {
    Report(PT::Warning, "Registration Failed due to: %s", *error);
	dialog->SetText(*error);
    return;
  }

  dialog->SetText("Registration succeeded");
  Report(PT::Notify, "Registration succeeded!");
}

void UserHandler::handleCharList(GenericMessage* msg)
{
  CharListMessage char_msg;
  char_msg.deserialise(msg->getByteStream());
  //printf("Got %d character:\n---------------------------\n", char_msg.getCharacterCount());
  PT::GUI::GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  PT::GUI::Windows::SelectCharWindow* selectCharWindow = (PT::GUI::Windows::SelectCharWindow*)guimanager->GetWindow(SELECTCHARWINDOW);
  selectCharWindow->EmptyCharList();
  for (int i=0; i<char_msg.getCharacterCount(); i++)
  {
    selectCharWindow->AddCharacter(
      char_msg.getCharId(i), *char_msg.getName(i),
      char_msg.getSkinColour(i), char_msg.getHairColour(i), char_msg.getDecalColour(i));
    //printf("Char %d: %s\n", char_msg.getCharacterId(i), char_msg.getCharacterName(i));
  }
}

void UserHandler::handleCharCreateResponse(GenericMessage* msg)
{
  CharCreateResponseMessage answer_msg;
  answer_msg.deserialise(msg->getByteStream());
  PT::GUI::GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (answer_msg.getError().isNull())
  {
	  PT::GUI::Windows::SelectCharWindow* selectCharWindow = (PT::GUI::Windows::SelectCharWindow*)guimanager->GetWindow(SELECTCHARWINDOW);
      selectCharWindow->AddCharacter(answer_msg.getCharId(), *answer_msg.getName(),
      answer_msg.getSkinColour(), answer_msg.getHairColour(), answer_msg.getDecalColour());
  }
  else
  {
    Report(PT::Warning, "Character creation failed due to: %s", *answer_msg.getError());
    PT::GUI::Windows::OkDialogWindow* dialog = new PT::GUI::Windows::OkDialogWindow(guimanager);
    dialog->SetText(*answer_msg.getError());
  }
}

void UserHandler::handleCharSelectResponse(GenericMessage* msg)
{
  CharSelectResponseMessage answer_msg;
  answer_msg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> stateEvent = evmgr->CreateEvent("state.play", true);
  stateEvent->Add("ownEntityId", answer_msg.getEntityId());
  evmgr->AddEvent(stateEvent);
}
