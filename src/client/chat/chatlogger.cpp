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

#include "chatlogger.h"

#include <csutil/csstring.h>
#include <iutil/objreg.h>

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/entity/player/playerentity.h"

namespace PT
{
  namespace Chat
  {
    const char *logFilename[CHATLOGGER_LOG_N] =
    {
      "chat.log",
      "system.log"
    };

    const bool logContent[CHATLOGGER_LOG_N][CHATLOGGER_MESSAGE_N] =
    {
        { false,false, true, true ,true ,true  },
        { true , true, false,false,false,false }
    };

    ChatLogger::ChatLogger ()
    {
      // Nothing
    }

    ChatLogger::~ChatLogger ()
    {
      // Nothing
    }

    void ChatLogger::LogMessage( enum CHATLOGGER_MESSAGE_TYPE type,
      const char* message)
    {
      Report (PT::Debug, "Logging message: %s", message);

      for (int log=0; log < CHATLOGGER_LOG_N; log++)
      {
        if (!logContent[log][type])
        {
          continue;
        }

        std::string ownnick = Entity::PlayerEntity::Instance()->GetName();

        if (!logFile[log])
        {
          PointerLibrary* ptr_lib = PointerLibrary::getInstance();
          iObjectRegistry* objreg = ptr_lib->getObjectRegistry();
          csRef<iVFS> vfs (csQueryRegistry<iVFS> (objreg));
          csString filename;

          Report (PT::Debug, "Creating new logfile", message);

          filename.Format ("/UserData/log/%s-%s", ownnick.c_str(), logFilename[log]);

          logFile[log] = vfs->Open (filename, VFS_FILE_APPEND);
          if (logFile[log])
          {
            time_t     clock;
            struct tm *timesig;
            char       buf[32];
            csString   buffer;

            time (&clock);
            timesig = localtime (&clock);
            strftime (buf, 32, "%a %Y-%b-%d %H:%M:%S", timesig);

            buffer.Format ("%s Log for %s\n"
             "------------------------------------------------\n",
             buf, ownnick.c_str());

            logFile[log]->Write (buffer.GetData(), buffer.Length());
          } // if
          else
          {
            Report (PT::Error, "Failed to create chat log file");
          } // else
        } // if
        if (logFile[log])
        {
          time_t     clock;
          struct tm *timesig;
          char       buf[32];
          csString   buffer;

          time (&clock);
          timesig = localtime (&clock);
          strftime (buf, 32, "(%H:%M:%S)", timesig);
          buffer.Format ("%s   %s\n", buf, message);
          logFile[log]->Write (buffer.GetData(), buffer.Length());
          logFile[log]->Flush ();
        } // if
      } // for
    } // LogMessage

    void ChatLogger::LogMessage (enum CHATLOGGER_MESSAGE_TYPE type,
      const char* nick, const char* message)
    {
      csString text;

      switch (type)
      {
        case CHATLOGGER_MESSAGE_WHISPER:
        {
          text.Format ("%s whispers '%s' to you",  nick,message);
          break;
        }
        case CHATLOGGER_MESSAGE_SAY:
        {
          text.Format ("%s says: %s", nick, message);
          break;
        }
        default:
        {
          text.Format ("<%s> %s", nick, message);
          break;
        }
      } // switch
      LogMessage (type, text);
    } // LogMessage

  } // Chat namespace
} // PT namespace
