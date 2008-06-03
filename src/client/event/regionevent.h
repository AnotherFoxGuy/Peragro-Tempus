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
 * @file regionevent.h
 *
 * @brief Helper classes for region events.
 */

#ifndef PT_REGION_EVENT_H
#define PT_REGION_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Provides helper functions for region events.
     */
    struct WorldHelper
    {
      /**
       * Get the current progress in loading the world.
       * @param event The event.
       * @return A decimal number from 0 to 1 representing the current progress.
       */
      static float GetProgress(const iEvent* event)
      {
        float progress = 0.0f;
        if (event->Retrieve("progress", progress) != csEventErrNone)
          Report(PT::Error, "WorldHelper::GetProgress failed!");

        return progress;
      }

    };
  } // Events namespace
} // PT namespace

#endif // PT_REGION_EVENT_H
