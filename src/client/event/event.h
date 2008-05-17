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
/**
 * @file event.h
 *
 * @brief General helper classes for events.
 */

#ifndef EVENT_H
#define EVENT_H

#include <cssysdef.h>

#include <iutil/event.h>

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Provides helper functions for events.
     */
    struct Helper
    {
      /**
       * Print all the event's attributes to the console.
       * @param event The event.
       */
      void DisplayEvent(iEvent* event)
      {
        csRef<iEventAttributeIterator> it = event->GetAttributeIterator();
        printf("------------------\n");
        printf("event:\n");
        while (it->HasNext())
        {
          const char* at = it->Next();
          printf("%s\n", at);
        }
        printf("------------------\n");
      }
    };

  } // Events namespace
} // PT namespace

#endif // EVENT_H
