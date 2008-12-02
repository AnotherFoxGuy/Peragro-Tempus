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
#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>

#ifndef CHATLOGGER_H
#define CHATLOGGER_H

namespace PT
{
  namespace Chat
  {
    enum CHATLOGGER_MESSAGE_TYPE
    {
      CHATLOGGER_MESSAGE_UNDEFINED = 0,
      CHATLOGGER_MESSAGE_SYSTEM,
      CHATLOGGER_MESSAGE_EMOTE,
      CHATLOGGER_MESSAGE_SAY,
      CHATLOGGER_MESSAGE_WHISPER,
      CHATLOGGER_MESSAGE_GROUP,
      CHATLOGGER_MESSAGE_N
    };

    enum CHATLOGGER_LOG
    {
      CHATLOGGER_LOG_ALL = 0,
      CHATLOGGER_LOG_SYSTEM,
      CHATLOGGER_LOG_N
    };

    //--------------------------------------------------------------------------
    class ChatLogger
    {
    public:
      ChatLogger ();
      ~ChatLogger ();

      void LogMessage (enum CHATLOGGER_MESSAGE_TYPE type, const char* message);

      void LogMessage (enum CHATLOGGER_MESSAGE_TYPE type, const char* nick,
        const char* message);

    private:
      csRef<iFile> logFile[CHATLOGGER_LOG_N];

    };
    //--------------------------------------------------------------------------
  } // Chat namespace
} // PT namespace

#endif // CHATLOGGER_H
