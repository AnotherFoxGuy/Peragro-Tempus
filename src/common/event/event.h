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

#include "common/reporter/reporter.h"

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
      static void DisplayEvent(iEvent* event)
      {
        csRef<iEventAttributeIterator> it = event->GetAttributeIterator();
        Report(PT::Debug, "------------------");
        Report(PT::Debug, "event:");
        while (it->HasNext())
        {
          const char* at = it->Next();
          Report(PT::Debug, "%s", at);
        }
        Report(PT::Debug, "------------------");
      }

       static bool HasError(iEvent* event)
       {
         bool error = event->AttributeExists("error");
         if (error)
         {
           const char* errorStr = "";
           event->Retrieve("error", errorStr);
           error = errorStr[0] != 0;
         }
         return error;
       }

       static std::string GetError(iEvent* event)
       {
         const char* str = "";
         if (event->Retrieve("error", str) != csEventErrNone)
           Report(PT::Error, "Helper::HasError failed!");

         std::string text = str;
         return text;
       }

      /**
       * Get an event parameter's value.
       * @param ev An event.
       * @param name The name of the parameter.
       * @return The parameter's value.
       */
      static unsigned int GetUInt(const iEvent* event, const char* name)
      {
        unsigned int i = 0;
        if (event->Retrieve(name, i) != csEventErrNone)
          Report(PT::Error, "Helper::GetUInt '%s' failed!", name);
        return i;
      }

      /**
       * Get an event parameter's value.
       * @param ev An event.
       * @param name The name of the parameter.
       * @return The parameter's value.
       */
      static unsigned char GetUChar(const iEvent* event, const char* name)
      {
        unsigned char i = 0;
        if (event->Retrieve(name, i) != csEventErrNone)
          Report(PT::Error, "Helper::GetUChar '%s' failed!", name);
        return i;
      }

      /**
       * Get an event parameter's value.
       * @param ev An event.
       * @param name The name of the parameter.
       * @return The parameter's value.
       */
      static std::string GetString(const iEvent* event, const char* name)
      {
        const char* str = "";
        if (event->Retrieve(name, str) != csEventErrNone)
          Report(PT::Error, "Helper::GetString '%s' failed!", name);

        std::string text = str;
        return text;
      }
    };

  } // Events namespace
} // PT namespace

#endif // EVENT_H
