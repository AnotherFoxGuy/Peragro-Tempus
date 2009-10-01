/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
/**
 * @file eventid.h
 *
 * @basic EventId.
 */

#ifndef PT_EVENT_EVENTID_H
#define PT_EVENT_EVENTID_H

#include "common/utility/stringid.h"

namespace PT
{
  namespace Event
  {
    struct EventIdTag {};
    typedef StringId<EventIdTag> EventId;
    struct EventAttrIdTag {};
    typedef StringId<EventAttrIdTag> EventAttrId;

  } // Event namespace
} // PT namespace

#endif
