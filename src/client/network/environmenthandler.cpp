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

#include "client/event/eventmanager.h"
#include "client/event/environmentevent.h"

#include "client/reporter/reporter.h"

void EnvironmentHandler::handleInitTime(GenericMessage* msg)
{
  InitTimeMessage time_msg;
  time_msg.deserialise(msg->getByteStream());
  unsigned char minute = time_msg.getMinute();
  unsigned char hour = time_msg.getHour();
  unsigned char minutesPerHour = time_msg.getMinutesPerHour();
  unsigned char hoursPerDay = time_msg.getHoursPerDay();
  size_t realPerGame = time_msg.getRealPerGame();

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> envEvent = evmgr->CreateEvent("environment.inittime", true);
  envEvent->Add("minute", minute);
  envEvent->Add("hour", hour);
  envEvent->Add("minutesperhour", minutesPerHour);
  envEvent->Add("hoursperday", hoursPerDay);
  envEvent->Add("realpergame", realPerGame);
  evmgr->AddEvent(envEvent);
}

void EnvironmentHandler::handleUpdateTime(GenericMessage* msg)
{
  UpdateTimeMessage time_msg;
  time_msg.deserialise(msg->getByteStream());
  unsigned char minute = time_msg.getMinute();
  unsigned char hour = time_msg.getHour();

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> envEvent = evmgr->CreateEvent("environment.updatetime", true);
  envEvent->Add("minute", minute);
  envEvent->Add("hour", hour);
  evmgr->AddEvent(envEvent);
}

void EnvironmentHandler::handleWeather(GenericMessage* msg)
{
  // TODO: Do something
}
