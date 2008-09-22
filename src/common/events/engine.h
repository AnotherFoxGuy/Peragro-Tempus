/*
    Copyright (C) 2006 Development Team of Peragro Tempus

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

#ifndef EVENTS_ENGINE
#define EVENTS_ENGINE

#include "common/util/array.h"

#include "queue.h"
#include "eventtype.h"
#include "handlerhome.h"
#include "workerhome.h"

class Event;
class EventHandler;

class EventEngine
{
private:
  EventQueue queue;
  EventWorkerHome workers;
  EventHandlerHome handlers;



public:
  EventEngine() : workers(this) {}

  size_t getEventCount() { return queue.getEventCount(); }
  Event* getNextEvent() { return queue.getNextEvent(); }
  Event* getFreeEvent() { return queue.getFreeEvent(); }

  void addEvent(Event* ev) { queue.addEvent(ev); }

  void registerHandler(EventHandler* handler, Events::Type type) { handlers.registerHandler(handler, type); }
  EventHandlerHome* getHandlerHome() { return &handlers; };

  void addWorker() { workers.addWorker(); }
  void start() { workers.start(); }
  void stop() { workers.stop(); }
};

#endif // EVENTS_ENGINE


