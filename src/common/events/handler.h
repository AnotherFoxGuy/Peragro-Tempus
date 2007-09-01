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

#ifndef _EVENTS_HANDLER_
#define _EVENTS_HANDLER_

class Event;

/**
 * This is a generic interface for all event handlers which can then subscribe
 * to receive certain events. If two handler subscribe for the same event types,
 * then both will receive it! This way we can handle things like 'PlayerDied' at
 * multiple places whereever we need to take care of that (stop walking, stop fighting, etc.)
 */
class EventHandler
{
private:

public:
  virtual void handle(Event*) = 0;
  virtual ~EventHandler() { };

};

#endif // _EVENTS_HANDLER_


