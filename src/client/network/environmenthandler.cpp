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

#include "client/network/environmenthandler.h"

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/entityevent.h"

#include "client/pointer/pointer.h"

void EnvironmentHandler::handleInitTime(GenericMessage* msg)
{
  InitTimeMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("environment.inittime", true);
    pEvent->Add("seconds", pmsg.getSeconds());
    pEvent->Add("epoch", pmsg.getEpoch());
    pEvent->Add("secondsPerMinute", pmsg.getSecondsPerMinute());
    pEvent->Add("minutesPerHour", pmsg.getMinutesPerHour());
    pEvent->Add("hoursPerDay", pmsg.getHoursPerDay());
    pEvent->Add("daysPerWeek", pmsg.getDaysPerWeek());
    pEvent->Add("weeksPerMonth", pmsg.getWeeksPerMonth());
    pEvent->Add("monthsPerSeason", pmsg.getMonthsPerSeason());
    pEvent->Add("seasonsPerYear", pmsg.getSeasonsPerYear());
    pEvent->Add("gamePerReal", pmsg.getGamePerReal());

    evmgr->AddEvent(pEvent);
  }

} // end handleInitTime

void EnvironmentHandler::handleUpdateTime(GenericMessage* msg)
{
  UpdateTimeMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("environment.updatetime", true);
    pEvent->Add("seconds", pmsg.getSeconds());

    evmgr->AddEvent(pEvent);
  }

} // end handleUpdateTime

void EnvironmentHandler::handleWeather(GenericMessage* msg)
{
  WeatherMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("environment.weather", true);
    pEvent->Add("weatherId", pmsg.getWeatherId());

    evmgr->AddEvent(pEvent);
  }

} // end handleWeather

