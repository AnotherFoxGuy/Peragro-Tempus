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
 * @file chatevent.h
 *
 * @brief Helper classes for chat events.
 */

#ifndef PT_CHAT_EVENT_H
#define PT_CHAT_EVENT_H

#include <cssysdef.h>

#include "event.h"

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Provides helper funtions for chat events.
     */
    struct ChatHelper
    {
      /**
       * Gets the nickname from a chat event.
       * @param event The event.
       * @return The nickname.
       */
      static std::string GetNickName(const iEvent* event)
      {
        const char* nickstr = "";
        if (event->Retrieve("speakerName", nickstr) != csEventErrNone)
          Report(PT::Error, "ChatHelper::GetNickName failed!");

        std::string nick = nickstr;
        return nick;
      }

      /**
       * Gets the message content of a chat event.
       * @param event The event.
       * @return The message.
       */
      static std::string GetMessage(const iEvent* event)
      {
        const char* messagestr = "";
        if (event->Retrieve("message", messagestr) != csEventErrNone)
          Report(PT::Error, "ChatHelper::GetMessage failed!");

        std::string message = messagestr;
        return message;
      }

      /**
       * Gets the volume a message is send with.
       * @param event The event.
       * @return The volume.
       */
      static unsigned int GetVolume(const iEvent* event)
      {
        unsigned int volume = 0;
        if (event->Retrieve("volume", volume) != csEventErrNone)
          Report(PT::Error, "ChatHelper::GetVolume failed!");
        return volume;
      }

    };
  } // Events namespace
} // PT namespace

#endif // PT_CHAT_EVENT_H
