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

#ifndef EVENTS_EVENT
#define EVENTS_EVENT

#include "common/util/queue.h"
#include "eventtype.h"

/**
 * This class represents one event... rather obvious, huh? :)
 * It's not meant to be an interface but all events shall be the same.
 * So we can block allocate them and reuse. Since we will use events 
 * heavily, this is essential for the performance. I expect there to be
 * like hundreds or thousand of events each second.
 */
class Event : public iLinkable
{
public:
  Events::Type type;
  size_t num1, num2, num3, num4;
  float flt1, flt2, flt3, flt4;

};

#endif // EVENTS_EVENT
